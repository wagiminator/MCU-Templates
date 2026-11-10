// ===================================================================================
// Basic NeoPixel Functions using PIO for RP2040                              * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "neo_pio.h"
#include "pio.h"

// ===================================================================================
// PIO Program (ws2812.pio from the Raspberry Pi Pico examples, BSD-3-Clause)
// ===================================================================================
// One bit takes T1 + T2 + T3 = 10 cycles at 8MHz (800kHz): a "0" is high for 2 cycles,
// a "1" is high for 7 cycles. One side-set bit drives the pin.
#define NEO_T1          2
#define NEO_T2          5
#define NEO_T3          3

static const uint16_t NEO_program[] = {
  PIO_OUT(PIO_X, 1)         | PIO_SIDE(1, 0) | PIO_DELAY(NEO_T3 - 1), // 0: out x, 1   side 0 [2]
  PIO_JMP(PIO_NOT_X, 3)     | PIO_SIDE(1, 1) | PIO_DELAY(NEO_T1 - 1), // 1: jmp !x, 3  side 1 [1]
  PIO_JMP(PIO_ALWAYS, 0)    | PIO_SIDE(1, 1) | PIO_DELAY(NEO_T2 - 1), // 2: jmp 0      side 1 [4]
  PIO_NOP                   | PIO_SIDE(1, 0) | PIO_DELAY(NEO_T2 - 1)  // 3: nop        side 0 [4]
};

// ===================================================================================
// Pixel Buffer
// ===================================================================================
uint8_t NEO_buffer[3 * NEO_COUNT];

// ===================================================================================
// Init PIO State Machine
// ===================================================================================
void NEO_init(void) {
  PIO_init(NEO_PIO);                                      // reset and release PIO
  PIO_load(NEO_PIO, NEO_program, 4, NEO_OFFSET);          // load program
  PIO_SM_setClock(NEO_PIO, NEO_SM, 800000 * (NEO_T1 + NEO_T2 + NEO_T3));
  NEO_PIO->SM[NEO_SM].PINCTRL   = ((uint32_t)1 << PIO_SM_PINCTRL_SIDESET_COUNT_Pos)
                                | ((uint32_t)NEO_PIN << PIO_SM_PINCTRL_SIDESET_BASE_Pos)
                                | ((uint32_t)1 << PIO_SM_PINCTRL_SET_COUNT_Pos)
                                | ((uint32_t)NEO_PIN << PIO_SM_PINCTRL_SET_BASE_Pos);
  NEO_PIO->SM[NEO_SM].SHIFTCTRL = PIO_SM_SHIFTCTRL_FJOIN_TX       // 8-word TX FIFO
                                | PIO_SM_SHIFTCTRL_AUTOPULL       // pull automatically
                                | ((uint32_t)24 << PIO_SM_SHIFTCTRL_PULL_THRESH_Pos); // 24 bits
  PIO_SM_setWrap(NEO_PIO, NEO_SM, NEO_OFFSET, NEO_OFFSET + 3);
  PIO_SM_restart(NEO_PIO, NEO_SM);                        // clear shift registers
  PIO_SM_exec(NEO_PIO, NEO_SM, PIO_SET(PIO_PINDIRS, 1));  // pin as output
  PIO_SM_exec(NEO_PIO, NEO_SM, PIO_JMP(PIO_ALWAYS, NEO_OFFSET)); // start address
  PIO_pin(NEO_PIO, NEO_PIN);                              // connect pin to PIO
  PIO_SM_enable(NEO_PIO, NEO_SM);                         // start state machine
}

// ===================================================================================
// Send a Data Byte to the Pixels String (MSB first)
// ===================================================================================
static uint32_t NEO_word;                                 // collects 3 bytes (24 bits)
static uint8_t  NEO_bytes;

void NEO_sendByte(uint8_t data) {
  NEO_word = (NEO_word << 8) | data;
  if(++NEO_bytes == 3) {                                  // one pixel complete
    PIO_SM_put(NEO_PIO, NEO_SM, NEO_word << 8);           // left aligned 24 bits
    NEO_bytes = 0;
  }
}

// ===================================================================================
// Write Buffer to Pixels
// ===================================================================================
void NEO_update(void) {
  uint16_t i;
  uint8_t *ptr;
  ptr = NEO_buffer;
  NEO_bytes = 0;
  for(i=3*NEO_COUNT; i; i--) NEO_sendByte(*ptr++);
  while(!(NEO_PIO->FSTAT & ((uint32_t)1 << (PIO_FSTAT_TXEMPTY_Pos + NEO_SM)))); // FIFO empty
  DLY_us(30);                                             // last pixel (24 bits) sent
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
