// ===================================================================================
// Basic BME280 Temperature, Pressure and Humidity Sensor Functions           * v1.1 *
// ===================================================================================
//
// Functions available:
// --------------------
// BME_init()               Init BME280
// BME_sleep(s)             Set BME280 sleep state (0: wake up, 1: sleep)
// BME_getTemp()            Read temperature (value "2391" means 23.91 °C)
// BME_getPressure()        Read pressure (value "101945" means 1019.45 hPa)
// BME_getHumidity()        Read humidity (value "4697" means 46.97 %RH)
// BME_adjustTemp(a)        Set user temperature adjustment ("-127" means -1.27 °C)
//
// Notes:
// ------
// - BME_getPressure() and BME_getHumidity() use the fine temperature of the last
//   BME_getTemp() call, so call BME_getTemp() first.
// - Also works with BMP280 (without humidity).
//
// References:
// -----------
// Adafruit:                https://github.com/adafruit/Adafruit_BME280_Library
// David Johnson-Davies:    https://github.com/technoblogy/tiny-bme280
//
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c.h"                      // choose your I2C library
#include "system.h"
#ifdef __SDCC
#include "delay.h"
#endif

// BME280 Parameters
#define BME_INIT_I2C          1       // 1: init I2C with BME_init()
#define BME_INIT_RESET        0       // 1: reset BME280 with BME_init()
#define BME_INIT_TEMP         0       // 1: make first temp read with BME_init() for calib
#define BME_BOOT_TIME         0       // BME280 boot up time in milliseconds
#define BME_ADDR              0x76    // BME280 I2C device address (0x76 or 0x77)

// BME280 Calibration Data Registers
#define BME_REG_CALIB_T1      0x88    // temperature/pressure (0x88..0x9F), H1 (0xA1)
#define BME_REG_CALIB_H2      0xE1    // humidity (0xE1..0xE7)

// BME280 Control Registers
#define BME_REG_CHIPID        0xD0
#define BME_REG_SOFTRESET     0xE0
#define BME_REG_CONTROLHUMID  0xF2
#define BME_REG_STATUS        0xF3
#define BME_REG_CONTROL       0xF4
#define BME_REG_CONFIG        0xF5

// BME280 Data Registers
#define BME_REG_PRESSUREDATA  0xF7
#define BME_REG_TEMPDATA      0xFA
#define BME_REG_HUMIDDATA     0xFD

// BME280 Functions
void BME_init(void);
void BME_sleep(uint8_t slp);
int32_t BME_getTemp(void);
uint32_t BME_getPressure(void);
uint32_t BME_getHumidity(void);
void BME_adjustTemp(int32_t adjust);

#ifdef __cplusplus
}
#endif
