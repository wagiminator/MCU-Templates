// ===================================================================================
// UART with DMA RX Buffer for CH32V002, V004, V005, V006, V007, M007         * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "uart_dma.h"

// Circular RX buffer
char UART_RX_buffer[UART_RX_BUF_SIZE];
uint16_t UART_RX_tptr = 0;
#define UART_RX_hptr ((uint16_t)((UART_RX_BUF_SIZE - DMA1_Channel5->CNTR) % UART_RX_BUF_SIZE))

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
  USART1->BRR    = ((2 * F_CPU / UART_BAUD) + 1) / 2;
  USART1->CTLR3 |= USART_CTLR3_DMAR;
  USART1->CTLR1  = USART_CTLR1_RE | USART_CTLR1_TE | USART_CTLR1_UE;

  // Setup DMA Channel 5 (USART1_RX)
  RCC->AHBPCENR |= RCC_DMA1EN;
  DMA1_Channel5->CNTR  = (uint16_t)UART_RX_BUF_SIZE;
  DMA1_Channel5->MADDR = (uint32_t)UART_RX_buffer;
  DMA1_Channel5->PADDR = (uint32_t)&USART1->DATAR;
  DMA1_Channel5->CFGR  = DMA_CFGR1_MINC       // increment memory address
                       | DMA_CFGR1_CIRC       // circular mode
                       | DMA_CFGR1_EN;        // enable
}

// Check if something is in the RX buffer
uint8_t UART_available(void) {
  return(UART_RX_hptr != UART_RX_tptr);
}

// Read from UART buffer
char UART_read(void) {
  char result;
  while(!UART_available());
  result = UART_RX_buffer[UART_RX_tptr++];
  if(UART_RX_tptr >= UART_RX_BUF_SIZE) UART_RX_tptr = 0;
  return result;
}

// Send byte via UART
void UART_write(const char c) {
  while(!UART_ready());
  USART1->DATAR = c;
}

// ===================================================================================
// Buffer Functions
// ===================================================================================

// Send buffer (*buf) with length (len) via UART
void UART_writeBuffer(const uint8_t* buf, uint16_t len) {
  while(len--) UART_write(*buf++);
}

// Receive buffer (*buf) with length (len) via UART
void UART_readBuffer(uint8_t* buf, uint16_t len) {
  while(len--) *buf++ = UART_read();
}
