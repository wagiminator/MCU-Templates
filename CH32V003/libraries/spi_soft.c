// ===================================================================================
// Software SPI Master Functions                                              * v1.0 *
// ===================================================================================
//
// Simple SPI bitbanging. Slave select pin (NSS) must be controlled by the application.
//
// Further information:     https://github.com/wagiminator/ATtiny13-TinySonar
// 2022 by Stefan Wagner:   https://github.com/wagiminator

#include "spi_soft.h"


// ===================================================================================
// SPI Delay
// ===================================================================================
#define SPI_delay()   DLY_ticks(((F_CPU / SPI_CLKRATE) / 2) - 7)

// ===================================================================================
// I2C Functions
// ===================================================================================

// SPI init function (setup pins)
void SPI_init(void) {
  PIN_output(PIN_SCK);
  PIN_output(PIN_MOSI);
  PIN_input_PU(PIN_MISO);
  PIN_low(PIN_SCK);
}

// Transmit and receive one data byte
uint8_t SPI_transfer(uint8_t data) {
  for(uint8_t i=8; i; i--) {              // shift out 8 bits, MSB first
    if(data & 0x80) PIN_high(PIN_MOSI);   // set MOSI according to data bit
    else            PIN_low(PIN_MOSI);
    SPI_delay();                          // delay
    PIN_high(PIN_SCK);                    // SCK high -> shift bit in/out
    data <<= 1;                           // shift data byte
    if(PIN_read(PIN_MISO)) data |= 1;     // set data bit according to MISO
    SPI_delay();                          // delay
    PIN_low(PIN_SCK);                     // SCK low -> prepare next bit
  }
  return data;
}
