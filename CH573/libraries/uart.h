// ===================================================================================
// Basic UART Functions for CH573/CH571  (no buffer/interrupt/DMA)            * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// UARTx_init()             Init UART with default BAUD rate (115200), 8N1
// UARTx_setBAUD(n)         Set BAUD rate
// UARTx_setStopBits(n)     Set number of stop bits (n = 1, 2)
// UARTx_setNoParity()      Set no parity bit
// UARTx_setOddParity()     Set parity bit, odd
// UARTx_setEvenParity()    Set parity bit, even
//
// UARTx_ready()            Check if UART is ready to write (FIFO not full)
// UARTx_available()        Check if there is something to read
// UARTx_completed()        Check if transmission is completed
//
// UARTx_read()             Read character via UART
// UARTx_write(c)           Send character via UART
//
// UARTx_TX_enable()        Enable transmitter (TX pin)
// UARTx_TX_disable()       Disable transmitter (TX pin)
//
// If print functions are activated (see below, print.h must be included):
// -----------------------------------------------------------------------
// UARTx_printf(f, ...)     printf (supports %s, %c, %d, %u, %x, %b, %02d, %%)
// UARTx_printD(n)          Print decimal value
// UARTx_printW(n)          Print 32-bit hex word value
// UARTx_printH(n)          Print 16-bit hex half-word value
// UARTx_printB(n)          Print  8-bit hex byte value
// UARTx_printS(s)          Print string
// UARTx_print(s)           Print string (alias)
// UARTx_println(s)         Print string with newline
// UARTx_newline()          Send newline
//
// UARTx remap settings (set below in UART parameters):
// ----------------------------------------------------
//               --- UART0 ---    --- UART1 ---    --- UART2 ---    --- UART3 ---
// UARTx_REMAP   TX-pin RX-pin    TX-pin RX-pin    TX-pin RX-pin    TX-pin RX-pin
//         0      PB7    PB4       PA9    PA8       PB23   PB22      PA5    PA4
//         1      PA14   PA15      PB13   PB12
//
// Notes:
// ------
// - x = 0..3 (UART0..UART3). The UARTs have an 8-byte FIFO for each direction.
// - The baud rate is calculated from F_CPU, change it after the system clock.
// - UART0 additionally supports modem signals, which are not used here.
// - The UART2 pins are the BOOT (PB22) and RST (PB23) key pins on many boards.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// ===================================================================================
// UART Parameters
// ===================================================================================
#define UART_PRINT        0               // 1: include print functions (needs print.h)

#define UART0_BAUD        115200          // default UART0 baud rate
#define UART0_REMAP       0               // UART0 pin remapping (see above)

#define UART1_BAUD        115200          // default UART1 baud rate
#define UART1_REMAP       0               // UART1 pin remapping (see above)

#define UART2_BAUD        115200          // default UART2 baud rate

#define UART3_BAUD        115200          // default UART3 baud rate

// ===================================================================================
// UART Macros
// ===================================================================================
#define UART0_ready()         (R8_UART0_TFC != UART_FIFO_SIZE)          // ready to write
#define UART0_available()     (R8_UART0_LSR & RB_LSR_DATA_RDY)           // ready to read
#define UART0_completed()     (R8_UART0_LSR & RB_LSR_TX_ALL_EMP)         // transmission completed

#define UART0_TX_enable()     R8_UART0_IER |=  RB_IER_TXD_EN              // enable transmitter
#define UART0_TX_disable()    R8_UART0_IER &= ~RB_IER_TXD_EN              // disable transmitter

#define UART0_setBAUD(n)      R16_UART0_DL = (uint16_t)(((10 * F_CPU / 8 / (n)) + 5) / 10)
#define UART0_setStopBits(n)  ((n)==2 ? (R8_UART0_LCR |= RB_LCR_STOP_BIT) : (R8_UART0_LCR &= ~RB_LCR_STOP_BIT))
#define UART0_setEvenParity() R8_UART0_LCR = (R8_UART0_LCR & ~RB_LCR_PAR_MOD) | RB_LCR_PAR_EN | (1 << 4)
#define UART0_setOddParity()  R8_UART0_LCR = (R8_UART0_LCR & ~RB_LCR_PAR_MOD) | RB_LCR_PAR_EN
#define UART0_setNoParity()   R8_UART0_LCR &= ~RB_LCR_PAR_EN

