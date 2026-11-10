// ===================================================================================
// Millis Functions for RP2350 (using the 64-bit microsecond TIMER0)          * v1.0 *
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
// The millis (MIL) functions use the free-running 64-bit microsecond counter of the
// TIMER0 peripheral (no interrupt required), so the delay (DLY) functions, which use
// the SysTick, continue to work properly. The TIMER0 alarms remain available for other
// purposes (e.g. timer library, SLEEP_ms). TIMER0 counts with the 1us tick of the
// tick generator (TICKS), which is set up by CLK_init(). TIMER1 is not used.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

void MIL_init(void);
void MIL_reset(void);
uint32_t MIL_read(void);
uint32_t MIC_read(void);

#ifdef __cplusplus
};
#endif
