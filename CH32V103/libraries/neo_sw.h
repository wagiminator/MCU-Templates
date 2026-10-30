// ===================================================================================
// Basic NeoPixel Functions using Software Bit-banging for CH32V103           * v1.0 *
// ===================================================================================
//
// Basic control functions for 800kHz addressable LEDs (NeoPixel). A simplified 
// protocol is used which should work with most LEDs.
//
// Functions available:
// --------------------
// NEO_init()               init NeoPixels
// NEO_clearAll()           clear all pixels and update
// NEO_clearPixel(p)        clear pixel p
// NEO_writeColor(p,r,g,b)  write RGB color to pixel p
// NEO_writeHue(p,h,b)      write hue (h=0..191) and brightness (b=0..2) to pixel p
// NEO_update()             update pixels string (write buffer to pixels)
//
// NEO_sendByte(d)          send one data byte to pixels string
// NEO_latch()              latch the data sent
//
// Notes:
// ------
// - Works with most 800kHz addressable LEDs (NeoPixels).
// - Set pin, number of pixels, and pixel type in the parameters below!
// - The SysTick of the QingKe V3A core only runs with F_CPU/8, which is too coarse.
//   Therefore the timing is derived from the counter of a general-purpose timer
//   (NEO_TIM: 2, 3 or 4, running with F_CPU), which is used during NEO_update()
//   only. This makes it independent of flash wait states and compiler optimization.
// - System clock frequency must be at least 48MHz (polling loop ~8 cycles).
// - Interrupts are disabled during NEO_update().
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "gpio.h"

// ===================================================================================
// NeoPixel Definitions
// ===================================================================================
#ifndef PIN_NEO
  #define PIN_NEO       PA0   // define NeoPixel pin
#endif

#ifndef NEO_TIM
  #define NEO_TIM       2     // timer used for timing (2, 3 or 4, TIM4 not on C6)
#endif

#ifndef NEO_COUNT
  #define NEO_COUNT     8     // total number of pixels in the string
#endif

#define NEO_GRB               // type of pixels: NEO_GRB or NEO_RGB
#define NEO_LATCH_TIME  281   // latch time in microseconds

// ===================================================================================
// NeoPixel Functions and Macros
// ===================================================================================
#define NEO_latch()     DLY_us(NEO_LATCH_TIME)
void NEO_init(void);
void NEO_sendByte(uint8_t data);
void NEO_update(void);
void NEO_clearAll(void);
void NEO_writeColor(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b);
void NEO_writeHue(uint8_t pixel, uint8_t hue, uint8_t bright);
void NEO_clearPixel(uint8_t pixel);

#ifdef __cplusplus
};
#endif
