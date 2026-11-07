// ===================================================================================
// Basic BH1750 Ambient Light Sensor Functions                                * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// BH_init()                init sensor in continuous high resolution mode (1 lx)
// BH_getLux()              read illuminance in lx (0 - 54612)
// BH_sleep(s)              set sleep state (0: wake up, 1: power down)
//
// Notes:
// ------
// - A new value is available every 120ms (the first one 180ms after BH_init()).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c.h"                      // choose your I2C library

// BH1750 parameters
#define BH_INIT_I2C           1       // 1: init I2C with BH_init()
#define BH_ADDR               0x23    // BH1750 I2C address (0x23: ADDR low, 0x5C: high)

// BH1750 functions
void BH_init(void);                   // init sensor
uint16_t BH_getLux(void);             // read illuminance in lx
void BH_sleep(uint8_t slp);           // set sleep state

#ifdef __cplusplus
}
#endif
