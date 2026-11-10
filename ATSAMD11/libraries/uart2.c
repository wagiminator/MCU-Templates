// ===================================================================================
// Basic UART2 Functions (SERCOM USART) for SAMD11 (no buffer, no interrupt)  * v1.1 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "uart2.h"
#include "gpio.h"

static uint8_t UART2_txflag = 0;           // something was transmitted

// Disable USART for configuration (wait for ongoing transmission), return CTRLA
static uint32_t UART2_stop(void) {
  uint32_t ctrla = UART2_SER->USART.CTRLA.reg;
  if(ctrla & SERCOM_USART_CTRLA_ENABLE) {
    if(UART2_txflag) while(!UART2_completed());       // wait for TX completed
    UART2_SER->USART.CTRLA.reg = ctrla & ~SERCOM_USART_CTRLA_ENABLE;
    UART2_sync();
  }
  return ctrla;
}

// Re-enable USART after configuration if it was enabled before
static void UART2_restart(uint32_t ctrla) {
  if(ctrla & SERCOM_USART_CTRLA_ENABLE) {
    UART2_SER->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
    UART2_sync();
  }
}

// Set BAUD rate (fractional BAUD rate generator, 16x oversampling)
void UART2_setBAUD(uint32_t baud) {
  uint32_t ctrla = UART2_stop();
  uint32_t div8  = (F_CPU + baud) / (baud << 1);    // F_CPU / (16 * baud) * 8
  UART2_SER->USART.BAUD.reg = SERCOM_USART_BAUD_FRAC_BAUD(div8 >> 3)
                           | SERCOM_USART_BAUD_FRAC_FP(div8 & 7);
  UART2_restart(ctrla);
}

// Change CTRLA register (enable-protected bits)
void UART2_setCTRLA(uint32_t mask, uint32_t val) {
  uint32_t ctrla = UART2_stop();
  UART2_SER->USART.CTRLA.reg = (UART2_SER->USART.CTRLA.reg & ~mask) | val;
  UART2_restart(ctrla);
}

// Change CTRLB register (enable-protected bits)
void UART2_setCTRLB(uint32_t mask, uint32_t val) {
  uint32_t ctrla = UART2_stop();
  UART2_SER->USART.CTRLB.reg = (UART2_SER->USART.CTRLB.reg & ~mask) | val;
  UART2_sync();
  UART2_restart(ctrla);
}

// Init UART
void UART2_init(void) {
  // Set GPIO pins (RX with pullup)
  #ifdef UART2_PMUX
  PIN_alternate(UART2_TX_PIN, UART2_PMUX);
  PIN_input_PU(UART2_RX_PIN);
  PIN_alternate(UART2_RX_PIN, UART2_PMUX);
  #else
    #warning No automatic pin mapping for UART
  #endif

  // Enable SERCOM clocks
  PM->APBCMASK.reg |= UART2_APBMASK;                 // enable SERCOM module clock
  GCLK_setCH(UART2_GCLK_ID, GCLK_SYS);               // GCLK0 -> SERCOM

  // Setup and start UART (8N1, RX/TX, default BAUD rate)
  UART2_SER->USART.CTRLA.reg = SERCOM_USART_CTRLA_SWRST;
  while(UART2_SER->USART.CTRLA.reg & SERCOM_USART_CTRLA_SWRST);
  UART2_SER->USART.CTRLA.reg = SERCOM_USART_CTRLA_MODE_USART_INT_CLK // internal clock
                            | SERCOM_USART_CTRLA_SAMPR(1)           // 16x fractional
                            | SERCOM_USART_CTRLA_TXPO(UART2_TXPO)    // TX pad
                            | SERCOM_USART_CTRLA_RXPO(UART2_RXPO)    // RX pad
                            | SERCOM_USART_CTRLA_DORD;              // LSB first
  UART2_SER->USART.CTRLB.reg = SERCOM_USART_CTRLB_TXEN               // enable TX
                            | SERCOM_USART_CTRLB_RXEN;              // enable RX
  UART2_sync();
  UART2_setBAUD(UART2_BAUD);                                          // set BAUD rate
  UART2_SER->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;           // enable USART
  UART2_sync();
}

// Read byte via UART
char UART2_read(void) {
  while(!UART2_available());
  return UART2_SER->USART.DATA.reg;
}

// Send byte via UART
void UART2_write(const char c) {
  while(!UART2_ready());
  UART2_SER->USART.DATA.reg = c;
  UART2_txflag = 1;
}

// ===================================================================================
// Buffer Functions
// ===================================================================================

// Send buffer (*buf) with length (len) via UART
void UART2_writeBuffer(const uint8_t* buf, uint16_t len) {
  while(len--) UART2_write(*buf++);
}

// Receive buffer (*buf) with length (len) via UART
void UART2_readBuffer(uint8_t* buf, uint16_t len) {
  while(len--) *buf++ = UART2_read();
}
