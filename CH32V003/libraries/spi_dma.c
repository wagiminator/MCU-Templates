// ===================================================================================
// SPI Master Functions with DMA for CH32V003                                 * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "spi_dma.h"

// Init SPI
void SPI_init(void) {
  // Enable GPIO and SPI module clock
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPCEN | RCC_SPI1EN;
  
  // Setup GPIO pins PC5 (SCK), PC6 (MOSI), PC7 (MISO)
  GPIOC->CFGLR  = (GPIOC->CFGLR & ~(((uint32_t)0b1111<<(5<<2)) | ((uint32_t)0b1111<<(6<<2)) | ((uint32_t)0b1111<<(7<<2)) ))
                                |  (((uint32_t)0b1001<<(5<<2)) | ((uint32_t)0b1001<<(6<<2)) | ((uint32_t)0b1000<<(7<<2)) );
  GPIOC->BSHR   = (uint32_t)1<<7;

  // Setup and enable SPI master, standard configuration
  SPI1->CTLR1 = (SPI_PRESC << 3)      // set prescaler
              | SPI_CTLR1_MSTR        // master configuration
              | SPI_CTLR1_SSM         // software control of NSS
              | SPI_CTLR1_SSI         // set internal NSS high
              | SPI_CTLR1_SPE;        // enable SPI
  SPI1->CTLR2 |= SPI_CTLR2_TXDMAEN;   // enable TX DMA request

  // Setup DMA
  RCC->AHBPCENR |= RCC_DMA1EN;                    // enable DMA module clock
  SPI_DMA_CHAN->PADDR = (uint32_t)&SPI1->DATAR;     // peripheral address
  SPI_DMA_CHAN->CFGR  = DMA_CFGR1_MINC            // increment memory address
                      | DMA_CFGR1_DIR;            // memory to peripheral
}

// Transfer one data byte (read and write)
uint8_t SPI_transfer(uint8_t data) {
  while(SPI_busy());                  // wait for DMA transfer
  while(SPI_available()) (void)SPI1->DATAR; // discard old received data
  (void)SPI1->STATR;                   // clear overrun flag
  SPI1->DATAR = data;                 // send data byte
  while(!SPI_available());            // wait for reply
  return SPI1->DATAR;                 // return received data byte
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
