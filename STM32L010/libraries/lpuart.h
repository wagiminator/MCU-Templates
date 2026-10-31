// ===================================================================================
// Basic LPUART1 Functions for STM32L0xx  (no buffer, no interrupt, no DMA)   * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// LPUART_init()            Init UART with 8N1 and default BAUD rate (115200)
// LPUART_setBAUD(n)        Set BAUD rate
// LPUART_setDataBits(n)    Set number of data bits (n = 7, 8, 9)
// LPUART_setStopBits(n)    Set number of stop bits (n = 1, 2)
// LPUART_setNoParity()     Set no parity bit
// LPUART_setOddParity()    Set parity bit, odd
// LPUART_setEvenParity()   Set parity bit, even
//
// LPUART_ready()           Check if UART is ready to write
// LPUART_available()       Check if there is something to read
// LPUART_completed()       Check if transmission is completed
//
// LPUART_read()            Read character via UART
// LPUART_write(c)          Send character via UART
//
// LPUART_enable()          Enable USART
// LPUART_disable()         Disable USART
// LPUART_TX_enable()       Enable transmitter
// LPUART_TX_disable()      Disable transmitter
// LPUART_RX_enable()       Enable receiver
// LPUART_RX_disable()      Disable receiver
// LPUART_LP_enable()       Enable wake from STOP mode (UESM)
// LPUART_LP_disable()      Disable wake from low-power
//
// If print functions are activated (see below, print.h must be included):
// -----------------------------------------------------------------------
// LPUART_printf(f, ...)    printf (supports %s, %c, %d, %u, %x, %b, %02d, %%)
// LPUART_printD(n)         Print decimal value
// LPUART_printW(n)         Print 32-bit hex word value
// LPUART_printH(n)         Print 16-bit hex half-word value
// LPUART_printB(n)         Print  8-bit hex byte value
// LPUART_printS(s)         Print string
// LPUART_print(s)          Print string (alias)
// LPUART_println(s)        Print string with newline
// LPUART_newline()         Send newline
//
// LPUART1 pin mapping (set below in UART parameters):
// ---------------------------------------------------
// LPUART_MAP   0     1     2     3     4
// TX-pin      PA2   PA14  PB6   PA1   No mapping
// RX-pin      PA3   PA13  PB7   PA0   No mapping
// (PA13/PA14 are also SWDIO/SWCLK! PB6 and PB7 are not available on TSSOP20)
//
// Notes:
// - LPUART clock is PCLK (= F_CPU) or LSE (LPUART_LSE = 1, max. 9600 BAUD, allows
//   wake-up from STOP mode, see LPUART_LP_enable()).
// - BAUD rate range: fCK/4096 <= BAUD <= fCK/3 (BRR = 256 * fCK / BAUD, RM0451,
//   chapter 22.4.4), e.g. F_CPU = 32MHz: 7813..10666666 BAUD, F_CPU = 2.1MHz:
//   512..699050 BAUD (higher rates have larger errors).
// - The LPUART has no FIFO.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// UART parameters
#define LPUART_BAUD           115200      // default UART baud rate
#define LPUART_MAP            0           // UART pin mapping (see above)
#define LPUART_LSE            0           // 1: use external 32.768kHz clock (max 9600 BAUD)
#define LPUART_PRINT          0           // 1: include print functions (needs print.h)

// UART macros
#define LPUART_ready()        (LPUART1->ISR & USART_ISR_TXE)         // ready to write
#define LPUART_available()    (LPUART1->ISR & USART_ISR_RXNE)        // ready to read
#define LPUART_completed()    (LPUART1->ISR & USART_ISR_TC)          // transmission completed

// Write USART configuration register (BRR, M, PCE, PS and STOP can only be
// written while the USART is disabled; an ongoing transmission is completed first)
static inline void LPUART_config(volatile uint32_t* reg, uint32_t mask, uint32_t val) {
  uint32_t cr1 = LPUART1->CR1;
  if(cr1 & USART_CR1_UE) {
    if(cr1 & USART_CR1_TE) while(!(LPUART1->ISR & USART_ISR_TC)); // wait for TX completed
    LPUART1->CR1 = cr1 & ~USART_CR1_UE;                           // disable USART
  }
  *reg = (*reg & ~mask) | val;                                  // write configuration
  if(cr1 & USART_CR1_UE) LPUART1->CR1 |= USART_CR1_UE;            // re-enable USART
}

