// ===================================================================================
// Basic UART Functions for CH573/CH571  (no buffer/interrupt/DMA)            * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "uart.h"
#include "gpio.h"

// ===================================================================================
// UART0
// ===================================================================================

// Init UART
void UART0_init(void) {
#if UART0_REMAP == 0
  R16_PIN_ALTERNATE &= ~RB_PIN_UART0;              // RXD0/TXD0 on PB4/PB7
  PIN_high(PB7); PIN_output(PB7);         // TX: output, idle high
  PIN_input_PU(PB4);                       // RX: input with pullup
#elif UART0_REMAP == 1
  R16_PIN_ALTERNATE |=  RB_PIN_UART0;              // RXD0/TXD0 on PA15/PA14
  PIN_high(PA14); PIN_output(PA14);        // TX: output, idle high
  PIN_input_PU(PA15);                      // RX: input with pullup
#else
  #warning No UART0 REMAP
#endif

  // Setup and start UART (8N1, FIFO, default BAUD rate)
  R8_UART0_DIV = 1;                       // pre-divider 1
  UART0_setBAUD(UART0_BAUD);            // set baud rate
  R8_UART0_FCR = (2 << 6) | RB_FCR_TX_FIFO_CLR | RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;
  R8_UART0_LCR = RB_LCR_WORD_SZ;          // 8 data bits, 1 stop bit, no parity
  R8_UART0_IER = RB_IER_TXD_EN;           // enable TX pin
}

// Read byte via UART
char UART0_read(void) {
  while(!UART0_available());
  return R8_UART0_RBR;
}

// Send byte via UART
void UART0_write(const char c) {
  while(!UART0_ready());
  R8_UART0_THR = c;
}

// ===================================================================================
// UART1
// ===================================================================================

// Init UART
void UART1_init(void) {
#if UART1_REMAP == 0
  R16_PIN_ALTERNATE &= ~RB_PIN_UART1;              // RXD1/TXD1 on PA8/PA9
  PIN_high(PA9); PIN_output(PA9);         // TX: output, idle high
  PIN_input_PU(PA8);                       // RX: input with pullup
#elif UART1_REMAP == 1
  R16_PIN_ALTERNATE |=  RB_PIN_UART1;              // RXD1/TXD1 on PB12/PB13
  PIN_high(PB13); PIN_output(PB13);        // TX: output, idle high
  PIN_input_PU(PB12);                      // RX: input with pullup
#else
  #warning No UART1 REMAP
#endif

  // Setup and start UART (8N1, FIFO, default BAUD rate)
  R8_UART1_DIV = 1;                       // pre-divider 1
  UART1_setBAUD(UART1_BAUD);            // set baud rate
  R8_UART1_FCR = (2 << 6) | RB_FCR_TX_FIFO_CLR | RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;
  R8_UART1_LCR = RB_LCR_WORD_SZ;          // 8 data bits, 1 stop bit, no parity
  R8_UART1_IER = RB_IER_TXD_EN;           // enable TX pin
}

// Read byte via UART
char UART1_read(void) {
  while(!UART1_available());
  return R8_UART1_RBR;
}

// Send byte via UART
void UART1_write(const char c) {
  while(!UART1_ready());
  R8_UART1_THR = c;
}

// ===================================================================================
// UART2
// ===================================================================================

// Init UART
void UART2_init(void) {
  PIN_high(PB23); PIN_output(PB23);        // TX: output, idle high
  PIN_input_PU(PB22);                      // RX: input with pullup

  // Setup and start UART (8N1, FIFO, default BAUD rate)
  R8_UART2_DIV = 1;                       // pre-divider 1
  UART2_setBAUD(UART2_BAUD);            // set baud rate
  R8_UART2_FCR = (2 << 6) | RB_FCR_TX_FIFO_CLR | RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;
  R8_UART2_LCR = RB_LCR_WORD_SZ;          // 8 data bits, 1 stop bit, no parity
  R8_UART2_IER = RB_IER_TXD_EN;           // enable TX pin
}

// Read byte via UART
char UART2_read(void) {
  while(!UART2_available());
  return R8_UART2_RBR;
}

// Send byte via UART
void UART2_write(const char c) {
  while(!UART2_ready());
  R8_UART2_THR = c;
}

// ===================================================================================
// UART3
// ===================================================================================

// Init UART
void UART3_init(void) {
  PIN_high(PA5); PIN_output(PA5);        // TX: output, idle high
  PIN_input_PU(PA4);                      // RX: input with pullup

  // Setup and start UART (8N1, FIFO, default BAUD rate)
  R8_UART3_DIV = 1;                       // pre-divider 1
  UART3_setBAUD(UART3_BAUD);            // set baud rate
  R8_UART3_FCR = (2 << 6) | RB_FCR_TX_FIFO_CLR | RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;
  R8_UART3_LCR = RB_LCR_WORD_SZ;          // 8 data bits, 1 stop bit, no parity
  R8_UART3_IER = RB_IER_TXD_EN;           // enable TX pin
}

// Read byte via UART
char UART3_read(void) {
  while(!UART3_available());
  return R8_UART3_RBR;
}

// Send byte via UART
void UART3_write(const char c) {
  while(!UART3_ready());
  R8_UART3_THR = c;
}
