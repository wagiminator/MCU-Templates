// ===================================================================================
// Basic LPUART1 Functions for STM32L0xx  (no buffer, no interrupt, no DMA)   * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "lpuart.h"

// Set BAUD rate (BRR can only be written while the LPUART is disabled)
void LPUART_setBAUD(uint32_t baud) {
  uint32_t brr = (LPUART_CLK / baud) * 256 + ((LPUART_CLK % baud) * 256 + baud / 2) / baud;
  if(brr < 0x300)   brr = 0x300;                    // limit to valid range
  if(brr > 0xFFFFF) brr = 0xFFFFF;
  LPUART_config(&LPUART1->BRR, 0xFFFFF, brr);
}

// Init UART
void LPUART_init(void) {
  // Set GPIO pins
  #if LPUART_MAP == 0
    // Setup pin PA2 (TX) and pin PA3 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~(((uint32_t)0b11<<(2<<1)) | ((uint32_t)0b11<<(3<<1))))
                      |  (((uint32_t)0b10<<(2<<1)) | ((uint32_t)0b10<<(3<<1)));
    GPIOA->OTYPER  &= ~((uint32_t)1<<2);
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~((uint32_t)0b11<<(3<<1)))
                      |  ((uint32_t)0b01<<(3<<1));
    GPIOA->AFR[0]   = (GPIOA->AFR[0] & ~(((uint32_t)0xf<<(2<<2)) | ((uint32_t)0xf<<(3<<2))))
                      |  (((uint32_t)0x6<<(2<<2)) | ((uint32_t)0x6<<(3<<2)));
  #elif LPUART_MAP == 1
    // Setup pin PA14 (TX) and pin PA13 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~(((uint32_t)0b11<<(14<<1)) | ((uint32_t)0b11<<(13<<1))))
                      |  (((uint32_t)0b10<<(14<<1)) | ((uint32_t)0b10<<(13<<1)));
    GPIOA->OTYPER  &= ~((uint32_t)1<<14);
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~((uint32_t)0b11<<(13<<1)))
                      |  ((uint32_t)0b01<<(13<<1));
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~(((uint32_t)0xf<<(6<<2)) | ((uint32_t)0xf<<(5<<2))))
                      |  (((uint32_t)0x6<<(6<<2)) | ((uint32_t)0x6<<(5<<2)));
  #elif LPUART_MAP == 2
    // Setup pin PB6 (TX) and pin PB7 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOBEN;
    GPIOB->MODER    = (GPIOB->MODER  & ~(((uint32_t)0b11<<(6<<1)) | ((uint32_t)0b11<<(7<<1))))
                      |  (((uint32_t)0b10<<(6<<1)) | ((uint32_t)0b10<<(7<<1)));
    GPIOB->OTYPER  &= ~((uint32_t)1<<6);
    GPIOB->PUPDR    = (GPIOB->PUPDR  & ~((uint32_t)0b11<<(7<<1)))
                      |  ((uint32_t)0b01<<(7<<1));
    GPIOB->AFR[0]   = (GPIOB->AFR[0] & ~(((uint32_t)0xf<<(6<<2)) | ((uint32_t)0xf<<(7<<2))))
                      |  (((uint32_t)0x6<<(6<<2)) | ((uint32_t)0x6<<(7<<2)));
  #elif LPUART_MAP == 3
    // Setup pin PA1 (TX) and pin PA0 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~(((uint32_t)0b11<<(1<<1)) | ((uint32_t)0b11<<(0<<1))))
                      |  (((uint32_t)0b10<<(1<<1)) | ((uint32_t)0b10<<(0<<1)));
    GPIOA->OTYPER  &= ~((uint32_t)1<<1);
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~((uint32_t)0b11<<(0<<1)))
                      |  ((uint32_t)0b01<<(0<<1));
    GPIOA->AFR[0]   = (GPIOA->AFR[0] & ~(((uint32_t)0xf<<(1<<2)) | ((uint32_t)0xf<<(0<<2))))
                      |  (((uint32_t)0x6<<(1<<2)) | ((uint32_t)0x6<<(0<<2)));
  #else
    #warning No automatic pin mapping for LPUART1
  #endif

  // Setup LPUART clock
  RCC->APB1ENR |= RCC_APB1ENR_LPUART1EN;            // enable LPUART module clock
  #if LPUART_LSE > 0
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;                // enable power control clock
  PWR->CR      |= PWR_CR_DBP;                       // enable access to RTC domain
  RCC->CSR     |= RCC_CSR_LSEON;                    // enable LSE
  while(!(RCC->CSR & RCC_CSR_LSERDY));              // wait until LSE is stable
  RCC->CCIPR   |= RCC_CCIPR_LPUART1SEL;             // LSE as LPUART clock source (11)
  #else
  RCC->CCIPR   &= ~RCC_CCIPR_LPUART1SEL;            // PCLK as LPUART clock source
  #endif

  // Setup and start UART (8N1, RX/TX, default BAUD rate)
  LPUART1->CR1 = 0;                                 // disable LPUART
  LPUART_setBAUD(LPUART_BAUD);                      // set BAUD rate
  LPUART1->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
}

// Read byte via UART
char LPUART_read(void) {
  while(!LPUART_available());
  return LPUART1->RDR;
}

// Send byte via UART
void LPUART_write(const char c) {
  while(!LPUART_ready());
  LPUART1->TDR = c;
}
