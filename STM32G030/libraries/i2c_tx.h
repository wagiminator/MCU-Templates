// ===================================================================================
// Basic I2C Master Functions for STM32G0xx (write only)                      * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// I2C_init()               Init I2C with defined clock rate (see below)
// I2C_start(addr)          I2C start transmission, addr must contain R/W bit
// I2C_write(b)             I2C transmit one data byte via I2C
// I2C_stop()               I2C stop transmission
//
// I2C_sendBuffer(addr,buf,len) Send buffer (*buf) with length (len) to device (addr)
// I2C_writeBuffer(buf,len)     Write buffer (*buf) with length (len) via I2C and stop
//
// I2C pin mapping (set below in I2C parameters):
// ----------------------------------------------
// I2C_MAP   0     1     2     3     4
// SCL-pin  PA9   PA11  PB6   PB8   No mapping
// SDA-pin  PA10  PA12  PB7   PB9   No mapping
//
// External pull-up resistors (4k7 - 10k) are mandatory!
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// I2C Parameters
#define I2C_CLKRATE   400000    // I2C bus clock rate (Hz)
#define I2C_MAP       1         // I2C pin mapping (see above)

// I2C Functions
void I2C_init(void);            // I2C init function
void I2C_start(uint8_t addr);   // I2C start transmission, addr must contain R/W bit
void I2C_write(uint8_t data);   // I2C transmit one data byte via I2C
void I2C_stop(void);            // I2C stop transmission

void I2C_writeBuffer(uint8_t* buf, uint16_t len);

#define I2C_sendBuffer(addr,buf,len)  {I2C_start(addr); I2C_writeBuffer(buf,len);}

#ifdef __cplusplus
};
#endif
