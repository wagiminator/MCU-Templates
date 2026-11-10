// ===================================================================================
// Millis Functions for CH573/CH571                                           * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// MIL_init()               init and start millis counter at zero
// MIL_read()               read current millis counter value (32-bit)
// MIL_reset()              reset millis counter to zero
// MIC_read()               read current micros counter value (32-bit)
//
// Notes:
// ------
// The millis (MIL) functions use the 64-bit SysTick counter, which is started at
// system start (SYS_TICK_INIT in system.h). No interrupt is used. The delay (DLY)
// functions continue to work properly.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

#if SYS_TICK_INIT == 0
  #error SysTick must be started (SYS_TICK_INIT in system.h)!
#endif

void     MIL_init(void);              // init and start millis counter at zero
uint32_t MIL_read(void);              // read millis counter
uint32_t MIC_read(void);              // read micros counter

#define MIL_reset   MIL_init          // reset millis counter

#ifdef __cplusplus
};
#endif
