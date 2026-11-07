// ===================================================================================
// Basic BH1750 Ambient Light Sensor Functions                                * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "bh1750.h"

// BH1750 commands
#define BH_CMD_POWER_DOWN     0x00    // power down
#define BH_CMD_POWER_ON       0x01    // power on
#define BH_CMD_CONT_HRES      0x10    // continuous high resolution mode (1 lx)

// Send command (internal)
static void BH_command(uint8_t cmd) {
  I2C_start((uint8_t)(BH_ADDR << 1));
  I2C_write(cmd);
  I2C_stop();
}

// Init sensor in continuous high resolution mode
void BH_init(void) {
  #if BH_INIT_I2C > 0
  I2C_init();
  #endif
  BH_sleep(0);
}

// Set sleep state (0: wake up, 1: power down)
void BH_sleep(uint8_t slp) {
  if(slp) BH_command(BH_CMD_POWER_DOWN);
  else {
    BH_command(BH_CMD_POWER_ON);
    BH_command(BH_CMD_CONT_HRES);
  }
}

// Read illuminance in lx
uint16_t BH_getLux(void) {
  uint16_t raw;
  I2C_start((uint8_t)((BH_ADDR << 1) | 1));
  raw  = (uint16_t)I2C_read(1) << 8;
  raw |= I2C_read(0);
  I2C_stop();
  return ((uint32_t)raw * 5) / 6;                 // lx = raw / 1.2
}
