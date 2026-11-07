// ===================================================================================
// Lightweight Print Functions for CH558 and CH559                            * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "print.h"

// Print string
void printS(PRINT_putchar putchr, const char* str) {
  while(*str) putchr(*str++);
}

// Print string with newline
void println(PRINT_putchar putchr, const char* str) {
  printS(putchr, str);
  putchr('\n');
}

// Print 32-bit unsigned decimal value
void printL(PRINT_putchar putchr, uint32_t value) {
  __idata char buf[10];
  uint8_t i = 0;
  do {
    buf[i++] = '0' + (value % 10);
    value /= 10;
  } while(value);
  while(i) putchr(buf[--i]);
}

// Print 16-bit unsigned decimal value (subtraction method, no division)
void printD(PRINT_putchar putchr, uint16_t value) {
  static __code const uint16_t div[] = {10000, 1000, 100, 10};
  uint8_t i, digit, lead = 0;
  for(i = 0; i < 4; i++) {
    digit = '0';
    while(value >= div[i]) {
      value -= div[i];
      digit++;
    }
    if(digit > '0') lead = 1;
    if(lead) putchr(digit);
  }
  putchr('0' + (uint8_t)value);
}

// Print 16-bit signed decimal value
void printI(PRINT_putchar putchr, int16_t value) {
  uint16_t v = (uint16_t)value;
  if(value < 0) {
    putchr('-');
    v = (uint16_t)0 - v;
  }
  printD(putchr, v);
}

// Print 4-bit hex nibble
static void printN(PRINT_putchar putchr, uint8_t nibble) {
  nibble &= 0x0f;
  putchr(nibble + (nibble < 10 ? '0' : 'A' - 10));
}

// Print 8-bit hex value
void printB(PRINT_putchar putchr, uint8_t value) {
  printN(putchr, value >> 4);
  printN(putchr, value);
}

// Print 16-bit hex value
void printH(PRINT_putchar putchr, uint16_t value) {
  printB(putchr, value >> 8);
  printB(putchr, value);
}

// Print 32-bit hex value
void printW(PRINT_putchar putchr, uint32_t value) {
  printH(putchr, value >> 16);
  printH(putchr, value);
}
