// ===================================================================================
// Terminal Functions via USB CDC (virtual serial port)                       * v1.0 *
// ===================================================================================
//
// Simple text output and input via the USB serial port (usb_cdc library). The output
// functions never block for long: if no terminal program has opened the port (DTR
// not set) or the host does not fetch the data, characters are dropped.
//
// Functions available:
// --------------------
// TERM_init()              init USB serial port
// TERM_connected()         check if a terminal program has opened the port (DTR)
// TERM_available()         check if received characters are waiting
// TERM_read()              read received character (check TERM_available() first)
// TERM_putchar(c)          send character ('\n' -> "\r\n")
// TERM_flush()             send buffered characters
// TERM_print(s)            print string and flush
// TERM_printf(f, ...)      printf and flush (supports %s, %c, %d, %u, %x, %b, %02d, %%)
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"
#include "print.h"
#include "usb_cdc.h"

#define TERM_TIMEOUT_US     2000        // max time to wait for a free transmit buffer

#define TERM_init()         CDC_init()
#define TERM_connected()    CDC_getDTR()
#define TERM_available()    CDC_available()
#define TERM_read()         CDC_read()

void TERM_putchar(char c);
void TERM_flush(void);
#define TERM_print(s)       (printS(TERM_putchar, s), TERM_flush())
#define TERM_printf(f, ...) (printF(TERM_putchar, f, ##__VA_ARGS__), TERM_flush())

#ifdef __cplusplus
};
#endif
