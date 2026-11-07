// ===================================================================================
// Basic I2S Functions (TX, master) for SAMD21                                * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// I2S_init()               Init I2S master transmitter (stereo, see parameters below)
// I2S_write(l, r)          Write one stereo sample (left, right), waits until ready
// I2S_ready()              Check if I2S is ready to write the next sample
// I2S_underrun()           Check if a transmit underrun occurred (and clear flag)
// I2S_enable()             Enable I2S (clock and serializer)
// I2S_disable()            Disable I2S
//
// I2S pin mapping (set below in I2S parameters):
// ----------------------------------------------
// I2S_MAP    0     1
// SCK-pin   PA10  PA20  (bit clock, BCLK)
// FS-pin    PA11  PA21  (frame sync, word select, LRCLK)
// SD-pin    PA7   PA19  (serial data out, DIN of the DAC)
// Device    EGJ   GJ
//
// Notes:
// ------
// - Standard I2S format (Philips), 16 or 32 bits per channel, two channels.
// - The bit clock is derived from the DFLL48M via generic clock generator GCLK5:
//   SCK = 48MHz / round(48MHz / (I2S_FS * 2 * I2S_BITS)), e.g. 16-bit at 44100Hz:
//   48MHz / 34 = 1.4118MHz -> 44118Hz. The DFLL48M must be running (F_CPU = 48, 24,
//   16, 12 or 6MHz).
// - Suitable for I2S DACs without master clock (e.g. MAX98357A, PCM5102A, UDA1334A).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// I2S parameters
#define I2S_FS        44100       // sample rate in Hz
#define I2S_BITS      16          // bits per channel (16 or 32)
#define I2S_MAP       0           // I2S pin mapping (see above)

// I2S pin mapping
#if   I2S_MAP == 0
  #define I2S_SCK_PIN PA10
  #define I2S_FS_PIN  PA11
  #define I2S_SD_PIN  PA7
#elif I2S_MAP == 1
  #define I2S_SCK_PIN PA20
  #define I2S_FS_PIN  PA21
  #define I2S_SD_PIN  PA19
  #if defined(SAMD21E)
    #error Pin mapping not available on this device
  #endif
#endif

#if CLK_DFLL == 0
  #error I2S requires the DFLL48M (F_CPU = 48, 24, 16, 12 or 6MHz)!
#endif

// I2S clock divider (GCLK5 = DFLL48M / I2S_DIV = bit clock)
#define I2S_DIV       ((DFLL48M_VALUE + I2S_FS * I2S_BITS) / (I2S_FS * 2 * I2S_BITS))
#if (I2S_DIV < 1) || (I2S_DIV > 255)
  #error Unsupported I2S sample rate
#endif

// I2S macros
#define I2S_sync()      while(I2S->SYNCBUSY.reg)
#define I2S_ready()     (I2S->INTFLAG.reg & I2S_INTFLAG_TXRDY0)
#define I2S_underrun()  ((I2S->INTFLAG.reg & I2S_INTFLAG_TXUR0) ? \
                         (I2S->INTFLAG.reg = I2S_INTFLAG_TXUR0, 1) : 0)

// I2S functions
void I2S_init(void);
void I2S_enable(void);
void I2S_disable(void);
#if I2S_BITS == 32
void I2S_write(int32_t left, int32_t right);
#else
void I2S_write(int16_t left, int16_t right);
#endif

#ifdef __cplusplus
};
#endif
