// ===================================================================================
// Basic HX711 24-Bit ADC for Load Cells Functions                            * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// HX_init()                init pins
// HX_ready()               check if a new conversion result is available
// HX_read()                wait for and read raw value (signed 24-bit), returns
//                          HX_ERROR on timeout (no HX711 connected)
// HX_tare(n)               average n readings and use the result as zero offset
// HX_get()                 read value minus zero offset
// HX_sleep(s)              set sleep state (0: wake up, 1: power down)
//
// Notes:
// ------
// - Gain/channel is set with HX_GAIN and is applied from the next conversion on.
// - Conversion rate is 10 SPS (RATE pin low) or 80 SPS (RATE pin high).
// - The scale factor (raw value per gram) must be determined with a known weight.
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

// HX711 parameters
#define HX_PIN_DOUT           P14     // pin connected to DOUT
#define HX_PIN_SCK            P15     // pin connected to PD_SCK
#define HX_GAIN               128     // 128: channel A x128, 64: channel A x64,
                                      // 32: channel B x32
#define HX_ERROR              ((int32_t)0x80000000)

// HX711 variables
extern int32_t HX_offset;             // zero offset

// HX711 functions
void HX_init(void);                   // init pins
int32_t HX_read(void);                // read raw value
void HX_tare(uint8_t n);              // set zero offset
void HX_sleep(uint8_t slp);           // set sleep state
#define HX_ready()            (!PIN_read(HX_PIN_DOUT))
#define HX_get()              (HX_read() - HX_offset)

#ifdef __cplusplus
}
#endif
