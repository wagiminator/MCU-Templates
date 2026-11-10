// ===================================================================================
// UART with RX Interrupt and Ring Buffer for CH573/CH571                     * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "uart_int.h"
#include "gpio.h"

// UART RX ring buffer
volatile uint8_t UART_buffer[UART_RX_BUF_SIZE];   // UART RX ring buffer
volatile uint8_t UART_rptr = 0;                   // UART RX buffer read pointer
volatile uint8_t UART_wptr = 0;                   // UART RX buffer write pointer

// Init UART
void UART_init(void) {
  // Setup pins
#if   (UART_NUM == 0) && (UART_REMAP == 0)
  R16_PIN_ALTERNATE &= ~RB_PIN_UART0;             // RXD0/TXD0 on PB4/PB7
  PIN_high(PB7); PIN_output(PB7);                 // TX: output, idle high
  PIN_input_PU(PB4);                              // RX: input with pullup
#elif (UART_NUM == 0) && (UART_REMAP == 1)
  R16_PIN_ALTERNATE |=  RB_PIN_UART0;             // RXD0/TXD0 on PA15/PA14
  PIN_high(PA14); PIN_output(PA14);
  PIN_input_PU(PA15);
#elif (UART_NUM == 1) && (UART_REMAP == 0)
  R16_PIN_ALTERNATE &= ~RB_PIN_UART1;             // RXD1/TXD1 on PA8/PA9
  PIN_high(PA9); PIN_output(PA9);
  PIN_input_PU(PA8);
#elif (UART_NUM == 1) && (UART_REMAP == 1)
  R16_PIN_ALTERNATE |=  RB_PIN_UART1;             // RXD1/TXD1 on PB12/PB13
  PIN_high(PB13); PIN_output(PB13);
  PIN_input_PU(PB12);
#elif UART_NUM == 2
  PIN_high(PB23); PIN_output(PB23);               // TXD2 on PB23
  PIN_input_PU(PB22);                             // RXD2 on PB22
#elif UART_NUM == 3
  PIN_high(PA5); PIN_output(PA5);                 // TXD3 on PA5
  PIN_input_PU(PA4);                              // RXD3 on PA4
#else
  #error Wrong UART_NUM or UART_REMAP
#endif

  // Setup and start UART (8N1, FIFO, default BAUD rate, RX interrupt)
  UART_R_DIV = 1;                                 // pre-divider 1
  UART_setBAUD(UART_BAUD);                        // set baud rate
  UART_R_FCR = (2 << 6)                           // RX FIFO trigger level: 4 bytes
             | RB_FCR_TX_FIFO_CLR | RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;
  UART_R_LCR = RB_LCR_WORD_SZ;                    // 8 data bits, 1 stop bit, no parity
  UART_R_IER = RB_IER_TXD_EN                      // enable TX pin
             | RB_IER_RECV_RDY;                   // RX data (and timeout) interrupt
  UART_R_MCR = RB_MCR_INT_OE;                     // enable interrupt output
  NVIC_EnableIRQ(UART_IRQn);                      // enable UART interrupt
}

// Read byte from RX buffer
char UART_read(void) {
  char result;
  while(!UART_available());                       // wait for incoming data byte
  result = UART_buffer[UART_rptr];                // get data byte from buffer
  UART_rptr = (UART_rptr + 1) & (UART_RX_BUF_SIZE - 1); // increase read pointer
  return result;                                  // return the received byte
}

// Send byte via UART
void UART_write(const char c) {
  while(!UART_ready());
  UART_R_THR = c;
}

// UART interrupt service routine (RX data available or RX timeout)
void UART_IRQHandler(void) __attribute__((interrupt));
void UART_IRQHandler(void) {
  (void)UART_R_IIR;                               // read interrupt ID
  while(UART_R_LSR & RB_LSR_DATA_RDY) {           // data in RX FIFO?
    uint8_t data = UART_R_RBR;                    // read received byte
    uint8_t next = (UART_wptr + 1) & (UART_RX_BUF_SIZE - 1);
    if(next != UART_rptr) {                       // buffer not full?
      UART_buffer[UART_wptr] = data;              // push received byte to buffer
      UART_wptr = next;                           // increase write pointer
    }
  }
}

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
