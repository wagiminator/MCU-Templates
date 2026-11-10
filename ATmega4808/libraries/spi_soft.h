// ===================================================================================
// Software SPI Master Functions for megaAVR 0-Series                         * v1.1 *
// ===================================================================================
//
// Simple SPI bitbanging.
//
// Functions available:
// --------------------
// SPI_init()               SPI init function
// SPI_transfer(d)          transmit and receive one data byte
// SPI_write(d)             Transmit one data byte
// SPI_read()               Receive one data byte (sends 0xFF)
// SPI_writeBuffer(buf,len) Transmit buffer (*buf) with length (len)
// SPI_readBuffer(buf,len)  Receive buffer (*buf) with length (len), sends 0xFF
//
// SCK/MOSI/MISO pin and SPI clock rate must be defined below.
// Slave select pins (NSS) must be defined and controlled by the application.
//
// Further information:     https://github.com/wagiminator/ATtiny13-TinySonar
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"
#include "gpio.h"

// SPI parameters
#define PIN_SCK       PA3         // serial clock pin
#define PIN_MOSI      PA1         // master out / slave in  pin
#define PIN_MISO      PA2         // master in  / slave out pin
#define SPI_CLKRATE   1000000     // clock rate (Hz)

// SPI functions
void SPI_init(void);
uint8_t SPI_transfer(uint8_t data);
#define SPI_write(d)        (void)SPI_transfer(d)  // transmit one data byte
#define SPI_read()          SPI_transfer(0xff)     // receive one data byte
void SPI_writeBuffer(const uint8_t* buf, uint16_t len);
void SPI_readBuffer(uint8_t* buf, uint16_t len);

#ifdef __cplusplus
}
#endif
