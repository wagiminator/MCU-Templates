// ===================================================================================
// Basic NeoPixel Functions using Hardware-SPI for CH573/CH571               * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "neo_spi.h"
#include "gpio.h"

// ===================================================================================
// SPI Parameters and Variables
// ===================================================================================

// Define SPI clock divider for 6 MHz SPI frequency -> 166.67ns per bit
#if F_CPU % 6000000 == 0
#define NEO_SPI_DIV   (F_CPU / 6000000)
#else
#error Unsupported system frequency for NeoPixels!
#endif

uint8_t NEO_buffer[3 * NEO_COUNT];          // pixel buffer

// ===================================================================================
// Init SPI for Neopixels
// ===================================================================================
void NEO_init(void) {
  // Set MOSI pin to output
  #if NEO_MAP == 0
    R16_PIN_ALTERNATE &= ~RB_PIN_SPI0;
    PIN_low(PA14); PIN_output(PA14);
  #elif NEO_MAP == 1
    R16_PIN_ALTERNATE |=  RB_PIN_SPI0;
    PIN_low(PB14); PIN_output(PB14);
  #else
    #warning No automatic pin mapping for SPI
  #endif

  // Setup and enable SPI master, MOSI only, mode 0
  R8_SPI0_CLOCK_DIV = NEO_SPI_DIV;          // set clock divider
  R8_SPI0_CTRL_MOD  = RB_SPI_ALL_CLEAR;     // reset FIFO
  R8_SPI0_CTRL_MOD  = RB_SPI_MOSI_OE;       // master, MOSI output only, FIFO out
  R8_SPI0_CTRL_CFG  = 0;                    // MSB first, no DMA
}

// ===================================================================================
// Send one Data Byte to Neopixels
// ===================================================================================
void NEO_sendByte(uint8_t data) {
  uint8_t i;
  while(R8_SPI0_FIFO_COUNT);                // wait for FIFO empty
  while(!(R8_SPI0_INT_FLAG & RB_SPI_FREE)); // wait for last byte sent
  R16_SPI0_TOTAL_CNT = 8;                   // 8 SPI bytes per data byte
  for(i=8; i; i--, data<<=1) {              // 8 bits, MSB first
    if(data & 0x80) R8_SPI0_FIFO = 0x7c;    // 833ns high for "1"-bit
    else            R8_SPI0_FIFO = 0x60;    // 333ns high for "0"-bit
  }
}

// ===================================================================================
// Write Buffer to Pixels
// ===================================================================================
void NEO_update(void) {
  uint16_t i, n;
  uint8_t  data;
  uint8_t  *ptr = NEO_buffer;
  while(R8_SPI0_FIFO_COUNT);                // wait for FIFO empty
  while(!(R8_SPI0_INT_FLAG & RB_SPI_FREE)); // wait for last byte sent
  n = 3 * NEO_COUNT * 8;                    // number of SPI bytes
  R16_SPI0_TOTAL_CNT = (n > 0xFFF) ? 0xFFF : n;
  for(i=3*NEO_COUNT; i; i--) {
    data = *ptr++;
    for(uint8_t b=8; b; b--, data<<=1) {
      while(R8_SPI0_FIFO_COUNT >= SPI_FIFO_SIZE);
      R8_SPI0_FIFO = (data & 0x80) ? 0x7c : 0x60;
    }
  }
  while(R8_SPI0_FIFO_COUNT);
  while(!(R8_SPI0_INT_FLAG & RB_SPI_FREE));
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
