// ===================================================================================
// Delay Functions for CH558 and CH559                                        * v1.1 *
// ===================================================================================
//
// Functions available:
// --------------------
// DLY_us(n)                delay n microseconds (n = 0..65535)
// DLY_ms(n)                delay n milliseconds (n = 0..65535)
//
// Notes:
// ------
// - Cycle-counted loops, interrupts extend the delay.
// - At F_CPU = 4MHz the resolution is 2us, at 3MHz 4us.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>

void DLY_us(uint16_t n);    // delay in units of us
void DLY_ms(uint16_t n);    // delay in units of ms
