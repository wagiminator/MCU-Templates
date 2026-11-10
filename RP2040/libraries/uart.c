// ===================================================================================
// Basic UART Functions for RP2040 (UART0/UART1, no buffer, no interrupt)    * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "uart.h"
#include "gpio.h"

// Set BAUD rate (integer and 6-bit fractional divider, 16x oversampling)
void UART_setBAUD(uint32_t baud) {
  uint32_t div = (8 * F_PERI) / baud;             // 64 * F_PERI / (16 * baud) * 2
  uint32_t ibrd = div >> 7;
  uint32_t fbrd = ((div & 0x7F) + 1) >> 1;
  if(!ibrd)           { ibrd = 1; fbrd = 0; }
  if(ibrd >= 65535)   { ibrd = 65535; fbrd = 0; }
  while(!UART_completed());                       // wait for ongoing transmission
  UART_DEV->UARTIBRD = ibrd;
  UART_DEV->UARTFBRD = fbrd;
  UART_DEV->UARTLCR_H = UART_DEV->UARTLCR_H;    // LCR_H write latches the divider
}

// Change line control register (data bits, stop bits, parity)
void UART_setLCR(uint32_t mask, uint32_t val) {
  while(!UART_completed());                       // wait for ongoing transmission
  UART_DEV->UARTLCR_H = (UART_DEV->UARTLCR_H & ~mask) | val;
}

// Init UART (8N1, FIFOs enabled, default BAUD rate)
void UART_init(void) {
  RESET_cycle(UART_RESET);                        // reset and release UART
  UART_setBAUD(UART_BAUD);                        // set BAUD rate
  UART_DEV->UARTLCR_H = ((uint32_t)3 << UART_UARTLCR_H_WLEN_Pos)  // 8 data bits
                       | UART_UARTLCR_H_FEN;      // enable FIFOs
  UART_DEV->UARTCR    = UART_UARTCR_UARTEN | UART_UARTCR_TXE | UART_UARTCR_RXE;
  UART_DEV->UARTDMACR = UART_UARTDMACR_TXDMAE | UART_UARTDMACR_RXDMAE; // DREQ signals
  PIN_alternate(UART_TX_PIN, GPIO_FUNC_UART);     // set pin functions
  PIN_alternate(UART_RX_PIN, GPIO_FUNC_UART);
  PIN_pullup(UART_RX_PIN);                        // RX pin pullup (idle high)
}

// Receive byte via UART
char UART_read(void) {
  while(!UART_available());                       // wait for data
  return UART_DEV->UARTDR;                       // return received byte
}

// Send byte via UART
void UART_write(const char c) {
  while(!UART_ready());                           // wait for space in TX FIFO
  UART_DEV->UARTDR = c;                          // send data byte
}
