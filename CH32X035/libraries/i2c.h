// ===================================================================================
// Basic I2C Master Functions for CH32X035/X034/X033                          * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// I2C_init()               Init I2C with defined clock rate (see below)
// I2C_start(addr)          I2C start transmission, addr must contain R/W bit
// I2C_restart(addr)        I2C restart transmission, addr must contain R/W bit
// I2C_write(b)             I2C transmit one data byte via I2C
// I2C_read(ack)            I2C receive one data byte (set ack=0 for last byte)
// I2C_stop()               I2C stop transmission
//
// I2C_sendBuffer(addr,buf,len) Send buffer (*buf) with length (len) to device (addr)
// I2C_getBuffer(addr,buf,len)  Receive buffer (*buf) with length (len) from (addr)
// I2C_writeBuffer(buf,len)     Write buffer (*buf) with length (len) via I2C and stop
// I2C_readBuffer(buf,len)      Read buffer (*buf) with length (len) via I2C and stop
//
// I2C pin mapping (set below in I2C parameters):
// ----------------------------------------------
// I2C_MAP     0      1      2      3      4      5
// SCL-pin    PA10   PA13   PC16   PC19   PC17   PC18
// SDA-pin    PA11   PA14   PC17   PC18   PC16   PC19
//
// Note: PC16/PC17 are also the USB pins (UDM/UDP), PC18/PC19 the debug pins
// (SWDIO/SWCLK). Not all pins are available on all packages (see datasheet).
//
// Notes:
// ------
// - External pull-up resistors (4k7 - 10k) are mandatory!
// - Clock stretching by the slave is supported (done by hardware).
// - The I2C module runs with PCLK (= F_CPU in this template).
// - The functions don't check for ACK/NACK of the slave and don't use timeouts.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// I2C Parameters
#ifndef I2C_CLKRATE
#define I2C_CLKRATE   400000    // I2C bus clock rate (Hz, max 400000)
#endif
#ifndef I2C_MAP
#define I2C_MAP       0         // I2C pin mapping (see above)
#endif

// I2C Functions
void I2C_init(void);            // I2C init function
void I2C_start(uint8_t addr);   // I2C start transmission, addr must contain R/W bit
void I2C_stop(void);            // I2C stop transmission
void I2C_write(uint8_t data);   // I2C transmit one data byte via I2C
uint8_t I2C_read(uint8_t ack);  // I2C receive one data byte from the slave

void I2C_writeBuffer(uint8_t* buf, uint16_t len);
void I2C_readBuffer(uint8_t* buf, uint16_t len);

#define I2C_restart       I2C_start
#define I2C_sendBuffer(addr,buf,len)  (I2C_start(addr), I2C_writeBuffer(buf,len))
#define I2C_getBuffer(addr,buf,len)   (I2C_start(addr), I2C_readBuffer(buf,len))

#ifdef __cplusplus
};
#endif
