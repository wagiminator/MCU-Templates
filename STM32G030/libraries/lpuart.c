// ===================================================================================
// Basic LPUART1 Functions for STM32G0xx  (no buffer, no interrupt, no DMA)   * v1.1 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "lpuart.h"

// Prescaler values and register codes (LPUART_PRESC)
static const uint16_t LPUART_PRE[]  = {1, 2, 4, 8, 16, 32, 64, 128, 256};
static const uint8_t  LPUART_PREC[] = {0, 1, 2, 4,  7,  8,  9,  10,  11};

// Set BAUD rate (BRR, PRESC can only be written while the LPUART is disabled)
void LPUART_setBAUD(uint32_t baud) {
  uint8_t  i = 0;
  uint32_t q, brr;
  do {
    q   = LPUART_CLK / LPUART_PRE[i];               // prescaled kernel clock
    brr = (q / baud) * 256 + ((q % baud) * 256 + baud / 2) / baud;
  } while((brr > 0xFFFFF) && (++i < sizeof(LPUART_PRE)));
  LPUART_config(&LPUART1->PRESC, USART_PRESC_PRESCALER, LPUART_PREC[i]);
  LPUART_config(&LPUART1->BRR, 0xFFFFF, brr);       // BRR must be >= 0x300
}

// Init UART
void LPUART_init(void) {
  // Set GPIO pins
  #if LPUART_MAP == 0
    // Setup pin PA2 (TX) and pin PA3 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(2<<1)) | ((uint32_t)0b11<<(3<<1)) ))
                                     |  ( ((uint32_t)0b10<<(2<<1)) | ((uint32_t)0b10<<(3<<1)) );
    GPIOA->OTYPER  &=                  ~  ((uint32_t)0b1 <<(2<<0));
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~(                            ((uint32_t)0b11<<(3<<1)) ))
                                     |  (                            ((uint32_t)0b01<<(3<<1)) );
    GPIOA->AFR[0]   = (GPIOA->AFR[0] & ~( ((uint32_t)0xf <<(2<<2)) | ((uint32_t)0xf <<(3<<2)) ))
                                     |  ( ((uint32_t)0x6 <<(2<<2)) | ((uint32_t)0x6 <<(3<<2)) );
  #elif LPUART_MAP == 1
    // Setup pin PB11 (TX) and pin PB10 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOBEN;
    GPIOB->MODER    = (GPIOB->MODER  & ~( ((uint32_t)0b11<<(11<<1)) | ((uint32_t)0b11<<(10<<1)) ))
                                     |  ( ((uint32_t)0b10<<(11<<1)) | ((uint32_t)0b10<<(10<<1)) );
    GPIOB->OTYPER  &=                  ~  ((uint32_t)0b1 <<(11<<0));
    GPIOB->PUPDR    = (GPIOB->PUPDR  & ~(                             ((uint32_t)0b11<<(10<<1)) ))
                                     |  (                             ((uint32_t)0b01<<(10<<1)) );
    GPIOB->AFR[1]   = (GPIOB->AFR[1] & ~( ((uint32_t)0xf <<( 3<<2)) | ((uint32_t)0xf <<( 2<<2)) ))
                                     |  ( ((uint32_t)0x1 <<( 3<<2)) | ((uint32_t)0x1 <<( 2<<2)) );
  #else
    #warning No automatic pin mapping for LPUART1
  #endif

  // Setup LPUART clock
  RCC->APBENR1 |= RCC_APBENR1_LPUART1EN;            // enable LPUART module clock
  #if LPUART_LSE > 0
  RCC->APBENR1 |= RCC_APBENR1_PWREN;                // enable power control clock
  PWR->CR1     |= PWR_CR1_DBP;                      // enable access to RTC domain
  RCC->BDCR    |= RCC_BDCR_LSEON;                   // enable LSE
  while(!(RCC->BDCR & RCC_BDCR_LSERDY));            // wait until LSE is stable
  RCC->CCIPR   |= RCC_CCIPR_LPUART1SEL;             // LSE as LPUART clock source
  #else
  RCC->CCIPR   &= ~RCC_CCIPR_LPUART1SEL;            // PCLK as LPUART clock source
  #endif

  // Setup and start UART (8N1, RX/TX, default BAUD rate)
  LPUART1->CR1 = 0;                                 // disable LPUART
  LPUART_setBAUD(LPUART_BAUD);                      // set prescaler and BAUD rate
  #if LPUART_FIFO > 0
  LPUART1->CR1 = USART_CR1_FIFOEN | USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
  #else
  LPUART1->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
  #endif
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
