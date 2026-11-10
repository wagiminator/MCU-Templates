// ===================================================================================
// Terminal Functions via USB CDC (virtual serial port)                       * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "term.h"

// Wait for free transmit buffer (max. TERM_TIMEOUT_US), returns 0 on timeout
static uint8_t TERM_waitReady(void) {
  uint32_t n = TERM_TIMEOUT_US;
  while(!CDC_ready()) {
    if(!n--) return 0;
    DLY_us(1);
  }
  return 1;
}

// Send character via USB serial port (with carriage return before newline)
void TERM_putchar(char c) {
  if(!TERM_connected()) return;               // no terminal: drop character
  if(c == '\n') {
    if(!TERM_waitReady()) return;
    CDC_write('\r');
  }
  if(!TERM_waitReady()) return;
  CDC_write(c);
}

// Send buffered characters (waits for the previous transfer to be completed)
void TERM_flush(void) {
  if(TERM_connected() && TERM_waitReady()) CDC_flush();
}
