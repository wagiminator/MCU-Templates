// ===================================================================================
// Basic UART Functions for CH32V002, V004, V005, V006, V007, M007            * v1.0 *
// ===================================================================================
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#include "uart.h"

// Pin assignment for the selected mapping (CH32V00X RM: AFIO_PCFR1 USART1_RM[3:0])
#if   UART_MAP == 0                     // TX: PD5, RX: PD6
  #define UART_TX_GPIO  GPIOD
  #define UART_TX_PIN   5
  #define UART_RX_GPIO  GPIOD
  #define UART_RX_PIN   6
  #define UART_PORT_EN  RCC_IOPDEN
#elif UART_MAP == 1                     // TX: PD6, RX: PD5
  #define UART_TX_GPIO  GPIOD
  #define UART_TX_PIN   6
  #define UART_RX_GPIO  GPIOD
  #define UART_RX_PIN   5
  #define UART_PORT_EN  RCC_IOPDEN
#elif UART_MAP == 2                     // TX: PD0, RX: PD1
  #define UART_TX_GPIO  GPIOD
  #define UART_TX_PIN   0
  #define UART_RX_GPIO  GPIOD
  #define UART_RX_PIN   1
  #define UART_PORT_EN  RCC_IOPDEN
#elif UART_MAP == 3                     // TX: PC0, RX: PC1
  #define UART_TX_GPIO  GPIOC
  #define UART_TX_PIN   0
  #define UART_RX_GPIO  GPIOC
  #define UART_RX_PIN   1
  #define UART_PORT_EN  RCC_IOPCEN
#elif UART_MAP == 4                     // TX: PD1, RX: PB3
  #define UART_TX_GPIO  GPIOD
  #define UART_TX_PIN   1
  #define UART_RX_GPIO  GPIOB
  #define UART_RX_PIN   3
  #define UART_PORT_EN  (RCC_IOPDEN | RCC_IOPBEN)
#elif UART_MAP == 5                     // TX: PB3, RX: PD1
  #define UART_TX_GPIO  GPIOB
  #define UART_TX_PIN   3
  #define UART_RX_GPIO  GPIOD
  #define UART_RX_PIN   1
  #define UART_PORT_EN  (RCC_IOPDEN | RCC_IOPBEN)
#elif UART_MAP == 6                     // TX: PC5, RX: PC6
  #define UART_TX_GPIO  GPIOC
  #define UART_TX_PIN   5
  #define UART_RX_GPIO  GPIOC
  #define UART_RX_PIN   6
  #define UART_PORT_EN  RCC_IOPCEN
#elif UART_MAP == 7                     // TX: PB5, RX: PB6
  #define UART_TX_GPIO  GPIOB
  #define UART_TX_PIN   5
  #define UART_RX_GPIO  GPIOB
  #define UART_RX_PIN   6
  #define UART_PORT_EN  RCC_IOPBEN
#elif UART_MAP == 8                     // TX: PA0, RX: PA1
  #define UART_TX_GPIO  GPIOA
  #define UART_TX_PIN   0
  #define UART_RX_GPIO  GPIOA
  #define UART_RX_PIN   1
  #define UART_PORT_EN  RCC_IOPAEN
#elif UART_MAP == 9                     // TX: PA0, RX: PC4
  #define UART_TX_GPIO  GPIOA
  #define UART_TX_PIN   0
  #define UART_RX_GPIO  GPIOC
  #define UART_RX_PIN   4
  #define UART_PORT_EN  (RCC_IOPAEN | RCC_IOPCEN)
#else
  #error Wrong UART_MAP (0..9)
#endif

// Init UART
void UART_init(void) {
  // Enable AFIO, GPIO port(s) and USART1
  RCC->PB2PCENR |= RCC_AFIOEN | UART_PORT_EN | RCC_USART1EN;

  // Select pin mapping (USART1_RM[3:0] = AFIO_PCFR1[9:6])
  AFIO->PCFR1 = (AFIO->PCFR1 & ~AFIO_PCFR1_USART1_REMAP) | ((uint32_t)UART_MAP << 6);

  // Set TX pin to output, push-pull, 30MHz, multiplex
  UART_TX_GPIO->CFGLR = (UART_TX_GPIO->CFGLR & ~((uint32_t)0b1111 << (UART_TX_PIN << 2)))
                                             |  ((uint32_t)0b1001 << (UART_TX_PIN << 2));

  // Set RX pin to input, pullup
  UART_RX_GPIO->CFGLR = (UART_RX_GPIO->CFGLR & ~((uint32_t)0b1111 << (UART_RX_PIN << 2)))
                                             |  ((uint32_t)0b1000 << (UART_RX_PIN << 2));
  UART_RX_GPIO->BSHR  = (uint32_t)1 << UART_RX_PIN;

  // Setup and start UART (8N1, RX/TX, default BAUD rate)
  USART1->CTLR1 = 0;
  USART1->CTLR2 = 0;
  USART1->CTLR3 = 0;
  USART1->BRR   = ((2 * F_CPU / UART_BAUD) + 1) / 2;
  USART1->CTLR1 = USART_CTLR1_RE | USART_CTLR1_TE | USART_CTLR1_UE;
}

// Read byte via UART
char UART_read(void) {
  while(!UART_available());
  return USART1->DATAR;
}

// Send byte via UART
void UART_write(const char c) {
  while(!UART_ready());
  USART1->DATAR = c;
}
