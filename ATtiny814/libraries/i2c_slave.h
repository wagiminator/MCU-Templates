// ===================================================================================
// Basic I2C Slave Functions for tinyAVR 0-Series and 1-Series                * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// I2C_init()               init I2C slave device with parameters set below
// I2C_busy()               check if I2C is currently receiving new register values
// I2C_changed()            check if something was changed in the slave registers
// I2C_clear()              clear the changed flag
// I2C_REG[]                slave register array
//
// I2C pin mapping (set below in I2C parameters):
// ----------------------------------------------
// I2C_MAP   0     1     2
// SDA-pin  PB1   PA1   No mapping
// SCL-pin  PB0   PA2   No mapping
//
// External pull-up resistors (4k7 - 10k) are mandatory!
// 2021 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// I2C Slave Parameters
#define I2C_MAP         0                 // I2C pin mapping (see above)
#define I2C_ADDR        0x36              // I2C address of the device
#define I2C_REG_SIZE    10                // number of 8-bit registers
#define I2C_REG_SLCT    0                 // 0: transmission will always start with register 0
                                          // 1: first byte written is register address (pointer)

// I2C Slave Registers
extern volatile uint8_t I2C_REG[];        // slave register array
extern volatile uint8_t I2C_FLAG_changed; // slave register change flag
extern volatile uint8_t I2C_FLAG_busy;    // I2C busy flag

// I2C Slave Functions and Macros
void I2C_init(void);                      // I2C slave init function

#define I2C_busy()      (I2C_FLAG_busy)
#define I2C_changed()   (I2C_FLAG_changed)
#define I2C_clear()     I2C_FLAG_changed = 0

#ifdef __cplusplus
};
#endif
