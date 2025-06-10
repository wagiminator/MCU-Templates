// ===================================================================================
// Software SPI Master Functions for tinyAVR 0-Series and 1-Series            * v1.0 *
// ===================================================================================
//
// Simple SPI bitbanging. Slave select pin (NSS) must be controlled by the application.
//
// Further information:     https://github.com/wagiminator/ATtiny13-TinySonar
// 2021 by Stefan Wagner:   https://github.com/wagiminator

#include "spi_soft.h"


// ===================================================================================
// SPI Delay
// ===================================================================================
#define SPI_DLY_TICKS_H     ((((F_CPU / SPI_CLKRATE) + 1) / 2) - 4)
#define SPI_DLY_TICKS_L     ((((F_CPU / SPI_CLKRATE) + 1) / 2) - 8)

#if SPI_DLY_TICKS_H >= 1
  #define SPI_DELAY_H()     DLY_ticks(SPI_DLY_TICKS_H)
#else
  #define SPI_DELAY_H()
#endif

#if SPI_DLY_TICKS_L >= 1
  #define SPI_DELAY_L()     DLY_ticks(SPI_DLY_TICKS_L)
#else
  #define SPI_DELAY_L()
#endif

// ===================================================================================
// SPI Functions
// ===================================================================================

// SPI init function (setup pins)
void SPI_init(void) {
  PIN_output(PIN_SCK);
  PIN_output(PIN_MOSI);
  PIN_input(PIN_MISO);
  PIN_pullup(PIN_MISO);
  PIN_low(PIN_SCK);
}

// Transmit and receive one data byte
uint8_t SPI_transfer(uint8_t data) {
  for(uint8_t i=8; i; i--) {              // shift out 8 bits, MSB first
    if(data & 0x80) PIN_high(PIN_MOSI);   // set MOSI according to data bit
    else            PIN_low(PIN_MOSI);
    SPI_DELAY_L();                        // delay
    PIN_high(PIN_SCK);                    // SCK high -> shift bit in/out
    data <<= 1;                           // shift data byte
    if(PIN_read(PIN_MISO)) data |= 1;     // set data bit according to MISO
    SPI_DELAY_H();                        // delay
    PIN_low(PIN_SCK);                     // SCK low -> prepare next bit
  }
  return data;
}
