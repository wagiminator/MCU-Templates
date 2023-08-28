// ===================================================================================
// SPI Master Functions for CH551, CH552 and CH554                            * v1.0 *
// ===================================================================================
//
// Basic SPI functions without buffers and interrupts.
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "ch554.h"

// SPI parameters
#define SPI_BITORDER_MSB              // transfer bit order: LSB or MSB first
#define SPI_CLOCK_PRESC     4         // SPI clock prescaler
#define SPI_CLOCK_MODE      0         // mode0: SCK idle LOW, mode3: SCK idle HIGH

// SPI init
inline void SPI_init(void) {
  #ifdef SPI_BITORDER_LSB
  SPI0_SETUP = bS0_BIT_ORDER;                   // set SPI bit order LSB first
  #endif

  #ifdef SPI_CLOCK_PRESC
  SPI0_CK_SE = SPI_CLOCK_PRESC;                 // set SPI clock prescaler
  #endif

  #if SPI_CLOCK_MODE == 0
  SPI0_CTRL  = bS0_MOSI_OE                      // MOSI output enable
             | bS0_SCK_OE;                      // SCK output enable
  #else
  SPI0_CTRL  = bS0_MOSI_OE                      // MOSI output enable
             | bS0_SCK_OE                       // SCK output enable
             | bS0_MST_CLK;                     // master clock mode 3
  #endif
}

// SPI transmit and receive a byte
inline uint8_t SPI_transfer(uint8_t data) {
  SPI0_DATA = data;                             // start exchanging data byte
  while(!S0_FREE);                              // wait for transfer to complete
  return SPI0_DATA;                             // return received byte
}
