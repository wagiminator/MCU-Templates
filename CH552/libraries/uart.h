// ===================================================================================
// UART Functions for CH551, CH552 and CH554                                  * v1.2 *
// ===================================================================================
//
// Basic UART functions without buffers and interrupts.
//
// Functions available:
// --------------------
// UART0_init()             init UART0 (8N1, TIMER1 as BAUD rate generator)
// UART0_ready()            check if UART0 is ready to write
// UART0_available()        check if UART0 has received a data byte
// UART0_read()             read data byte from UART0 (waits for data)
// UART0_write(b)           write data byte to UART0
// UART1_init()             init UART1 (8N1)
// UART1_ready()            check if UART1 is ready to write
// UART1_available()        check if UART1 has received a data byte
// UART1_read()             read data byte from UART1 (waits for data)
// UART1_write(b)           write data byte to UART1
//
// Pins (UARTx_ALTER 0/1):
// -----------------------
// UART0: RXD P3.0 / P1.2, TXD P3.1 / P1.3
// UART1: RXD1 P1.6 / P3.4, TXD1 P1.7 / P3.2 (not available on CH551)
//
// To use the print functions (print.h), define a small wrapper function, since the
// inline functions have no address:
// void UART0_putchar(char c) {UART0_write(c);}
// printD(UART0_putchar, 1234);
//
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

// Check BAUD rates (register must be 1..255, error should be < 2%)
#define UART0_BAUD_DIV  (((F_CPU / 8 / UART0_BAUD) + 1) / 2)
#define UART1_BAUD_DIV  (((F_CPU / 8 / UART1_BAUD) + 1) / 2)
#if UART0_BAUD_DIV < 1 || UART0_BAUD_DIV > 255
  #error UART0_BAUD not possible with this F_CPU
#elif ((F_CPU / 16 / UART0_BAUD_DIV) > (UART0_BAUD + UART0_BAUD / 50)) || \
      ((F_CPU / 16 / UART0_BAUD_DIV) < (UART0_BAUD - UART0_BAUD / 50))
  #warning UART0_BAUD error > 2% with this F_CPU
#endif
#if UART1_BAUD_DIV < 1 || UART1_BAUD_DIV > 255
  #error UART1_BAUD not possible with this F_CPU
#elif ((F_CPU / 16 / UART1_BAUD_DIV) > (UART1_BAUD + UART1_BAUD / 50)) || \
      ((F_CPU / 16 / UART1_BAUD_DIV) < (UART1_BAUD - UART1_BAUD / 50))
  #warning UART1_BAUD error > 2% with this F_CPU
#endif

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
inline void UART0_write(char data) {
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
inline void UART1_write(char data) {
  while(!U1TI);                             // wait for last data byte to be transmitted
  U1TI = 0;                                 // clear transmit complete flag
  SBUF1 = data;                             // start transmitting data byte
}
