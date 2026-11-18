// ===================================================================================
// Basic NeoPixel Functions using PIOC for CH32X035/X034/X033                 * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "neo_pioc.h"

// ===================================================================================
// PIOC Parameters and Variables
// ===================================================================================
#define NEO_BUF_OFFS    0x0400      // byte offset of pixel data in PIOC code RAM
#define NEO_BYTES       (3 * NEO_COUNT)
#define NEO_CYCLE       ((F_CPU / 1000000) * NEO_BIT_TIME / 1000 - 1)
#define NEO_LATCH_CNT   ((F_CPU / 1000000) * NEO_LATCH_TIME / 8)

#if (F_CPU < 8000000) || (NEO_CYCLE > 127)
  #error Unsupported system frequency for NeoPixels!
#endif
#if NEO_BYTES > (PIOC_RAM_SIZE - NEO_BUF_OFFS)
  #error Too many pixels!
#endif

uint8_t NEO_buffer[NEO_BYTES];              // pixel buffer
static uint8_t NEO_active = 0;              // transfer started

// PIOC program (assembled from neo_pioc.asm with pioc_asm.py)
static const uint8_t NEO_PIOC_CODE[208] __attribute__((aligned(4))) = {
  0x00, 0x00, 0x02, 0x60, 0x08, 0x01, 0x0B, 0x40, 0x0A, 0x48, 0x80, 0x28, 0x0C, 0x10, 0x14, 0x00,
  0x1E, 0x02, 0x7F, 0x29, 0x3B, 0x10, 0x20, 0x02, 0x21, 0x0A, 0x5D, 0x34, 0x00, 0x28, 0x3C, 0x10,
  0x02, 0x28, 0x3D, 0x10, 0x3C, 0x02, 0x04, 0x10, 0x3D, 0x02, 0x18, 0x00, 0x1F, 0x10, 0x80, 0x29,
  0x3B, 0x0A, 0x08, 0x10, 0x13, 0x00, 0xAE, 0x00, 0x13, 0x00, 0xAD, 0x00, 0x13, 0x00, 0xAC, 0x00,
  0x13, 0x00, 0xAB, 0x00, 0x13, 0x00, 0xAA, 0x00, 0x20, 0x15, 0x20, 0x04, 0x03, 0x52, 0x21, 0x15,
  0x13, 0x00, 0xA9, 0x00, 0x20, 0x02, 0x21, 0x0A, 0x13, 0x00, 0x53, 0x34, 0xA8, 0x00, 0x13, 0x00,
  0x04, 0x02, 0x1F, 0x10, 0xAF, 0x00, 0x13, 0x00, 0xAE, 0x00, 0x13, 0x00, 0xAD, 0x00, 0x13, 0x00,
  0xAC, 0x00, 0x13, 0x00, 0xAB, 0x00, 0x3C, 0x14, 0x03, 0x52, 0x3D, 0x14, 0x13, 0x00, 0xAA, 0x00,
  0x20, 0x15, 0x20, 0x04, 0x03, 0x52, 0x21, 0x15, 0x13, 0x00, 0xA9, 0x00, 0x20, 0x02, 0x21, 0x0A,
  0x13, 0x00, 0x53, 0x34, 0xA8, 0x00, 0x3C, 0x02, 0x04, 0x10, 0x3D, 0x02, 0x18, 0x00, 0x13, 0x00,
  0x1F, 0x10, 0xAF, 0x00, 0x1A, 0x60, 0xA8, 0x00, 0x13, 0x00, 0x08, 0x47, 0x12, 0x00, 0x08, 0x01,
  0x22, 0x02, 0x3E, 0x10, 0x23, 0x02, 0x3F, 0x10, 0x60, 0x70, 0x04, 0x00, 0x1D, 0x10, 0x07, 0x60,
  0x3E, 0x15, 0x3E, 0x04, 0x03, 0x52, 0x3F, 0x15, 0x3E, 0x02, 0x3F, 0x0A, 0x60, 0x30, 0x30, 0x00,
};

// ===================================================================================
// Init PIOC for NeoPixels
// ===================================================================================
void NEO_init(void) {
  PIOC_init(PIOC_IO0);                      // init PIOC, IO0 to PIOC
  PIOC_load(NEO_PIOC_CODE, sizeof(NEO_PIOC_CODE)); // load program
  PIOC_start();                             // start program
  PIOC_setReg16(2, NEO_LATCH_CNT);          // set latch time
  NEO_active = 0;
}

// ===================================================================================
// Check if Transfer is still Running
// ===================================================================================
uint8_t NEO_busy(void) {
  return(NEO_active && !PIOC_available());
}

// ===================================================================================
// Write Buffer to Pixels
// ===================================================================================
void NEO_update(void) {
  if(NEO_active) PIOC_read();               // wait for previous transfer to finish
  PIOC_writeRAM(NEO_BUF_OFFS, NEO_buffer, NEO_BYTES); // copy buffer to PIOC
  PIOC_setReg16(0, NEO_BYTES);              // number of bytes
  PIOC_write(NEO_CYCLE);                    // start transfer
  NEO_active = 1;
}

// ===================================================================================
// Clear all Pixels
// ===================================================================================
void NEO_clearAll(void) {
  uint16_t i;
  uint8_t *ptr;
  ptr = NEO_buffer;
  for(i=NEO_BYTES; i; i--) *ptr++ = 0;
  NEO_update();
}

// ===================================================================================
// Write Color to a Single Pixel in Buffer
// ===================================================================================
void NEO_writeColor(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b) {
  uint8_t *ptr;
  if(pixel >= NEO_COUNT) return;
  ptr = NEO_buffer + (3 * pixel);
  #if defined (NEO_GRB)
    *ptr++ = g; *ptr++ = r; *ptr = b;
  #elif defined (NEO_RGB)
    *ptr++ = r; *ptr++ = g; *ptr = b;
  #else
    #error Wrong or missing NeoPixel type definition!
  #endif
}

// ===================================================================================
// Write Hue Value (0..191) and Brightness (0..2) to a Single Pixel in Buffer
// ===================================================================================
void NEO_writeHue(uint8_t pixel, uint8_t hue, uint8_t bright) {
  uint8_t phase = hue >> 6;
  uint8_t step  = (hue & 63) << bright;
  uint8_t nstep = (63 << bright) - step;
  switch(phase) {
    case 0:   NEO_writeColor(pixel, nstep,  step,     0); break;
    case 1:   NEO_writeColor(pixel,     0, nstep,  step); break;
    case 2:   NEO_writeColor(pixel,  step,     0, nstep); break;
    default:  break;
  }
}

// ===================================================================================
// Clear Single Pixel in Buffer
// ===================================================================================
void NEO_clearPixel(uint8_t pixel) {
  NEO_writeColor(pixel, 0, 0, 0);
}
