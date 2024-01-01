// ===================================================================================
// USART0 with RX Interrupt and Ring Buffer for tinyAVR                       * v1.0 *
// ===================================================================================
// 2021 by Stefan Wagner:   https://github.com/wagiminator

#include <stdarg.h>
#include "uart_int.h"

volatile uint8_t UART_buffer[64];     // UART RX ring buffer
volatile uint8_t UART_rptr = 0;       // UART RX buffer read pointer
volatile uint8_t UART_wptr = 0;       // UART RX buffer write pointer

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
  USART0.BAUD  = (((F_CPU * 8) / UART_BAUD) + 1) / 2; // set BAUD
  USART0.CTRLA = USART_RXCIE_bm;                // enable RX complete interrupt
  USART0.CTRLB = USART_TXEN_bm | USART_RXEN_bm; // enable USART0 RX/TX
  INT_enable();                                 // global interrupt enable
}

// Read byte via UART
char UART_read(void) {
  while(!UART_available());                     // wait for incoming data byte
  char result = UART_buffer[UART_rptr++];       // get data byte from buffer
  UART_rptr &= (sizeof(UART_buffer) - 1);       // increase ring buffer read pointer
  return result;                                // return the received byte
}

// Send byte via UART
void UART_write(const char c) {
  while(!UART_ready());                         // wait for ready to write
  USART0.TXDATAL = c;                           // send character
}

// UART RXC interrupt service routine
ISR(USART0_RXC_vect) {
  UART_buffer[UART_wptr++] = USART0.RXDATAL;    // push received byte to buffer...
  UART_wptr &= (sizeof(UART_buffer) - 1);       // increase ring buffer write pointer
}
