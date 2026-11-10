// ===================================================================================
// Basic UART Functions for RP2040 (UART0/UART1, no buffer, no interrupt)     * v1.1 *
// ===================================================================================
//
// Functions available:
// --------------------
// UART_init()              Init UART with 8N1 and default BAUD rate (115200)
// UART_setBAUD(n)          Set BAUD rate
// UART_setDataBits(n)      Set number of data bits (n = 5, 6, 7, 8)
// UART_setStopBits(n)      Set number of stop bits (n = 1, 2)
// UART_setNoParity()       Set no parity bit
// UART_setOddParity()      Set parity bit, odd
// UART_setEvenParity()     Set parity bit, even
//
// UART_ready()             Check if UART is ready to write (TX FIFO not full)
// UART_available()         Check if there is something to read (RX FIFO not empty)
// UART_completed()         Check if transmission is completed (UART not busy)
//
// UART_read()              Read character via UART
// UART_write(c)            Send character via UART
// UART_writeBuffer(buf,len) Send buffer (*buf) with length (len) via UART
// UART_readBuffer(buf,len) Read buffer (*buf) with length (len) via UART (waits)
//
// UART_enable()            Enable UART
// UART_disable()           Disable UART
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
// UART pins (set below in UART parameters):
// -----------------------------------------
// UART0 TX: GP0, GP12, GP16, GP28    RX: GP1, GP13, GP17, GP29
// UART1 TX: GP4, GP8,  GP20, GP24    RX: GP5, GP9,  GP21, GP25
//
// Notes:
// ------
// - The UART (UART0 or UART1) is selected automatically by the TX pin. TX and RX pin
//   must belong to the same UART.
// - Don't use the same UART for this and other libraries (e.g. debug_serial, uart2)
//   at the same time. Default: UART0 on GP0/GP1 (Raspberry Pi Debug Probe).
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
#define UART_TX_PIN       GP0             // TX pin (see above)
#define UART_RX_PIN       GP1             // RX pin (see above)
#define UART_BAUD         115200          // default UART baud rate
#define UART_PRINT        0               // 1: include print function (needs print.h)

// UART instance (selected by TX pin, evaluated by the compiler)
#define UART_NUM          (((UART_TX_PIN >> 2) ^ (UART_TX_PIN >> 3)) & 1)
#define UART_DEV         (UART_NUM ? UART1 : UART0)
#define UART_RESET        (UART_NUM ? RESETS_RESET_UART1 : RESETS_RESET_UART0)
#define UART_IRQn         (UART_NUM ? UART1_IRQ_IRQn : UART0_IRQ_IRQn)
#define UART_DREQ_TX      (UART_NUM ? DREQ_UART1_TX : DREQ_UART0_TX)
#define UART_DREQ_RX      (UART_NUM ? DREQ_UART1_RX : DREQ_UART0_RX)

_Static_assert(((UART_TX_PIN & 3) == 0) && ((UART_RX_PIN & 3) == 1), "Wrong UART pins");
_Static_assert(UART_NUM == (((UART_RX_PIN >> 2) ^ (UART_RX_PIN >> 3)) & 1),
               "UART TX and RX pin must belong to the same UART");

// UART macros
#define UART_ready()          (!(UART_DEV->UARTFR & UART_UARTFR_TXFF))
#define UART_available()      (!(UART_DEV->UARTFR & UART_UARTFR_RXFE))
#define UART_completed()      (!(UART_DEV->UARTFR & UART_UARTFR_BUSY))

#define UART_enable()         REG_SET(UART_DEV->UARTCR, UART_UARTCR_UARTEN)
#define UART_disable()        REG_CLR(UART_DEV->UARTCR, UART_UARTCR_UARTEN)
#define UART_TX_enable()      REG_SET(UART_DEV->UARTCR, UART_UARTCR_TXE)
#define UART_TX_disable()     REG_CLR(UART_DEV->UARTCR, UART_UARTCR_TXE)
#define UART_RX_enable()      REG_SET(UART_DEV->UARTCR, UART_UARTCR_RXE)
#define UART_RX_disable()     REG_CLR(UART_DEV->UARTCR, UART_UARTCR_RXE)

#define UART_setDataBits(n)   UART_setLCR(UART_UARTLCR_H_WLEN_Msk,                     \
                              (uint32_t)(((n) - 5) & 3) << UART_UARTLCR_H_WLEN_Pos)
#define UART_setStopBits(n)   UART_setLCR(UART_UARTLCR_H_STP2, ((n) == 2) ? UART_UARTLCR_H_STP2 : 0)
#define UART_setNoParity()    UART_setLCR(UART_UARTLCR_H_PEN | UART_UARTLCR_H_EPS, 0)
#define UART_setEvenParity()  UART_setLCR(UART_UARTLCR_H_PEN | UART_UARTLCR_H_EPS,      \
                                          UART_UARTLCR_H_PEN | UART_UARTLCR_H_EPS)
#define UART_setOddParity()   UART_setLCR(UART_UARTLCR_H_PEN | UART_UARTLCR_H_EPS,      \
                                          UART_UARTLCR_H_PEN)

// UART functions
void UART_init(void);                     // init UART with default BAUD rate
char UART_read(void);                     // read character via UART
void UART_write(const char c);            // send character via UART
void UART_writeBuffer(const uint8_t* buf, uint16_t len); // send buffer
void UART_readBuffer(uint8_t* buf, uint16_t len); // receive buffer
void UART_setBAUD(uint32_t baud);         // set BAUD rate
void UART_setLCR(uint32_t mask, uint32_t val);  // change line control register

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
