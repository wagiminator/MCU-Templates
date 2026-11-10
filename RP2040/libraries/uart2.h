// ===================================================================================
// Basic UART2 Functions for RP2040 (UART0/UART1, no buffer, no interrupt)   * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// UART2_init()              Init UART with 8N1 and default BAUD rate (115200)
// UART2_setBAUD(n)          Set BAUD rate
// UART2_setDataBits(n)      Set number of data bits (n = 5, 6, 7, 8)
// UART2_setStopBits(n)      Set number of stop bits (n = 1, 2)
// UART2_setNoParity()       Set no parity bit
// UART2_setOddParity()      Set parity bit, odd
// UART2_setEvenParity()     Set parity bit, even
//
// UART2_ready()             Check if UART is ready to write (TX FIFO not full)
// UART2_available()         Check if there is something to read (RX FIFO not empty)
// UART2_completed()         Check if transmission is completed (UART not busy)
//
// UART2_read()              Read character via UART
// UART2_write(c)            Send character via UART
//
// UART2_enable()            Enable UART
// UART2_disable()           Disable UART
// UART2_TX_enable()         Enable transmitter
// UART2_TX_disable()        Disable transmitter
// UART2_RX_enable()         Enable receiver
// UART2_RX_disable()        Disable receiver
//
// If print functions are activated (see below, print.h must be included):
// -----------------------------------------------------------------------
// UART2_printf(f, ...)      printf (supports %s, %c, %d, %u, %x, %b, %02d, %%)
// UART2_printD(n)           Print decimal value
// UART2_printW(n)           Print 32-bit hex word value
// UART2_printH(n)           Print 16-bit hex half-word value
// UART2_printB(n)           Print  8-bit hex byte value
// UART2_printS(s)           Print string
// UART2_print(s)            Print string (alias)
// UART2_println(s)          Print string with newline
// UART2_newline()           Send newline
//
// UART pins (set below in UART parameters):
// -----------------------------------------
// UART0 TX: GP0, GP12, GP16, GP28    RX: GP1, GP13, GP17, GP29
// UART1 TX: GP4, GP8,  GP20, GP24    RX: GP5, GP9,  GP21, GP25
//
// Notes:
// ------
// - The UART (UART0 or UART1) is selected automatically by the TX pin. TX and RX pin
//   must belong to the same UART.
// - Don't use the same UART for this and other libraries (e.g. debug_serial, uart)
//   at the same time. Default: UART1 on GP4/GP5.
// - The UART is clocked by clk_peri (F_CPU), max BAUD rate is F_CPU / 16.
// - The UART has a 32-byte RX and TX FIFO.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// UART parameters
#define UART2_TX_PIN      GP4             // TX pin (see above)
#define UART2_RX_PIN      GP5             // RX pin (see above)
#define UART2_BAUD         115200          // default UART baud rate
#define UART2_PRINT        0               // 1: include print function (needs print.h)

// UART instance (selected by TX pin, evaluated by the compiler)
#define UART2_NUM          (((UART2_TX_PIN >> 2) ^ (UART2_TX_PIN >> 3)) & 1)
#define UART2_DEV         (UART2_NUM ? UART1 : UART0)
#define UART2_RESET        (UART2_NUM ? RESETS_RESET_UART1 : RESETS_RESET_UART0)
#define UART2_IRQn         (UART2_NUM ? UART1_IRQ_IRQn : UART0_IRQ_IRQn)
#define UART2_DREQ_TX      (UART2_NUM ? DREQ_UART1_TX : DREQ_UART0_TX)
#define UART2_DREQ_RX      (UART2_NUM ? DREQ_UART1_RX : DREQ_UART0_RX)

_Static_assert(((UART2_TX_PIN & 3) == 0) && ((UART2_RX_PIN & 3) == 1), "Wrong UART pins");
_Static_assert(UART2_NUM == (((UART2_RX_PIN >> 2) ^ (UART2_RX_PIN >> 3)) & 1),
               "UART TX and RX pin must belong to the same UART");

// UART macros
#define UART2_ready()          (!(UART2_DEV->UARTFR & UART_UARTFR_TXFF))
#define UART2_available()      (!(UART2_DEV->UARTFR & UART_UARTFR_RXFE))
#define UART2_completed()      (!(UART2_DEV->UARTFR & UART_UARTFR_BUSY))

#define UART2_enable()         REG_SET(UART2_DEV->UARTCR, UART_UARTCR_UARTEN)
#define UART2_disable()        REG_CLR(UART2_DEV->UARTCR, UART_UARTCR_UARTEN)
#define UART2_TX_enable()      REG_SET(UART2_DEV->UARTCR, UART_UARTCR_TXE)
#define UART2_TX_disable()     REG_CLR(UART2_DEV->UARTCR, UART_UARTCR_TXE)
#define UART2_RX_enable()      REG_SET(UART2_DEV->UARTCR, UART_UARTCR_RXE)
#define UART2_RX_disable()     REG_CLR(UART2_DEV->UARTCR, UART_UARTCR_RXE)

#define UART2_setDataBits(n)   UART2_setLCR(UART_UARTLCR_H_WLEN_Msk,                     \
                              (uint32_t)(((n) - 5) & 3) << UART_UARTLCR_H_WLEN_Pos)
#define UART2_setStopBits(n)   UART2_setLCR(UART_UARTLCR_H_STP2, ((n) == 2) ? UART_UARTLCR_H_STP2 : 0)
#define UART2_setNoParity()    UART2_setLCR(UART_UARTLCR_H_PEN | UART_UARTLCR_H_EPS, 0)
#define UART2_setEvenParity()  UART2_setLCR(UART_UARTLCR_H_PEN | UART_UARTLCR_H_EPS,      \
                                          UART_UARTLCR_H_PEN | UART_UARTLCR_H_EPS)
#define UART2_setOddParity()   UART2_setLCR(UART_UARTLCR_H_PEN | UART_UARTLCR_H_EPS,      \
                                          UART_UARTLCR_H_PEN)

// UART functions
void UART2_init(void);                     // init UART with default BAUD rate
char UART2_read(void);                     // read character via UART
void UART2_write(const char c);            // send character via UART
void UART2_setBAUD(uint32_t baud);         // set BAUD rate
void UART2_setLCR(uint32_t mask, uint32_t val);  // change line control register

// Additional print functions (if activated, see above)
#if UART2_PRINT == 1
#include "print.h"
#define UART2_printD(n)        printD(UART2_write, n)   // print decimal as string
#define UART2_printW(n)        printW(UART2_write, n)   // print word as string
#define UART2_printH(n)        printH(UART2_write, n)   // print half-word as string
#define UART2_printB(n)        printB(UART2_write, n)   // print byte as string
#define UART2_printS(s)        printS(UART2_write, s)   // print string
#define UART2_println(s)       println(UART2_write, s)  // print string with newline
#define UART2_print            UART2_printS             // alias
#define UART2_newline()        UART2_write('\n')        // send newline
#define UART2_printf(f, ...)   printF(UART2_write, f, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
};
#endif
