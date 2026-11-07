// ===================================================================================
// Software USB Handler (V-USB) for megaAVR 0-Series                          * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include <avr/pgmspace.h>
#include "usb_handler.h"
#include "usb_descr.h"

// ===================================================================================
// Oscillator Calibration
// ===================================================================================
// usbMeasureFrameLength() returns the time between two keep-alive pulses (1ms) in
// multiples of 7 CPU cycles. The oscillator calibration value is changed step by step
// (~0.75% per step) until the measured value is closest to the target value.
#define USB_CAL_TARGET  (int16_t)(((uint32_t)(F_CPU / 1000) * 1499UL + 5250) / 10500)

static void USB_writeCal(uint8_t cal) {
  _PROTECTED_WRITE(CLKCTRL.OSC20MCALIBA,
                  (CLKCTRL.OSC20MCALIBA & ~CLKCTRL_CAL20M_gm) | cal);
}

void usbHadReset(void) {
  uint8_t cal  = CLKCTRL.OSC20MCALIBA & CLKCTRL_CAL20M_gm;
  uint8_t best = cal;
  int16_t bestdiff = 0x7fff;
  uint8_t i;

  cli();                                        // cycle counting -> no interrupts
  for(i = 0; i < 128; i++) {
    int16_t diff = (int16_t)usbMeasureFrameLength() - USB_CAL_TARGET;
    int16_t absdiff = (diff < 0) ? -diff : diff;
    if(absdiff >= bestdiff) break;              // getting worse -> previous was best
    bestdiff = absdiff;
    best = cal;
    if(diff < 0) {                              // frequency too low
      if(cal >= CLKCTRL_CAL20M_gm) break;
      cal++;
    }
    else if(diff > 0) {                         // frequency too high
      if(!cal) break;
      cal--;
    }
    else break;                                 // perfect match
    USB_writeCal(cal);
  }
  USB_writeCal(best);                           // set best calibration value
  sei();
}

// ===================================================================================
// USB Polling (Timer Interrupt)
// ===================================================================================
#define USB_CAT(a, b)       a ## b
#define USB_XCAT(a, b)      USB_CAT(a, b)
#define USB_CAT3(a, b, c)   a ## b ## c
#define USB_XCAT3(a, b, c)  USB_CAT3(a, b, c)
#if   USB_POLL_TIMER == 1
  #define USB_TCB_NUM       0                   // TCB0
#elif USB_POLL_TIMER == 2
  #define USB_TCB_NUM       1                   // TCB1
#elif USB_POLL_TIMER == 3
  #define USB_TCB_NUM       2                   // TCB2
#elif USB_POLL_TIMER == 4
  #define USB_TCB_NUM       3                   // TCB3
#endif

static inline void USB_poll(void) {
  usbPoll();                                    // process USB requests
  usb_handle_user_in();                         // send reports
}

#if USB_POLL_TIMER == 0
// RTC periodic interrupt (every 32 / 32768Hz = ~1ms)
ISR(RTC_PIT_vect) {
  RTC.PITINTFLAGS = RTC_PI_bm;                  // clear interrupt flag
  USB_poll();
}
#elif USB_POLL_TIMER >= 1 && USB_POLL_TIMER <= 4
// TCBn periodic interrupt (every 1ms)
#define USB_TCB           USB_XCAT(TCB, USB_TCB_NUM)
#define USB_TCB_vect      USB_XCAT3(TCB, USB_TCB_NUM, _INT_vect)
ISR(USB_TCB_vect) {
  USB_TCB.INTFLAGS = TCB_CAPT_bm;               // clear interrupt flag
  USB_poll();
}
#else
  #error Wrong USB_POLL_TIMER!
#endif

// ===================================================================================
// USB Init
// ===================================================================================
#define USB_VPORT         USB_XCAT(VPORT, USB_PORT)
#define USB_PORTx         USB_XCAT(PORT,  USB_PORT)
#define USB_MASK          ((1 << USB_PIN_DP) | (1 << USB_PIN_DM))

