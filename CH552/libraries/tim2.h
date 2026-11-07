// ===================================================================================
// Timer2 Functions (Capture, Counter, Clock Output) for CH551, CH552, CH554   * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// TIM2_CAP_init(mode)      init capture mode on CAP2 (T2EX pin) and CAP1 (T2 pin)
// TIM2_CAP1_available()    check if a new CAP1 value is available
// TIM2_CAP1_read()         read CAP1 value (16-bit timer count) and clear flag
// TIM2_CAP2_available()    check if a new CAP2 value is available
// TIM2_CAP2_read()         read CAP2 value (16-bit timer count) and clear flag
//
// TIM2_COUNT_init()        init counter mode (counts falling edges on T2 pin)
// TIM2_CLKOUT_init(f)      output clock with frequency f (Hz) on T2 pin
// TIM2_read()              read 16-bit timer/counter value
// TIM2_write(n)            write 16-bit timer/counter value
// TIM2_start()             start timer
// TIM2_stop()              stop timer
//
// Capture modes:
// --------------
// TIM2_CAP_FALLING         capture on falling edges
// TIM2_CAP_ANY             capture on any edge
// TIM2_CAP_RISING          capture on rising edges
//
// Pins (TIM2_ALTER 0/1):
// ----------------------
// T2/CAP1:   P1.0 / P1.4
// T2EX/CAP2: P1.1 / RST
//
// Notes:
// ------
// - The difference between two consecutive capture values is the period (or pulse
//   width in TIM2_CAP_ANY mode) in timer clock cycles (overflow is handled by
//   16-bit unsigned subtraction): period = (uint16_t)(new - old).
// - Timer clock (TIM2_CLK): 0: Fsys/12, 1: Fsys/4, 2: Fsys. TIM2_CLK = 1 clears
//   bTMR_CLK, which also affects TIMER0/1 in fast clock mode (e.g. UART0 BAUD rate).
// - TIMER2 can not be used together with the millis library.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "ch554.h"

// TIM2 parameters
#define TIM2_CLK            2               // timer clock (0: Fsys/12, 1: Fsys/4, 2: Fsys)
#define TIM2_ALTER          0               // alternate pins (0: P1.0/P1.1, 1: P1.4/RST)
#define TIM2_CAP1_EN        1               // also capture on T2 pin (CAP1)

// Timer clock frequency
#if   TIM2_CLK == 0
  #define TIM2_FREQ         (F_CPU / 12)
#elif TIM2_CLK == 1
  #define TIM2_FREQ         (F_CPU / 4)
#else
  #define TIM2_FREQ         (F_CPU)
#endif

// Capture modes (bT2_CAP_M1, bT2_CAP_M0)
#define TIM2_CAP_FALLING    0x00
#define TIM2_CAP_ANY        bT2_CAP_M0
#define TIM2_CAP_RISING     (bT2_CAP_M1 | bT2_CAP_M0)

// Set timer clock and pins (internal)
inline void TIM2_setup(void) {
  T2CON = 0;                                // stop timer, reset mode
  #if   TIM2_CLK == 0
  T2MOD &= ~bT2_CLK;
  #elif TIM2_CLK == 1
  T2MOD  = (T2MOD & ~bTMR_CLK) | bT2_CLK;
  #else
  T2MOD |= bTMR_CLK | bT2_CLK;
  #endif
  #if TIM2_ALTER == 1
  PIN_FUNC |= bT2_PIN_X | bT2EX_PIN_X;
  #else
  PIN_FUNC &= ~(bT2_PIN_X | bT2EX_PIN_X);
  #endif
  TL2 = 0; TH2 = 0;
}

// Init capture mode
inline void TIM2_CAP_init(uint8_t mode) {
  TIM2_setup();
  T2MOD = (T2MOD & ~(bT2_CAP_M1 | bT2_CAP_M0 | T2OE | bT2_CAP1_EN)) | mode
        #if TIM2_CAP1_EN > 0
        | bT2_CAP1_EN
        #endif
        ;
  CP_RL2 = 1;                               // capture mode
  EXEN2  = 1;                               // enable capture by T2EX pin
  TR2    = 1;                               // start timer
}

#define TIM2_CAP1_available()   (CAP1F)
#define TIM2_CAP2_available()   (EXF2)

inline uint16_t TIM2_CAP1_read(void) {
  uint16_t value = T2CAP1L | ((uint16_t)T2CAP1H << 8);
  CAP1F = 0;
  return value;
}

inline uint16_t TIM2_CAP2_read(void) {
  uint16_t value = RCAP2L | ((uint16_t)RCAP2H << 8);
  EXF2 = 0;
  return value;
}

// Init counter mode (falling edges on T2 pin)
inline void TIM2_COUNT_init(void) {
  TIM2_setup();
  C_T2 = 1;                                 // count falling edges on T2 pin
  TR2  = 1;                                 // start counter
}

// Output clock on T2 pin (f = TIM2_FREQ / 2 / (65536 - reload))
#define TIM2_CLKOUT_init(f) do {                                            \
  uint16_t _r = (uint16_t)(65536 - (TIM2_FREQ / 2 / (f)));                  \
  TIM2_setup();                                                             \
  RCAP2L = (uint8_t)_r; RCAP2H = (uint8_t)(_r >> 8);                        \
  TL2 = RCAP2L; TH2 = RCAP2H;                                               \
  T2MOD |= T2OE;                            /* enable clock output */       \
  TR2 = 1; } while(0)

// Read/write timer value (read high byte twice to avoid inconsistent values)
inline uint16_t TIM2_read(void) {
  uint8_t h, l;
  do {
    h = TH2;
    l = TL2;
  } while(h != TH2);
  return l | ((uint16_t)h << 8);
}

#define TIM2_write(n)       do {TL2 = (uint8_t)(n); TH2 = (uint8_t)((n) >> 8);} while(0)
#define TIM2_start()        TR2 = 1
#define TIM2_stop()         TR2 = 0
