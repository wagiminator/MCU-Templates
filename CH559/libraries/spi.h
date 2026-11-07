// ===================================================================================
// SPI0 Master Functions for CH558 and CH559                                  * v1.0 *
// ===================================================================================
//
// Basic SPI functions without buffers and interrupts.
//
// Functions available:
// --------------------
// SPI_init()               init SPI0 as master
// SPI_transfer(d)          transmit and receive one data byte
// SPI_write(d)             transmit one data byte
// SPI_read()               receive one data byte (sends 0xFF)
//
// Pins: SCS P1.4, MOSI P1.5, MISO P1.6, SCK P1.7 (SPI_ALTER: SCS P4.6, SCK P4.7).
// The chip select pin must be controlled by the application.
// SPI clock = F_CPU / SPI_CLOCK_PRESC.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "ch559.h"

// SPI parameters
#define SPI_BITORDER_MSB              // transfer bit order: LSB or MSB first
#define SPI_CLOCK_PRESC     4         // SPI clock prescaler (min 2)
#define SPI_CLOCK_MODE      0         // mode0: SCK idle LOW, mode3: SCK idle HIGH
#define SPI_ALTER           0         // SCK on P4.7 instead of P1.7 (0:no, 1:yes)

// SPI init
inline void SPI_init(void) {
  #if SPI_ALTER == 1
  P4_CFG |= bSPI0_PIN_X;                       // SCS/SCK on P4.6/P4.7
  P4_DIR |= (1 << 7);                           // SCK pin as output
  #endif
  #ifdef SPI_BITORDER_LSB
  SPI0_SETUP = bS0_BIT_ORDER;                   // set SPI bit order LSB first
  #else
  SPI0_SETUP = 0;                               // master mode, MSB first
  #endif
  SPI0_CK_SE = SPI_CLOCK_PRESC;                 // set SPI clock prescaler
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

#define SPI_write(d)    SPI_transfer(d)
#define SPI_read()      SPI_transfer(0xFF)
