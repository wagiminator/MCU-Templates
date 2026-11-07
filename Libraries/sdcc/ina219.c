// ===================================================================================
// Basic INA219 Bidirectional Current/Power Monitor Functions                 * v1.1 *
// ===================================================================================
// 2019 by Stefan Wagner:   https://github.com/wagiminator

#include "ina219.h"

// INA219 write a register value
void INA_write(uint8_t reg, uint16_t value) {
  I2C_start((uint8_t)(INA_ADDR << 1));            // start transmission to INA219
  I2C_write(reg);                                 // write register address
  I2C_write(value >> 8);                          // write register content high byte
  I2C_write(value);                               // write register content low  byte
  I2C_stop();                                     // stop transmission
}

// INA219 read a register
uint16_t INA_read(uint8_t reg) {
  uint16_t result;                                // result variable
  I2C_start((uint8_t)(INA_ADDR << 1));            // start transmission to INA219
  I2C_write(reg);                                 // write register address
  I2C_stop();                                     // stop transmission
  I2C_start((uint8_t)((INA_ADDR << 1) | 1));      // start for reading
  result  = (uint16_t)I2C_read(1) << 8;           // read register content high byte
  result |= I2C_read(0);                          // read register content low byte
  I2C_stop();                                     // stop transmission
  return result;                                  // return result
}

// INA219 write inital configuration and calibration values
void INA_init(void) {
  #if INA_INIT_I2C > 0
  I2C_init();                                     // initialize I2C first
  #endif
  INA_write(INA_REG_CONFIG, INA_CONFIG);          // write INA219 configuration
  INA_write(INA_REG_CALIB,  INA_CALIB);           // write INA219 calibration
}

// INA219 read voltage in mV
uint16_t INA_readVoltage(void) {
  return((INA_read(INA_REG_VOLTAGE) >> 1) & 0xFFFC);
}

// INA219 read current in mA
int16_t INA_readCurrent(void) {
  return (int16_t)INA_read(INA_REG_CURRENT);
}

// INA219 read power in mW
uint32_t INA_readPower(void) {
  return (uint32_t)INA_read(INA_REG_POWER) * 20;
}
