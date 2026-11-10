// ===================================================================================
// Basic NeoPixel Functions using PIO for RP2350                              * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// NEO_init()               init PIO state machine for NeoPixels on NEO_PIN
// NEO_clearAll()           clear all pixels and update
// NEO_clearPixel(p)        clear pixel p
// NEO_writeColor(p,r,g,b)  write RGB color to pixel p
// NEO_writeHue(p,h,b)      write hue (h=0..191) and brightness (b=0..2) to pixel p
// NEO_update()             update pixels string (write buffer to pixels)
//
// NEO_sendByte(d)          send one data byte via PIO to pixels string
// NEO_latch()              latch the data sent
//
// Notes:
// ------
// - Connect NEO_PIN (any GPIO, define below) to DIN of the pixels string.
// - Works with most 800kHz addressable LEDs (NeoPixels, WS2812, SK6812).
// - Set number of pixels and pixel type in the parameters below!
// - A PIO state machine generates the timing (program from the Raspberry Pi Pico
//   examples, 4 instructions), so the timing is independent of interrupts and the
//   system clock (F_CPU >= 16MHz).
// - Max. 255 pixels (8-bit pixel index).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// ===================================================================================
// NeoPixel Definitions
// ===================================================================================
#define NEO_PIN         GP16  // pin connected to DIN of the pixels string
#define NEO_GRB               // type of pixels: NEO_GRB or NEO_RGB
#define NEO_COUNT       8     // total number of pixels in the string
#define NEO_LATCH_TIME  281   // latch time in microseconds
#define NEO_PIO         PIO0  // PIO block (PIO0, PIO1 or PIO2)
#define NEO_SM          0     // state machine (0..3)
#define NEO_OFFSET      0     // offset of the PIO program in instruction memory (0..28)

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
