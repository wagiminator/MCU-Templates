// ===================================================================================
// Basic Serial Debug Functions for RP2350 (UART0/UART1, TX only)            * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// DEBUG_init()             Init serial DEBUG (8N1, BAUD: 115200)
// DEBUG_setBAUD(n)         Set BAUD rate
//
// DEBUG_write(c)           Send character
// DEBUG_print(s)           Send string
// DEBUG_println(s)         Send string with newline
// DEBUG_printS(s)          Send string (alias)
// DEBUG_printD(n)          Send decimal value as string
// DEBUG_printW(n)          Send 32-bit hex word value as string
// DEBUG_printH(n)          Send 16-bit hex half-word value as string
// DEBUG_printB(n)          Send 8-bit hex byte value as string
// DEBUG_newline()          Send newline
// DEBUG_printf(s, ...)     Uses printf (supports %s, %c, %d, %u, %x, %b, %02d, %%)
//
// UART TX pins (set below in DEBUG parameters):
// ----------------------------------------------
// UART0: GP0, GP12, GP16, GP28, GP2, GP14, GP18 (the latter with UART_AUX function)
// UART1: GP4, GP8,  GP20, GP24, GP6, GP10, GP22, GP26 (the latter with UART_AUX)
//
// Notes:
// ------
// - The UART (UART0 or UART1) is selected automatically by the TX pin.
// - Don't use the same UART for DEBUG and other libraries (e.g. uart) at the same
//   time. Default: UART0 on GP0 (Raspberry Pi Debug Probe: connect to its RX).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// DEBUG parameters
#define DEBUG_ENABLE      1               // enable serial DEBUG (0:no, 1:yes)
#define DEBUG_TX          GP0             // UART TX pin (see above)
#define DEBUG_BAUD        115200          // default UART baud rate

// DEBUG UART instance (selected by TX pin, evaluated by the compiler)
#define DEBUG_NUM         (((DEBUG_TX >> 2) ^ (DEBUG_TX >> 3)) & 1)
#define DEBUG_DEV         (DEBUG_NUM ? UART1 : UART0)
#define DEBUG_RESET       (DEBUG_NUM ? RESETS_RESET_UART1 : RESETS_RESET_UART0)

_Static_assert((DEBUG_TX & 1) == 0, "Wrong DEBUG TX pin");

// DEBUG functions and macros
#if DEBUG_ENABLE > 0
  void DEBUG_init(void);                  // init UART with default BAUD rate
  void DEBUG_setBAUD(uint32_t baud);      // set BAUD rate
  void DEBUG_write(const char c);         // send character via UART
  void DEBUG_print(const char* str);      // send string via UART
  void DEBUG_println(const char* str);    // send string with newline via UART
  void DEBUG_printD(uint32_t value);      // send decimal value as string
  void DEBUG_printW(uint32_t value);      // send 32-bit hex word value as string
  void DEBUG_printH(uint16_t value);      // send 16-bit hex half-word value as string
  void DEBUG_printB(uint8_t value);       // send 8-bit hex byte value as string
  void DEBUG_printf(const char *format, ...); // use printf (requires more memory)
#else
  #define DEBUG_init()
  #define DEBUG_setBAUD(n)
  #define DEBUG_write(x)
  #define DEBUG_print(x)
  #define DEBUG_println(x)
  #define DEBUG_printD(x)
  #define DEBUG_printW(x)
  #define DEBUG_printH(x)
  #define DEBUG_printB(x)
  #define DEBUG_printf(f, ...)
#endif

#define DEBUG_newline()   DEBUG_write('\n')                   // send newline
#define DEBUG_printS      DEBUG_print                         // alias for print

#ifdef __cplusplus
};
#endif
