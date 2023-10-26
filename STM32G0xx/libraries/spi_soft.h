// ===================================================================================
// Software SPI Master Functions                                              * v1.0 *
// ===================================================================================
//
// Simple SPI bitbanging.
//
// Functions available:
// --------------------
// SPI_init()               SPI init function
// SPI_transfer(d)          transmit and receive one data byte
//
// SCK/MOSI/MISO pin and SPI clock rate must be defined below.
// Slave select pins (NSS) must be defined and controlled by the application.
//
// Further information:     https://github.com/wagiminator/ATtiny13-TinySonar
// 2022 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"
#include "gpio.h"

// SPI parameters
#define PIN_SCK       PA5         // serial clock pin
#define PIN_MOSI      PA7         // master out / slave in  pin
#define PIN_MISO      PA6         // master in  / slave out pin
#define SPI_CLKRATE   1000000     // clock rate (Hz)

// SPI functions
void SPI_init(void);
uint8_t SPI_transfer(uint8_t data);

#ifdef __cplusplus
};
#endif
