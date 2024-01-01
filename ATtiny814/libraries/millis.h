// ===================================================================================
// Millis Functions for tinyAVR 0-Series and 1-Series                         * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// MIL_init()               init and start millis counter
// MIL_read()               read current millis counter value (32-bit)
//
// 2021 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "system.h"

// Millis parameters
#define MIL_TIMER           0   // 0: TCB0 - more accurate, 1: PIT - less accurate

// Millis functions
void MIL_init(void);
uint32_t MIL_read(void);

#ifdef __cplusplus
};
#endif
