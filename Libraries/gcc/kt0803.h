// ===================================================================================
// Basic KT0803 K/L FM Transmitter Functions                                  * v1.0 *
// ===================================================================================
//
// Collection of the most necessary functions for controlling an KT0803 FM transmitter
// IC.
//
// Functions available:
// --------------------
// KT_init()              KT0802 init function (write default values)
// KT_getFreq()           Get current frequency (in 100kHz steps, 885 means 88.5Mhz)
// KT_setFreq(f)          Set frequency (in 100kHz steps, 885 means 88.5Mhz)
// KT_setGain(g)          Set gain (0: -12dB, ... , 3: 0dB, ... , 6: +12dB)
// KT_setMute(m)          Set mude (0: unmute, 1: mute)
// KT_setRegion(r)        Set region (0: USA/Japan, 1: Europe/Australia)
//
// 2023 by Stefan Wagner: https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c_soft.h"

// KT0802 I2C address
#define KT_I2C_ADDR         0b0111110

// Pre-emphasis time-constant depending on region
#define KT_USA_JAPAN        0
#define KT_EUROPE_AUSTRALIA 1

// Frequency range (in 100kHz)
#define KT_FREQ_MIN         875
#define KT_FREQ_MAX         1080

// KT0802 Functions
void KT_init(void);                   // KT0802 init function (write default values)
uint16_t KT_getFreq(void);            // Get current frequency (in 100kHz steps, 885 means 88.5Mhz)
void KT_setFreq(uint16_t freq);       // Set frequency (in 100kHz steps, 885 means 88.5Mhz)
void KT_setGain(uint8_t gain);        // Set gain (0: -12dB, ... , 3: 0dB, ... , 6: +12dB)
void KT_setMute(uint8_t mute);        // Set mude (0: unmute, 1: mute)
void KT_setRegion(uint8_t region);    // Set pre-emphasis time-constant depending on region

#ifdef __cplusplus
};
#endif
