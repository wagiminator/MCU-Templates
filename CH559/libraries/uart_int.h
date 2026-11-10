// ===================================================================================
// UART with RX Interrupt and Ring Buffer for CH558 and CH559                 * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// UART_init()              init UART (8N1) with RX interrupt
// UART_available()         check if received data is in the buffer
// UART_read()              read data byte from buffer (waits for data)
// UART_write(c)            write data byte (waits until UART is ready)
// UART_ready()             check if UART is ready to write
//
// Pins (UART_ALTER 0/1):
// ----------------------
// UART_PORT 0: RXD P3.0 / P0.2, TXD P3.1 / P0.3 (uses TIMER1 as BAUD rate generator)
// UART_PORT 1: RXD1 P2.6 / P4.0, TXD1 P2.7 / P4.4 (8-byte FIFO)
//
// Notes:
// ------
// - The header must be included in the file containing main(), so that SDCC knows
//   the interrupt service routine.
// - Received bytes are dropped if the buffer is full.
// - UART_write() can be used with the print functions (print.h).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "ch559.h"

// UART parameters
#define UART_PORT       0                   // UART port (0 or 1)
#define UART_ALTER      0                   // UART alternate pins (0:no, 1:yes)
#define UART_BAUD       115200              // UART baud rate
#define UART_BUF_SIZE   16                  // RX buffer size (power of 2, max 128)

// BAUD rate calculation and check (error should be < 2%)
#if UART_PORT == 0
  // UART0: TIMER1 clock Fsys (fast) or Fsys/12 (slow), SMOD = 1
  #if (((F_CPU / 8 / UART_BAUD) + 1) / 2) <= 255
    #define UART_BAUD_DIV (((F_CPU / 8 / UART_BAUD) + 1) / 2)
    #define UART_T2MOD    (bTMR_CLK | bT1_CLK)
    #define UART_FCLK     (F_CPU)
  #else
    #define UART_BAUD_DIV (((F_CPU / 96 / UART_BAUD) + 1) / 2)
    #define UART_T2MOD    0
    #define UART_FCLK     (F_CPU / 12)
  #endif
  #if UART_BAUD_DIV < 1 || UART_BAUD_DIV > 255
    #error UART_BAUD not possible with this F_CPU
  #elif ((UART_FCLK / 16 / UART_BAUD_DIV) > (UART_BAUD + UART_BAUD / 50)) || \
        ((UART_FCLK / 16 / UART_BAUD_DIV) < (UART_BAUD - UART_BAUD / 50))
    #warning UART_BAUD error > 2% with this F_CPU
  #endif
#else
  // UART1: divisor = Fsys * 2 / SER1_DIV / 16 / BAUD, SER1_DIV = 1
  #define UART_BAUD_DIV   (((F_CPU / 4 / UART_BAUD) + 1) / 2)
  #if UART_BAUD_DIV < 1 || UART_BAUD_DIV > 65535
    #error UART_BAUD not possible with this F_CPU
  #elif ((F_CPU / 8 / UART_BAUD_DIV) > (UART_BAUD + UART_BAUD / 50)) || \
        ((F_CPU / 8 / UART_BAUD_DIV) < (UART_BAUD - UART_BAUD / 50))
    #warning UART_BAUD error > 2% with this F_CPU
  #endif
#endif

// UART variables
extern volatile __idata uint8_t UART_rptr;  // RX buffer read pointer
extern volatile __idata uint8_t UART_wptr;  // RX buffer write pointer

// UART functions
void UART_init(void);                       // init UART
char UART_read(void);                       // read byte from buffer
void UART_write(char c);                    // write byte

#define UART_available()  (UART_rptr != UART_wptr)
#if UART_PORT == 0
  #define UART_ready()    (TI)
  void UART_interrupt(void) __interrupt(INT_NO_UART0);
#else
  #define UART_ready()    (SER1_LSR & bLSR_T_FIFO_EMP)
  void UART_interrupt(void) __interrupt(INT_NO_UART1);
#endif
