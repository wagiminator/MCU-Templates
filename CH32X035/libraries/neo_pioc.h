// ===================================================================================
// Basic NeoPixel Functions using PIOC for CH32X035/X034/X033                 * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// NEO_init()               init PIOC for NeoPixels on IO0 (PC18 or PC7)
// NEO_clearAll()           clear all pixels and update
// NEO_clearPixel(p)        clear pixel p
// NEO_writeColor(p,r,g,b)  write RGB color to pixel p
// NEO_writeHue(p,h,b)      write hue (h=0..191) and brightness (b=0..2) to pixel p
// NEO_update()             update pixels string (write buffer to pixels)
// NEO_busy()               check if update (incl. latch time) is still running
//
// PIOC IO0 pin mapping (set PIOC_MAP in pioc.h):
// -----------------------------------------------
// PIOC_MAP    0     1
// DIN-pin    PC18  PC7
//
// Notes:
// ------
// - Connect IO0 of the PIOC (see above) to DIN of the pixels string.
// - Works with most 800kHz addressable LEDs (NeoPixels, WS2812, SK6812).
// - Set number of pixels and pixel type in the parameters below!
// - The bits are encoded in hardware by the bit encoder of the PIOC, the PIOC
//   program reads the pixel data from its code RAM. NEO_update() copies the buffer
//   into the PIOC and returns immediately, the transfer runs in the background.
//   The next NEO_update() waits until the previous transfer has been completed.
// - F_CPU must be 8MHz or higher. Max. 255 pixels (8-bit pixel index).
// - Requires the pioc library (pioc.c/pioc.h). The RAM size in the linker script
//   must be reduced to 16K (see pioc.h)!
// - If PIOC_MAP is 0, the debug interface (SWD) is disabled, upload new firmware
//   via the USB bootloader.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"
#include "pioc.h"

// ===================================================================================
// NeoPixel Definitions
// ===================================================================================
#ifndef NEO_COUNT
  #define NEO_COUNT     8     // total number of pixels in the string (max. 255)
#endif

#define NEO_GRB               // type of pixels: NEO_GRB or NEO_RGB
#define NEO_LATCH_TIME  281   // latch time in microseconds
#define NEO_BIT_TIME    1200  // bit period in nanoseconds (1250ns = 800kHz)

// ===================================================================================
// NeoPixel Functions and Macros
// ===================================================================================
void NEO_init(void);
void NEO_update(void);
void NEO_clearAll(void);
void NEO_writeColor(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b);
void NEO_writeHue(uint8_t pixel, uint8_t hue, uint8_t bright);
void NEO_clearPixel(uint8_t pixel);
uint8_t NEO_busy(void);

#ifdef __cplusplus
};
#endif
