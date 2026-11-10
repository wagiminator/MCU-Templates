// ===================================================================================
// SPI Master Functions with DMA for CH32V203                                 * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "spi_dma.h"

// Init SPI
void SPI_init(void) {
  // Setup GPIO pins: SCK/MOSI alternate push-pull 50MHz, MISO input with pullup
  #if SPI_MAP == 0
    // PA5 (SCK), PA6 (MISO), PA7 (MOSI)
    RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPAEN | RCC_SPI1EN;
    AFIO->PCFR1    &= ~AFIO_PCFR1_SPI1_REMAP;
    GPIOA->CFGLR    = (GPIOA->CFGLR & ~(((uint32_t)0b1111<<(5<<2)) | ((uint32_t)0b1111<<(6<<2)) | ((uint32_t)0b1111<<(7<<2))))
                                    |  (((uint32_t)0b1011<<(5<<2)) | ((uint32_t)0b1000<<(6<<2)) | ((uint32_t)0b1011<<(7<<2)));
    GPIOA->BSHR     = (uint32_t)1<<6;
  #elif SPI_MAP == 1
    // PB3 (SCK), PB4 (MISO), PB5 (MOSI)
    RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPBEN | RCC_SPI1EN;
    AFIO->PCFR1    |= AFIO_PCFR1_SPI1_REMAP;
    GPIOB->CFGLR    = (GPIOB->CFGLR & ~(((uint32_t)0b1111<<(3<<2)) | ((uint32_t)0b1111<<(4<<2)) | ((uint32_t)0b1111<<(5<<2))))
                                    |  (((uint32_t)0b1011<<(3<<2)) | ((uint32_t)0b1000<<(4<<2)) | ((uint32_t)0b1011<<(5<<2)));
    GPIOB->BSHR     = (uint32_t)1<<4;
  #elif SPI_MAP == 2
    // PB13 (SCK), PB14 (MISO), PB15 (MOSI)
    RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPBEN;
    RCC->APB1PCENR |= RCC_SPI2EN;
    GPIOB->CFGHR    = (GPIOB->CFGHR & ~(((uint32_t)0b1111<<(5<<2)) | ((uint32_t)0b1111<<(6<<2)) | ((uint32_t)0b1111<<(7<<2))))
                                    |  (((uint32_t)0b1011<<(5<<2)) | ((uint32_t)0b1000<<(6<<2)) | ((uint32_t)0b1011<<(7<<2)));
    GPIOB->BSHR     = (uint32_t)1<<14;
  #else
    #error Wrong SPI_MAP
  #endif

  // Setup and enable SPI master, standard configuration
  SPIx->CTLR1 = (SPI_PRESC << 3)      // set prescaler
              | SPI_CTLR1_MSTR        // master configuration
              | SPI_CTLR1_SSM         // software control of NSS
              | SPI_CTLR1_SSI         // set internal NSS high
              | SPI_CTLR1_SPE;        // enable SPI
  SPIx->CTLR2 |= SPI_CTLR2_TXDMAEN;   // enable TX DMA request

  // Setup DMA
  RCC->AHBPCENR |= RCC_DMA1EN;                    // enable DMA module clock
  SPI_DMA_CHAN->PADDR = (uint32_t)&SPIx->DATAR;     // peripheral address
  SPI_DMA_CHAN->CFGR  = DMA_CFGR1_MINC            // increment memory address
                      | DMA_CFGR1_DIR;            // memory to peripheral
}

// Transfer one data byte (read and write)
uint8_t SPI_transfer(uint8_t data) {
  while(SPI_busy());                  // wait for DMA transfer
  while(SPI_available()) (void)SPIx->DATAR; // discard old received data
  (void)SPIx->STATR;                   // clear overrun flag
  while(!SPI_ready());                // wait for transmit buffer empty
  SPIx->DATAR = data;                 // send data byte
  while(!SPI_available());            // wait for reply
  return SPIx->DATAR;                 // return received data byte
}

// ===================================================================================
// Buffer Functions
// ===================================================================================

// Transmit buffer (*buf) with length (len) using DMA (returns immediately)
void SPI_writeBuffer(const uint8_t* buf, uint16_t len) {
  if(!len) return;                                // nothing to send
  while(SPI_busy());                              // wait for previous transfer
  SPI_DMA_CHAN->CFGR &= ~DMA_CFGR1_EN;            // disable DMA channel
  SPI_DMA_CHAN->CNTR  = len;                      // number of bytes to be transfered
  SPI_DMA_CHAN->MADDR = (uint32_t)buf;            // memory address
  SPI_DMA_CHAN->CFGR |=  DMA_CFGR1_EN;            // enable DMA channel -> start
}

// Receive buffer (*buf) with length (len) via SPI
void SPI_readBuffer(uint8_t* buf, uint16_t len) {
  while(len--) *buf++ = SPI_transfer(0xff);
}
