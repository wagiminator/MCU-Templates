// ===================================================================================
// Delay Functions for STC8H Microcontrollers                                 * v1.0 *
// ===================================================================================

#pragma once
#include <stdint.h>
#include "stc8h.h"

void DLY_us(uint16_t n);   // delay in units of us
void DLY_ms(uint16_t n);   // delay in units of ms
