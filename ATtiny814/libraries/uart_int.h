// ===================================================================================
// USART0 with RX Interrupt and Ring Buffer for tinyAVR                       * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// UART_init()              Init UART with 8N1 and default BAUD rate (115200)
// UART_setBaud(n)          Set BAUD rate
// UART_setDataBits(n)      Set number of data bits (n = 5, 6, 7, 8)
// UART_setStopBits(n)      Set number of stop bits (n = 1, 2)
// UART_setNoParity()       Set no parity bit
// UART_setOddParity()      Set parity bit, odd
// UART_setEvenParity()     Set parity bit, even
//
// UART_ready()             Check if UART is ready to write
// UART_available()         Check if there is something to read
// UART_completed()         Check if transmission is completed
//
// UART_read()              Read character via UART
// UART_write(c)            Send character via UART
//
// UART_TX_enable()         Enable transmitter
// UART_TX_disable()        Disable transmitter
// UART_RX_enable()         Enable receiver
// UART_RX_disable()        Disable receiver
//
// If print functions are activated (see below, print.h must be included):
// -----------------------------------------------------------------------
// UART_printf(f, ...)      printf (supports %s, %c, %d, %u, %x, %b, %02d, %%)
// UART_printD(n)           Print decimal value
// UART_printW(n)           Print 32-bit hex word value
// UART_printH(n)           Print 16-bit hex half-word value
// UART_printB(n)           Print  8-bit hex byte value
// UART_printS(s)           Print string
// UART_print(s)            Print string (alias)
// UART_println(s)          Print string with newline
// UART_newline()           Send newline
//
// USART0 pin mapping (set below in UART parameters):
// --------------------------------------------------
// UART_MAP   0     1     2
// TX-pin    PB2   PA1   No mapping
// RX-pin    PB3   PA2   No mapping
//
// 2021 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "system.h"

// UART parameters
#define UART_BAUD             115200      // default UART baud rate
#define UART_MAP              0           // UART pin mapping (see above)
#define UART_PRINT            0           // 1: include print functions (needs print.h)

// UART macros
#define UART_available()      (UART_rptr != UART_wptr)            // something in buffer?
#define UART_ready()          (USART0.STATUS & USART_DREIF_bm)    // ready to write
#define UART_completed()      (USART0.STATUS & USART_TXCIF_bm)    // transmission completed

#define UART_TX_enable()      USART0.CTRLB |=  USART_TXEN_bm      // enable transmitter
#define UART_TX_disable()     USART0.CTRLB &= ~USART_TXEN_bm      // disable transmitter
#define UART_RX_enable()      USART0.CTRLB |=  USART_RXEN_bm      // enable receiver
#define UART_RX_disable()     USART0.CTRLB &= ~USART_RXEN_bm      // disable receiver

#define UART_setBAUD(n)       USART0.BAUD=(((F_CPU*8)/(n))+1)/2   // set BAUD rate
#define UART_setDataBits(n)   USART0.CTRLC = (USART0.CTRLC & 0xf8) | (((n) - 5) & 3)
#define UART_setStopBits(n)   (n==2 ? (USART0.CTRLC |= (1<<3)) : (USART0.CTRLC &= ~(1<<3)))
#define UART_setEvenParity()  USART0.CTRLC = (USART0.CTRLC & ~USART_PMODE_gm) | USART_PMODE_EVEN_gc
#define UART_setOddParity()   USART0.CTRLC = (USART0.CTRLC & ~USART_PMODE_gm) | USART_PMODE_ODD_gc
#define UART_setNoParity()    USART0.CTRLC &= ~USART_PMODE_gm

// UART variables
volatile uint8_t UART_rptr;           // UART RX buffer read pointer
volatile uint8_t UART_wptr;           // UART RX buffer write pointer

// UART functions
void UART_init(void);                 // init UART with default BAUD rate
char UART_read(void);                 // read character via UART
void UART_write(const char c);        // send character via UART

// Additional print functions (if activated, see above)
#if UART_PRINT == 1
#include "print.h"
#define UART_printD(n)        printD(UART_write, n)   // print decimal as string
#define UART_printW(n)        printW(UART_write, n)   // print word as string
#define UART_printH(n)        printH(UART_write, n)   // print half-word as string
#define UART_printB(n)        printB(UART_write, n)   // print byte as string
#define UART_printS(s)        printS(UART_write, s)   // print string
#define UART_println(s)       println(UART_write, s)  // print string with newline
#define UART_print            UART_printS             // alias
#define UART_newline()        UART_write('\n')        // send newline
#define UART_printf(f, ...)   printF(UART_write, f, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
};
#endif
