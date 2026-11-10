// ===================================================================================
// UART with RX Interrupt and Ring Buffer for CH573/CH571                     * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// UART_init()              Init UART with default BAUD rate (115200), 8N1
// UART_setBAUD(n)          Set BAUD rate
// UART_setStopBits(n)      Set number of stop bits (n = 1, 2)
// UART_setNoParity()       Set no parity bit
// UART_setOddParity()      Set parity bit, odd
// UART_setEvenParity()     Set parity bit, even
//
// UART_ready()             Check if UART is ready to write (FIFO not full)
// UART_available()         Check if there is something in the RX buffer
// UART_completed()         Check if transmission is completed
//
// UART_read()              Read character from RX buffer (waits)
// UART_write(c)            Send character via UART
// UART_writeBuffer(buf,len) Send buffer (*buf) with length (len) via UART
// UART_readBuffer(buf,len) Read buffer (*buf) with length (len) via UART (waits)
//
// UART_TX_enable()         Enable transmitter (TX pin)
// UART_TX_disable()        Disable transmitter (TX pin)
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
// UART selection and pin mapping (set below in UART parameters):
// --------------------------------------------------------------
//               --- UART0 ---    --- UART1 ---    --- UART2 ---    --- UART3 ---
// UART_REMAP    TX-pin RX-pin    TX-pin RX-pin    TX-pin RX-pin    TX-pin RX-pin
//         0      PB7    PB4       PA9    PA8       PB23   PB22      PA5    PA4
//         1      PA14   PA15      PB13   PB12
//
// Notes:
// ------
// - The UART (0..3) is selected by UART_NUM below. Received bytes are written into a
//   ring buffer (size set below) by the UART interrupt (RX FIFO trigger level and
//   timeout). If the buffer is full, further received bytes are discarded.
// - The baud rate is calculated from F_CPU, change it after the system clock.
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
#define UART_NUM          1               // UART to be used (0..3)
#define UART_REMAP        0               // UART pin remapping (see above)
#define UART_BAUD         115200          // default UART baud rate
#define UART_RX_BUF_SIZE  64              // RX ring buffer size (2, 4, 8, .. 128)
#define UART_PRINT        0               // 1: include print functions (needs print.h)

// Interrupt enable check
#if SYS_USE_VECTORS == 0
  #error Interrupt vector table must be enabled (SYS_USE_VECTORS in system.h)!
#endif

// ===================================================================================
// UART Registers (selected by UART_NUM)
// ===================================================================================
#define UART_CAT3(a,b,c)      a##b##c
#define UART_REG(t,n,r)       UART_CAT3(t,n,r)
#define UART_R_RBR              UART_REG(R8_UART, UART_NUM, _RBR)
#define UART_R_THR              UART_REG(R8_UART, UART_NUM, _THR)
#define UART_R_IER              UART_REG(R8_UART, UART_NUM, _IER)
#define UART_R_IIR              UART_REG(R8_UART, UART_NUM, _IIR)
#define UART_R_FCR              UART_REG(R8_UART, UART_NUM, _FCR)
#define UART_R_LCR              UART_REG(R8_UART, UART_NUM, _LCR)
#define UART_R_LSR              UART_REG(R8_UART, UART_NUM, _LSR)
#define UART_R_MCR              UART_REG(R8_UART, UART_NUM, _MCR)
#define UART_R_TFC              UART_REG(R8_UART, UART_NUM, _TFC)
#define UART_R_DIV              UART_REG(R8_UART, UART_NUM, _DIV)
#define UART_R_DL               UART_REG(R16_UART, UART_NUM, _DL)
#define UART_IRQn             UART_REG(UART, UART_NUM, _IRQn)
#define UART_IRQHandler       UART_REG(UART, UART_NUM, _IRQHandler)
#define UART_FIFO_SIZE        8

// ===================================================================================
// UART Macros
// ===================================================================================
#define UART_ready()          (UART_R_TFC != UART_FIFO_SIZE)      // ready to write
#define UART_available()      (UART_rptr != UART_wptr)            // something in buffer?
#define UART_completed()      (UART_R_LSR & RB_LSR_TX_ALL_EMP)    // transmission completed

#define UART_TX_enable()      UART_R_IER |=  RB_IER_TXD_EN        // enable transmitter
#define UART_TX_disable()     UART_R_IER &= ~RB_IER_TXD_EN        // disable transmitter

#define UART_setBAUD(n)       UART_R_DL = (uint16_t)(((10 * F_CPU / 8 / (n)) + 5) / 10)
#define UART_setStopBits(n)   ((n)==2 ? (UART_R_LCR |= RB_LCR_STOP_BIT) : (UART_R_LCR &= ~RB_LCR_STOP_BIT))
#define UART_setEvenParity()  UART_R_LCR = (UART_R_LCR & ~RB_LCR_PAR_MOD) | RB_LCR_PAR_EN | (1 << 4)
#define UART_setOddParity()   UART_R_LCR = (UART_R_LCR & ~RB_LCR_PAR_MOD) | RB_LCR_PAR_EN
#define UART_setNoParity()    UART_R_LCR &= ~RB_LCR_PAR_EN

// ===================================================================================
// UART Functions
// ===================================================================================
void UART_init(void);                     // init UART with default BAUD rate
char UART_read(void);                     // read character from RX buffer
void UART_write(const char c);            // send character via UART
void UART_writeBuffer(const uint8_t* buf, uint16_t len); // send buffer
void UART_readBuffer(uint8_t* buf, uint16_t len); // receive buffer

// UART variables
extern volatile uint8_t UART_rptr;        // UART RX buffer read pointer
extern volatile uint8_t UART_wptr;        // UART RX buffer write pointer

// ===================================================================================
// Additional Print Functions (if activated, see above)
// ===================================================================================
#if UART_PRINT == 1
#include "print.h"
#define UART_printD(n)        printD(UART_write, n)     // print decimal as string
#define UART_printW(n)        printW(UART_write, n)     // print word as string
#define UART_printH(n)        printH(UART_write, n)     // print half-word as string
#define UART_printB(n)        printB(UART_write, n)     // print byte as string
#define UART_printS(s)        printS(UART_write, s)     // print string
#define UART_println(s)       println(UART_write, s)    // print string with newline
#define UART_print            UART_printS               // alias
#define UART_newline()        UART_write('\n')          // send newline
#define UART_printf(f, ...)   printF(UART_write, f, ##__VA_ARGS__)
#endif // UART_PRINT == 1

#ifdef __cplusplus
};
#endif
