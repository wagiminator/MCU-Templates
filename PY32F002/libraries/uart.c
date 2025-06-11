// ===================================================================================
// Basic USART1 Functions for PY32F0xx                                        * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "uart.h"

// Init UART
void UART_init(void) {
  // Set GPIO pins
  #if UART_MAP == 0
    // Setup pin PA2 (TX) and pin PA3 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(2<<1)) | ((uint32_t)0b11<<(3<<1)) ))
                                     |  ( ((uint32_t)0b10<<(2<<1)) | ((uint32_t)0b10<<(3<<1)) );
    GPIOA->OTYPER  &=                  ~  ((uint32_t)0b1 <<(2<<0));
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~(                            ((uint32_t)0b11<<(3<<1)) ))
                                     |  (                            ((uint32_t)0b01<<(3<<1)) );
    GPIOA->AFR[0]   = (GPIOA->AFR[0] & ~( ((uint32_t)0xf <<(2<<2)) | ((uint32_t)0xf <<(3<<2)) ))
                                     |  ( ((uint32_t)0x1 <<(2<<2)) | ((uint32_t)0x1 <<(3<<2)) );
  #elif UART_MAP == 1
    // Setup pin PA7 (TX) and pin PA8 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(7<<1)) | ((uint32_t)0b11<<(8<<1)) ))
                                     |  ( ((uint32_t)0b10<<(7<<1)) | ((uint32_t)0b10<<(8<<1)) );
    GPIOA->OTYPER  &=                  ~  ((uint32_t)0b1 <<(7<<0));
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~(                            ((uint32_t)0b11<<(8<<1)) ))
                                     |  (                            ((uint32_t)0b01<<(8<<1)) );
    GPIOA->AFR[0]   = (GPIOA->AFR[0] & ~( ((uint32_t)0xf <<(7<<2))                            ))
                                     |  ( ((uint32_t)0x8 <<(7<<2))                            );
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~(                            ((uint32_t)0xf <<(0<<2)) ))
                                     |  (                            ((uint32_t)0x8 <<(0<<2)) );
  #elif UART_MAP == 2
    // Setup pin PA9 (TX) and pin PA10 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(9<<1)) | ((uint32_t)0b11<<(10<<1)) ))
                                     |  ( ((uint32_t)0b10<<(9<<1)) | ((uint32_t)0b10<<(10<<1)) );
    GPIOA->OTYPER  &=                  ~  ((uint32_t)0b1 <<(9<<0));
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~(                            ((uint32_t)0b11<<(10<<1)) ))
                                     |  (                            ((uint32_t)0b01<<(10<<1)) );
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~( ((uint32_t)0xf <<(1<<2)) | ((uint32_t)0xf <<( 2<<2)) ))
                                     |  ( ((uint32_t)0x1 <<(1<<2)) | ((uint32_t)0x1 <<( 2<<2)) );
  #elif UART_MAP == 3
    // Setup pin PA14 (TX) and pin PA13 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(14<<1)) | ((uint32_t)0b11<<(13<<1)) ))
                                     |  ( ((uint32_t)0b10<<(14<<1)) | ((uint32_t)0b10<<(13<<1)) );
    GPIOA->OTYPER  &=                  ~  ((uint32_t)0b1 <<(14<<0));
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~(                             ((uint32_t)0b11<<(13<<1)) ))
                                     |  (                             ((uint32_t)0b01<<(13<<1)) );
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~( ((uint32_t)0xf <<( 6<<2)) | ((uint32_t)0xf <<( 5<<2)) ))
                                     |  ( ((uint32_t)0x1 <<( 6<<2)) | ((uint32_t)0x8 <<( 5<<2)) );
  #elif UART_MAP == 4
    // Setup pin PA14 (TX) and pin PA15 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(14<<1)) | ((uint32_t)0b11<<(15<<1)) ))
                                     |  ( ((uint32_t)0b10<<(14<<1)) | ((uint32_t)0b10<<(15<<1)) );
    GPIOA->OTYPER  &=                  ~  ((uint32_t)0b1 <<(14<<0));
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~(                             ((uint32_t)0b11<<(15<<1)) ))
                                     |  (                             ((uint32_t)0b01<<(15<<1)) );
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~( ((uint32_t)0xf <<( 6<<2)) | ((uint32_t)0xf <<( 7<<2)) ))
                                     |  ( ((uint32_t)0x1 <<( 6<<2)) | ((uint32_t)0x1 <<( 7<<2)) );
  #elif UART_MAP == 5
    // Setup pin PB6 (TX) and pin PB7 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOBEN;
    GPIOB->MODER    = (GPIOB->MODER & ~( ((uint32_t)0b11<<(6<<1)) | ((uint32_t)0b11<<(7<<1)) ))
                                    |  ( ((uint32_t)0b10<<(6<<1)) | ((uint32_t)0b10<<(7<<1)) );
    GPIOB->OTYPER  &=                 ~  ((uint32_t)0b1 <<(6<<0));
    GPIOB->PUPDR    = (GPIOB->PUPDR & ~(                            ((uint32_t)0b11<<(7<<1)) ))
                                    |  (                            ((uint32_t)0b01<<(7<<1)) );
    GPIOB->AFR[0]  &=                 ~( ((uint32_t)0xf <<(6<<2)) | ((uint32_t)0xf <<(7<<2)) );
  #elif UART_MAP == 6
    // Setup pin PF1 (TX) and pin PF0 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOFEN;
    GPIOF->MODER    = (GPIOF->MODER  & ~( ((uint32_t)0b11<<(1<<1)) | ((uint32_t)0b11<<(0<<1)) ))
                                     |  ( ((uint32_t)0b10<<(1<<1)) | ((uint32_t)0b10<<(0<<1)) );
    GPIOF->OTYPER  &=                  ~  ((uint32_t)0b1 <<(1<<0));
    GPIOF->PUPDR    = (GPIOF->PUPDR  & ~(                            ((uint32_t)0b11<<(0<<1)) ))
                                     |  (                            ((uint32_t)0b01<<(0<<1)) );
    GPIOF->AFR[0]   = (GPIOF->AFR[0] & ~( ((uint32_t)0xf <<(1<<2)) | ((uint32_t)0xf <<(0<<2)) ))
                                     |  ( ((uint32_t)0x8 <<(1<<2)) | ((uint32_t)0x8 <<(0<<2)) );
  #else
    #warning No automatic pin mapping for USART1
  #endif
	
  // Setup and start UART (8N1, RX/TX, default BAUD rate)
  RCC->APBENR2 |= RCC_APBENR2_USART1EN;
  USART1->BRR = ((2 * F_CPU / UART_BAUD) + 1) / 2;
  USART1->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
}

// Read byte via UART
char UART_read(void) {
  while(!UART_available());
  return USART1->DR;
}

// Send byte via UART
void UART_write(const char c) {
  while(!UART_ready());
  USART1->DR = c;
}
