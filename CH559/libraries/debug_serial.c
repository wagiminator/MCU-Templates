// ===================================================================================
// Serial Debug Functions for CH558 and CH559                                 * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "debug_serial.h"

#if DEBUG_PORT == 0
// ===================================================================================
// UART0 (TIMER1 as BAUD rate generator, clock Fsys or Fsys/12, SMOD = 1)
// ===================================================================================
#if (((F_CPU / 8 / DEBUG_BAUD) + 1) / 2) <= 255
  #define DEBUG_DIV     (((F_CPU / 8 / DEBUG_BAUD) + 1) / 2)
  #define DEBUG_T2MOD   (bTMR_CLK | bT1_CLK)
  #define DEBUG_FCLK    (F_CPU)
#else
  #define DEBUG_DIV     (((F_CPU / 96 / DEBUG_BAUD) + 1) / 2)
  #define DEBUG_T2MOD   0
  #define DEBUG_FCLK    (F_CPU / 12)
#endif
#if DEBUG_DIV < 1 || DEBUG_DIV > 255
  #error DEBUG_BAUD not possible with this F_CPU
#elif ((DEBUG_FCLK / 16 / DEBUG_DIV) > (DEBUG_BAUD + DEBUG_BAUD / 50)) || \
      ((DEBUG_FCLK / 16 / DEBUG_DIV) < (DEBUG_BAUD - DEBUG_BAUD / 50))
  #warning DEBUG_BAUD error > 2% with this F_CPU
#endif

void DEBUG_init(void) {
  #if DEBUG_ALTER == 1
  PIN_FUNC |= bUART0_PIN_X;                 // UART0 set alternate pins
  #endif
  SCON   = 0x40;                            // UART0 mode 1 (8 bits)
  RCLK   = 0;                               // UART0 receive clock:  TIMER1
  TCLK   = 0;                               // UART0 transmit clock: TIMER1
  PCON  |= SMOD;                            // UART0 fast BAUD rate
  TMOD   = TMOD & ~(bT1_GATE | bT1_CT | MASK_T1_MOD) | bT1_M1; // TIMER1 8-bit auto-reload
  T2MOD  = T2MOD & ~bT1_CLK | DEBUG_T2MOD;  // TIMER1 clock
  TH1    = (uint8_t)(256 - DEBUG_DIV);      // TIMER1 configure for BAUD rate
  TR1    = 1;                               // TIMER1 start
  TI     = 1;                               // UART0 set transmit complete flag
}

void DEBUG_write(char c) {
  while(!TI);
  TI = 0;
  SBUF = c;
}

#elif DEBUG_PORT == 1
// ===================================================================================
// UART1 (divisor = Fsys * 2 / SER1_DIV / 16 / BAUD, SER1_DIV = 1)
// ===================================================================================
#define DEBUG_DIV       (((F_CPU / 4 / DEBUG_BAUD) + 1) / 2)
#if DEBUG_DIV < 1 || DEBUG_DIV > 65535
  #error DEBUG_BAUD not possible with this F_CPU
#elif ((F_CPU / 8 / DEBUG_DIV) > (DEBUG_BAUD + DEBUG_BAUD / 50)) || \
      ((F_CPU / 8 / DEBUG_DIV) < (DEBUG_BAUD - DEBUG_BAUD / 50))
  #warning DEBUG_BAUD error > 2% with this F_CPU
#endif

void DEBUG_init(void) {
  SER1_LCR = bLCR_DLAB;                     // access divisor latches
  SER1_DIV = 1;                             // pre-divisor 1
  SER1_DLM = (uint8_t)(DEBUG_DIV >> 8);     // set divisor
  SER1_DLL = (uint8_t)(DEBUG_DIV);
  SER1_LCR = bLCR_WORD_SZ1 | bLCR_WORD_SZ0; // 8N1, close divisor latches
  SER1_FCR = bFCR_FIFO_EN | bFCR_T_FIFO_CLR | bFCR_R_FIFO_CLR; // enable and clear FIFO
  #if DEBUG_ALTER == 1
  SER1_IER = bIER_PIN_MOD0;                 // TXD1 on P4.4
  P4_DIR  |= (1 << 4);                      // TXD1 pin as output
  #else
  SER1_IER = bIER_PIN_MOD1;                 // TXD1 on P2.7
  P2_DIR  |= (1 << 7);                      // TXD1 pin as output
  #endif
}

void DEBUG_write(char c) {
  while(!(SER1_LSR & bLSR_T_FIFO_EMP));
  SER1_THR = c;
}

#else
  #error Wrong DEBUG_PORT
#endif

// Send character for printf
#if SDCC < 370
void putchar(char c) {
  DEBUG_write(c);
}
#else
int putchar(int c) {
  DEBUG_write(c);
  return c;
}
#endif

// Send string
void DEBUG_print(const char* str) {
  while(*str) DEBUG_write(*str++);
}

// Send string with newline
void DEBUG_println(const char* str) {
  DEBUG_print(str);
  DEBUG_write('\n');
}

// Send 16-bit decimal value
void DEBUG_printD(uint16_t value) {
  static __code const uint16_t div[] = {10000, 1000, 100, 10};
  uint8_t i, digit, lead = 0;
  for(i = 0; i < 4; i++) {
    digit = '0';
    while(value >= div[i]) {
      value -= div[i];
      digit++;
    }
    if(digit > '0') lead = 1;
    if(lead) DEBUG_write(digit);
  }
  DEBUG_write('0' + (uint8_t)value);
}

// Send 8-bit hex value
void DEBUG_printB(uint8_t value) {
  uint8_t n = value >> 4;
  DEBUG_write(n + (n < 10 ? '0' : 'A' - 10));
  n = value & 0x0f;
  DEBUG_write(n + (n < 10 ? '0' : 'A' - 10));
}

// Send 16-bit hex value
void DEBUG_printH(uint16_t value) {
  DEBUG_printB(value >> 8);
  DEBUG_printB(value);
}
