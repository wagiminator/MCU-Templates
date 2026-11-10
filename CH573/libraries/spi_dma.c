// ===================================================================================
// SPI Master Functions with DMA for CH573/CH571                              * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "spi_dma.h"
#include "gpio.h"

// Clock divider
#define SPI_DIV ((F_CPU / SPI_CLKRATE) < 2 ? 2 : (F_CPU / SPI_CLKRATE) > 255 ? 255 : (F_CPU / SPI_CLKRATE))

// Init SPI0 as master
void SPI_init(void) {
  #if SPI_REMAP == 0
  R16_PIN_ALTERNATE &= ~RB_PIN_SPI0;
  PIN_low(PA13); PIN_output(PA13);              // SCK
  PIN_output(PA14);                             // MOSI
  PIN_input_PU(PA15);                           // MISO
  #else
  R16_PIN_ALTERNATE |=  RB_PIN_SPI0;
  PIN_low(PB13); PIN_output(PB13);              // SCK
  PIN_output(PB14);                             // MOSI
  PIN_input_PU(PB15);                           // MISO
  #endif
  R8_SPI0_CLOCK_DIV = SPI_DIV;                  // set clock rate
  R8_SPI0_CTRL_MOD  = RB_SPI_ALL_CLEAR;         // reset FIFO and count
  R8_SPI0_CTRL_MOD  = RB_SPI_MOSI_OE | RB_SPI_SCK_OE;  // master, mode 0
  R8_SPI0_CTRL_CFG  = RB_SPI_AUTO_IF;           // auto clear flag, MSB first, no DMA
}

// Check if a buffer can be transferred by the DMA (word-aligned RAM buffer)
#define SPI_DMA_OK(buf,len) ((((uint32_t)(buf) & 0xFFFF0003) == 0x20000000) && ((len) < 0x1000))

// Check if SPI is busy (DMA transfer or transmission ongoing)
uint8_t SPI_busy(void) {
  if(R8_SPI0_CTRL_CFG & RB_SPI_DMA_ENABLE) {    // DMA active?
    if(!(R8_SPI0_INT_FLAG & RB_SPI_IF_CNT_END)) return 1; // still transferring
    R8_SPI0_CTRL_CFG &= ~RB_SPI_DMA_ENABLE;     // transfer finished -> disable DMA
  }
  return !(R8_SPI0_INT_FLAG & RB_SPI_FREE);
}

// Transmit and receive one data byte
uint8_t SPI_transfer(uint8_t data) {
  while(SPI_busy());                            // wait for DMA transfer
  R8_SPI0_CTRL_MOD &= ~RB_SPI_FIFO_DIR;         // FIFO direction: out
  R8_SPI0_BUFFER = data;                        // start transfer
  while(!(R8_SPI0_INT_FLAG & RB_SPI_FREE));     // wait until finished
  return R8_SPI0_BUFFER;                        // return received byte
}

// Transmit buffer using DMA (returns immediately) or via FIFO
void SPI_writeBuffer(const uint8_t* buf, uint16_t len) {
  while(SPI_busy());                            // wait for previous transfer
  R8_SPI0_CTRL_MOD &= ~RB_SPI_FIFO_DIR;         // FIFO direction: out
  if(!len) return;
  if(SPI_DMA_OK(buf, len)) {                    // DMA possible?
    R16_SPI0_DMA_BEG   = (uint32_t)buf;         // DMA start address
    R16_SPI0_DMA_END   = (uint32_t)(buf + len); // DMA end address
    R16_SPI0_TOTAL_CNT = len;                   // number of bytes
    R8_SPI0_INT_FLAG   = RB_SPI_IF_CNT_END | RB_SPI_IF_DMA_END; // clear flags
    R8_SPI0_CTRL_CFG  |= RB_SPI_DMA_ENABLE;     // start DMA
    return;
  }
  while(len) {
    uint16_t n = (len > 0xFFF) ? 0xFFF : len;   // max 4095 bytes per run
    len -= n;
    R16_SPI0_TOTAL_CNT = n;
    R8_SPI0_INT_FLAG   = RB_SPI_IF_CNT_END;
    while(n) {
      if(R8_SPI0_FIFO_COUNT < SPI_FIFO_SIZE) {
        R8_SPI0_FIFO = *buf++;
        n--;
      }
    }
    while(R8_SPI0_FIFO_COUNT);                  // wait until FIFO is empty
    while(!(R8_SPI0_INT_FLAG & RB_SPI_FREE));   // wait until last byte is sent
  }
}

// Receive buffer using DMA or via FIFO (waits until all bytes are received)
void SPI_readBuffer(uint8_t* buf, uint16_t len) {
  while(SPI_busy());                            // wait for previous transfer
  R8_SPI0_CTRL_MOD |= RB_SPI_FIFO_DIR;          // FIFO direction: in
  if(len && SPI_DMA_OK(buf, len)) {             // DMA possible?
    R16_SPI0_DMA_BEG   = (uint32_t)buf;         // DMA start address
    R16_SPI0_DMA_END   = (uint32_t)(buf + len); // DMA end address
    R16_SPI0_TOTAL_CNT = len;                   // number of bytes
    R8_SPI0_INT_FLAG   = RB_SPI_IF_CNT_END | RB_SPI_IF_DMA_END; // clear flags
    R8_SPI0_CTRL_CFG  |= RB_SPI_DMA_ENABLE;     // start DMA
    while(SPI_busy());                          // wait until finished
    len = 0;                                    // done
  }
  while(len) {
    uint16_t n = (len > 0xFFF) ? 0xFFF : len;
    len -= n;
    R16_SPI0_TOTAL_CNT = n;
    R8_SPI0_INT_FLAG   = RB_SPI_IF_CNT_END;
    while(n) {
      if(R8_SPI0_FIFO_COUNT) {
        *buf++ = R8_SPI0_FIFO;
        n--;
      }
    }
  }
  R8_SPI0_CTRL_MOD &= ~RB_SPI_FIFO_DIR;
}
