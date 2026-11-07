// ===================================================================================
// Basic MCP4725 12-Bit DAC Functions                                         * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "mcp4725.h"

// Init I2C
void MCP_init(void) {
  #if MCP_INIT_I2C > 0
  I2C_init();
  #endif
}

// Set output value (fast mode write)
void MCP_write(uint16_t value) {
  if(value > 4095) value = 4095;
  I2C_start((uint8_t)(MCP_ADDR << 1));
  I2C_write(value >> 8);                          // fast mode, normal power
  I2C_write(value);
  I2C_stop();
}

// Set output value and store it in EEPROM
void MCP_writeEEPROM(uint16_t value) {
  if(value > 4095) value = 4095;
  I2C_start((uint8_t)(MCP_ADDR << 1));
  I2C_write(0x60);                                // write DAC register and EEPROM
  I2C_write(value >> 4);
  I2C_write(value << 4);
  I2C_stop();
  DLY_ms(50);                                     // EEPROM write time
}