#define UART1_ready()         (R8_UART1_TFC != UART_FIFO_SIZE)          // ready to write
#define UART1_available()     (R8_UART1_LSR & RB_LSR_DATA_RDY)           // ready to read
#define UART1_completed()     (R8_UART1_LSR & RB_LSR_TX_ALL_EMP)         // transmission completed

#define UART1_TX_enable()     R8_UART1_IER |=  RB_IER_TXD_EN              // enable transmitter
#define UART1_TX_disable()    R8_UART1_IER &= ~RB_IER_TXD_EN              // disable transmitter

#define UART1_setBAUD(n)      R16_UART1_DL = (uint16_t)(((10 * F_CPU / 8 / (n)) + 5) / 10)
#define UART1_setStopBits(n)  ((n)==2 ? (R8_UART1_LCR |= RB_LCR_STOP_BIT) : (R8_UART1_LCR &= ~RB_LCR_STOP_BIT))
#define UART1_setEvenParity() R8_UART1_LCR = (R8_UART1_LCR & ~RB_LCR_PAR_MOD) | RB_LCR_PAR_EN | (1 << 4)
#define UART1_setOddParity()  R8_UART1_LCR = (R8_UART1_LCR & ~RB_LCR_PAR_MOD) | RB_LCR_PAR_EN
#define UART1_setNoParity()   R8_UART1_LCR &= ~RB_LCR_PAR_EN

#define UART2_ready()         (R8_UART2_TFC != UART_FIFO_SIZE)          // ready to write
#define UART2_available()     (R8_UART2_LSR & RB_LSR_DATA_RDY)           // ready to read
#define UART2_completed()     (R8_UART2_LSR & RB_LSR_TX_ALL_EMP)         // transmission completed

#define UART2_TX_enable()     R8_UART2_IER |=  RB_IER_TXD_EN              // enable transmitter
#define UART2_TX_disable()    R8_UART2_IER &= ~RB_IER_TXD_EN              // disable transmitter

#define UART2_setBAUD(n)      R16_UART2_DL = (uint16_t)(((10 * F_CPU / 8 / (n)) + 5) / 10)
#define UART2_setStopBits(n)  ((n)==2 ? (R8_UART2_LCR |= RB_LCR_STOP_BIT) : (R8_UART2_LCR &= ~RB_LCR_STOP_BIT))
#define UART2_setEvenParity() R8_UART2_LCR = (R8_UART2_LCR & ~RB_LCR_PAR_MOD) | RB_LCR_PAR_EN | (1 << 4)
#define UART2_setOddParity()  R8_UART2_LCR = (R8_UART2_LCR & ~RB_LCR_PAR_MOD) | RB_LCR_PAR_EN
#define UART2_setNoParity()   R8_UART2_LCR &= ~RB_LCR_PAR_EN

#define UART3_ready()         (R8_UART3_TFC != UART_FIFO_SIZE)          // ready to write
#define UART3_available()     (R8_UART3_LSR & RB_LSR_DATA_RDY)           // ready to read
#define UART3_completed()     (R8_UART3_LSR & RB_LSR_TX_ALL_EMP)         // transmission completed

#define UART3_TX_enable()     R8_UART3_IER |=  RB_IER_TXD_EN              // enable transmitter
#define UART3_TX_disable()    R8_UART3_IER &= ~RB_IER_TXD_EN              // disable transmitter

