// ===================================================================================
// Basic USART2 Functions for STM32F03x  (no buffer, no interrupt, no DMA)    * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// UART2_init()             Init UART with 8N1 and default BAUD rate (115200)
// UART2_setBAUD(n)         Set BAUD rate
// UART2_setDataBits(n)     Set number of data bits (n = 8, 9)
// UART2_setStopBits(n)     Set number of stop bits (n = 1, 2)
// UART2_setNoParity()      Set no parity bit
// UART2_setOddParity()     Set parity bit, odd
// UART2_setEvenParity()    Set parity bit, even
//
// UART2_ready()            Check if UART is ready to write
// UART2_available()        Check if there is something to read
// UART2_completed()        Check if transmission is completed
//
// UART2_read()             Read character via UART
// UART2_write(c)           Send character via UART
//
// UART2_enable()           Enable USART
// UART2_disable()          Disable USART
// UART2_TX_enable()        Enable transmitter
// UART2_TX_disable()       Disable transmitter
// UART2_RX_enable()        Enable receiver
// UART2_RX_disable()       Disable receiver
//
// If print functions are activated (see below, print.h must be included):
// -----------------------------------------------------------------------
// UART2_printf(f, ...)     printf (supports %s, %c, %d, %u, %x, %b, %02d, %%)
// UART2_printD(n)          Print decimal value
// UART2_printW(n)          Print 32-bit hex word value
// UART2_printH(n)          Print 16-bit hex half-word value
// UART2_printB(n)          Print  8-bit hex byte value
// UART2_printS(s)          Print string
// UART2_print(s)           Print string (alias)
// UART2_println(s)         Print string with newline
// UART2_newline()          Send newline
//
// USART2 pin mapping (set below in UART parameters):
// --------------------------------------------------
// UART2_MAP   0     1     2
// TX-pin     PA2   PA14  no mapping
// RX-pin     PA3   PA15  no mapping
// (USART2 is only available on STM32F030x8 and STM32F030xC, PA14 is also SWCLK!)
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// UART parameters
#define UART2_BAUD            115200      // default UART baud rate
#define UART2_MAP             0           // UART pin mapping (see above)
#define UART2_PRINT           0           // 1: include print functions (needs print.h)

// BAUD rate check (oversampling by 16: BRR >= 16)
#if (F_CPU / UART2_BAUD) < 16
  #warning UART2_BAUD too high for F_CPU
#endif

// UART macros
#define UART2_ready()         (USART2->ISR & USART_ISR_TXE)         // ready to write
#define UART2_available()     (USART2->ISR & USART_ISR_RXNE)        // ready to read
#define UART2_completed()     (USART2->ISR & USART_ISR_TC)          // transmission completed

// Write USART configuration register (BRR, M, PCE, PS and STOP can only be written
// while the USART is disabled; an ongoing transmission is completed first)
static inline void UART2_config(volatile uint32_t* reg, uint32_t mask, uint32_t val) {
  uint32_t cr1 = USART2->CR1;
  if(cr1 & USART_CR1_UE) {
    if(cr1 & USART_CR1_TE) while(!(USART2->ISR & USART_ISR_TC)); // wait for TX completed
    USART2->CR1 = cr1 & ~USART_CR1_UE;                           // disable USART
  }
  *reg = (*reg & ~mask) | val;                                  // write configuration
  if(cr1 & USART_CR1_UE) USART2->CR1 |= USART_CR1_UE;            // re-enable USART
}

#define UART2_enable()        (USART2->CR1 |=  USART_CR1_UE)        // enable USART
#define UART2_disable()       (USART2->CR1 &= ~USART_CR1_UE)        // disable USART
#define UART2_TX_enable()     (USART2->CR1 |=  USART_CR1_TE)        // enable transmitter
#define UART2_TX_disable()    (USART2->CR1 &= ~USART_CR1_TE)        // disable transmitter
#define UART2_RX_enable()     (USART2->CR1 |=  USART_CR1_RE)        // enable receiver
#define UART2_RX_disable()    (USART2->CR1 &= ~USART_CR1_RE)        // disable receiver

#define UART2_setBAUD(n)      UART2_config(&USART2->BRR, 0xffff, ((2*F_CPU/(n))+1)/2)
#define UART2_setDataBits(n)  UART2_config(&USART2->CR1, USART_CR1_M, ((n)==9 ? USART_CR1_M : 0))
#define UART2_setStopBits(n)  UART2_config(&USART2->CR2, USART_CR2_STOP, ((n)==2 ? USART_CR2_STOP_1 : 0))
#define UART2_setEvenParity() UART2_config(&USART2->CR1, USART_CR1_PCE | USART_CR1_PS, USART_CR1_PCE)
#define UART2_setOddParity()  UART2_config(&USART2->CR1, USART_CR1_PCE | USART_CR1_PS, USART_CR1_PCE | USART_CR1_PS)
#define UART2_setNoParity()   UART2_config(&USART2->CR1, USART_CR1_PCE, 0)

// UART functions
void UART2_init(void);                  // init UART with default BAUD rate
char UART2_read(void);                  // read character via UART
void UART2_write(const char c);         // send character via UART

// Additional print functions (if activated, see above)
#if UART2_PRINT == 1
#include "print.h"
#define UART2_printD(n)       printD(UART2_write, n)  // print decimal as string
#define UART2_printW(n)       printW(UART2_write, n)  // print word as string
#define UART2_printH(n)       printH(UART2_write, n)  // print half-word as string
#define UART2_printB(n)       printB(UART2_write, n)  // print byte as string
#define UART2_printS(s)       printS(UART2_write, s)  // print string
#define UART2_println(s)      println(UART2_write, s) // print string with newline
#define UART2_print           UART2_printS            // alias
#define UART2_newline()       UART2_write('\n')       // send newline
#define UART2_printf(f, ...)  printF(UART2_write, f, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
};
#endif
