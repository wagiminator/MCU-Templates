// ===================================================================================
// Basic DS3231/DS1307 Real-Time Clock Functions                              * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// DS_init()                init I2C (if DS_INIT_I2C is set) and start oscillator
// DS_read()                read date and time into DS_time
// DS_write()               write date and time from DS_time to the RTC
// DS_getTemp()             read temperature in 0.25°C ("100" means 25.00°C, DS3231 only)
//
// DS_time structure (binary values, not BCD):
// -------------------------------------------
// DS_time.second           0 - 59
// DS_time.minute           0 - 59
// DS_time.hour             0 - 23 (24-hour mode)
// DS_time.weekday          1 - 7 (user defined, e.g. 1 = Monday)
// DS_time.day              1 - 31
// DS_time.month            1 - 12
// DS_time.year             0 - 99 (2000 - 2099)
//
// Notes:
// ------
// - The DS1307 needs 5V supply and a 32.768kHz crystal, the DS3231 has an integrated
//   temperature compensated oscillator and works from 2.3V.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c.h"                      // choose your I2C library

// DS3231 parameters
#define DS_INIT_I2C           1       // 1: init I2C with DS_init()
#define DS_ADDR               0x68    // I2C address (DS3231 and DS1307)

// DS3231 date and time structure
typedef struct {
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t weekday;
  uint8_t day;
  uint8_t month;
  uint8_t year;
} DS_TIME_t;

extern DS_TIME_t DS_time;             // date and time

// DS3231 functions
void DS_init(void);                   // init I2C and start oscillator
void DS_read(void);                   // read date and time into DS_time
void DS_write(void);                  // write date and time from DS_time
int16_t DS_getTemp(void);             // read temperature in 0.25°C (DS3231 only)

#ifdef __cplusplus
}
#endif
