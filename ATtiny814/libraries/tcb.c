// ===================================================================================
// Timer/Counter B (TCB) Functions for tinyAVR 0/1-Series                     * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "tcb.h"
#include "gpio.h"

#if TCB_NUM == 1 && !defined(TCB1)
  #error TCB1 is not available on this device
#endif

#define TCB_CTRLA     (((TCB_CLK) & 3) << 1) | TCB_ENABLE_bm

// Set up WO pin
static void TCB_setPin(void) {
  #if   TCB_NUM == 0 && TCB_ALT == 0
    #ifdef PORTB
    PIN_output(PA5);
    #else
    PIN_output(PA6);                            // 8-pin devices
    #endif
  #elif TCB_NUM == 0 && TCB_ALT == 1
    PORTMUX.CTRLD |= PORTMUX_TCB0_bm;
    PIN_output(PC0);
  #elif TCB_NUM == 1 && TCB_ALT == 0
    PIN_output(PA3);
  #elif TCB_NUM == 1 && TCB_ALT == 1
    PORTMUX.CTRLD |= PORTMUX_TCB1_bm;
    PIN_output(PC4);
  #endif
}

// Route input pin to TCB capture event input via asynchronous event channel
static void TCB_setEvent(void) {
  PIN_input(TCB_PIN_IN);                        // set pin as input
  uint8_t port = TCB_PIN_IN >> 3;               // 0: PORTA, 1: PORTB, 2: PORTC
  (&EVSYS.ASYNCCH0)[port] = 0x0A + (TCB_PIN_IN & 7); // pin as event generator
  #if TCB_NUM == 0
  EVSYS.ASYNCUSER0  = 3 + port;                 // TCB0 uses ASYNCCHn
  #else
  EVSYS.ASYNCUSER11 = 3 + port;                 // TCB1 uses ASYNCCHn
  #endif
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
