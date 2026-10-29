// ===================================================================================
// Basic NeoPixel Functions using Software Bit-banging for CH32V203           * v1.0 *
// ===================================================================================
//
// Basic control functions for 800kHz addressable LEDs (NeoPixel). A simplified 
// protocol is used which should work with most LEDs. Instead of cycle counted
// assembly code the timing is derived from the 64-bit SysTick counter, which
// runs with F_CPU. This makes it independent of flash wait states and compiler
// optimizations.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "neo_sw.h"

// ===================================================================================
// Neopixel Parameters and Variables
// ===================================================================================

// Protocol timings in SysTick ticks (= F_CPU cycles).
// There are three essential conditions:
// - T0H (HIGH-time for "0"-bit) must be max.  500ns -> 350ns used
// - T1H (HIGH-time for "1"-bit) must be min.  625ns -> 800ns used
// - TCT (total clock time) must be      min. 1150ns -> 1250ns used
#define NEO_T0H         ((F_CPU / 1000000) *  350 / 1000)
#define NEO_T1H         ((F_CPU / 1000000) *  800 / 1000)
#define NEO_TCT         ((F_CPU / 1000000) * 1250 / 1000)

#if F_CPU < 24000000
  #error Unsupported system frequency for NeoPixels (min. 24MHz)!
#endif

// Define some constants depending on the NeoPixel pin
#define NEO_GPIO \
  ((PIN_NEO>=PA0)&&(PIN_NEO<=PA15) ? ( GPIOA ) : \
  ((PIN_NEO>=PB0)&&(PIN_NEO<=PB15) ? ( GPIOB ) : \
  ((PIN_NEO>=PC0)&&(PIN_NEO<=PC15) ? ( GPIOC ) : \
  ((PIN_NEO>=PD0)&&(PIN_NEO<=PD15) ? ( GPIOD ) : \
(0)))))
#define NEO_PIN_BM      ((uint32_t)1<<((PIN_NEO)&15))

// NeoPixel buffer
uint8_t NEO_buffer[3 * NEO_COUNT];          // pixel buffer

// ===================================================================================
// Init Neopixels
// ===================================================================================
void NEO_init(void) {
  PIN_low(PIN_NEO);
  PIN_output(PIN_NEO);
  NEO_latch();
}

// ===================================================================================
// Send one Data Byte to Neopixels
// ===================================================================================
// This is the most time sensitive part. Outside of the function, it must be 
// ensured that interrupts are disabled and that the time between the 
// transmission of the individual bytes is less than the pixel's latch time.
void NEO_sendByte(uint8_t data) {
  GPIO_TypeDef* gpio = NEO_GPIO;
  uint32_t start = STK->CNTL;
  uint8_t  i;
  for(i=8; i; i--, data<<=1) {
    uint32_t th = (data & 0x80) ? NEO_T1H : NEO_T0H;
    while((int32_t)(STK->CNTL - start) < 0);          // wait for start of bit
    gpio->BSHR = NEO_PIN_BM;                          // set neopixel pin HIGH
    while((int32_t)(STK->CNTL - start) < (int32_t)th);// HIGH time
    gpio->BCR  = NEO_PIN_BM;                          // set neopixel pin LOW
    start += NEO_TCT;                                 // start time of next bit
  }
  while((int32_t)(STK->CNTL - start) < 0);            // complete LOW time of last bit
}

// ===================================================================================
// Write Buffer to Pixels
// ===================================================================================
void NEO_update(void) {
  uint16_t i;
  uint8_t *ptr;
  ptr = NEO_buffer;
  __disable_irq();                          // timing critical
  for(i=3*NEO_COUNT; i; i--) NEO_sendByte(*ptr++);
  __enable_irq();
  NEO_latch();
}

// ===================================================================================
// Clear all Pixels
// ===================================================================================
void NEO_clearAll(void) {
  uint16_t i;
  uint8_t *ptr;
  ptr = NEO_buffer;
  for(i=3*NEO_COUNT; i; i--) *ptr++ = 0;
  NEO_update();
}

// ===================================================================================
// Write Color to a Single Pixel in Buffer
// ===================================================================================
void NEO_writeColor(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b) {
  uint8_t *ptr;
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
