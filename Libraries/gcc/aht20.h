// ===================================================================================
// Basic AHT10/AHT20/AHT21/AHT25 Temperature and Humidity Sensor Functions    * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// AHT_init()               init sensor (and I2C if AHT_INIT_I2C is set)
// AHT_read()               start measurement, wait and read results (~80ms),
//                          returns 0 on success, 1 on error
// AHT_getTemp()            get temperature of last measurement (in 0.01°C,
//                          "2391" means 23.91°C)
// AHT_getHumidity()        get humidity of last measurement (in 0.01%RH,
//                          "4697" means 46.97%RH)
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c.h"                      // choose your I2C library
#include "system.h"
#ifdef __SDCC
#include "delay.h"
#endif

// AHT parameters
#define AHT_INIT_I2C          1       // 1: init I2C with AHT_init()
#define AHT_ADDR              0x38    // AHT I2C device address
#define AHT_TYPE              20      // 10: AHT10, 20: AHT20/AHT21/AHT25

// AHT variables
extern int16_t  AHT_temp;             // temperature in 0.01°C
extern uint16_t AHT_hum;              // humidity in 0.01%RH

// AHT functions
void AHT_init(void);                  // init sensor
uint8_t AHT_read(void);               // measure, returns 0 on success
#define AHT_getTemp()         (AHT_temp)
#define AHT_getHumidity()     (AHT_hum)

#ifdef __cplusplus
}
#endif
