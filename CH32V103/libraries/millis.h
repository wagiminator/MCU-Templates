// ===================================================================================
// Millis Functions for CH32V103                                              * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// MIL_init()               init and start millis counter at zero
// MIL_read()               read current millis counter value (32-bit)
// MIL_reset()              reset millis counter to zero
//
// Notes:
// ------
// The millis (MIL) functions use the SysTick compare interrupt. The SysTick counter
// keeps running freely, so the delay (DLY) functions continue to work properly.
// On the QingKe V3A core the SysTick registers can only be written byte by byte.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

#if SYS_USE_VECTORS == 0
  #error Interrupt vector table must be enabled (SYS_USE_VECTORS in system.h)!
#endif

void MIL_init(void);
void MIL_reset(void);
uint32_t MIL_read(void);

#ifdef __cplusplus
};
#endif
