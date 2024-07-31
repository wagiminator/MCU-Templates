// ===================================================================================
// Basic USART Functions (no buffer, no interrupt) for tinyAVR                * v1.0 *
// ===================================================================================
// 2021 by Stefan Wagner:   https://github.com/wagiminator

#include <stdarg.h>
#include "uart.h"

// Init UART
void UART_init(void) {
  // Set GPIO pins
  #if UART_MAP == 0
    // Setup pin PB2 (TX) and pin PB3 (RX)
    VPORTB.DIR |= (1 << 2);
    PORTB.PIN3CTRL |= PORT_PULLUPEN_bm;
  #elif UART_MAP == 1
    // Setup pin PA1 (TX) and pin PA2 (RX)
    VPORTA.DIR |= (1 << 1);
    PORTA.PIN2CTRL |= PORT_PULLUPEN_bm;
    PORTMUX.CTRLB |= PORTMUX_USART0_bm;
  #else
    #warning No automatic pin mapping for USART0
  #endif
	
  // Setup and start UART (8N1, RX/TX, default BAUD rate)
  USART0.BAUD  = (((F_CPU * 8) / UART_BAUD) + 1) / 2;
  USART0.CTRLB = USART_TXEN_bm | USART_RXEN_bm;
}

// Read byte via UART
char UART_read(void) {
  while(!UART_available());
  return USART0.RXDATAL;
}

// Send byte via UART
void UART_write(const char c) {
  while(!UART_ready());
  USART0.TXDATAL = c;
}
