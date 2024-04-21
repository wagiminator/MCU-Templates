// ===================================================================================
// Basic INA219 Bidirectional Current/Power Monitor Functions                 * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// INA_init()               write inital configuration and calibration
// INA_readVoltage()        read voltage in mV
// INA_readCurrent()        read current in mA
//
// 2019 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c_soft.h"

// INA219 register values for 10 mOhms shunt resistor
#define INA_ADDR        0x80                      // I2C write address of INA219
#define INA_CONFIG      0b0010011111111111        // INA config register according to datasheet
#define INA_CALIB       4096                      // INA calibration register according to R_SHUNT
#define INA_REG_CONFIG  0x00                      // INA configuration register address
#define INA_REG_CALIB   0x05                      // INA calibration register address
#define INA_REG_SHUNT   0x01                      // INA shunt voltage register address
#define INA_REG_VOLTAGE 0x02                      // INA bus voltage register address
#define INA_REG_POWER   0x03                      // INA power register address
#define INA_REG_CURRENT 0x04                      // INA current register address

// INA219 functions
void INA_init(void);                              // write inital configuration and calibration
uint16_t INA_read(uint8_t reg);                   // read a register
void INA_write(uint8_t reg, uint16_t value);      // write a register value
uint16_t INA_readVoltage(void);                   // read voltage in mV
int16_t INA_readCurrent(void);                    // read current in mA

#ifdef __cplusplus
};
#endif
