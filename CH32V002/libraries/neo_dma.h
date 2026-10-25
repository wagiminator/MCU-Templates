// ===================================================================================
// Basic NeoPixel Functions using Hardware-SPI and DMA                        * v1.0 *
// for CH32V002, V004, V005, V006, V007, M007
// ===================================================================================
//
// Functions available:
// --------------------
// NEO_init()               init hardware-SPI for NeoPixels on MOSI pin (see below)
// NEO_clearAll()           clear all pixels and update
// NEO_clearPixel(p)        clear pixel p
// NEO_writeColor(p,r,g,b)  write RGB color to pixel p
// NEO_writeHue(p,h,b)      write hue (h=0..191) and brightness (b=0..2) to pixel p
// NEO_update()             update pixels string (write buffer to pixels)
// NEO_latch()              latch the data sent
//
// Notes:
// ------
// - Connect the SPI MOSI pin (default PC6) to DIN of the pixels string.
// - The MOSI pin is selected by NEO_MAP (SPI1 remap, value = AFIO_PCFR1 SPI1_RM):
//   NEO_MAP   0     1     2     3     4     5     6
//   MOSI-pin PC6   PC6   PD3   PC0   PD6   PA2   PB4
// - Works with most 800kHz addressable LEDs (NeoPixels).
// - Set number of pixels and pixel type in the parameters below!
// - System clock frequency must be 48MHz, 24MHz, 12MHz, or 6MHz.
//
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// ===================================================================================
// NeoPixel Definitions
// ===================================================================================
#ifndef NEO_MAP
#define NEO_MAP         0     // SPI pin mapping of MOSI (see above)
#endif

#ifndef NEO_COUNT
#define NEO_COUNT       8     // total number of pixels in the string
#endif

#define NEO_GRB               // type of pixels: NEO_GRB or NEO_RGB
#define NEO_LATCH_TIME  281   // latch time in microseconds

// ===================================================================================
// Interrupt enable check
// ===================================================================================
#if SYS_USE_VECTORS == 0
  #error Interrupt vector table must be enabled (SYS_USE_VECTORS in system.h)!
#endif

// ===================================================================================
// NeoPixel Functions and Macros
// ===================================================================================
void NEO_init(void);
void NEO_update(void);
void NEO_clearAll(void);
void NEO_writeColor(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b);
void NEO_writeHue(uint8_t pixel, uint8_t hue, uint8_t bright);
void NEO_clearPixel(uint8_t pixel);

#ifdef __cplusplus
};
#endif
