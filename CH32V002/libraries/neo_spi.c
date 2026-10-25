// ===================================================================================
// Basic NeoPixel Functions using Hardware-SPI                                * v1.0 *
// for CH32V002, V004, V005, V006, V007, M007
// ===================================================================================
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#include "neo_spi.h"

// Pin assignment (MOSI) for the selected SPI mapping (CH32V00X RM: AFIO_PCFR1 SPI1_RM[2:0])
#if   NEO_MAP == 0                      // MOSI: PC6
  #define NEO_GPIO    GPIOC
  #define NEO_PIN     6
  #define NEO_PORT_EN RCC_IOPCEN
#elif NEO_MAP == 1                      // MOSI: PC6
  #define NEO_GPIO    GPIOC
  #define NEO_PIN     6
  #define NEO_PORT_EN RCC_IOPCEN
#elif NEO_MAP == 2                      // MOSI: PD3
  #define NEO_GPIO    GPIOD
  #define NEO_PIN     3
  #define NEO_PORT_EN RCC_IOPDEN
#elif NEO_MAP == 3                      // MOSI: PC0
  #define NEO_GPIO    GPIOC
  #define NEO_PIN     0
  #define NEO_PORT_EN RCC_IOPCEN
#elif NEO_MAP == 4                      // MOSI: PD6
  #define NEO_GPIO    GPIOD
  #define NEO_PIN     6
  #define NEO_PORT_EN RCC_IOPDEN
#elif NEO_MAP == 5                      // MOSI: PA2
  #define NEO_GPIO    GPIOA
  #define NEO_PIN     2
  #define NEO_PORT_EN RCC_IOPAEN
#elif NEO_MAP == 6                      // MOSI: PB4
  #define NEO_GPIO    GPIOB
  #define NEO_PIN     4
  #define NEO_PORT_EN RCC_IOPBEN
#else
  #error Wrong NEO_MAP (0..6)
#endif


// ===================================================================================
// SPI Parameters and Variables
// ===================================================================================

// Define SPI prescaler for 3 MHz SPI frequency -> 333.33us per bit, 750kHz byte freq
#if F_CPU == 48000000
#define SPI_PRESC   3
#elif F_CPU == 24000000
#define SPI_PRESC   2
#elif F_CPU == 12000000
#define SPI_PRESC   1
#elif F_CPU ==  6000000
#define SPI_PRESC   0
#else
#error Unsupported system frequency for NeoPixels!
#endif

uint8_t NEO_buffer[3 * NEO_COUNT];          // pixel buffer

// ===================================================================================
// Init SPI for Neopixels
// ===================================================================================
void NEO_init(void) {
  // Enable AFIO, GPIO port and SPI module clock
  RCC->PB2PCENR |= RCC_AFIOEN | NEO_PORT_EN | RCC_SPI1EN;

  // Select pin mapping (SPI1_RM[2:0] = AFIO_PCFR1[2:0])
  AFIO->PCFR1 = (AFIO->PCFR1 & ~AFIO_PCFR1_SPI1_REMAP) | (uint32_t)NEO_MAP;

  // Setup GPIO pin MOSI (alternate output, push-pull, 30MHz)
  NEO_GPIO->CFGLR = (NEO_GPIO->CFGLR & ~((uint32_t)0b1111 << (NEO_PIN << 2)))
                                     |  ((uint32_t)0b1001 << (NEO_PIN << 2));

  // Setup and enable SPI master, TX only, standard configuration
  SPI1->CTLR1 = (SPI_PRESC << 3)            // set prescaler
              | SPI_CTLR1_MSTR              // master configuration
              | SPI_CTLR1_BIDIMODE          // one-line mode
              | SPI_CTLR1_BIDIOE            // transmit only
              | SPI_CTLR1_SSM               // software control of NSS
              | SPI_CTLR1_SSI               // set internal NSS high
              | SPI_CTLR1_SPE;              // enable SPI
}

// ===================================================================================
// Send one Data Byte to Neopixels
// ===================================================================================
void NEO_sendByte(uint8_t data) {
  uint8_t i, bits;
  for(i=4; i; i--, data<<=2) {              // 4x2 bits, MSB first
    if(data & 0x80) bits  = 0x60;           // 667us high for "1"-bit
    else            bits  = 0x40;           // 333us high for "0"-bit
    if(data & 0x40) bits |= 0x06;           // 667us high for "1"-bit
    else            bits |= 0x04;           // 333us high for "0"-bit
    while(!(SPI1->STATR & SPI_STATR_TXE));  // wait for transmit buffer empty
    SPI1->DATAR = bits;                     // send 2 bits
  }
}

// ===================================================================================
// Write Buffer to Pixels
// ===================================================================================
void NEO_update(void) {
  uint8_t i;
  uint8_t *ptr;
  ptr = NEO_buffer;
  for(i=3*NEO_COUNT; i; i--) NEO_sendByte(*ptr++);
  NEO_latch();
}

// ===================================================================================
// Clear all Pixels
// ===================================================================================
void NEO_clearAll(void) {
  uint8_t i;
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
