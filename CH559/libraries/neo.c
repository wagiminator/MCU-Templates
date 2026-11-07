// ===================================================================================
// NeoPixel (Addressable LED) Functions for CH558 and CH559                   * v1.0 *
// ===================================================================================
//
// Basic control functions for 800kHz addressable LEDs (NeoPixel). A simplified 
// protocol is used which should work with most LEDs.
//
// The following must be defined in config.h:
// PIN_NEO   - pin connected to DATA-IN of the pixel strip (via a ~330 ohms resistor).
// NEO_GRB   - type of pixel: NEO_GRB or NEO_RGB
// NEO_COUNT - total number of pixels
// System clock frequency must be at least 6 MHz. Only pins of P0-P3 (bit-addressable).
//
// Further information:     https://github.com/wagiminator/ATtiny13-NeoController
// 2023 by Stefan Wagner:   https://github.com/wagiminator

// ===================================================================================
// Libraries, Variables and Constants
// ===================================================================================
#include "neo.h"

#define NEOPIN PIN_asm(PIN_NEO)             // convert PIN_NEO for inline assembly
__xdata uint8_t NEO_buffer[3 * NEO_COUNT];  // pixel buffer

// ===================================================================================
// Protocol Delays (calculated for any F_CPU >= 6MHz)
// ===================================================================================
// Target timing: T0H ~ 350ns, T1H ~ 800ns, bit period ~ 1250ns
// Bit loop: rlc(1) setb(2) [D0] mov(2) [D1] clr(2) [D2] djnz(4-6) ~ 12 + D0 + D1 + D2
// (all values are evaluated by the assembler, so keep them small)
#if   F_CPU == 56000000
  #define NEO_MHZ       56
#elif F_CPU == 48000000
  #define NEO_MHZ       48
#elif F_CPU == 32000000
  #define NEO_MHZ       32
#elif F_CPU == 24000000
  #define NEO_MHZ       24
#elif F_CPU == 16000000
  #define NEO_MHZ       16
#elif F_CPU == 12000000
  #define NEO_MHZ       12
#elif F_CPU == 8000000
  #define NEO_MHZ       8
#elif F_CPU == 6000000
  #define NEO_MHZ       6
#else
  #error Unsupported system clock frequency for NeoPixels!
#endif
#define NEO_CYC(ns)     ((NEO_MHZ * ((ns) / 10) + 50) / 100)
#if (NEO_CYC(350) - 2) > 0
  #define NEO_D0        (NEO_CYC(350) - 2)
#else
  #define NEO_D0        0
#endif
#if (NEO_CYC(800) - 4 - NEO_D0) > 0
  #define NEO_D1        (NEO_CYC(800) - 4 - NEO_D0)
#else
  #define NEO_D1        0
#endif
#if (NEO_CYC(1250) - 12 - NEO_D0 - NEO_D1) > 0
  #define NEO_D2        (NEO_CYC(1250) - 12 - NEO_D0 - NEO_D1)
#else
  #define NEO_D2        0
#endif

// ===================================================================================
// Send a Data Byte to the Pixels String
// ===================================================================================
// This is the most time sensitive part. Outside of the function, it must be 
// ensured that interrupts are disabled and that the time between the 
// transmission of the individual bytes is less than the pixel's latch time.
void NEO_sendByte(uint8_t data) { 
  data;                 // stop unreferenced argument warning
  __asm
    mov  r7, #8         ; 2 CLK - 8 bits to transfer
    xch  a, dpl         ; 2 CLK - data byte -> accu
    01$:
    rlc  a              ; 1 CLK - data bit -> carry (MSB first)
    setb NEOPIN         ; 2 CLK - NEO pin HIGH
    .rept NEO_D0
    nop                 ; T0H delay
    .endm
    mov  NEOPIN, c      ; 2 CLK - "0"-bit? -> NEO pin LOW now
    .rept NEO_D1
    nop                 ; T1H delay
    .endm
    clr  NEOPIN         ; 2 CLK - "1"-bit? -> NEO pin LOW a little later
    .rept NEO_D2
    nop                 ; TCT delay
    .endm
    djnz r7, 01$        ; 4-6 CLK - repeat for all bits
  __endasm;
}

// ===================================================================================
// Write Buffer to Pixels
// ===================================================================================
void NEO_update(void) {
  uint8_t i;
  __bit ea = EA;                            // save interrupt state
  __xdata uint8_t* ptr = NEO_buffer;
  EA = 0;
  for(i=3*NEO_COUNT; i; i--) NEO_sendByte(*ptr++);
  EA = ea;                                  // restore interrupt state
  NEO_latch();
}

// ===================================================================================
// Clear all Pixels
// ===================================================================================
void NEO_clearAll(void) {
  uint8_t i;
  __xdata uint8_t* ptr = NEO_buffer;
  for(i=3*NEO_COUNT; i; i--) *ptr++ = 0;
  NEO_update();
}

// ===================================================================================
// Write Color to a Single Pixel in Buffer
// ===================================================================================
void NEO_writeColor(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b) {
  __xdata uint8_t* ptr;
  if(pixel >= NEO_COUNT) return;            // check pixel index
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
