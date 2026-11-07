// ===================================================================================
// Basic DHT11/DHT22 (AM2302) Temperature and Humidity Sensor Functions       * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// DHT_init()               init data pin
// DHT_read()               read sensor (~25ms), returns 0 on success, 1 on error
// DHT_getTemp()            get temperature of last reading (in 0.1°C, "239" = 23.9°C)
// DHT_getHumidity()        get humidity of last reading (in 0.1%RH, "469" = 46.9%RH)
//
// Notes:
// ------
// - The data line needs a pull-up resistor (4.7k - 10kOhm) to VCC.
// - Wait at least 1s (DHT11) or 2s (DHT22) between two readings and after power-up.
// - The bit timing is measured with a polling loop (each high phase is compared with
//   the preceding 50us low phase, so the result does not depend on F_CPU).
// - Interrupts during the reading can corrupt the data, set DHT_INT_LOCK to 1 to
//   disable interrupts while reading.
// - Requires F_CPU >= 4MHz.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "gpio.h"
#include "system.h"
#ifdef __SDCC
#include "delay.h"
#endif

// DHT parameters
#define DHT_PIN               P14     // pin connected to DHT data line
#define DHT_TYPE              22      // 11: DHT11, 22: DHT22/AM2302
#define DHT_INT_LOCK          0       // 1: disable interrupts while reading

// DHT variables
extern int16_t  DHT_temp;             // temperature in 0.1°C
extern uint16_t DHT_hum;              // humidity in 0.1%RH

// DHT functions
void DHT_init(void);                  // init data pin
uint8_t DHT_read(void);               // read sensor, returns 0 on success
#define DHT_getTemp()         (DHT_temp)
#define DHT_getHumidity()     (DHT_hum)

#ifdef __cplusplus
}
#endif
