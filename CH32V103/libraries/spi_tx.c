// ===================================================================================
// Basic SPI Master Functions (TX only) for CH32V103                          * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "spi_tx.h"

// Init SPI
void SPI_init(void) {
  // Setup GPIO pins: SCK/MOSI alternate push-pull 50MHz
  #if SPI_MAP == 0
    // PA5 (SCK), PA7 (MOSI)
    RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPAEN | RCC_SPI1EN;
    AFIO->PCFR1    &= ~AFIO_PCFR1_SPI1_REMAP;
    GPIOA->CFGLR    = (GPIOA->CFGLR & ~(((uint32_t)0b1111<<(5<<2)) | ((uint32_t)0b1111<<(7<<2))))
                                    |  (((uint32_t)0b1011<<(5<<2)) | ((uint32_t)0b1011<<(7<<2)));
  #elif SPI_MAP == 1
    // PB3 (SCK), PB5 (MOSI)
    RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPBEN | RCC_SPI1EN;
    AFIO->PCFR1    |= AFIO_PCFR1_SPI1_REMAP;
    GPIOB->CFGLR    = (GPIOB->CFGLR & ~(((uint32_t)0b1111<<(3<<2)) | ((uint32_t)0b1111<<(5<<2))))
                                    |  (((uint32_t)0b1011<<(3<<2)) | ((uint32_t)0b1011<<(5<<2)));
  #elif SPI_MAP == 2
    // PB13 (SCK), PB15 (MOSI)
    RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPBEN;
    RCC->APB1PCENR |= RCC_SPI2EN;
    GPIOB->CFGHR    = (GPIOB->CFGHR & ~(((uint32_t)0b1111<<(5<<2)) | ((uint32_t)0b1111<<(7<<2))))
                                    |  (((uint32_t)0b1011<<(5<<2)) | ((uint32_t)0b1011<<(7<<2)));
  #else
    #error Wrong SPI_MAP
  #endif

  // Setup and enable SPI master, TX only, standard configuration
  SPIx->CTLR1 = (SPI_PRESC << 3)      // set prescaler
              | SPI_CTLR1_MSTR        // master configuration
              | SPI_CTLR1_BIDIMODE    // one-line mode
              | SPI_CTLR1_BIDIOE      // transmit only
              | SPI_CTLR1_SSM         // software control of NSS
              | SPI_CTLR1_SSI         // set internal NSS high
              | SPI_CTLR1_SPE;        // enable SPI
}

// Transmit one data byte
void SPI_write(uint8_t data) {
  while(!SPI_ready());                // wait for ready to write
  SPIx->DATAR = data;                 // send data byte
}
