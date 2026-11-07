// ===================================================================================
// Basic AHT10/AHT20/AHT21/AHT25 Temperature and Humidity Sensor Functions    * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "aht20.h"

// AHT commands
#if AHT_TYPE == 10
  #define AHT_CMD_INIT        0xE1    // calibration/init command AHT10
#else
  #define AHT_CMD_INIT        0xBE    // calibration/init command AHT2x
#endif
#define AHT_CMD_MEASURE       0xAC    // trigger measurement

// AHT variables
int16_t  AHT_temp;                    // temperature in 0.01°C
uint16_t AHT_hum;                     // humidity in 0.01%RH

// Send command with two parameter bytes (internal)
static void AHT_command(uint8_t cmd, uint8_t p1) {
  I2C_start((uint8_t)(AHT_ADDR << 1));
  I2C_write(cmd);
  I2C_write(p1);
  I2C_write(0x00);
  I2C_stop();
}

// Read status byte (internal)
static uint8_t AHT_status(void) {
  uint8_t status;
  I2C_start((uint8_t)((AHT_ADDR << 1) | 1));
  status = I2C_read(0);
  I2C_stop();
  return status;
}

// Init sensor
void AHT_init(void) {
  #if AHT_INIT_I2C > 0
  I2C_init();
  #endif
  DLY_ms(40);                                     // power-on time
  if(!(AHT_status() & 0x08)) {                    // not calibrated?
    AHT_command(AHT_CMD_INIT, 0x08);              // -> load calibration
    DLY_ms(10);
  }
}

// Start measurement, wait and read results, returns 0 on success
uint8_t AHT_read(void) {
  uint8_t  i, buf[6];
  uint32_t raw;
  AHT_command(AHT_CMD_MEASURE, 0x33);             // trigger measurement
  DLY_ms(80);                                     // measurement time
  for(i = 10; (AHT_status() & 0x80) && i; i--) DLY_ms(5); // wait while busy
  I2C_start((uint8_t)((AHT_ADDR << 1) | 1));      // read status and data
  for(i = 0; i < 5; i++) buf[i] = I2C_read(1);
  buf[5] = I2C_read(0);
  I2C_stop();
  if(buf[0] & 0x80) return 1;                     // still busy -> error

  raw = ((uint32_t)buf[1] << 12) | ((uint16_t)buf[2] << 4) | (buf[3] >> 4);
  AHT_hum  = (raw * 625) >> 16;                   // raw * 10000 / 2^20
  raw = ((uint32_t)(buf[3] & 0x0F) << 16) | ((uint16_t)buf[4] << 8) | buf[5];
  AHT_temp = (int16_t)((raw * 625) >> 15) - 5000; // raw * 20000 / 2^20 - 5000
  return 0;
}