#define LPUART_enable()       (LPUART1->CR1 |=  USART_CR1_UE)        // enable USART
#define LPUART_disable()      (LPUART1->CR1 &= ~USART_CR1_UE)        // disable USART
#define LPUART_TX_enable()    (LPUART1->CR1 |=  USART_CR1_TE)        // enable transmitter
#define LPUART_TX_disable()   (LPUART1->CR1 &= ~USART_CR1_TE)        // disable transmitter
#define LPUART_RX_enable()    (LPUART1->CR1 |=  USART_CR1_RE)        // enable receiver
#define LPUART_RX_disable()   (LPUART1->CR1 &= ~USART_CR1_RE)        // disable receiver
#define LPUART_LP_enable()    (LPUART1->CR1 |=  USART_CR1_UESM)      // enable wake from low-power
#define LPUART_LP_disable()   (LPUART1->CR1 &= ~USART_CR1_UESM)      // disable wake from low-power

#define LPUART_setDataBits(n)   LPUART_config(&LPUART1->CR1, USART_CR1_M1 | USART_CR1_M0, \
                                ((n)==9 ? USART_CR1_M0 : ((n)==7 ? USART_CR1_M1 : 0)))
#define LPUART_setStopBits(n)   LPUART_config(&LPUART1->CR2, USART_CR2_STOP, ((n)==2 ? USART_CR2_STOP_1 : 0))
#define LPUART_setEvenParity()  LPUART_config(&LPUART1->CR1, USART_CR1_PCE | USART_CR1_PS, USART_CR1_PCE)
#define LPUART_setOddParity()   LPUART_config(&LPUART1->CR1, USART_CR1_PCE | USART_CR1_PS, USART_CR1_PCE | USART_CR1_PS)
#define LPUART_setNoParity()    LPUART_config(&LPUART1->CR1, USART_CR1_PCE, 0)

// LPUART kernel clock
#if LPUART_LSE > 0
  #define LPUART_CLK          LSE_VALUE
#else
  #define LPUART_CLK          F_CPU
#endif

// BAUD rate check (fCK >= 3 * BAUD, BRR >= 0x300, BRR <= 0xFFFFF)
#if (LPUART_CLK < 3 * LPUART_BAUD) || ((256ULL * LPUART_CLK / LPUART_BAUD) < 0x300)
  #warning LPUART_BAUD too high for LPUART clock
#endif
#if (256ULL * LPUART_CLK / LPUART_BAUD) > 0xFFFFF
  #warning LPUART_BAUD too low for LPUART clock
#endif

// Set BAUD rate (BRR = 256 * fCK / BAUD, limited to 0x300..0xFFFFF)
void LPUART_setBAUD(uint32_t baud);

// UART functions
void LPUART_init(void);                     // init UART with default BAUD rate
char LPUART_read(void);                     // read character via UART
void LPUART_write(const char c);            // send character via UART

// Additional print functions (if activated, see above)
#if LPUART_PRINT == 1
#include "print.h"
#define LPUART_printD(n)      printD(LPUART_write, n)   // print decimal as string
#define LPUART_printW(n)      printW(LPUART_write, n)   // print word as string
#define LPUART_printH(n)      printH(LPUART_write, n)   // print half-word as string
#define LPUART_printB(n)      printB(LPUART_write, n)   // print byte as string
#define LPUART_printS(s)      printS(LPUART_write, s)   // print string
#define LPUART_println(s)     println(LPUART_write, s)  // print string with newline
#define LPUART_print          LPUART_printS             // alias
#define LPUART_newline()      LPUART_write('\n')        // send newline
#define LPUART_printf(f, ...)   printF(LPUART_write, f, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
};
#endif
