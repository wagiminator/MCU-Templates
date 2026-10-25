// ===================================================================================
// Basic SPI Master Functions for CH32V002, V004, V005, V006, V007, M007      * v1.0 *
// ===================================================================================
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#include "spi.h"

// Pin assignment for the selected mapping (CH32V00X RM Table 7-12: AFIO_PCFR1 SPI1_RM[2:0])
#if   SPI_MAP == 0                      // SCK: PC5, MOSI: PC6, MISO: PC7
  #define SPI_SCK_PORT  C
  #define SPI_SCK_PIN   5
  #define SPI_MOSI_PORT C
  #define SPI_MOSI_PIN  6
  #define SPI_MISO_PORT C
  #define SPI_MISO_PIN  7
#elif SPI_MAP == 1                      // SCK: PC5, MOSI: PC6, MISO: PC7 (NSS: PC0)
  #define SPI_SCK_PORT  C
  #define SPI_SCK_PIN   5
  #define SPI_MOSI_PORT C
  #define SPI_MOSI_PIN  6
  #define SPI_MISO_PORT C
  #define SPI_MISO_PIN  7
#elif SPI_MAP == 2                      // SCK: PD2, MOSI: PD3, MISO: PB3
  #define SPI_SCK_PORT  D
  #define SPI_SCK_PIN   2
  #define SPI_MOSI_PORT D
  #define SPI_MOSI_PIN  3
  #define SPI_MISO_PORT B
  #define SPI_MISO_PIN  3
#elif SPI_MAP == 3                      // SCK: PB1, MOSI: PC0, MISO: PB2
  #define SPI_SCK_PORT  B
  #define SPI_SCK_PIN   1
  #define SPI_MOSI_PORT C
  #define SPI_MOSI_PIN  0
  #define SPI_MISO_PORT B
  #define SPI_MISO_PIN  2
#elif SPI_MAP == 4                      // SCK: PD4, MOSI: PD6, MISO: PD5
  #define SPI_SCK_PORT  D
  #define SPI_SCK_PIN   4
  #define SPI_MOSI_PORT D
  #define SPI_MOSI_PIN  6
  #define SPI_MISO_PORT D
  #define SPI_MISO_PIN  5
#elif SPI_MAP == 5                      // SCK: PA1, MOSI: PA2, MISO: PB5
  #define SPI_SCK_PORT  A
  #define SPI_SCK_PIN   1
  #define SPI_MOSI_PORT A
  #define SPI_MOSI_PIN  2
  #define SPI_MISO_PORT B
  #define SPI_MISO_PIN  5
#elif SPI_MAP == 6                      // SCK: PB5, MOSI: PB4, MISO: PC7
  #define SPI_SCK_PORT  B
  #define SPI_SCK_PIN   5
  #define SPI_MOSI_PORT B
  #define SPI_MOSI_PIN  4
  #define SPI_MISO_PORT C
  #define SPI_MISO_PIN  7
#else
  #error Wrong SPI_MAP (0..6)
#endif

#define SPI_CAT(a,b)    a##b
#define SPI_XCAT(a,b)   SPI_CAT(a,b)
#define SPI_GPIO(x)     SPI_XCAT(GPIO, x)
#define SPI_IOPEN(x)    SPI_XCAT(SPI_XCAT(RCC_IOP, x), EN)
#define SPI_CFG(pin,v)  ((uint32_t)(v) << ((pin) << 2))
#define SPI_MSK(pin)    ((uint32_t)0xF << ((pin) << 2))

// Init SPI
void SPI_init(void) {
  // Enable AFIO, GPIO port(s) and SPI module clock
  RCC->PB2PCENR |= RCC_AFIOEN | RCC_SPI1EN
                |  SPI_IOPEN(SPI_SCK_PORT) | SPI_IOPEN(SPI_MOSI_PORT) | SPI_IOPEN(SPI_MISO_PORT);

  // Select pin mapping (SPI1_RM[2:0] = AFIO_PCFR1[2:0])
  AFIO->PCFR1 = (AFIO->PCFR1 & ~AFIO_PCFR1_SPI1_REMAP) | (uint32_t)SPI_MAP;

  // Setup GPIO pins: SCK, MOSI = AF push-pull 30MHz, MISO = input with pull-up
  SPI_GPIO(SPI_SCK_PORT)->CFGLR  = (SPI_GPIO(SPI_SCK_PORT)->CFGLR  & ~SPI_MSK(SPI_SCK_PIN))  | SPI_CFG(SPI_SCK_PIN,  0b1001);
  SPI_GPIO(SPI_MOSI_PORT)->CFGLR = (SPI_GPIO(SPI_MOSI_PORT)->CFGLR & ~SPI_MSK(SPI_MOSI_PIN)) | SPI_CFG(SPI_MOSI_PIN, 0b1001);
  SPI_GPIO(SPI_MISO_PORT)->CFGLR = (SPI_GPIO(SPI_MISO_PORT)->CFGLR & ~SPI_MSK(SPI_MISO_PIN)) | SPI_CFG(SPI_MISO_PIN, 0b1000);
  SPI_GPIO(SPI_MISO_PORT)->BSHR  = (uint32_t)1 << SPI_MISO_PIN;

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
