// ===================================================================================
// Basic I2C Slave Functions Functions for CH32V003                           * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// I2C_init()               init I2C slave device with parameters set below
// I2C_busy()               check if I2C communication is currently on-going
// I2C_changed()            check if something was changed in the slave registers
// I2C_clear()              clear the changed flag
// I2C_REG[]                slave register array
//
// I2C pin mapping (set below in I2C parameters):
// ----------------------------------------------
// I2C_MAP    0     1     2
// SDA-pin   PC1   PD0   PC6
// SCL-pin   PC2   PD1   PC5
//
// External pull-up resistors (4k7 - 10k) are mandatory!
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// I2C Parameters
#ifndef I2C_MAP
  #define I2C_MAP       0           // I2C pin mapping (see above)
#endif
#ifndef I2C_ADDR
  #define I2C_ADDR      0x36        // I2C address of the device
#endif
#ifndef I2C_REG_SIZE
  #define I2C_REG_SIZE  10          // number of 8-bit registers
#endif

#define I2C_CLKRATE     400000      // I2C bus clock rate (Hz)

#define I2C_REG_SLCT    0           // 0: transmission will always start with register 0
                                    // 1: first byte written is register address (pointer)

// Interrupt enable check
#if SYS_USE_VECTORS == 0
  #error Interrupt vector table must be enabled (SYS_USE_VECTORS in system.h)!
#endif

// I2C Slave Registers
extern volatile uint8_t I2C_REG[];        // slave register array
extern volatile uint8_t I2C_FLAG_changed; // slave register change flag

// I2C Slave Functions and Macros
void I2C_init(void);                // I2C slave init function

#define I2C_busy()      (I2C1->STAR2 & I2C_STAR2_BUSY)
#define I2C_changed()   (I2C_FLAG_changed)
#define I2C_clear()     I2C_FLAG_changed = 0

#ifdef __cplusplus
};
#endif
