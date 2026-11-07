// ===================================================================================
// Basic MAX31855/MAX6675 Thermocouple Interface Functions                    * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// THERMO_init()            init CS pin (and SPI if THERMO_INIT_SPI is set)
// THERMO_read()            read thermocouple temperature in 0.25°C ("100" = 25.00°C),
//                          returns THERMO_ERROR if thermocouple is open or shorted
//
// Notes:
// ------
// - MAX31855: K-type, -270°C .. +1800°C, conversion time 100ms.
// - MAX6675:  K-type, 0°C .. +1024°C, conversion time 220ms.
// - Reading during a conversion aborts it, so don't read faster than the conversion.
// - SPI: mode 0, max. 4-5MHz, only SCK and MISO (SO) are needed.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "gpio.h"
#include "spi.h"                      // choose your SPI library

// Thermocouple parameters
#define THERMO_PIN_CS         PA4     // pin connected to CS
#define THERMO_TYPE           31855   // 31855: MAX31855, 6675: MAX6675
#define THERMO_INIT_SPI       1       // 1: init SPI with THERMO_init()
#define THERMO_ERROR          -32768  // returned in case of error

// Thermocouple functions
void THERMO_init(void);               // init CS pin (and SPI)
int16_t THERMO_read(void);            // read temperature in 0.25°C

#ifdef __cplusplus
}
#endif
