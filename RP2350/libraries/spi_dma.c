// ===================================================================================
// SPI Master Functions with DMA for RP2350 (SPI0/SPI1)                       * v1.1 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "spi_dma.h"
#include "gpio.h"

// Init SPI (master, 8-bit, Motorola frame format)
void SPI_init(void) {
  RESET_cycle(SPI_RESET);                         // reset and release SPI module
  SPI_DEV->SSPCPSR = SPI_PRESC;                   // clock prescaler
  SPI_DEV->SSPCR0  = ((uint32_t)SPI_DIV << SPI_SSPCR0_SCR_Pos)  // clock divider
                   | (SPI_CPOL ? SPI_SSPCR0_SPO : 0)            // clock polarity
                   | (SPI_CPHA ? SPI_SSPCR0_SPH : 0)            // clock phase
                   | ((uint32_t)7 << SPI_SSPCR0_DSS_Pos);       // 8-bit data
  SPI_DEV->SSPDMACR = SPI_SSPDMACR_TXDMAE | SPI_SSPDMACR_RXDMAE;// DREQ signals
  SPI_DEV->SSPCR1  = SPI_SSPCR1_SSE;              // enable SPI (master)
  PIN_alternate(SPI_SCK_PIN,  GPIO_FUNC_SPI);     // set pin functions
  PIN_alternate(SPI_MOSI_PIN, GPIO_FUNC_SPI);
  if(SPI_MISO_PIN >= 0) PIN_alternate(SPI_MISO_PIN, GPIO_FUNC_SPI);
  DMA_init();                                     // release DMA from reset
}

// Transmit and receive one data byte
uint8_t SPI_transfer(uint8_t data) {
  while(SPI_busy());                              // wait for DMA transfer
  while(SPI_available()) (void)SPI_DEV->SSPDR;    // discard old received data
  while(!SPI_ready());                            // wait for space in TX FIFO
  SPI_DEV->SSPDR = data;                          // send data byte
  while(!SPI_available());                        // wait for received data
  return SPI_DEV->SSPDR;                          // return received byte
}

// Transmit one data byte (received data is discarded)
void SPI_write(uint8_t data) {
  while(DMA_busy(SPI_DMA_CH));                    // wait for DMA transfer
  while(!SPI_ready());                            // wait for space in TX FIFO
  SPI_DEV->SSPDR = data;                          // send data byte
}

// Transmit buffer using DMA (returns immediately, check SPI_busy())
void SPI_writeBuffer(const uint8_t* buf, uint16_t len) {
  if(!len) return;
  while(DMA_busy(SPI_DMA_CH));                    // wait for previous transfer
  DMA_CH(SPI_DMA_CH).READ_ADDR   = (uint32_t)buf;
  DMA_CH(SPI_DMA_CH).WRITE_ADDR  = (uint32_t)&SPI_DEV->SSPDR;
  DMA_CH(SPI_DMA_CH).TRANS_COUNT = len;
  DMA_CH(SPI_DMA_CH).CTRL_TRIG   = DMA_CH_CTRL_TRIG_EN | DMA_CH_CTRL_TRIG_INCR_READ
                                 | ((uint32_t)SPI_DREQ_TX << DMA_CH_CTRL_TRIG_TREQ_SEL_Pos)
                                 | ((uint32_t)SPI_DMA_CH  << DMA_CH_CTRL_TRIG_CHAIN_TO_Pos);
}

// ===================================================================================
// Buffer Functions
// ===================================================================================

// Receive buffer (*buf) with length (len) via SPI
void SPI_readBuffer(uint8_t* buf, uint16_t len) {
  while(len--) *buf++ = SPI_transfer(0xff);
}
