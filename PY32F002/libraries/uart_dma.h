// ===================================================================================
// UART1 with DMA RX Buffer for PY32F0xx                                      * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// UART_init()              Init UART with 8N1 and default BAUD rate (115200)
// UART_setBaud(n)          Set BAUD rate
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
// UART_MAP   0     1     2     3     4     5     6     7
// TX-pin    PA2   PA7   PA9   PA14  PA14  PB6   PF1   No mapping
// RX-pin    PA3   PA8   PA10  PA13  PA15  PB7   PF0   No mapping
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// UART parameters
#define UART_BAUD             115200    // default UART baud rate
#define UART_RX_BUF_SIZE      64        // UART RX buffer size
#define UART_MAP              0         // UART pin mapping (see above)
#define UART_DMA_CHANNEL      1         // DMA channel (1 - 3)
#define UART_PRINT            0         // 1: include print functions (needs print.h)

// UART macros
#define UART_ready()          (USART1->SR & USART_SR_TXE)     // ready to write
#define UART_completed()      (USART1->SR & USART_SR_TC)      // transmission completed

#define UART_enable()         USART1->CR1 |= USART_CR1_UE     // enable USART
#define UART_disable()        USART1->CR1 &= ~USART_CR1_UE    // disable USART
#define UART_TX_enable()      USART1->CR1 |= USART_CR1_TE     // enable transmitter
#define UART_TX_disable()     USART1->CR1 &= ~USART_CR1_TE    // disable transmitter
#define UART_RX_enable()      USART1->CR1 |= USART_CR1_RE     // enable receiver
#define UART_RX_disable()     USART1->CR1 &= ~USART_CR1_RE    // disable receiver

#define UART_setBAUD(n)       USART1->BRR = ((2*F_CPU/(n))+1)/2   // set BAUD rate
#define UART_setDataBits(n)   (n==9 ? (USART1->CR1 |= USART_CR1_M) : (USART1->CR1 &= ~USART_CR1_M))
#define UART_setStopBits(n)   (n==2 ? (USART1->CR2 |= USART_CR2_STOP) : (USART1->CR2 &= ~USART_CR2_STOP))
#define UART_setEvenParity()  {USART1->CR1 |= USART_CR1_PCE; USART1->CR1 &= ~USART_CR1_PS;}
#define UART_setOddParity()   {USART1->CR1 |= USART_CR1_PCE; USART1->CR1 |=  USART_CR1_PS;}
#define UART_setNoParity()    USART1->CR1 &= ~USART_CR1_PCE

// UART functions
void UART_init(void);                   // init UART with default BAUD rate
char UART_read(void);                   // read character via UART
void UART_write(const char c);          // send character via UART
uint8_t UART_available(void);           // check if there is something to read

// Additional print functions (if activated, see above)
#if UART_PRINT == 1
#include "print.h"
#define UART_printD(n)    printD(UART_write, n)   // print decimal as string
#define UART_printW(n)    printW(UART_write, n)   // print word as string
#define UART_printH(n)    printH(UART_write, n)   // print half-word as string
#define UART_printB(n)    printB(UART_write, n)   // print byte as string
#define UART_printS(s)    printS(UART_write, s)   // print string
#define UART_println(s)   println(UART_write, s)  // print string with newline
#define UART_print        UART_printS             // alias
#define UART_newline()    UART_write('\n')        // send newline
#define UART_printf(f, ...)   printF(UART_write, f, ##__VA_ARGS__)
#endif

// DMA channel defines
#if   UART_DMA_CHANNEL == 1
  #define UART_DMA_CHAN   DMA1_Channel1
  #define UART_DMA_POS    SYSCFG_CFGR3_DMA1_MAP_Pos
#elif UART_DMA_CHANNEL == 2
  #define UART_DMA_CHAN   DMA1_Channel2
  #define UART_DMA_POS    SYSCFG_CFGR3_DMA2_MAP_Pos
#elif UART_DMA_CHANNEL == 3
  #define UART_DMA_CHAN   DMA1_Channel3
  #define UART_DMA_POS    SYSCFG_CFGR3_DMA3_MAP_Pos
#endif

#ifdef __cplusplus
};
#endif
