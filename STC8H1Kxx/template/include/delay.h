// ===================================================================================
// Delay Functions for STC8H Microcontrollers                                 * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// DLY_us(n)                delay in units of microseconds
// DLY_ms(n)                delay in units of milliseconds
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "stc8h.h"

#if   F_CPU <=  187000
  #define DLY_us(n)   ((n >= 16) ? DLY_us_h((n)>>4) : (0))
#elif F_CPU <=  375000
  #define DLY_us(n)   ((n >=  8) ? DLY_us_h((n)>>3) : (0))
#elif F_CPU <= 1000000
  #define DLY_us(n)   ((n >=  4) ? DLY_us_h((n)>>2) : (0))
#elif F_CPU <  3000000
  #define DLY_us(n)   ((n >=  2) ? DLY_us_h((n)>>1) : (0))
#else
  #define DLY_us(n)   ((n >=  1) ? DLY_us_h(n) : (0))
#endif

#define DLY_ms(n)     ((n > 0) ? DLY_ms_h(n) : (0))

void DLY_us_h(uint16_t n);   // helper function
void DLY_ms_h(uint16_t n);   // helper function
