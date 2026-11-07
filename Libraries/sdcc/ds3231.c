// ===================================================================================
// Basic DS3231/DS1307 Real-Time Clock Functions                              * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "ds3231.h"

DS_TIME_t DS_time;                    // date and time

// Set register pointer and start reading (internal)
static void DS_readStart(uint8_t reg) {
  I2C_start((uint8_t)(DS_ADDR << 1));
  I2C_write(reg);
  I2C_stop();
  I2C_start((uint8_t)((DS_ADDR << 1) | 1));
}

// BCD conversion (internal)
static uint8_t DS_fromBCD(uint8_t v) {
  return (v >> 4) * 10 + (v & 0x0F);
}

static uint8_t DS_toBCD(uint8_t v) {
  return ((v / 10) << 4) | (v % 10);
}

// Init I2C and start oscillator
void DS_init(void) {
  uint8_t sec;
  #if DS_INIT_I2C > 0
  I2C_init();
  #endif
  DS_readStart(0x00);                             // read seconds register
  sec = I2C_read(0);
  I2C_stop();
  if(sec & 0x80) {                                // DS1307: clock halted?
    I2C_start((uint8_t)(DS_ADDR << 1));           // -> start oscillator
    I2C_write(0x00);
    I2C_write(sec & 0x7F);
    I2C_stop();
  }
}

// Read date and time into DS_time
void DS_read(void) {
  uint8_t i;
  uint8_t* ptr = (uint8_t*)&DS_time;
  DS_readStart(0x00);
  for(i = 0; i < 7; i++) *ptr++ = I2C_read(i < 6);
  I2C_stop();
  DS_time.second  = DS_fromBCD(DS_time.second & 0x7F);
  DS_time.minute  = DS_fromBCD(DS_time.minute & 0x7F);
  if(DS_time.hour & 0x40) {                       // 12-hour mode?
    i = DS_fromBCD(DS_time.hour & 0x1F) % 12;
    if(DS_time.hour & 0x20) i += 12;              // PM
    DS_time.hour = i;
  }
  else DS_time.hour = DS_fromBCD(DS_time.hour & 0x3F);
  DS_time.weekday &= 0x07;
  DS_time.day     = DS_fromBCD(DS_time.day & 0x3F);
  DS_time.month   = DS_fromBCD(DS_time.month & 0x1F);
  DS_time.year    = DS_fromBCD(DS_time.year);
}

// Write date and time from DS_time
void DS_write(void) {
  uint8_t i;
  uint8_t* ptr = (uint8_t*)&DS_time;
  I2C_start((uint8_t)(DS_ADDR << 1));
  I2C_write(0x00);
  for(i = 0; i < 7; i++) {
    if(i == 3) I2C_write(*ptr++);                 // weekday (binary = BCD)
    else       I2C_write(DS_toBCD(*ptr++));       // 24-hour mode, oscillator enabled
  }
  I2C_stop();
}

// Read temperature in 0.25°C (DS3231 only)
int16_t DS_getTemp(void) {
  int16_t temp;
  DS_readStart(0x11);
  temp = (int16_t)((int8_t)I2C_read(1)) << 2;
  temp |= I2C_read(0) >> 6;
  I2C_stop();
  return temp;
}
