// ===================================================================================
// Timer/Counter B (TCB) Functions for megaAVR 0-Series                       * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "tcb.h"
#include "gpio.h"

#if TCB_NUM == 3 && !defined(TCB3)
  #error TCB3 is only available on 40/48-pin devices
#endif

#define TCB_CTRLA     (((TCB_CLK) & 3) << 1) | TCB_ENABLE_bm

// WO pins
#if   TCB_NUM == 0
  #define TCB_PIN_WO      (TCB_ALT ? PF4 : PA2)
#elif TCB_NUM == 1
  #define TCB_PIN_WO      (TCB_ALT ? PF5 : PA3)
#elif TCB_NUM == 2
  #define TCB_PIN_WO      (TCB_ALT ? PB4 : PC0)
#else
  #define TCB_PIN_WO      (TCB_ALT ? PC1 : PB5)
#endif

// Set up WO pin
static void TCB_setPin(void) {
  #if TCB_ALT > 0
  PORTMUX.TCBROUTEA |=  (1 << TCB_NUM);         // alternative pin
  #else
  PORTMUX.TCBROUTEA &= ~(1 << TCB_NUM);         // default pin
  #endif
  PIN_output(TCB_PIN_WO);
}

// Route input pin to TCB capture event input via event channel
static void TCB_setEvent(void) {
  PIN_input(TCB_PIN_IN);                        // set pin as input
  uint8_t ch = (TCB_PIN_IN >> 3) & 6;           // event channel for this port
  (&EVSYS.CHANNEL0)[ch] = 0x40 + (TCB_PIN_IN & 15); // pin as event generator
  (&EVSYS.USERTCB0)[TCB_NUM] = ch + 1;          // TCB uses this channel
  TCBx.EVCTRL = TCB_CAPTEI_bm                   // enable capture event input
              | (TCB_EDGE   ? TCB_EDGE_bm   : 0)
              | (TCB_FILTER ? TCB_FILTER_bm : 0);
}

// Init periodic interrupt timer with period n clock ticks
void TCB_timer_init(uint16_t n) {
  TCBx.CTRLA   = 0;                             // disable timer
  TCBx.CTRLB   = TCB_CNTMODE_INT_gc;            // periodic interrupt mode
  TCBx.CCMP    = n - 1;                         // set TOP value
  TCBx.CNT     = 0;                             // reset counter
  TCBx.INTFLAGS = TCB_CAPT_bm;                  // clear flag
  TCBx.CTRLA   = TCB_CTRLA;                     // set clock and enable timer
}

// Init 8-bit PWM
void TCB_PWM_init(void) {
  TCBx.CTRLA   = 0;                             // disable timer
  TCBx.CCMPL   = TCB_PWM_TOP;                   // set TOP value
  TCBx.CCMPH   = 0;                             // duty cycle 0
  TCBx.CTRLB   = TCB_CCMPEN_bm                  // enable output
               | TCB_CNTMODE_PWM8_gc;           // 8-bit PWM mode
  TCB_setPin();                                 // set up output pin
  TCBx.CTRLA   = TCB_CTRLA;                     // set clock and enable timer
}

// Init capture mode
static void TCB_CAPT_init(uint8_t mode) {
  TCBx.CTRLA   = 0;                             // disable timer
  TCBx.CTRLB   = mode;                          // set capture mode
  TCB_setEvent();                               // route input pin via event system
  TCBx.INTFLAGS = TCB_CAPT_bm;                  // clear flag
  TCBx.CTRLA   = TCB_CTRLA;                     // set clock and enable timer
}

void TCB_FRQ_init(void)   {TCB_CAPT_init(TCB_CNTMODE_FRQ_gc);}
void TCB_PW_init(void)    {TCB_CAPT_init(TCB_CNTMODE_PW_gc);}
void TCB_FRQPW_init(void) {TCB_CAPT_init(TCB_CNTMODE_FRQPW_gc);}

// Wait for and read capture value (reading CCMP clears the flag)
uint16_t TCB_CAPT_read(void) {
  while(!TCB_CAPT_available());
  return TCBx.CCMP;
}
