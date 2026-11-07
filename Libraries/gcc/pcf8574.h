// ===================================================================================
// Basic PCF8574/PCF8574A 8-Bit I/O Expander Functions                        * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// PCF_init()               init I2C (if PCF_INIT_I2C is set) and set all pins HIGH
// PCF_write(v)             write all 8 pins
// PCF_read()               read all 8 pins
// PCF_high(p)              set pin p (0 - 7) HIGH (also used as input)
// PCF_low(p)               set pin p (0 - 7) LOW
// PCF_toggle(p)            toggle pin p (0 - 7)
// PCF_readPin(p)           read pin p (0 - 7), returns 0 or 1
//
// Notes:
// ------
// - The pins are quasi-bidirectional: a pin set HIGH has a weak pull-up and can be
//   used as input. Only LOW is actively driven.
// - The output state is kept in PCF_state.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c.h"                      // choose your I2C library

// PCF8574 parameters
#define PCF_INIT_I2C          1       // 1: init I2C with PCF_init()
#define PCF_ADDR              0x20    // I2C address (PCF8574: 0x20-0x27, PCF8574A: 0x38-0x3F)

// PCF8574 variables
extern uint8_t PCF_state;             // current output state

// PCF8574 functions
void PCF_init(void);                  // init I2C, all pins HIGH
void PCF_write(uint8_t value);        // write all pins
uint8_t PCF_read(void);               // read all pins
#define PCF_high(p)           PCF_write(PCF_state |  (1 << (p)))
#define PCF_low(p)            PCF_write(PCF_state & ~(1 << (p)))
#define PCF_toggle(p)         PCF_write(PCF_state ^  (1 << (p)))
#define PCF_readPin(p)        ((PCF_read() >> (p)) & 1)

#ifdef __cplusplus
}
#endif
