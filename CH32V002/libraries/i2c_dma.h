// ===================================================================================
// I2C Master TX DMA Functions for CH32V002, V004, V005, V006, V007, M007     * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// I2C_init()               Init I2C with defined clock rate (see below)
// I2C_start(addr)          I2C start transmission, addr must contain R/W bit
// I2C_write(b)             I2C transmit one data byte via I2C
// I2C_read(ack)            I2C receive one data byte (set ack=0 for last byte)
// I2C_stop()               I2C stop transmission
// I2C_busy()               Check if I2C bus is busy transmitting
//
// I2C_sendBuffer(addr,buf,len) Send buffer (*buf) with length (len) to device (addr)
// I2C_getBuffer(addr,buf,len)  Receive buffer (*buf) with length (len) from (addr)
// I2C_writeBuffer(buf,len)     Write buffer (*buf) with length (len) via I2C and stop
// I2C_readBuffer(buf,len)      Read buffer (*buf) with length (len) via I2C and stop
//
// I2C pin mapping (set below in I2C parameters, value = AFIO_PCFR1 I2C1_RM):
// ---------------------------------------------------------------------------
// I2C_MAP    0     1     2     3     4
// SDA-pin   PC1   PD0   PC6*  PB6   PD1
// SCL-pin   PC2   PD1   PC5   PB5   PB3
//
// (*) CH32V007 and CH32M007 use PC4 as SDA for I2C_MAP 2 (set I2C_V007 to 1 below).
//
// Notes:
// ------
// - External pull-up resistors (4k7 - 10k) are mandatory!
// - Not all pins are available on every package/chip variant.
// - PD1 is also the SWIO (programming/debug) pin, PD7 is the reset pin (if enabled).
// - The mapping numbers differ from the CH32V003 library (there, the AFIO bits are
//   different and only three mappings exist). Here the number is written directly
//   to the I2C1_RM bits.
// - The I2C module clock (CTLR2.FREQ) must be between 8 and 48 MHz, so F_CPU must be
//   at least 8 MHz.
//
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// I2C Parameters
#define I2C_CLKRATE   400000    // I2C bus clock rate (Hz)
#define I2C_MAP       0         // I2C pin mapping (see above)
#define I2C_V007      0         // 1: CH32V007/CH32M007 (I2C_MAP 2 uses SDA = PC4)

// Interrupt enable check
#if SYS_USE_VECTORS == 0
  #error Interrupt vector table must be enabled (SYS_USE_VECTORS in system.h)!
#endif

// I2C Functions
void I2C_init(void);              // I2C init function
void I2C_start(uint8_t addr);     // I2C start transmission, addr must contain R/W bit
void I2C_stop(void);              // I2C stop transmission
void I2C_write(uint8_t data);     // I2C transmit one data byte via I2C
uint8_t I2C_read(uint8_t ack);    // I2C receive one data byte from the slave

void I2C_writeBuffer(uint8_t* buf, uint16_t len);
void I2C_readBuffer(uint8_t* buf, uint16_t len);

#define I2C_sendBuffer(addr,buf,len)  do{I2C_start(addr); I2C_writeBuffer(buf,len);}while(0)
#define I2C_getBuffer(addr,buf,len)   do{I2C_start(addr); I2C_readBuffer(buf,len);}while(0)
#define I2C_busy()                    (I2C1->STAR2 & I2C_STAR2_BUSY)

#ifdef __cplusplus
};
#endif