#define UART3_setBAUD(n)      R16_UART3_DL = (uint16_t)(((10 * F_CPU / 8 / (n)) + 5) / 10)
#define UART3_setStopBits(n)  ((n)==2 ? (R8_UART3_LCR |= RB_LCR_STOP_BIT) : (R8_UART3_LCR &= ~RB_LCR_STOP_BIT))
#define UART3_setEvenParity() R8_UART3_LCR = (R8_UART3_LCR & ~RB_LCR_PAR_MOD) | RB_LCR_PAR_EN | (1 << 4)
#define UART3_setOddParity()  R8_UART3_LCR = (R8_UART3_LCR & ~RB_LCR_PAR_MOD) | RB_LCR_PAR_EN
#define UART3_setNoParity()   R8_UART3_LCR &= ~RB_LCR_PAR_EN

// ===================================================================================
// UART Functions
// ===================================================================================
void UART0_init(void);                    // init UART with default BAUD rate
char UART0_read(void);                    // read character via UART
void UART0_write(const char c);           // send character via UART

void UART1_init(void);                    // init UART with default BAUD rate
char UART1_read(void);                    // read character via UART
void UART1_write(const char c);           // send character via UART

void UART2_init(void);                    // init UART with default BAUD rate
char UART2_read(void);                    // read character via UART
void UART2_write(const char c);           // send character via UART

void UART3_init(void);                    // init UART with default BAUD rate
char UART3_read(void);                    // read character via UART
void UART3_write(const char c);           // send character via UART

// ===================================================================================
// Additional Print Functions (if activated, see above)
// ===================================================================================
#if UART_PRINT == 1

#include "print.h"

#define UART0_printD(n)       printD(UART0_write, n)    // print decimal as string
#define UART0_printW(n)       printW(UART0_write, n)    // print word as string
#define UART0_printH(n)       printH(UART0_write, n)    // print half-word as string
#define UART0_printB(n)       printB(UART0_write, n)    // print byte as string
#define UART0_printS(s)       printS(UART0_write, s)    // print string
#define UART0_println(s)      println(UART0_write, s)   // print string with newline
#define UART0_print           UART0_printS              // alias
#define UART0_newline()       UART0_write('\n')         // send newline
#define UART0_printf(f, ...)  printF(UART0_write, f, ##__VA_ARGS__)

#define UART1_printD(n)       printD(UART1_write, n)    // print decimal as string
#define UART1_printW(n)       printW(UART1_write, n)    // print word as string
#define UART1_printH(n)       printH(UART1_write, n)    // print half-word as string
#define UART1_printB(n)       printB(UART1_write, n)    // print byte as string
#define UART1_printS(s)       printS(UART1_write, s)    // print string
#define UART1_println(s)      println(UART1_write, s)   // print string with newline
#define UART1_print           UART1_printS              // alias
#define UART1_newline()       UART1_write('\n')         // send newline
#define UART1_printf(f, ...)  printF(UART1_write, f, ##__VA_ARGS__)

#define UART2_printD(n)       printD(UART2_write, n)    // print decimal as string
#define UART2_printW(n)       printW(UART2_write, n)    // print word as string
#define UART2_printH(n)       printH(UART2_write, n)    // print half-word as string
#define UART2_printB(n)       printB(UART2_write, n)    // print byte as string
#define UART2_printS(s)       printS(UART2_write, s)    // print string
#define UART2_println(s)      println(UART2_write, s)   // print string with newline
#define UART2_print           UART2_printS              // alias
#define UART2_newline()       UART2_write('\n')         // send newline
#define UART2_printf(f, ...)  printF(UART2_write, f, ##__VA_ARGS__)

#define UART3_printD(n)       printD(UART3_write, n)    // print decimal as string
#define UART3_printW(n)       printW(UART3_write, n)    // print word as string
#define UART3_printH(n)       printH(UART3_write, n)    // print half-word as string
#define UART3_printB(n)       printB(UART3_write, n)    // print byte as string
#define UART3_printS(s)       printS(UART3_write, s)    // print string
#define UART3_println(s)      println(UART3_write, s)   // print string with newline
#define UART3_print           UART3_printS              // alias
#define UART3_newline()       UART3_write('\n')         // send newline
#define UART3_printf(f, ...)  printF(UART3_write, f, ##__VA_ARGS__)

#endif // UART_PRINT = 1

#ifdef __cplusplus
};
#endif
