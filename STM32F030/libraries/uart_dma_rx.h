// ===================================================================================
// USART1 with DMA RX Buffer (RX only) for STM32F03x                          * v1.0 *
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
// UART_available()         Check if there is something to read
//
// UART_read()              Read character via UART
//
// UART_enable()            Enable USART
// UART_disable()           Disable USART
// UART_TX_enable()         Enable transmitter
// UART_TX_disable()        Disable transmitter
// UART_RX_enable()         Enable receiver
// UART_RX_disable()        Disable receiver
//
// USART1 pin mapping (set below in UART parameters):
// --------------------------------------------------
// UART_MAP    0     1     2     3     4
// RX-pin     PA3   PA10  PA15  PB7   no mapping
// (MAP 0 and 2 only on STM32F030x4/x6 and STM32F031)
//
// The received data is written into a circular buffer by DMA (DMA channel 3, or 5 with SYSCFG remap (not on STM32F030xC)).
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// UART parameters
#define UART_BAUD             115200      // default UART baud rate
#define UART_RX_BUF_SIZE      64          // UART RX buffer size
#define UART_MAP              1           // UART pin mapping (see above)
#define UART_DMA_CHANNEL      3           // DMA channel (3 or 5)

// BAUD rate check (oversampling by 16: BRR >= 16)
#if (F_CPU / UART_BAUD) < 16
  #warning UART_BAUD too high for F_CPU
#endif

// UART macros

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
uint8_t UART_available(void);           // check if there is something to read

// DMA channel defines
#if   UART_DMA_CHANNEL == 3
  #define UART_DMA_CHAN   DMA1_Channel3
#elif UART_DMA_CHANNEL == 5
  #define UART_DMA_CHAN   DMA1_Channel5
#else
  #error Wrong DMA channel for USART1 (3 or 5)
#endif

#ifdef __cplusplus
};
#endif
