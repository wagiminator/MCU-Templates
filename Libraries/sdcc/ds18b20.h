// ===================================================================================
// Basic DS18B20 1-Wire Temperature Sensor Functions                          * v1.0 *
// ===================================================================================
//
// Functions for a single DS18B20 (or DS18S20/DS1822) on the 1-Wire bus.
// Needs the onewire library (set the pin in onewire.h).
//
// Functions available:
// --------------------
// DS18_init()              init 1-Wire bus, returns 1 if sensor was found
// DS18_start()             start temperature conversion (takes up to 750ms)
// DS18_ready()             check if conversion is finished
// DS18_read()              read temperature after conversion (in 0.01°C, "2391" means
//                          23.91°C), returns DS18_ERROR if sensor is missing or CRC fails
// DS18_getTemp()           start conversion, wait and read temperature (blocking)
//
// Notes:
// ------
// - Only one sensor per bus is supported (SKIP ROM addressing).
// - Resolution is 12 bits (0.0625°C) as default.
// - Parasite power mode is not supported, connect VDD of the sensor.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "onewire.h"

// DS18B20 parameters
#define DS18_ERROR        -32768    // returned in case of error

// DS18B20 functions
uint8_t DS18_init(void);            // init 1-Wire bus, returns 1 if sensor was found
uint8_t DS18_start(void);           // start temperature conversion
int16_t DS18_read(void);            // read temperature in 0.01°C
int16_t DS18_getTemp(void);         // start, wait and read temperature in 0.01°C
#define DS18_ready()      OW_readBit()

#ifdef __cplusplus
}
#endif