void USB_init(void) {
  uint8_t i;

  // Set system clock: internal 16MHz oscillator without prescaler
  _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0);

  // Setup USB pins: input, no pullup, input buffer enabled, no interrupt
  USB_VPORT.OUT &= ~USB_MASK;
  USB_VPORT.DIR &= ~USB_MASK;
  (&USB_PORTx.PIN0CTRL)[USB_PIN_DP] = 0;
  (&USB_PORTx.PIN0CTRL)[USB_PIN_DM] = 0;

  // Init V-USB, enforce re-enumeration
  usbInit();                                    // D+ interrupt on rising edge
  usbDeviceDisconnect();
  for(i = 25; i; i--) DLY_ms(10);               // wait 250ms
  usbDeviceConnect();

  // USB pin interrupt with high priority (level 1)
  CPUINT.LVL1VEC = USB_INTR_VECTOR_NUM;

  // Start USB polling timer
  #if USB_POLL_TIMER == 0
  while(RTC.STATUS);                            // wait for RTC to be ready
  RTC.CLKSEL     = RTC_CLKSEL_INT32K_gc;        // internal 32.768kHz oscillator
  while(RTC.PITSTATUS);                         // wait for PIT to be ready
  RTC.PITINTCTRL = RTC_PI_bm;                   // enable periodic interrupt
  RTC.PITCTRLA   = RTC_PERIOD_CYC32_gc          // period: 32 cycles (~1ms)
                 | RTC_PITEN_bm;                // enable PIT
  #else
  USB_TCB.CCMP    = (F_CPU / 1000) - 1;         // period: 1ms
  USB_TCB.CTRLB   = TCB_CNTMODE_INT_gc;         // periodic interrupt mode
  USB_TCB.INTCTRL = TCB_CAPT_bm;                // enable interrupt
  USB_TCB.CTRLA   = TCB_ENABLE_bm;              // enable timer, CLK_PER
  #endif

  sei();                                        // enable global interrupts
}

// ===================================================================================
// V-USB Callback Functions
// ===================================================================================

// HID class request variables
static uint8_t USB_idleRate;                    // idle rate in 4ms units
static uint8_t USB_protocol = 1;                // 0: boot protocol, 1: report protocol

// Get descriptor from descriptor table (flash)
usbMsgLen_t usbFunctionDescriptor(usbRequest_t *rq) {
  uint8_t i;
  uint32_t iv = ((uint32_t)rq->wIndex.word << 16) | rq->wValue.word;
  for(i = 0; i < DESCRIPTOR_LIST_ENTRIES; i++) {
    if(pgm_read_dword(&descriptor_list[i].lIndexValue) == iv) {
      usbMsgPtr = (usbMsgPtr_t)pgm_read_word(&descriptor_list[i].addr);
      return pgm_read_byte(&descriptor_list[i].length);
    }
  }
  return 0;
}

// Handle class requests (HID)
usbMsgLen_t usbFunctionSetup(uint8_t data[8]) {
  usbRequest_t *rq = (void *)data;
  if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {
    switch(rq->bRequest) {
      case USBRQ_HID_GET_REPORT:
        return usb_handle_get_report(rq->wValue.bytes[0]);
      case USBRQ_HID_SET_REPORT:
        return USB_NO_MSG;                      // data stage -> usbFunctionWrite()
      case USBRQ_HID_GET_IDLE:
        usbMsgPtr = (usbMsgPtr_t)&USB_idleRate;
        return 1;
      case USBRQ_HID_SET_IDLE:
        USB_idleRate = rq->wValue.bytes[1];
        return 0;
      case USBRQ_HID_GET_PROTOCOL:
        usbMsgPtr = (usbMsgPtr_t)&USB_protocol;
        return 1;
      case USBRQ_HID_SET_PROTOCOL:
        USB_protocol = rq->wValue.bytes[0];
        return 0;
    }
  }
  return 0;
}

// Data stage of SET_REPORT (control transfer)
uint8_t usbFunctionWrite(uint8_t *data, uint8_t len) {
  usb_handle_user_data(0, data, len);
  return 1;                                     // all data received
}

// Data received via OUT endpoint
void usbFunctionWriteOut(uint8_t *data, uint8_t len) {
  usb_handle_user_data(usbRxToken, data, len);
}
