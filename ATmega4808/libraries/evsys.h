// ===================================================================================
// Event System (EVSYS) Functions for megaAVR 0-Series                        * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// EVSYS_CH_set(ch,gen)     set generator gen for event channel ch (0..7, x08: 0..5)
// EVSYS_pin(PIN)           route PIN to its event channel (see below) and return the
//                          user value for this channel
// EVSYS_USER_set(u,v)      connect user u to channel value v
// EVSYS_strobe(ch)         generate software event on channel ch
// EVOUT_enable(n)          enable event output pin Px2 of port n (0: A, 1: B, .. 5: F)
// EVOUT_ALT_enable(n)      enable alternative event output pin Px7 (0: A, 2: C, 3: D)
// EVOUT_disable(n)         set event output pin of port n back to input
//
// User channel values (v):
// ------------------------
// EVSYS_CH_OFF, EVSYS_CH0, EVSYS_CH1, EVSYS_CH2, EVSYS_CH3, EVSYS_CH4, EVSYS_CH5,
// EVSYS_CH6, EVSYS_CH7
//
// Users (u):
// ----------
// EVSYS_USER_LUT0A, EVSYS_USER_LUT0B, EVSYS_USER_LUT1A, EVSYS_USER_LUT1B,
// EVSYS_USER_LUT2A, EVSYS_USER_LUT2B, EVSYS_USER_LUT3A, EVSYS_USER_LUT3B,
// EVSYS_USER_ADC0,  EVSYS_USER_EVOUTA .. EVSYS_USER_EVOUTF,
// EVSYS_USER_USART0 .. EVSYS_USER_USART3, EVSYS_USER_TCA0,
// EVSYS_USER_TCB0 .. EVSYS_USER_TCB3
//
// Pin event channels (EVSYS_pin):
// -------------------------------
// PORTA, PORTB: channel 0 (or 1)
// PORTC, PORTD: channel 2 (or 3)
// PORTE, PORTF: channel 4 (or 5)
// EVSYS_pin() always uses the first channel, use EVSYS_CH_set(ch, EVSYS_GEN_pin(PIN))
// to route a pin to the second channel of the pair.
//
// Example: route pin PD2 to TCB0 capture input:
//   EVSYS_USER_set(EVSYS_USER_TCB0, EVSYS_pin(PD2));
// Example: output RTC overflow event on pin PA2:
//   EVSYS_CH_set(0, EVSYS_GENERATOR_RTC_OVF_gc);
//   EVSYS_USER_set(EVSYS_USER_EVOUTA, EVSYS_CH0);
//   EVOUT_enable(0);
//
// Notes:
// ------
// - Generators are defined in the device header (e.g. EVSYS_GENERATOR_AC0_OUT_gc).
// - Channels 6, 7, the users TCB3, USART3 and the ports B and E are only available on
//   40/48-pin devices. Refer to the datasheet.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// User channel values
#define EVSYS_CH_OFF          0
#define EVSYS_CH0             1
#define EVSYS_CH1             2
#define EVSYS_CH2             3
#define EVSYS_CH3             4
#define EVSYS_CH4             5
#define EVSYS_CH5             6
#define EVSYS_CH6             7
#define EVSYS_CH7             8

// Users
#define EVSYS_USER_LUT0A      0
#define EVSYS_USER_LUT0B      1
#define EVSYS_USER_LUT1A      2
#define EVSYS_USER_LUT1B      3
#define EVSYS_USER_LUT2A      4
#define EVSYS_USER_LUT2B      5
#define EVSYS_USER_LUT3A      6
#define EVSYS_USER_LUT3B      7
#define EVSYS_USER_ADC0       8
#define EVSYS_USER_EVOUTA     9
#define EVSYS_USER_EVOUTB     10
#define EVSYS_USER_EVOUTC     11
#define EVSYS_USER_EVOUTD     12
#define EVSYS_USER_EVOUTE     13
#define EVSYS_USER_EVOUTF     14
#define EVSYS_USER_USART0     15
#define EVSYS_USER_USART1     16
#define EVSYS_USER_USART2     17
#define EVSYS_USER_USART3     18
#define EVSYS_USER_TCA0       19
#define EVSYS_USER_TCB0       20
#define EVSYS_USER_TCB1       21
#define EVSYS_USER_TCB2       22
#define EVSYS_USER_TCB3       23

// Functions
#define EVSYS_CH_set(ch,gen)  (&EVSYS.CHANNEL0)[(ch)] = (gen)
#define EVSYS_USER_set(u,v)   (&EVSYS.USERCCLLUT0A)[(u)] = (v)
#define EVSYS_strobe(ch)      EVSYS.STROBE = (1 << (ch))
#define EVSYS_GEN_pin(PIN)    (0x40 + ((PIN) & 15))   // generator value of pin
#define EVSYS_CH_pin(PIN)     (((PIN) >> 3) & 6)      // first event channel of pin

// Route pin to its event channel and return user channel value
static inline uint8_t EVSYS_pin(uint8_t pin) {
  (&EVSYS.CHANNEL0)[EVSYS_CH_pin(pin)] = EVSYS_GEN_pin(pin); // pin as event generator
  return EVSYS_CH_pin(pin) + 1;                 // user value for this channel
}

// Event outputs
#define EVOUT_enable(n)     do { PORTMUX.EVSYSROUTEA &= ~(1 << (n));             \
                                 (&VPORTA.DIR)[(n) << 2] |= (1 << 2); } while(0)
#define EVOUT_ALT_enable(n) do { PORTMUX.EVSYSROUTEA |=  (1 << (n));             \
                                 (&VPORTA.DIR)[(n) << 2] |= (1 << 7); } while(0)
#define EVOUT_disable(n)    (&VPORTA.DIR)[(n) << 2] &=                           \
                            ~((PORTMUX.EVSYSROUTEA & (1 << (n))) ? (1 << 7) : (1 << 2))

#ifdef __cplusplus
}
#endif
