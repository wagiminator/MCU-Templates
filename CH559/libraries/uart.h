// ===================================================================================
// UART Functions for CH558 and CH559                                         * v1.0 *
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
// UART0_write(c)           write data byte to UART0
// UART1_init()             init UART1 (8N1, 8-byte FIFO)
// UART1_ready()            check if UART1 is ready to write (TX FIFO empty)
// UART1_available()        check if UART1 has received a data byte
// UART1_read()             read data byte from UART1 (waits for data)
// UART1_write(c)           write data byte to UART1
//
// Pins (UARTx_ALTER 0/1):
// -----------------------
// UART0: RXD P3.0 / P0.2, TXD P3.1 / P0.3
// UART1: RXD1 P2.6 / P4.0, TXD1 P2.7 / P4.4
//
// To use the print functions (print.h), define a small wrapper function, since the
// inline functions have no address:
// void UART0_putchar(char c) {UART0_write(c);}
// printD(UART0_putchar, 1234);
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "ch559.h"

// UART parameters
#define UART0_ALTER     0                   // UART0 alternate pins (0:no, 1:yes)
#define UART1_ALTER     0                   // UART1 alternate pins (0:no, 1:yes)
#define UART0_BAUD      115200              // UART0 baud rate
#define UART1_BAUD      115200              // UART1 baud rate

// UART0 BAUD rate calculation: TIMER1 clock Fsys (fast) or Fsys/12 (slow), SMOD = 1
#if (((F_CPU / 8 / UART0_BAUD) + 1) / 2) <= 255
  #define UART0_BAUD_DIV  (((F_CPU / 8 / UART0_BAUD) + 1) / 2)
  #define UART0_T2MOD     (bTMR_CLK | bT1_CLK)
  #define UART0_FCLK      (F_CPU)
#else
  #define UART0_BAUD_DIV  (((F_CPU / 96 / UART0_BAUD) + 1) / 2)
  #define UART0_T2MOD     0
  #define UART0_FCLK      (F_CPU / 12)
#endif
#if UART0_BAUD_DIV < 1 || UART0_BAUD_DIV > 255
  #error UART0_BAUD not possible with this F_CPU
#elif ((UART0_FCLK / 16 / UART0_BAUD_DIV) > (UART0_BAUD + UART0_BAUD / 50)) || \
      ((UART0_FCLK / 16 / UART0_BAUD_DIV) < (UART0_BAUD - UART0_BAUD / 50))
  #warning UART0_BAUD error > 2% with this F_CPU
#endif

// UART1 BAUD rate calculation: divisor = Fsys * 2 / SER1_DIV / 16 / BAUD, SER1_DIV = 1
#define UART1_BAUD_DIV  (((F_CPU / 4 / UART1_BAUD) + 1) / 2)
#if UART1_BAUD_DIV < 1 || UART1_BAUD_DIV > 65535
  #error UART1_BAUD not possible with this F_CPU
#elif ((F_CPU / 8 / UART1_BAUD_DIV) > (UART1_BAUD + UART1_BAUD / 50)) || \
      ((F_CPU / 8 / UART1_BAUD_DIV) < (UART1_BAUD - UART1_BAUD / 50))
  #warning UART1_BAUD error > 2% with this F_CPU
#endif

// UART state macros
#define UART0_ready()       (TI)                        // UART0 ready to write?
#define UART0_available()   (RI)                        // UART0 received data byte?
#define UART1_ready()       (SER1_LSR & bLSR_T_FIFO_EMP) // UART1 ready to write?
#define UART1_available()   (SER1_LSR & bLSR_DATA_RDY)  // UART1 received data byte?

// UART0 setup
inline void UART0_init(void) {
  #if UART0_ALTER == 1
  PIN_FUNC |= bUART0_PIN_X;                 // UART0 set alternate pins
  #endif
  SCON   = 0x50;                            // UART0 mode 1 (8 bits), receive enable
  RCLK   = 0;                               // UART0 receive clock:  TIMER1
  TCLK   = 0;                               // UART0 transmit clock: TIMER1
  PCON  |= SMOD;                            // UART0 fast BAUD rate
  TMOD   = TMOD & ~(bT1_GATE | bT1_CT | MASK_T1_MOD) | bT1_M1; // TIMER1 8-bit auto-reload
  T2MOD  = T2MOD & ~bT1_CLK | UART0_T2MOD;  // TIMER1 clock
  TH1    = (uint8_t)(256 - UART0_BAUD_DIV); // TIMER1 configure for BAUD rate
  TR1    = 1;                               // TIMER1 start
  TI     = 1;                               // UART0 set transmit complete flag
}

// UART0 read single byte
inline char UART0_read(void) {
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
  SER1_LCR = bLCR_DLAB;                     // access divisor latches
  SER1_DIV = 1;                             // pre-divisor 1
  SER1_DLM = (uint8_t)(UART1_BAUD_DIV >> 8);// set divisor
  SER1_DLL = (uint8_t)(UART1_BAUD_DIV);
  SER1_LCR = bLCR_WORD_SZ1 | bLCR_WORD_SZ0; // 8N1, close divisor latches
  SER1_FCR = bFCR_FIFO_EN | bFCR_T_FIFO_CLR | bFCR_R_FIFO_CLR; // enable and clear FIFO
  #if UART1_ALTER == 1
  SER1_IER = bIER_PIN_MOD0;                 // RXD1/TXD1 on P4.0/P4.4
  P4_DIR  |= (1 << 4);                      // TXD1 pin as output
  #else
  SER1_IER = bIER_PIN_MOD1;                 // RXD1/TXD1 on P2.6/P2.7
  P2_DIR  |= (1 << 7);                      // TXD1 pin as output
  #endif
}

// UART1 read single byte
inline char UART1_read(void) {
  while(!UART1_available());                // wait for incoming data byte
  return SER1_RBR;                          // return the received byte
}

// UART1 write single byte
inline void UART1_write(char data) {
  while(!UART1_ready());                    // wait for TX FIFO empty
  SER1_THR = data;                          // start transmitting data byte
}
