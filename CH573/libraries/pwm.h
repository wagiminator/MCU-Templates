// ===================================================================================
// Basic PWM Functions (PWMX, 8-bit) for CH573/CH571                          * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// PWM_init()               init PWMX module (clock and cycle, see parameters below)
// PWM_start(PIN)           start PWM output on PIN
// PWM_stop(PIN)            stop PWM output on PIN (PIN keeps last state as GPIO)
// PWM_write(PIN, val)      set PWM duty cycle (0..255 -> 0..255/256)
// PWM_invert(PIN)          invert PWM output of PIN
//
// PWM_INT_enable()         enable PWM cycle end interrupt
// PWM_INT_disable()        disable PWM cycle end interrupt
// PWM_ISR { }              PWM interrupt service routine (call PWM_INT_clear())
// PWM_INT_clear()          clear PWM cycle end interrupt flag
//
// PWM pins (channel):
// -------------------
// PA12 (PWM4), PA13 (PWM5), PB0 (PWM6), PB4 (PWM7), PB6 (PWM8), PB7 (PWM9),
// PB14 (PWM10), PB23 (PWM11). The CH573F has no PB0 and PB6.
//
// Notes:
// ------
// - PWM frequency = F_CPU / PWM_DIV / 256, e.g. 60MHz / 4 / 256 = 58.6kHz.
// - The CH571 has no PWMX module, use the timer PWM instead (see timer.h).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"
#include "gpio.h"

// PWM Parameters
#define PWM_DIV             4           // PWM clock divider (1..255)

// PWM channel bit of PIN (0 if PIN has no PWM)
#define PWM_CHANNEL(PIN) \
  ( (PIN) == PA12 ? 0x01 : (PIN) == PA13 ? 0x02 : (PIN) == PB0  ? 0x04 : (PIN) == PB4  ? 0x08 : \
    (PIN) == PB6  ? 0x10 : (PIN) == PB7  ? 0x20 : (PIN) == PB14 ? 0x40 : (PIN) == PB23 ? 0x80 : 0 )

// PWM data register of PIN
#define PWM_DATA(PIN) \
  (*((volatile uint8_t*)&R8_PWM4_DATA + ( \
    (PIN) == PA12 ? 0 : (PIN) == PA13 ? 1 : (PIN) == PB0  ? 2 : (PIN) == PB4  ? 3 : \
    (PIN) == PB6  ? 4 : (PIN) == PB7  ? 5 : (PIN) == PB14 ? 6 : 7 )))

// PWM Macros
#define PWM_init()          {R8_PWM_CLOCK_DIV = PWM_DIV; R8_PWM_CONFIG = 0; R8_PWM_POLAR = 0;}
#define PWM_start(PIN)      {PIN_output(PIN); R8_PWM_OUT_EN |= PWM_CHANNEL(PIN);}
#define PWM_stop(PIN)       R8_PWM_OUT_EN &= ~PWM_CHANNEL(PIN)
#define PWM_write(PIN, val) PWM_DATA(PIN) = (val)
#define PWM_invert(PIN)     R8_PWM_POLAR ^= PWM_CHANNEL(PIN)

#define PWM_INT_enable()    {R8_PWM_INT_CTRL |= RB_PWM_IE_CYC; NVIC_EnableIRQ(PWMX_IRQn);}
#define PWM_INT_disable()   {R8_PWM_INT_CTRL &= ~RB_PWM_IE_CYC; NVIC_DisableIRQ(PWMX_IRQn);}
#define PWM_INT_clear()     R8_PWM_INT_CTRL |= RB_PWM_IF_CYC
#define PWM_ISR             void PWMX_IRQHandler(void) __attribute__((interrupt)); \
                            void PWMX_IRQHandler(void)

#ifdef __cplusplus
};
#endif
