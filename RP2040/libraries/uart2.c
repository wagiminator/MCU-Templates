// ===================================================================================
// Basic UART2 Functions for RP2040 (UART0/UART1, no buffer, no interrupt)   * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "uart2.h"
#include "gpio.h"

// Set BAUD rate (integer and 6-bit fractional divider, 16x oversampling)
void UART2_setBAUD(uint32_t baud) {
  uint32_t div = (8 * F_PERI) / baud;             // 64 * F_PERI / (16 * baud) * 2
  uint32_t ibrd = div >> 7;
  uint32_t fbrd = ((div & 0x7F) + 1) >> 1;
  if(!ibrd)           { ibrd = 1; fbrd = 0; }
  if(ibrd >= 65535)   { ibrd = 65535; fbrd = 0; }
  while(!UART2_completed());                       // wait for ongoing transmission
  UART2_DEV->UARTIBRD = ibrd;
  UART2_DEV->UARTFBRD = fbrd;
  UART2_DEV->UARTLCR_H = UART2_DEV->UARTLCR_H;    // LCR_H write latches the divider
}

// Change line control register (data bits, stop bits, parity)
void UART2_setLCR(uint32_t mask, uint32_t val) {
  while(!UART2_completed());                       // wait for ongoing transmission
  UART2_DEV->UARTLCR_H = (UART2_DEV->UARTLCR_H & ~mask) | val;
}

// Init UART (8N1, FIFOs enabled, default BAUD rate)
void UART2_init(void) {
  RESET_cycle(UART2_RESET);                        // reset and release UART
  UART2_setBAUD(UART2_BAUD);                        // set BAUD rate
  UART2_DEV->UARTLCR_H = ((uint32_t)3 << UART_UARTLCR_H_WLEN_Pos)  // 8 data bits
                       | UART_UARTLCR_H_FEN;      // enable FIFOs
  UART2_DEV->UARTCR    = UART_UARTCR_UARTEN | UART_UARTCR_TXE | UART_UARTCR_RXE;
  UART2_DEV->UARTDMACR = UART_UARTDMACR_TXDMAE | UART_UARTDMACR_RXDMAE; // DREQ signals
  PIN_alternate(UART2_TX_PIN, GPIO_FUNC_UART);     // set pin functions
  PIN_alternate(UART2_RX_PIN, GPIO_FUNC_UART);
  PIN_pullup(UART2_RX_PIN);                        // RX pin pullup (idle high)
}

// Receive byte via UART
char UART2_read(void) {
  while(!UART2_available());                       // wait for data
  return UART2_DEV->UARTDR;                       // return received byte
}

// Send byte via UART
void UART2_write(const char c) {
  while(!UART2_ready());                           // wait for space in TX FIFO
  UART2_DEV->UARTDR = c;                          // send data byte
}
