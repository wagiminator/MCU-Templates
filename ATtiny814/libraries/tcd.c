// ===================================================================================
// PWM Functions using Timer/Counter D (TCD0) for tinyAVR 1-Series            * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "tcd.h"
#include "gpio.h"

#ifndef TCD0
  #error TCD0 is only available on tinyAVR 1-series devices
#endif

#if TCD_TOP > 4095
  #error TCD_TOP must be <= 4095
#endif

static uint16_t TCD_top = TCD_TOP;

// Channel pins
#ifdef PORTB
static const uint8_t TCD_PIN[] = {PA4, PA5, PC0, PC1};
#else
static const uint8_t TCD_PIN[] = {PA6, PA7, PA6, PA7};
#endif

// Enable timer
void TCD_enable(void) {
  while(!(TCD0.STATUS & TCD_ENRDY_bm));         // wait until ready to enable
  TCD0.CTRLA |= TCD_ENABLE_bm;                  // enable timer
}

// Disable timer
void TCD_disable(void) {
  TCD0.CTRLA &= ~TCD_ENABLE_bm;                 // disable timer
  while(!(TCD0.STATUS & TCD_ENRDY_bm));         // wait until disabled
}

// Init and start TCD0
void TCD_init(void) {
  TCD_disable();                                // timer must be disabled
  TCD0.CTRLB   = TCD_WGMODE_ONERAMP_gc;         // one-ramp mode
  TCD0.CTRLC   = TCD_CMPDSEL_bm;                // WOC = PWMA, WOD = PWMB
  TCD0.CMPASET = 0xfff;                         // channel A: duty cycle 0
  TCD0.CMPACLR = 0;
  TCD0.CMPBSET = TCD_TOP + 1;                   // channel B: duty cycle 0
  TCD0.CMPBCLR = TCD_TOP;                       // set TOP value
  TCD0.CTRLA   = ((TCD_CLK      & 3) << 5)      // clock select
               | ((TCD_CNTPRES  & 3) << 3)      // counter prescaler
               | ((TCD_SYNCPRES & 3) << 1);     // synchronizer prescaler
  TCD_enable();                                 // enable timer
}

// Synchronize double-buffered compare registers at end of cycle
static void TCD_sync(void) {
  if(TCD0.CTRLA & TCD_ENABLE_bm) {
    while(!(TCD0.STATUS & TCD_CMDRDY_bm));      // wait until ready for command
    TCD0.CTRLE = TCD_SYNCEOC_bm;                // update at end of cycle
  }
}

// Set duty cycle of channel (0 .. TOP+1)
void TCD_write(uint8_t ch, uint16_t v) {
  if(TCD0.CTRLA & TCD_ENABLE_bm)
    while(!(TCD0.STATUS & TCD_CMDRDY_bm));      // wait until last update is done
  if(v > TCD_top + 1) v = TCD_top + 1;
  if(!(ch & 1)) {                               // channel A (and C)
    if(v) {TCD0.CMPASET = 0;     TCD0.CMPACLR = v;}
    else  {TCD0.CMPASET = 0xfff; TCD0.CMPACLR = 0;}
  }
  else TCD0.CMPBSET = TCD_top + 1 - v;          // channel B (and D)
  TCD_sync();
}

// Set TOP value (duty cycles must be updated afterwards)
void TCD_setTOP(uint16_t v) {
  if(TCD0.CTRLA & TCD_ENABLE_bm)
    while(!(TCD0.STATUS & TCD_CMDRDY_bm));
  TCD_top = v & 0xfff;
  TCD0.CMPBCLR = TCD_top;
  TCD_sync();
}

// Enable/disable output on channel (FAULTCTRL is protected and only writable when disabled)
static void TCD_setOutput(uint8_t ch, uint8_t on) {
  if(ch > 3) return;
  uint8_t enabled = TCD0.CTRLA & TCD_ENABLE_bm;
  if(enabled) TCD_disable();
  uint8_t mask = 0x10 << ch;                    // CMPxEN bit
  _PROTECTED_WRITE(TCD0.FAULTCTRL, on ? (TCD0.FAULTCTRL | mask) : (TCD0.FAULTCTRL & ~mask));
  if(on) PIN_output(TCD_PIN[ch]);
  else   PIN_low(TCD_PIN[ch]);
  if(enabled) TCD_enable();
}

void TCD_start(uint8_t ch) {TCD_setOutput(ch, 1);}
void TCD_stop(uint8_t ch)  {TCD_setOutput(ch, 0);}
