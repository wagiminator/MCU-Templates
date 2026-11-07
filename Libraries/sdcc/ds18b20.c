// ===================================================================================
// Basic DS18B20 1-Wire Temperature Sensor Functions                          * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "ds18b20.h"

// DS18B20 commands
#define DS18_CMD_CONVERT  0x44      // start temperature conversion
#define DS18_CMD_READ     0xBE      // read scratchpad

// Init 1-Wire bus, returns 1 if sensor was found
uint8_t DS18_init(void) {
  OW_init();
  return OW_reset();
}

// Start temperature conversion, returns 0 if sensor is missing
uint8_t DS18_start(void) {
  if(!OW_reset()) return 0;
  OW_skip();
  OW_write(DS18_CMD_CONVERT);
  return 1;
}

// Read temperature after conversion in 0.01°C
int16_t DS18_read(void) {
  uint8_t i, buf[9];
  int16_t raw;
  if(!OW_reset()) return DS18_ERROR;
  OW_skip();
  OW_write(DS18_CMD_READ);
  for(i = 0; i < 9; i++) buf[i] = OW_read();     // read scratchpad incl. CRC
  if(OW_crc8(buf, 9) || !(buf[4] | buf[5] | buf[6] | buf[7])) return DS18_ERROR;
  raw = (int16_t)(((uint16_t)buf[1] << 8) | buf[0]);
  if(buf[4] == 0xFF)                              // DS18S20 (9-bit, 0.5°C)?
    raw = (int16_t)((raw & 0xFFFE) << 3) - 4 + 16 - buf[6]; // extended resolution
  return (int16_t)(((int32_t)raw * 25) >> 2);    // 1/16°C -> 1/100°C
}

// Start conversion, wait and read temperature in 0.01°C (blocking)
int16_t DS18_getTemp(void) {
  uint8_t timeout = 100;
  if(!DS18_start()) return DS18_ERROR;
  while(!DS18_ready() && --timeout) DLY_ms(10);  // wait for conversion (max. 1s)
  return DS18_read();
}
