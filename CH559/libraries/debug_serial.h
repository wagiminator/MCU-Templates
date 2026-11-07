// ===================================================================================
// Serial Debug Functions for CH558 and CH559                                 * v1.0 *
// ===================================================================================
//
// Basic UART TX debug functions. Use printf or the lightweight functions below.
//
// Functions available:
// --------------------
// DEBUG_init()             init UART for debug output
// DEBUG_write(c)           send character
// DEBUG_print(s)           send string
// DEBUG_println(s)         send string with newline
// DEBUG_printD(n)          send 16-bit decimal value
// DEBUG_printH(n)          send 16-bit hex value
// DEBUG_printB(n)          send  8-bit hex value
// putchar(c)               send character (used by printf)
//
// Notes:
// ------
// - printf needs 1-2KB of flash. Use printf_tiny (no field width, no long) or the
//   DEBUG_print functions to save memory.
// - Pins: UART0 TXD P3.1 (P0.3 if DEBUG_ALTER), UART1 TXD1 P2.7 (P4.4 if DEBUG_ALTER).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdio.h>
#include <stdint.h>
#include "ch559.h"

// DEBUG parameters
#define DEBUG_PORT      0                   // UART port (0 or 1)
#define DEBUG_ALTER     0                   // UART port alternate pins (0:no, 1:yes)
#define DEBUG_BAUD      115200              // UART baud rate

void DEBUG_init(void);                      // init UART
void DEBUG_write(char c);                   // send character
void DEBUG_print(const char* str);          // send string
void DEBUG_println(const char* str);        // send string with newline
void DEBUG_printD(uint16_t value);          // send 16-bit decimal value
void DEBUG_printH(uint16_t value);          // send 16-bit hex value
void DEBUG_printB(uint8_t value);           // send  8-bit hex value

// UART functions for printf
#if SDCC < 370
void putchar(char c);                       // send character (for printf)
#else
int putchar(int c);                         // send character (for printf)
#endif
