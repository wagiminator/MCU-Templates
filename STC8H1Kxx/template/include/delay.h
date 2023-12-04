// ===================================================================================
// Delay Functions for STC8H Microcontrollers                                 * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "stc8h.h"

void DLY_us(uint16_t n);   // delay in units of us
void DLY_ms(uint16_t n);   // delay in units of ms
