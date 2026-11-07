// ===================================================================================
// Basic ADS1115/ADS1015 16/12-Bit ADC Functions                              * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// ADS_init()               init I2C (if ADS_INIT_I2C is set)
// ADS_read(ch)             single-shot conversion of input ch, returns signed raw
//                          value (-32768 .. 32767, ADS1015: lowest 4 bits are 0)
// ADS_readMV(ch)           single-shot conversion of input ch, returns millivolts
//
// Inputs (ch):
// ------------
// ADS_AIN0 .. ADS_AIN3     single-ended inputs AIN0 .. AIN3 against GND
// ADS_DIFF01               differential AIN0 - AIN1
// ADS_DIFF03               differential AIN0 - AIN3
// ADS_DIFF13               differential AIN1 - AIN3
// ADS_DIFF23               differential AIN2 - AIN3
//
// Notes:
// ------
// - ADS_PGA sets the full-scale range for all channels (see below). The input voltage
//   must never exceed VDD + 0.3V, regardless of the range.
// - A conversion takes about 8ms (ADS1115 at 128 SPS) or 1ms (ADS1015).
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

// ADS parameters
#define ADS_INIT_I2C          1       // 1: init I2C with ADS_init()
#define ADS_ADDR              0x48    // I2C address (ADDR pin: GND 0x48, VDD 0x49,
                                      //              SDA 0x4A, SCL 0x4B)
#define ADS_PGA               1       // full-scale range: 0: +-6.144V, 1: +-4.096V,
                                      // 2: +-2.048V, 3: +-1.024V, 4: +-0.512V, 5: +-0.256V

// ADS inputs
#define ADS_DIFF01            0
#define ADS_DIFF03            1
#define ADS_DIFF13            2
#define ADS_DIFF23            3
#define ADS_AIN0              4
#define ADS_AIN1              5
#define ADS_AIN2              6
#define ADS_AIN3              7

// ADS functions
void ADS_init(void);                  // init I2C
int16_t ADS_read(uint8_t ch);         // read raw value
int16_t ADS_readMV(uint8_t ch);       // read millivolts

#ifdef __cplusplus
}
#endif
