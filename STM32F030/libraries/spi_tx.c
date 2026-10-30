// ===================================================================================
// Basic SPI Master Functions (TX only) for STM32F03x                         * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "spi_tx.h"

#define SPI_DR_8BIT *((volatile uint8_t*) &(SPI1->DR))  // for 8-bit data transfer

// Init SPI
void SPI_init(void) {
  // Set GPIO pins
  #if SPI_MAP == 0
    // Setup pins PA5 (SCK), PA7 (MOSI)
    RCC->AHBENR    |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~(((uint32_t)0b11<<(5<<1)) | ((uint32_t)0b11<<(7<<1))))
                      |  (((uint32_t)0b10<<(5<<1)) | ((uint32_t)0b10<<(7<<1)));
    GPIOA->OTYPER  &= ~(((uint32_t)1<<5) | ((uint32_t)1<<7));
    GPIOA->OSPEEDR |= (((uint32_t)0b11<<(5<<1)) | ((uint32_t)0b11<<(7<<1)));
    GPIOA->AFR[0]  &= ~(((uint32_t)0xf<<(5<<2)) | ((uint32_t)0xf<<(7<<2)));
  #elif SPI_MAP == 1
    // Setup pins PB3 (SCK), PB5 (MOSI)
    RCC->AHBENR    |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER    = (GPIOB->MODER  & ~(((uint32_t)0b11<<(3<<1)) | ((uint32_t)0b11<<(5<<1))))
                      |  (((uint32_t)0b10<<(3<<1)) | ((uint32_t)0b10<<(5<<1)));
    GPIOB->OTYPER  &= ~(((uint32_t)1<<3) | ((uint32_t)1<<5));
    GPIOB->OSPEEDR |= (((uint32_t)0b11<<(3<<1)) | ((uint32_t)0b11<<(5<<1)));
    GPIOB->AFR[0]  &= ~(((uint32_t)0xf<<(3<<2)) | ((uint32_t)0xf<<(5<<2)));
  #elif SPI_MAP == 2
    // Setup pins PB13 (SCK), PB15 (MOSI)
    RCC->AHBENR    |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER    = (GPIOB->MODER  & ~(((uint32_t)0b11<<(13<<1)) | ((uint32_t)0b11<<(15<<1))))
                      |  (((uint32_t)0b10<<(13<<1)) | ((uint32_t)0b10<<(15<<1)));
    GPIOB->OTYPER  &= ~(((uint32_t)1<<13) | ((uint32_t)1<<15));
    GPIOB->OSPEEDR |= (((uint32_t)0b11<<(13<<1)) | ((uint32_t)0b11<<(15<<1)));
    GPIOB->AFR[1]  &= ~(((uint32_t)0xf<<(5<<2)) | ((uint32_t)0xf<<(7<<2)));
  #else
    #warning No automatic pin mapping for SPI
  #endif


  // Setup and enable SPI master, TX only, standard configuration
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
  SPI1->CR2     = (7 << SPI_CR2_DS_Pos);        // 8-bit data size
  SPI1->CR1     = (SPI_PRESC << 3)              // set prescaler
                | SPI_CR1_MSTR                  // master configuration
                | SPI_CR1_BIDIMODE              // one-line mode
                | SPI_CR1_BIDIOE                // transmit only
                | SPI_CR1_SSM                   // software control of NSS
                | SPI_CR1_SSI;                  // set internal NSS high
  SPI1->CR1    |= SPI_CR1_SPE;                  // enable SPI
}

// Send one data byte
void SPI_write(uint8_t data) {
  while(!SPI_ready());                          // wait for ready to send
  SPI_DR_8BIT = data;                           // send data byte
}
