// ===================================================================================
// UART with RX Interrupt and Ring Buffer for RP2350 (UART0/UART1)            * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "uart_int.h"
#include "gpio.h"

// UART RX ring buffer
volatile uint8_t UART_buffer[UART_RX_BUF_SIZE];   // UART RX ring buffer
volatile uint8_t UART_rptr = 0;                   // UART RX buffer read pointer
volatile uint8_t UART_wptr = 0;                   // UART RX buffer write pointer

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
  PIN_alternate(UART_TX_PIN, UART_FUNC(UART_TX_PIN));   // set pin functions
  PIN_alternate(UART_RX_PIN, UART_FUNC(UART_RX_PIN));
  PIN_pullup(UART_RX_PIN);                        // RX pin pullup (idle high)
  UART_DEV->UARTIFLS  = 0;                        // RX interrupt at FIFO >= 1/8
  UART_DEV->UARTIMSC  = UART_UARTIMSC_RXIM | UART_UARTIMSC_RTIM; // RX + timeout int
  NVIC_EnableIRQ(UART_IRQn);                      // enable interrupt
}

// Receive byte via UART
char UART_read(void) {
  char result;
  while(!UART_available());                       // wait for incoming data byte
  result = UART_buffer[UART_rptr];                // get data byte from buffer
  UART_rptr = (UART_rptr + 1) & (UART_RX_BUF_SIZE - 1); // increase read pointer
  return result;                                  // return the received byte
}

// Send byte via UART
void UART_write(const char c) {
  while(!UART_ready());                           // wait for space in TX FIFO
  UART_DEV->UARTDR = c;                          // send data byte
}

// UART RX interrupt service routine
static void UART_ISR(void) {
  while(!(UART_DEV->UARTFR & UART_UARTFR_RXFE)) { // data in RX FIFO?
    uint8_t data = UART_DEV->UARTDR;              // read received byte
    uint8_t next = (UART_wptr + 1) & (UART_RX_BUF_SIZE - 1);
    if(next != UART_rptr) {                       // buffer not full?
      UART_buffer[UART_wptr] = data;              // push received byte to buffer
      UART_wptr = next;                           // increase write pointer
    }
  }
}
void UART0_IRQ_IRQHandler(void) { if(UART_NUM == 0) UART_ISR(); }
void UART1_IRQ_IRQHandler(void) { if(UART_NUM == 1) UART_ISR(); }

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
