// ===================================================================================
// UART Functions for CH551, CH552 and CH554                                  * v1.1 *
// ===================================================================================
//
// Basic UART functions without buffers and interrupts.
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "ch554.h"

// UART parameters
#define UART0_ALTER     0                   // UART0 alternate pins (0:no, 1:yes)
#define UART1_ALTER     0                   // UART1 alternate pins (0:no, 1:yes)
#define UART0_BAUD      9600                // UART0 baud rate
#define UART1_BAUD      9600                // UART1 baud rate

#define UART0_BAUD_SET  (uint8_t)(256 - (((F_CPU / 8 / UART0_BAUD) + 1) / 2))
#define UART1_BAUD_SET  (uint8_t)(256 - (((F_CPU / 8 / UART1_BAUD) + 1) / 2))

// UART state macros
#define UART0_ready()       (TI)            // UART0 ready to write?
#define UART0_available()   (RI)            // UART0 received data byte?
#define UART1_ready()       (U1TI)          // UART1 ready to write?
#define UART1_available()   (U1RI)          // UART1 received data byte?

// UART0 setup
inline void UART0_init(void) {
  #if UART0_ALTER == 1
  PIN_FUNC |= bUART0_PIN_X;                 // UART0 set alternate pins
  #endif
  SM0 = 0;                                  // UART0 8 data bits
  SM1 = 1;                                  // UART0 BAUD rate by timer
  SM2 = 0;                                  // UART0 no multi-device comm
  RCLK = 0;                                 // UART0 receive clock:  TIMER1
  TCLK = 0;                                 // UART0 transmit clock: TIMER1
  PCON |= SMOD;                             // UART0 fast BAUD rate
  TMOD &= ~(bT1_GATE | bT1_CT | MASK_T1_MOD);
  TMOD |= bT1_M1;                           // TIMER1 8-bit auto-reload
  T2MOD |= bTMR_CLK | bT1_CLK;              // TIMER1 fast clock selection
  TH1 = UART0_BAUD_SET;                     // TIMER1 configure for BAUD rate
  TR1 = 1;                                  // TIMER1 start
  TI  = 1;                                  // UART0 set transmit complete flag
  REN = 1;                                  // UART0 receive enable
}

// UART0 read single byte
inline uint8_t UART0_read(void) {
  while(!RI);                               // wait for incoming data byte
  RI = 0;                                   // clear receive complete flag
  return SBUF;                              // return the received byte
}

// UART0 write single byte
inline void UART0_write(uint8_t data) {
  while(!TI);                               // wait for last data byte to be transmitted
  TI = 0;                                   // clear transmit complete flag
  SBUF = data;                              // start transmitting data byte
}

// UART1 setup
inline void UART1_init(void) {
  #if UART1_ALTER == 1
  PIN_FUNC |= bUART1_PIN_X;                 // UART1 set alternate pins
  #endif
  U1SM0  = 0;                               // UART1 8 data bits
  U1SMOD = 1;                               // UART1 fast mode
  U1REN  = 1;                               // UART1 receive enable
  SBAUD1 = UART1_BAUD_SET;                  // UART1 set BAUD rate
  U1TI   = 1;                               // UART1 set transmit complete flag
}

// UART1 read single byte
inline uint8_t UART1_read(void) {
  while(!U1RI);                             // wait for incoming data byte
  U1RI = 0;                                 // clear receive complete flag
  return SBUF1;                             // return the received byte
}

// UART1 write single byte
inline void UART1_write(uint8_t data) {
  while(!U1TI);                             // wait for last data byte to be transmitted
  U1TI = 0;                                 // clear transmit complete flag
  SBUF1 = data;                             // start transmitting data byte
}
