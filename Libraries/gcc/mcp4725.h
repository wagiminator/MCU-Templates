// ===================================================================================
// Basic MCP4725 12-Bit DAC Functions                                         * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// MCP_init()               init I2C (if MCP_INIT_I2C is set)
// MCP_write(v)             set output value (0 - 4095), output = VDD * v / 4096
// MCP_writeEEPROM(v)       set output value and store it as power-on value in EEPROM
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c.h"                      // choose your I2C library
#include "system.h"
#ifdef __SDCC
#include "delay.h"
#endif

// MCP4725 parameters
#define MCP_INIT_I2C          1       // 1: init I2C with MCP_init()
#define MCP_ADDR              0x60    // I2C address (0x60 - 0x67, depending on version/A0)

// MCP4725 functions
void MCP_init(void);                  // init I2C
void MCP_write(uint16_t value);       // set output value (0 - 4095)
void MCP_writeEEPROM(uint16_t value); // set output value and store it in EEPROM

#ifdef __cplusplus
}
#endif
