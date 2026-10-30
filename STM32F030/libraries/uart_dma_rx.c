// ===================================================================================
// USART1 with DMA RX Buffer (RX only) for STM32F03x                          * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "uart_dma_rx.h"

// Circular RX buffer
char UART_RX_buffer[UART_RX_BUF_SIZE];
uint16_t UART_RX_tptr = 0;
#define UART_RX_hptr ((UART_RX_BUF_SIZE - UART_DMA_CHAN->CNDTR) % UART_RX_BUF_SIZE)

// Init UART
void UART_init(void) {
  // Set GPIO pins
  #if UART_MAP == 0
    // Setup pin PA3 (RX)
    RCC->AHBENR    |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~((uint32_t)0b11<<(3<<1)))
                      |  ((uint32_t)0b10<<(3<<1));
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~((uint32_t)0b11<<(3<<1)))
                      |  ((uint32_t)0b01<<(3<<1));
    GPIOA->AFR[0]   = (GPIOA->AFR[0] & ~((uint32_t)0xf<<(3<<2)))
                      |  ((uint32_t)0x1<<(3<<2));
  #elif UART_MAP == 1
    // Setup pin PA10 (RX)
    RCC->AHBENR    |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~((uint32_t)0b11<<(10<<1)))
                      |  ((uint32_t)0b10<<(10<<1));
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~((uint32_t)0b11<<(10<<1)))
                      |  ((uint32_t)0b01<<(10<<1));
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~((uint32_t)0xf<<(2<<2)))
                      |  ((uint32_t)0x1<<(2<<2));
  #elif UART_MAP == 2
    // Setup pin PA15 (RX)
    RCC->AHBENR    |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~((uint32_t)0b11<<(15<<1)))
                      |  ((uint32_t)0b10<<(15<<1));
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~((uint32_t)0b11<<(15<<1)))
                      |  ((uint32_t)0b01<<(15<<1));
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~((uint32_t)0xf<<(7<<2)))
                      |  ((uint32_t)0x1<<(7<<2));
  #elif UART_MAP == 3
    // Setup pin PB7 (RX)
    RCC->AHBENR    |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER    = (GPIOB->MODER  & ~((uint32_t)0b11<<(7<<1)))
                      |  ((uint32_t)0b10<<(7<<1));
    GPIOB->PUPDR    = (GPIOB->PUPDR  & ~((uint32_t)0b11<<(7<<1)))
                      |  ((uint32_t)0b01<<(7<<1));
    GPIOB->AFR[0]  &= ~((uint32_t)0xf<<(7<<2));
  #else
    #warning No automatic pin mapping for USART1
  #endif


  // Setup and start UART (8N1, RX, default BAUD rate)
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
  USART1->BRR   = ((2 * F_CPU / UART_BAUD) + 1) / 2;
  USART1->CR3  |= USART_CR3_DMAR;
  USART1->CR1   = USART_CR1_RE | USART_CR1_UE;

  // Setup DMA
  #if UART_DMA_CHANNEL == 5
  RCC->APB2ENR  |= RCC_APB2ENR_SYSCFGEN;              // enable SYSCFG module clock
  SYSCFG->CFGR1 |= SYSCFG_CFGR1_USART1RX_DMA_RMP;     // remap to DMA channel 5
  #endif
  RCC->AHBENR |= RCC_AHBENR_DMAEN;                    // enable DMA module clock
  UART_DMA_CHAN->CNDTR = (uint16_t)UART_RX_BUF_SIZE;  // ring buffer size
  UART_DMA_CHAN->CMAR  = (uint32_t)UART_RX_buffer;    // memory address
  UART_DMA_CHAN->CPAR  = (uint32_t)&USART1->RDR;      // peripheral address
  UART_DMA_CHAN->CCR   = DMA_CCR_MINC                 // increment memory address
                       | DMA_CCR_CIRC                 // circular mode
                       | DMA_CCR_EN;                  // enable
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
