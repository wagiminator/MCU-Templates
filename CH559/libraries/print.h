// ===================================================================================
// Lightweight Print Functions for CH558 and CH559                            * v1.0 *
// ===================================================================================
//
// Small replacement for printf (which needs 1-2KB of flash). Output goes to any
// function with the signature void f(char c), e.g. UART0_write or CDC_write.
//
// Functions available:
// --------------------
// printS(putchr, s)        print string
// println(putchr, s)       print string with newline
// printD(putchr, n)        print 16-bit unsigned decimal value
// printI(putchr, n)        print 16-bit signed decimal value
// printL(putchr, n)        print 32-bit unsigned decimal value
// printB(putchr, n)        print  8-bit hex value (2 digits)
// printH(putchr, n)        print 16-bit hex value (4 digits)
// printW(putchr, n)        print 32-bit hex value (8 digits)
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>

typedef void (*PRINT_putchar)(char c);

void printS(PRINT_putchar putchr, const char* str);
void println(PRINT_putchar putchr, const char* str);
void printD(PRINT_putchar putchr, uint16_t value);
void printI(PRINT_putchar putchr, int16_t value);
void printL(PRINT_putchar putchr, uint32_t value);
void printB(PRINT_putchar putchr, uint8_t value);
void printH(PRINT_putchar putchr, uint16_t value);
void printW(PRINT_putchar putchr, uint32_t value);
