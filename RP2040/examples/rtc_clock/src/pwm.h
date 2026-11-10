// ===================================================================================
// Basic PWM Functions for RP2350 (PWM slices 0..7)                          * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// PWM_init()               init all PWM slices with frequency and resolution set below
// PWM_start(PIN)           start PWM output on PIN (any pin GP0..GP29)
// PWM_stop(PIN)            stop PWM output on PIN (PIN becomes LOW output)
// PWM_write(PIN, val)      set PWM duty cycle of PIN (0 .. PWM_TOP+1 = 0% .. 100%)
// PWM_setTOP(n)            set PWM TOP value of all slices (period = n + 1 clock cycles)
// PWM_setPRESC(n)          set PWM clock prescaler of all slices (n = 1..255)
// PWM_enable()             enable all slices (start counters)
// PWM_disable()            disable all slices (stop counters)
//
// PWM pins and channels:
// ----------------------
// Each pin GPn belongs to slice (n / 2) % 8 and channel A (even n) or B (odd n), e.g.
// GP0: 0A, GP1: 0B, GP2: 1A, ... GP15: 7B, GP16: 0A, ... GP29: 6B.
//
// Notes:
// ------
// - Pins of the same slice and channel output the same PWM signal (e.g. GP0 and GP16).
// - PWM frequency = F_CPU / PRESCALER / (PWM_TOP + 1), e.g. 150MHz / 1 / 256 = 586kHz
// - All counters are 16-bit (PWM_TOP <= 65534).
// - The RP2350A has 8 PWM slices, the RP2350B has 12 (slices 8..11 on GP32..GP47).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"
#include "gpio.h"

// PWM parameters
#define PWM_TOP           255             // PWM TOP value (resolution: TOP + 1 steps)
#define PWM_PRESC         1               // PWM clock prescaler (1..255)

// PWM slice and channel of PIN
#define PWM_SLICE(PIN)    (((PIN) >> 1) & 7)
#define PWM_CHANNEL(PIN)  ((PIN) & 1)

// PWM functions
void PWM_init(void);                      // init all slices
void PWM_setTOP(uint16_t top);            // set TOP value of all slices
void PWM_setPRESC(uint8_t presc);         // set prescaler of all slices

#define PWM_enable()      (PWM->EN = 0xFF)
#define PWM_disable()     (PWM->EN = 0x00)
#define PWM_start(PIN)    PIN_alternate(PIN, GPIO_FUNC_PWM)
#define PWM_stop(PIN)     (PIN_low(PIN), PIN_output(PIN))

// Set duty cycle of PIN (channel A: bits 15:0, channel B: bits 31:16 of CC register)
static inline void PWM_write(uint8_t pin, uint16_t val) {
  if(PWM_CHANNEL(pin)) REG_WRITE_MASK(PWM->CH[PWM_SLICE(pin)].CC, (uint32_t)val << 16, 0xFFFF0000);
  else                 REG_WRITE_MASK(PWM->CH[PWM_SLICE(pin)].CC, val, 0x0000FFFF);
}

#ifdef __cplusplus
};
#endif
