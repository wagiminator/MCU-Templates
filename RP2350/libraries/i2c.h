// ===================================================================================
// Basic I2C Master Functions for RP2350 (I2C0/I2C1)                          * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// I2C_init()               Init I2C with defined clock rate (see below)
// I2C_start(addr)          I2C start transmission, addr must contain R/W bit
//                          (call again without stop for a repeated start)
// I2C_write(b)             I2C transmit one data byte via I2C
// I2C_read(ack)            I2C receive one data byte (set ack=0 for last byte)
// I2C_stop()               I2C stop transmission
// I2C_busy()               Check if I2C bus is busy
// I2C_nack()               Check if the last address or data byte was not acknowledged
//
// I2C_sendBuffer(addr,buf,len) Send buffer (*buf) with length (len) to device (addr)
// I2C_getBuffer(addr,buf,len)  Receive buffer (*buf) with length (len) from (addr)
// I2C_writeBuffer(buf,len)     Write buffer (*buf) with length (len) via I2C and stop
// I2C_readBuffer(buf,len)      Read buffer (*buf) with length (len) via I2C and stop
//
// I2C pins (set below in I2C parameters, SCL = SDA + 1):
// ------------------------------------------------------
// I2C0 SDA: GP0, GP4, GP8,  GP12, GP16, GP20, GP24, GP28
// I2C1 SDA: GP2, GP6, GP10, GP14, GP18, GP22, GP26
//
// Notes:
// ------
// - The I2C module (I2C0 or I2C1) is selected automatically by the SDA pin.
// - The I2C controller of the RP2350 sends the STOP or repeated START condition
//   together with a data byte. Therefore the last byte written is held back until
//   the next I2C_write(), I2C_start() or I2C_stop(). A repeated start must address the
//   same device (the target address can only be changed after a STOP).
// - The controller cannot send an address without data. I2C_start() followed directly
//   by I2C_stop() reads one dummy byte, so that I2C_nack() can be used to scan the bus.
// - If the slave does not acknowledge (NACK), the transfer is aborted (STOP), all
//   further writes are ignored and reads return 0xff until the next I2C_start().
// - I2C clock rates up to 400kHz use Standard/Fast-mode, above Fast-mode Plus (1MHz).
//
// External pull-up resistors (4k7 - 10k) are mandatory!
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// I2C parameters
#define I2C_SDA_PIN       GP4             // SDA pin (see above), SCL pin = SDA pin + 1
#define I2C_CLKRATE       400000          // I2C bus clock rate in Hz

// I2C instance (selected by SDA pin, evaluated by the compiler)
#define I2C_SCL_PIN       (I2C_SDA_PIN + 1)
#define I2C_NUM           ((I2C_SDA_PIN >> 1) & 1)
#define I2C_DEV           (I2C_NUM ? I2C1 : I2C0)
#define I2C_RESET         (I2C_NUM ? RESETS_RESET_I2C1 : RESETS_RESET_I2C0)
#define I2C_IRQn          (I2C_NUM ? I2C1_IRQ_IRQn : I2C0_IRQ_IRQn)

_Static_assert(((I2C_SDA_PIN & 1) == 0) && (I2C_SDA_PIN < GP29), "Wrong I2C SDA pin");

// I2C functions
void I2C_init(void);              // I2C init function
void I2C_start(uint8_t addr);     // I2C start transmission, addr must contain R/W bit
void I2C_stop(void);              // I2C stop transmission
void I2C_write(uint8_t data);     // I2C transmit one data byte via I2C
uint8_t I2C_read(uint8_t ack);    // I2C receive one data byte from the slave
uint8_t I2C_nack(void);           // check if last byte was not acknowledged

void I2C_writeBuffer(uint8_t* buf, uint16_t len);
void I2C_readBuffer(uint8_t* buf, uint16_t len);

#define I2C_sendBuffer(addr,buf,len)  (I2C_start(addr), I2C_writeBuffer(buf,len))
#define I2C_getBuffer(addr,buf,len)   (I2C_start(addr), I2C_readBuffer(buf,len))

#define I2C_busy()        (I2C_DEV->IC_STATUS & I2C_IC_STATUS_ACTIVITY)

#ifdef __cplusplus
};
#endif
