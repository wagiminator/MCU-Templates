// ===================================================================================
// Basic SHT3x/SHT4x Temperature and Humidity Sensor Functions                * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// SHT_init()               init I2C (if SHT_INIT_I2C is set)
// SHT_read()               start measurement, wait and read results (~15ms),
//                          returns 0 on success, 1 on error (no answer or CRC error)
// SHT_getTemp()            get temperature of last measurement (in 0.01°C,
//                          "2391" means 23.91°C)
// SHT_getHumidity()        get humidity of last measurement (in 0.01%RH,
//                          "4697" means 46.97%RH)
//
// Supported devices:
// ------------------
// SHT30, SHT31, SHT35 (SHT_TYPE 3), SHT40, SHT41, SHT43, SHT45 (SHT_TYPE 4)
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

// SHT parameters
#define SHT_INIT_I2C          1       // 1: init I2C with SHT_init()
#define SHT_ADDR              0x44    // SHT I2C device address (0x44 or 0x45)
#define SHT_TYPE              3       // 3: SHT3x, 4: SHT4x

// SHT variables
extern int16_t  SHT_temp;             // temperature in 0.01°C
extern uint16_t SHT_hum;              // humidity in 0.01%RH

// SHT functions
void SHT_init(void);                  // init I2C
uint8_t SHT_read(void);               // measure, returns 0 on success
#define SHT_getTemp()         (SHT_temp)
#define SHT_getHumidity()     (SHT_hum)

#ifdef __cplusplus
}
#endif
