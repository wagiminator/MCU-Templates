// ===================================================================================
// Basic MPU-6050 Accelerometer and Gyroscope Functions                       * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// IMU_init()               init sensor (wake up, set ranges and filter)
// IMU_read()               read all sensor values into the variables below
// IMU_getTemp()            get chip temperature of last reading in 0.01°C
// IMU_sleep(s)             set sleep state (0: wake up, 1: sleep)
//
// Variables (raw values of last IMU_read()):
// ------------------------------------------
// IMU_accel[3]             acceleration X, Y, Z (16384 LSB/g at +-2g, see below)
// IMU_gyro[3]              angular rate X, Y, Z (131 LSB/°/s at +-250°/s, see below)
// IMU_temp                 raw temperature value
//
// Sensitivity depending on range settings:
// ----------------------------------------
// IMU_ACCEL_RANGE   0: +-2g 16384 LSB/g, 1: +-4g 8192, 2: +-8g 4096, 3: +-16g 2048
// IMU_GYRO_RANGE    0: +-250°/s 131 LSB/(°/s), 1: +-500°/s 65.5, 2: +-1000°/s 32.8,
//                   3: +-2000°/s 16.4
//
// Notes:
// ------
// - Also works with MPU-6500 and MPU-9250 (accelerometer, gyroscope, temperature
//   formula differs slightly).
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

// IMU parameters
#define IMU_INIT_I2C          1       // 1: init I2C with IMU_init()
#define IMU_ADDR              0x68    // I2C address (0x68: AD0 low, 0x69: AD0 high)
#define IMU_ACCEL_RANGE       0       // accelerometer range (see above)
#define IMU_GYRO_RANGE        0       // gyroscope range (see above)
#define IMU_DLPF              3       // digital low-pass filter (0: 260Hz ... 6: 5Hz)

// IMU variables
extern int16_t IMU_accel[3];          // acceleration X, Y, Z
extern int16_t IMU_gyro[3];           // angular rate X, Y, Z
extern int16_t IMU_temp;              // raw temperature

// IMU functions
void IMU_init(void);                  // init sensor
void IMU_read(void);                  // read all sensor values
void IMU_sleep(uint8_t slp);          // set sleep state
#define IMU_getTemp()     ((int16_t)(((int32_t)IMU_temp * 100) / 340) + 3653)

#ifdef __cplusplus
}
#endif
