// ===================================================================================
// Basic SPI Master Functions for CH32V003                                    * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include"spi.h"

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
}

// Transfer one data byte (read and write)
uint8_t SPI_transfer(uint8_t data) {
  SPI1->DATAR = data;                 // send data byte
  while(!SPI_available());            // wait for reply
  return SPI1->DATAR;                 // return received data byte
}
