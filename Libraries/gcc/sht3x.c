// ===================================================================================
// Basic SHT3x/SHT4x Temperature and Humidity Sensor Functions                * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "sht3x.h"

// SHT variables
int16_t  SHT_temp;                    // temperature in 0.01°C
uint16_t SHT_hum;                     // humidity in 0.01%RH

// Calculate CRC8 of two bytes (polynomial 0x31, init 0xFF) (internal)
static uint8_t SHT_crc(const uint8_t* ptr) {
  uint8_t i, j, crc = 0xFF;
  for(j = 2; j; j--) {
    crc ^= *ptr++;
    for(i = 8; i; i--) crc = (crc & 0x80) ? (crc << 1) ^ 0x31 : (crc << 1);
  }
  return crc;
}

// Init I2C
void SHT_init(void) {
  #if SHT_INIT_I2C > 0
  I2C_init();
  #endif
  DLY_ms(2);                                      // power-up time
}

// Start measurement, wait and read results, returns 0 on success
uint8_t SHT_read(void) {
  uint8_t  i, buf[6];
  uint16_t raw;
  I2C_start((uint8_t)(SHT_ADDR << 1));            // start measurement
  #if SHT_TYPE == 4
  I2C_write(0xFD);                                // high precision
  I2C_stop();
  DLY_ms(10);                                     // measurement time
  #else
  I2C_write(0x24); I2C_write(0x00);               // high repeatability, no stretching
  I2C_stop();
  DLY_ms(16);                                     // measurement time
  #endif
  I2C_start((uint8_t)((SHT_ADDR << 1) | 1));      // read results
  for(i = 0; i < 5; i++) buf[i] = I2C_read(1);
  buf[5] = I2C_read(0);
  I2C_stop();
  if(SHT_crc(buf) != buf[2] || SHT_crc(buf + 3) != buf[5]) return 1;

  raw = ((uint16_t)buf[0] << 8) | buf[1];         // T = -45 + 175 * raw / 65535
  SHT_temp = (int16_t)(((uint32_t)raw * 17500) >> 16) - 4500;
  raw = ((uint16_t)buf[3] << 8) | buf[4];
  #if SHT_TYPE == 4                               // RH = -6 + 125 * raw / 65535
  {
    int16_t h = (int16_t)(((uint32_t)raw * 12500) >> 16) - 600;
    if(h < 0) h = 0;
    if(h > 10000) h = 10000;
    SHT_hum = h;
  }
  #else                                           // RH = 100 * raw / 65535
  SHT_hum = ((uint32_t)raw * 10000) >> 16;
  #endif
  return 0;
}
