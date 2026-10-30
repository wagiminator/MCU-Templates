// ===================================================================================
// Basic NeoPixel Functions using Hardware-SPI for STM32F03x              * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "neo_spi.h"

// ===================================================================================
// SPI Parameters and Variables
// ===================================================================================

// Define SPI prescaler (SPI clock = F_CPU / (2 << SPI_PRESC)) and bit patterns
// (MSB first) for "0"-bit and "1"-bit depending on system frequency:
// - 6MHz SPI: 166.7ns per SPI bit, "0": 333ns HIGH, "1": 833ns HIGH, 1333ns total
// - 8MHz SPI: 125.0ns per SPI bit, "0": 375ns HIGH, "1": 750ns HIGH, 1000ns total
#if   F_CPU == 48000000
  #define SPI_PRESC   2
  #define NEO_BIT0    0x60
  #define NEO_BIT1    0x7C
#elif F_CPU == 32000000
  #define SPI_PRESC   1
  #define NEO_BIT0    0x70
  #define NEO_BIT1    0x7E
#elif F_CPU == 24000000
  #define SPI_PRESC   1
  #define NEO_BIT0    0x60
  #define NEO_BIT1    0x7C
#elif F_CPU == 16000000
  #define SPI_PRESC   0
  #define NEO_BIT0    0x70
  #define NEO_BIT1    0x7E
#elif F_CPU == 12000000
  #define SPI_PRESC   0
  #define NEO_BIT0    0x60
  #define NEO_BIT1    0x7C
#else
  #error Unsupported system frequency for NeoPixels!
#endif

// Define SPI data register for 8-bit data transfer
#define SPI_DR_8BIT   *((volatile uint8_t*) &(SPI1->DR))

// NeoPixel data buffer
uint8_t NEO_buffer[3 * NEO_COUNT];

// ===================================================================================
// Init SPI for Neopixels
// ===================================================================================
void NEO_init(void) {
  // Set GPIO pins
  #if NEO_MAP == 0
    // Setup pin PA7 (MOSI)
    RCC->AHBENR    |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~((uint32_t)0b11<<(7<<1)))
                      |  ((uint32_t)0b10<<(7<<1));
    GPIOA->OTYPER  &= ~((uint32_t)1<<7);
    GPIOA->OSPEEDR |= ((uint32_t)0b11<<(7<<1));
    GPIOA->AFR[0]  &= ~((uint32_t)0xf<<(7<<2));
  #elif NEO_MAP == 1
    // Setup pin PB5 (MOSI)
    RCC->AHBENR    |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER    = (GPIOB->MODER  & ~((uint32_t)0b11<<(5<<1)))
                      |  ((uint32_t)0b10<<(5<<1));
    GPIOB->OTYPER  &= ~((uint32_t)1<<5);
    GPIOB->OSPEEDR |= ((uint32_t)0b11<<(5<<1));
    GPIOB->AFR[0]  &= ~((uint32_t)0xf<<(5<<2));
  #elif NEO_MAP == 2
    // Setup pin PB15 (MOSI)
    RCC->AHBENR    |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER    = (GPIOB->MODER  & ~((uint32_t)0b11<<(15<<1)))
                      |  ((uint32_t)0b10<<(15<<1));
    GPIOB->OTYPER  &= ~((uint32_t)1<<15);
    GPIOB->OSPEEDR |= ((uint32_t)0b11<<(15<<1));
    GPIOB->AFR[1]  &= ~((uint32_t)0xf<<(7<<2));
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

// ===================================================================================
// Send one Data Byte to Neopixels
// ===================================================================================
void NEO_sendByte(uint8_t data) {
  uint8_t i;
  for(i=8; i; i--, data<<=1) {              // 8 bits, MSB first
    while(!(SPI1->SR & SPI_SR_TXE));        // wait for transmit buffer empty
    if(data & 0x80) SPI_DR_8BIT = NEO_BIT1; // long HIGH pulse for "1"-bit
    else            SPI_DR_8BIT = NEO_BIT0; // short HIGH pulse for "0"-bit
  }
}

// ===================================================================================
// Write Buffer to Pixels
// ===================================================================================
void NEO_update(void) {
  uint16_t i;
  uint8_t *ptr;
  ptr = NEO_buffer;
  for(i=3*NEO_COUNT; i; i--) NEO_sendByte(*ptr++);
  while(SPI1->SR & SPI_SR_FTLVL);           // wait for TX FIFO empty
  while(SPI1->SR & SPI_SR_BSY);             // wait for last byte transmitted
  NEO_latch();
}

// ===================================================================================
// Clear all Pixels
// ===================================================================================
void NEO_clearAll(void) {
  uint16_t i;
  uint8_t *ptr;
  ptr = NEO_buffer;
  for(i=3*NEO_COUNT; i; i--) *ptr++ = 0;
  NEO_update();
}

// ===================================================================================
// Write Color to a Single Pixel in Buffer
// ===================================================================================
void NEO_writeColor(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b) {
  uint8_t *ptr;
  ptr = NEO_buffer + (3 * pixel);
  #if defined (NEO_GRB)
    *ptr++ = g; *ptr++ = r; *ptr = b;
  #elif defined (NEO_RGB)
    *ptr++ = r; *ptr++ = g; *ptr = b;
  #else
    #error Wrong or missing NeoPixel type definition!
  #endif
}

// ===================================================================================
// Write Hue Value (0..191) and Brightness (0..2) to a Single Pixel in Buffer
// ===================================================================================
void NEO_writeHue(uint8_t pixel, uint8_t hue, uint8_t bright) {
  uint8_t phase = hue >> 6;
  uint8_t step  = (hue & 63) << bright;
  uint8_t nstep = (63 << bright) - step;
  switch(phase) {
    case 0:   NEO_writeColor(pixel, nstep,  step,     0); break;
    case 1:   NEO_writeColor(pixel,     0, nstep,  step); break;
    case 2:   NEO_writeColor(pixel,  step,     0, nstep); break;
    default:  break;
  }
}

// ===================================================================================
// Clear Single Pixel in Buffer
// ===================================================================================
void NEO_clearPixel(uint8_t pixel) {
  NEO_writeColor(pixel, 0, 0, 0);
}
