// ===================================================================================
// UART with RX Interrupt and Ring Buffer for CH558 and CH559                 * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "uart_int.h"

#if (UART_BUF_SIZE & (UART_BUF_SIZE - 1)) || (UART_BUF_SIZE > 128)
  #error UART_BUF_SIZE must be a power of 2 (max 128)
#endif

__xdata uint8_t UART_buffer[UART_BUF_SIZE]; // RX ring buffer
volatile __idata uint8_t UART_rptr = 0;     // RX buffer read pointer
volatile __idata uint8_t UART_wptr = 0;     // RX buffer write pointer

// Init UART
void UART_init(void) {
#if UART_PORT == 0
  #if UART_ALTER == 1
  PIN_FUNC |= bUART0_PIN_X;                 // UART0 set alternate pins
  #endif
  SCON   = 0x50;                            // UART0 mode 1 (8 bits), RX enable
  RCLK   = 0;                               // UART0 receive clock:  TIMER1
  TCLK   = 0;                               // UART0 transmit clock: TIMER1
  PCON  |= SMOD;                            // UART0 fast BAUD rate
  TMOD   = TMOD & ~(bT1_GATE | bT1_CT | MASK_T1_MOD) | bT1_M1; // TIMER1 8-bit auto-reload
  T2MOD  = T2MOD & ~bT1_CLK | UART_T2MOD;   // TIMER1 clock
  TH1    = (uint8_t)(256 - UART_BAUD_DIV);  // TIMER1 configure for BAUD rate
  TR1    = 1;                               // TIMER1 start
  TI     = 1;                               // UART0 set transmit complete flag
  ES     = 1;                               // UART0 enable interrupt
#elif UART_PORT == 1
  SER1_LCR = bLCR_DLAB;                     // access divisor latches
  SER1_DIV = 1;                             // pre-divisor 1
  SER1_DLM = (uint8_t)(UART_BAUD_DIV >> 8); // set divisor
  SER1_DLL = (uint8_t)(UART_BAUD_DIV);
  SER1_LCR = bLCR_WORD_SZ1 | bLCR_WORD_SZ0; // 8N1, close divisor latches
  SER1_FCR = bFCR_FIFO_EN | bFCR_T_FIFO_CLR | bFCR_R_FIFO_CLR; // enable and clear FIFO
  #if UART_ALTER == 1
  SER1_IER = bIER_PIN_MOD0 | bIER_RECV_RDY; // RXD1/TXD1 on P4.0/P4.4, RX interrupt
  P4_DIR  |= (1 << 4);                      // TXD1 pin as output
  #else
  SER1_IER = bIER_PIN_MOD1 | bIER_RECV_RDY; // RXD1/TXD1 on P2.6/P2.7, RX interrupt
  P2_DIR  |= (1 << 7);                      // TXD1 pin as output
  #endif
  SER1_MCR = bMCR_OUT2;                     // enable interrupt request output
  IE_UART1 = 1;                             // UART1 enable interrupt
#else
  #error Wrong UART_PORT
#endif
  EA = 1;                                   // enable global interrupts
}

// Read byte from buffer
char UART_read(void) {
  char c;
  while(!UART_available());                 // wait for data
  c = UART_buffer[UART_rptr];
  UART_rptr = (UART_rptr + 1) & (UART_BUF_SIZE - 1);
  return c;
}

// Write byte
void UART_write(char c) {
#if UART_PORT == 0
  while(!TI);
  TI = 0;
  SBUF = c;
#else
  while(!UART_ready());
  SER1_THR = c;
#endif
}

// Interrupt service routine (only RX, TX flag is polled)
#if UART_PORT == 0
void UART_interrupt(void) __interrupt(INT_NO_UART0) {
  if(RI) {
    uint8_t next = (UART_wptr + 1) & (UART_BUF_SIZE - 1);
    RI = 0;
    if(next != UART_rptr) {                 // buffer not full?
      UART_buffer[UART_wptr] = SBUF;
      UART_wptr = next;
    }
  }
}
#else
void UART_interrupt(void) __interrupt(INT_NO_UART1) {
  while(SER1_LSR & bLSR_DATA_RDY) {         // data in RX FIFO?
    uint8_t next = (UART_wptr + 1) & (UART_BUF_SIZE - 1);
    uint8_t data = SER1_RBR;
    if(next != UART_rptr) {                 // buffer not full?
      UART_buffer[UART_wptr] = data;
      UART_wptr = next;
    }
  }
}
#endif
