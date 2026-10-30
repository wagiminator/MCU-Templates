// ===================================================================================
// Basic USART1 Functions for STM32F03x  (no buffer, no interrupt, no DMA)    * v1.1 *
// ===================================================================================
//
// Functions available:
// --------------------
// UART_init()              Init UART with 8N1 and default BAUD rate (115200)
// UART_setBAUD(n)          Set BAUD rate
// UART_setDataBits(n)      Set number of data bits (n = 8, 9)
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
// UART_enable()            Enable USART
// UART_disable()           Disable USART
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
// USART1 pin mapping (set below in UART parameters):
// --------------------------------------------------
// UART_MAP    0     1     2     3     4
// TX-pin     PA2   PA9   PA14  PB6   no mapping
// RX-pin     PA3   PA10  PA15  PB7   no mapping
// (MAP 0 and 2 only on STM32F030x4/x6 and STM32F031, on STM32F030x8/xC
//  these pins belong to USART2; PA14 is also SWCLK!)
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// UART parameters
#define UART_BAUD             115200      // default UART baud rate
#define UART_MAP              1           // UART pin mapping (see above)
#define UART_PRINT            0           // 1: include print functions (needs print.h)

// BAUD rate check (oversampling by 16: BRR >= 16)
#if (F_CPU / UART_BAUD) < 16
  #warning UART_BAUD too high for F_CPU
#endif

// UART macros
#define UART_ready()          (USART1->ISR & USART_ISR_TXE)         // ready to write
#define UART_available()      (USART1->ISR & USART_ISR_RXNE)        // ready to read
#define UART_completed()      (USART1->ISR & USART_ISR_TC)          // transmission completed

// Write USART configuration register (BRR, M, PCE, PS and STOP can only be written
// while the USART is disabled; an ongoing transmission is completed first)
static inline void UART_config(volatile uint32_t* reg, uint32_t mask, uint32_t val) {
  uint32_t cr1 = USART1->CR1;
  if(cr1 & USART_CR1_UE) {
    if(cr1 & USART_CR1_TE) while(!(USART1->ISR & USART_ISR_TC)); // wait for TX completed
    USART1->CR1 = cr1 & ~USART_CR1_UE;                           // disable USART
  }
  *reg = (*reg & ~mask) | val;                                  // write configuration
  if(cr1 & USART_CR1_UE) USART1->CR1 |= USART_CR1_UE;            // re-enable USART
}

#define UART_enable()         (USART1->CR1 |=  USART_CR1_UE)        // enable USART
#define UART_disable()        (USART1->CR1 &= ~USART_CR1_UE)        // disable USART
#define UART_TX_enable()      (USART1->CR1 |=  USART_CR1_TE)        // enable transmitter
#define UART_TX_disable()     (USART1->CR1 &= ~USART_CR1_TE)        // disable transmitter
#define UART_RX_enable()      (USART1->CR1 |=  USART_CR1_RE)        // enable receiver
#define UART_RX_disable()     (USART1->CR1 &= ~USART_CR1_RE)        // disable receiver

#define UART_setBAUD(n)       UART_config(&USART1->BRR, 0xffff, ((2*F_CPU/(n))+1)/2)
#define UART_setDataBits(n)   UART_config(&USART1->CR1, USART_CR1_M, ((n)==9 ? USART_CR1_M : 0))
#define UART_setStopBits(n)   UART_config(&USART1->CR2, USART_CR2_STOP, ((n)==2 ? USART_CR2_STOP_1 : 0))
#define UART_setEvenParity()  UART_config(&USART1->CR1, USART_CR1_PCE | USART_CR1_PS, USART_CR1_PCE)
#define UART_setOddParity()   UART_config(&USART1->CR1, USART_CR1_PCE | USART_CR1_PS, USART_CR1_PCE | USART_CR1_PS)
#define UART_setNoParity()    UART_config(&USART1->CR1, USART_CR1_PCE, 0)

// UART functions
void UART_init(void);                   // init UART with default BAUD rate
char UART_read(void);                   // read character via UART
void UART_write(const char c);          // send character via UART

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
