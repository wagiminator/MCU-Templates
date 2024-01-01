// ===================================================================================
// Millis Functions for tinyAVR 0-Series and 1-Series                         * v1.0 *
// ===================================================================================
// 2021 by Stefan Wagner:   https://github.com/wagiminator

#include "millis.h"

volatile uint32_t MIL_counter = 0;      // millis counter

#if MIL_TIMER == 0

// Init millis counter (TCB)
void MIL_init(void) {
  TCB0.CCMP    = (F_CPU / 1000) - 1;    // set TOP value (period)
  TCB0.CTRLA   = TCB_ENABLE_bm;         // enable timer/counter
  TCB0.INTCTRL = TCB_CAPT_bm;           // enable periodic interrupt
  INT_enable();                         // global interrupt enable
}

// Timer interrupt service routine (every millisecond)
ISR(TCB0_INT_vect) {
  TCB0.INTFLAGS = TCB_CAPT_bm;          // clear interrupt flag
  MIL_counter++;                        // increase millis counter
}

#else

volatile uint8_t MIL_OVF = 44;          // correction overflow counter

// Init millis counter (PIT)
void MIL_init(void) {
  PIT_enable(32);                       // enable PIT, period = 32 / 32768 = 0.125 s
  PIT_INT_enable();                     // enable PIT interrupt
  INT_enable();                         // enable interrupts
}

// PIT interrupt servise routine (every 1000/1024 milliseconds)
ISR(RTC_PIT_vect) {
  PIT_FLAG_clear();                     // clear PIT interrupt flag
  if(--MIL_OVF) MIL_counter++;
  else MIL_OVF = 44;
}

#endif    // MIL_TIMER

// Read millis counter value
uint32_t MIL_read(void) {
  uint32_t result;
  INT_ATOMIC_BLOCK {                              // atomic read
    result = MIL_counter;                         // get millis counter value
  }
  return result;                                  // return value
}
