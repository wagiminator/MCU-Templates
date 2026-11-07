// ===================================================================================
// Timer/Counter B (TCB) Functions for tinyAVR 0/1-Series                     * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// TCB_timer_init(n)        init periodic interrupt timer with period n clock ticks
// TCB_PWM_init()           init 8-bit PWM on WO pin with TOP = TCB_PWM_TOP
// TCB_PWM_write(v)         set PWM duty cycle (0 .. TOP)
// TCB_PWM_setTOP(v)        set PWM TOP value (frequency = F_TCB / (TOP + 1))
// TCB_FRQ_init()           init frequency (period) measurement on input pin
// TCB_PW_init()            init pulse-width measurement on input pin
// TCB_FRQPW_init()         init frequency and pulse-width measurement on input pin
// TCB_CAPT_available()     check if a new capture value is available
// TCB_CAPT_read()          read capture value in clock ticks (clears flag)
// TCB_CNT_read()           read counter value in clock ticks
// TCB_CNT_write(n)         write counter value
//
// TCB_enable()             enable timer
// TCB_disable()            disable timer
// TCB_STDBY_on()           keep timer running in STANDBY sleep mode
// TCB_STDBY_off()          disable timer in STANDBY sleep mode
// TCB_INT_enable()         enable capture/compare interrupt (TCBn_INT_vect)
// TCB_INT_disable()        disable capture/compare interrupt
// TCB_FLAG_read()          read capture/compare interrupt flag
// TCB_FLAG_clear()         clear capture/compare interrupt flag
//
// TCB pin mapping (set below in TCB parameters):
// ----------------------------------------------
// TCB_NUM       0     0     1     1
// TCB_ALT       0     1     0     1
// WO-pin       PA5   PC0   PA3   PC4    (8-pin devices: TCB0 WO = PA6)
//
// Notes:
// ------
// - TCB1 is only available on ATtiny1614/1616/3216.
// - In FRQ mode the capture value is the period of the input signal in clock ticks,
//   in PW mode the duration of the pulse (high pulse if TCB_EDGE = 0), in FRQPW mode
//   TCB_CAPT_read() returns the pulse width, TCB_CNT_read() the period. The capture
//   value is only valid after two edges.
// - The input pin is routed via the event system: PORTA pins use asynchronous event
//   channel 0, PORTB pins channel 1, PORTC pins channel 2.
// - The millis library also uses TCB0 (MIL_TIMER = 0).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// ===================================================================================
// TCB Parameters
// ===================================================================================
#define TCB_NUM           0             // TCB instance (0 or 1)
#define TCB_ALT           0             // 0: default WO pin, 1: alternative WO pin
#define TCB_CLK           0             // clock (0: CLK_PER, 1: CLK_PER/2, 2: TCA0 clock)
#define TCB_PWM_TOP       255           // PWM TOP value (1 .. 255)
#define TCB_PIN_IN        PA2           // input pin for capture modes
#define TCB_EDGE          0             // 0: rising edge, 1: falling edge (start of measurement)
#define TCB_FILTER        0             // 1: enable input capture noise filter

// ===================================================================================
// TCB Functions and Macros
// ===================================================================================
#if   TCB_NUM == 0
  #define TCBx            TCB0
#elif TCB_NUM == 1
  #define TCBx            TCB1
#else
  #error Wrong TCB_NUM
#endif

void TCB_timer_init(uint16_t n);        // init periodic interrupt timer
void TCB_PWM_init(void);                // init 8-bit PWM
void TCB_FRQ_init(void);                // init frequency measurement
void TCB_PW_init(void);                 // init pulse-width measurement
void TCB_FRQPW_init(void);              // init frequency and pulse-width measurement
uint16_t TCB_CAPT_read(void);           // wait for and read capture value

#define TCB_PWM_write(v)      TCBx.CCMPH = (v)
#define TCB_PWM_setTOP(v)     TCBx.CCMPL = (v)
#define TCB_CAPT_available()  (TCBx.INTFLAGS & TCB_CAPT_bm)
#define TCB_CNT_read()        (TCBx.CNT)
#define TCB_CNT_write(n)      TCBx.CNT = (n)

#define TCB_enable()          TCBx.CTRLA |=  TCB_ENABLE_bm
#define TCB_disable()         TCBx.CTRLA &= ~TCB_ENABLE_bm
#define TCB_STDBY_on()        TCBx.CTRLA |=  TCB_RUNSTDBY_bm
#define TCB_STDBY_off()       TCBx.CTRLA &= ~TCB_RUNSTDBY_bm
#define TCB_INT_enable()      TCBx.INTCTRL  =  TCB_CAPT_bm
#define TCB_INT_disable()     TCBx.INTCTRL  =  0
#define TCB_FLAG_read()       (TCBx.INTFLAGS & TCB_CAPT_bm)
#define TCB_FLAG_clear()      TCBx.INTFLAGS  = TCB_CAPT_bm

#ifdef __cplusplus
};
#endif
