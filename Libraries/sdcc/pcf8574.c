// ===================================================================================
// Basic PCF8574/PCF8574A 8-Bit I/O Expander Functions                        * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "pcf8574.h"

uint8_t PCF_state = 0xFF;             // current output state

// Init I2C and set all pins HIGH
void PCF_init(void) {
  #if PCF_INIT_I2C > 0
  I2C_init();
  #endif
  PCF_write(0xFF);
}

// Write all pins
void PCF_write(uint8_t value) {
  PCF_state = value;
  I2C_start((uint8_t)(PCF_ADDR << 1));
  I2C_write(value);
  I2C_stop();
}

// Read all pins
uint8_t PCF_read(void) {
  uint8_t value;
  I2C_start((uint8_t)((PCF_ADDR << 1) | 1));
  value = I2C_read(0);
  I2C_stop();
  return value;
}
