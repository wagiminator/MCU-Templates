// ===================================================================================
// PWM Functions using Timer/Counter A (TCA0) for tinyAVR 0/1-Series          * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "pwm.h"
#include "gpio.h"

#if PWM_MODE == 0 && PWM_TOP > 65535
  #error PWM_TOP must be <= 65535 in single mode
#elif PWM_MODE == 1 && PWM_TOP > 255
  #error PWM_TOP must be <= 255 in split mode
#endif

// Channel pins
#ifdef PORTB
static const uint8_t PWM_PIN[] = {
  (PWM_ALT & 0x01) ? PB3 : PB0,
  (PWM_ALT & 0x02) ? PB4 : PB1,
  (PWM_ALT & 0x04) ? PB5 : PB2,
  (PWM_ALT & 0x08) ? PC3 : PA3,
  (PWM_ALT & 0x10) ? PC4 : PA4,
  (PWM_ALT & 0x20) ? PC5 : PA5
};
#else
static const uint8_t PWM_PIN[] = {
  (PWM_ALT & 0x01) ? PA7 : PA3, PA1, PA2, PA3, PA3, PA3
};
#endif

// Init and start PWM timer
void PWM_init(void) {
  TCA0.SINGLE.CTRLA  = 0;                       // disable timer
  TCA0.SINGLE.CTRLESET = TCA_SINGLE_CMD_RESET_gc; // hard reset (needed for mode change)
  PORTMUX.CTRLC = PWM_ALT;                      // set alternative pins
  #if PWM_MODE == 0
  TCA0.SINGLE.CTRLB  = TCA_SINGLE_WGMODE_SINGLESLOPE_gc; // single-slope PWM
  TCA0.SINGLE.PER    = PWM_TOP;                 // set TOP value
  #else
  TCA0.SPLIT.CTRLD   = TCA_SPLIT_SPLITM_bm;     // split mode
  TCA0.SPLIT.LPER    = PWM_TOP;                 // set TOP values
  TCA0.SPLIT.HPER    = PWM_TOP;
  #endif
  TCA0.SINGLE.CTRLA  = (PWM_PRESC << 1)         // set prescaler
                     | TCA_SINGLE_ENABLE_bm;    // enable timer
}

// Enable PWM output on channel
void PWM_start(uint8_t ch) {
  #if PWM_MODE == 0
  if(ch > 2) return;
  TCA0.SINGLE.CTRLB |= (TCA_SINGLE_CMP0EN_bm << ch);
  #else
  if(ch > 5) return;
  TCA0.SPLIT.CTRLB  |= (ch < 3) ? (TCA_SPLIT_LCMP0EN_bm << ch) : (TCA_SPLIT_HCMP0EN_bm << (ch - 3));
  #endif
  PIN_output(PWM_PIN[ch]);
}

// Disable PWM output on channel
void PWM_stop(uint8_t ch) {
  #if PWM_MODE == 0
  if(ch > 2) return;
  TCA0.SINGLE.CTRLB &= ~(TCA_SINGLE_CMP0EN_bm << ch);
  #else
  if(ch > 5) return;
  TCA0.SPLIT.CTRLB  &= ~((ch < 3) ? (TCA_SPLIT_LCMP0EN_bm << ch) : (TCA_SPLIT_HCMP0EN_bm << (ch - 3)));
  #endif
  PIN_low(PWM_PIN[ch]);
}

// Set duty cycle of channel
void PWM_write(uint8_t ch, uint16_t v) {
  #if PWM_MODE == 0
  if(ch > 2) return;
  (&TCA0.SINGLE.CMP0BUF)[ch] = v;               // buffered -> update at end of period
  #else
  if(ch > 5) return;
  if(ch < 3) (&TCA0.SPLIT.LCMP0)[ch << 1] = v;
  else       (&TCA0.SPLIT.HCMP0)[(ch - 3) << 1] = v;
  #endif
}
