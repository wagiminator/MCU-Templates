// ===================================================================================
// Event System (EVSYS) Functions for tinyAVR 0/1-Series                      * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// EVSYS_ASYNC_set(ch,gen)  set generator gen for asynchronous channel ch (0..3)
// EVSYS_SYNC_set(ch,gen)   set generator gen for synchronous channel ch (0..1)
// EVSYS_ASYNC_pin(PIN)     route PIN to its asynchronous channel (PAx: 0, PBx: 1, PCx: 2)
//                          and return the user value for this channel
// EVSYS_USER_set(u,v)      connect asynchronous user u to channel value v
// EVSYS_SYNCUSER_set(u,v)  connect synchronous user u (0: TCA0, 1: USART0) to value v
// EVSYS_ASYNC_strobe(ch)   generate software event on asynchronous channel ch
// EVSYS_SYNC_strobe(ch)    generate software event on synchronous channel ch
// EVOUT_enable(n)          enable event output n on its pin (0: PA2, 1: PB2, 2: PC2)
// EVOUT_disable(n)         disable event output n
//
// User channel values (v):
// ------------------------
// EVSYS_CH_OFF, EVSYS_CH_SYNC0, EVSYS_CH_SYNC1,
// EVSYS_CH_ASYNC0, EVSYS_CH_ASYNC1, EVSYS_CH_ASYNC2, EVSYS_CH_ASYNC3
//
// Asynchronous users (u):
// -----------------------
// EVSYS_USER_TCB0, EVSYS_USER_ADC0, EVSYS_USER_LUT0EV0, EVSYS_USER_LUT1EV0,
// EVSYS_USER_LUT0EV1, EVSYS_USER_LUT1EV1, EVSYS_USER_TCD0EV0, EVSYS_USER_TCD0EV1,
// EVSYS_USER_EVOUT0, EVSYS_USER_EVOUT1, EVSYS_USER_EVOUT2, EVSYS_USER_TCB1,
// EVSYS_USER_ADC1
//
// Example: route pin PA2 to TCB0 capture input:
//   EVSYS_USER_set(EVSYS_USER_TCB0, EVSYS_ASYNC_pin(PA2));
//
// Notes:
// ------
// - Generators are defined in the device header (e.g. EVSYS_ASYNCCH0_AC0_OUT_gc).
// - Not all channels and users exist on every device, refer to the datasheet.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// User channel values
#define EVSYS_CH_OFF          0
#define EVSYS_CH_SYNC0        1
#define EVSYS_CH_SYNC1        2
#define EVSYS_CH_ASYNC0       3
#define EVSYS_CH_ASYNC1       4
#define EVSYS_CH_ASYNC2       5
#define EVSYS_CH_ASYNC3       6

// Asynchronous users
#define EVSYS_USER_TCB0       0
#define EVSYS_USER_ADC0       1
#define EVSYS_USER_LUT0EV0    2
#define EVSYS_USER_LUT1EV0    3
#define EVSYS_USER_LUT0EV1    4
#define EVSYS_USER_LUT1EV1    5
#define EVSYS_USER_TCD0EV0    6
#define EVSYS_USER_TCD0EV1    7
#define EVSYS_USER_EVOUT0     8
#define EVSYS_USER_EVOUT1     9
#define EVSYS_USER_EVOUT2     10
#define EVSYS_USER_TCB1       11
#define EVSYS_USER_ADC1       12

// Functions
#define EVSYS_ASYNC_set(ch,gen)   (&EVSYS.ASYNCCH0)[(ch)] = (gen)
#define EVSYS_SYNC_set(ch,gen)    (&EVSYS.SYNCCH0)[(ch)]  = (gen)
#define EVSYS_USER_set(u,v)       (&EVSYS.ASYNCUSER0)[(u)] = (v)
#define EVSYS_SYNCUSER_set(u,v)   (&EVSYS.SYNCUSER0)[(u)]  = (v)
#define EVSYS_ASYNC_strobe(ch)    EVSYS.ASYNCSTROBE = (1 << (ch))
#define EVSYS_SYNC_strobe(ch)     EVSYS.SYNCSTROBE  = (1 << (ch))

// Route pin to its asynchronous channel and return user channel value
static inline uint8_t EVSYS_ASYNC_pin(uint8_t pin) {
  uint8_t port = pin >> 3;                      // 0: PORTA, 1: PORTB, 2: PORTC
  (&EVSYS.ASYNCCH0)[port] = 0x0A + (pin & 7);   // pin as event generator
  return EVSYS_CH_ASYNC0 + port;                // user value for this channel
}

// Event outputs
#define EVOUT_enable(n)   do { PORTMUX.CTRLA |= (1 << (n));                    \
                               (&VPORTA.DIR)[(n) << 2] |= (1 << 2); } while(0)
#define EVOUT_disable(n)  PORTMUX.CTRLA &= ~(1 << (n))

#ifdef __cplusplus
};
#endif
