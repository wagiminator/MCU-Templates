// ===================================================================================
// Basic UART Functions (RX only) for CH32V003                                * v1.1 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "uart_rx.h"

// Init UART
void UART_init(void) {
  #if UART_MAP == 0
  // Enable GPIO port D and UART
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPDEN | RCC_USART1EN;

  // Set pin PD6 (RX) to input, pullup
  GPIOD->CFGLR = (GPIOD->CFGLR & ~((uint32_t)0b1111<<(6<<2)))
                               |  ((uint32_t)0b1000<<(6<<2));
  GPIOD->BSHR  = (uint32_t)1<<6;
  
  #elif UART_MAP == 1
  // Remap UART pins, enable GPIO port D and UART
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPDEN | RCC_USART1EN;
  AFIO->PCFR1    |= (uint32_t)1<<2;
  
  // Set pin PD1 (RX) to input, pullup
  GPIOD->CFGLR = (GPIOD->CFGLR & ~((uint32_t)0b1111<<(1<<2)))
                               |  ((uint32_t)0b1000<<(1<<2));
  GPIOD->BSHR  = (uint32_t)1<<1;
  
  #elif UART_MAP == 2
  // Remap UART pins, enable GPIO port D and UART
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPDEN | RCC_USART1EN;
  AFIO->PCFR1    |= (uint32_t)1<<21;

  // Set pin PD5 (RX) to input, pullup
  GPIOD->CFGLR = (GPIOD->CFGLR & ~((uint32_t)0b1111<<(5<<2)))
                               |  ((uint32_t)0b1000<<(5<<2));
  GPIOD->BSHR  = (uint32_t)1<<5;
  
  #elif UART_MAP == 3
  // Remap UART pins, enable GPIO port C and UART
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPCEN | RCC_USART1EN;
  AFIO->PCFR1    |= ((uint32_t)1<<21) | ((uint32_t)1<<2);

  // Set pin PC1 (RX) to input, pullup
  GPIOC->CFGLR = (GPIOC->CFGLR & ~((uint32_t)0b1111<<(1<<2)))
                               |  ((uint32_t)0b1000<<(1<<2));
  GPIOC->BSHR  = (uint32_t)1<<1;

  #else
    #warning Wrong UART REMAP
  #endif
	
  // Setup and start UART (8N1, RX, default BAUD rate)
  USART1->BRR   = ((2 * (F_CPU) / (UART_BAUD)) + 1) / 2;
  USART1->CTLR1 = USART_CTLR1_RE | USART_CTLR1_UE;
}

// Read byte via UART
char UART_read(void) {
  while(!UART_available());
  return USART1->DATAR;
}
