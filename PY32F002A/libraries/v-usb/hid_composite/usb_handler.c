// ===================================================================================
// Software USB Handler for PY32F0xx                                          * v1.0 *
// ===================================================================================
//
// This file contains a copy of rv003usb.c (https://github.com/cnlohr/rv003usb),
// copyright (c) 2023 CNLohr (MIT License), with some minor changes by Stefan Wagner
// (PY32 port: usb_setup() and usb_handle_se0() rewritten).

#include <stdint.h>

#define INSTANCE_DESCRIPTORS 1

#include "usb_handler.h"

#define ENDPOINT0_SIZE 8 //Fixed for USB 1.1, Low Speed.

struct rv003usb_internal rv003usb_internal_data;

#define LOCAL_CONCAT(A, B) A##B
#define LOCAL_EXP(A, B) LOCAL_CONCAT(A,B)

// Init USB pins, EXTI interrupt and calibration timer
void usb_setup(void) {
  uint8_t i;
  rv003usb_internal_data.se0_windup = 0;

  // Setup USB pins: input, no pull-up/down, low speed
  LOCAL_EXP(GPIO, USB_PORT)->MODER &= ~((3 << (USB_PIN_DP * 2)) | (3 << (USB_PIN_DM * 2)));
  LOCAL_EXP(GPIO, USB_PORT)->PUPDR &= ~((3 << (USB_PIN_DP * 2)) | (3 << (USB_PIN_DM * 2)));
  LOCAL_EXP(GPIO, USB_PORT)->OSPEEDR &= ~((3 << (USB_PIN_DP * 2)) | (3 << (USB_PIN_DM * 2)));
  LOCAL_EXP(GPIO, USB_PORT)->BSRR = USB_BSRR_J;

  #ifdef USB_PIN_DPU
  // Pull-up pin low (disconnected)
  LOCAL_EXP(GPIO, USB_PORT)->BRR = 1 << USB_PIN_DPU;
  LOCAL_EXP(GPIO, USB_PORT)->MODER = (LOCAL_EXP(GPIO, USB_PORT)->MODER
                                   & ~(3 << (USB_PIN_DPU * 2))) | (1 << (USB_PIN_DPU * 2));
  #endif

  #if USB_CALIBRATE > 0
  // TIM16 as free running counter (12MHz) for HSI calibration
  RCC->APBENR2 |= RCC_APBENR2_TIM16EN;
  TIM16->PSC  = 3;                          // 12MHz
  TIM16->ARR  = 0xffff;
  TIM16->EGR  = 1;
  TIM16->CR1  = 1;
  #endif

  // Configure D- pin as an interrupt on falling edge
  EXTI->EXTICR[USB_PIN_DM >> 2] = (EXTI->EXTICR[USB_PIN_DM >> 2]
                                & ~(0xff << ((USB_PIN_DM & 3) * 8)))
                                | (USB_EXTI_PORT << ((USB_PIN_DM & 3) * 8));
  EXTI->FTSR |= 1 << USB_PIN_DM;
  EXTI->RTSR &= ~(1 << USB_PIN_DM);
  EXTI->PR    = 1 << USB_PIN_DM;
  EXTI->IMR  |= 1 << USB_PIN_DM;

  // USB interrupt with highest priority, all others lower
  for(i = 0; i < 32; i++) NVIC_SetPriority((IRQn_Type)i, 1);
  NVIC_SetPriority(SysTick_IRQn, 1);
  NVIC_SetPriority((IRQn_Type)USB_IRQ_NUM, 0);
  NVIC_ClearPendingIRQ((IRQn_Type)USB_IRQ_NUM);
  NVIC_EnableIRQ((IRQn_Type)USB_IRQ_NUM);

  #ifdef USB_PIN_DPU
  // This drives USB_PIN_DPU (D- pull-up) high, which will tell the host that we are going on-bus.
  DLY_ms(1);
  LOCAL_EXP(GPIO, USB_PORT)->BSRR = 1 << USB_PIN_DPU;
  #endif

  // Enable interrupts
  __enable_irq();
}

// Keep-alive pulse (SE0) every 1ms: calibrate HSI using TIM16 (12MHz)
#define USB_CAL_TARGET  12000               // TIM16 counts per 1ms frame
#define USB_CAL_WINDOW  500                 // plausible deviation (+/- 4%)

void usb_handle_se0(void) {
  #if USB_CALIBRATE > 0
  struct rv003usb_internal * ist = &rv003usb_internal_data;
  uint16_t cnt  = TIM16->CNT;
  uint16_t diff = cnt - ist->last_se0_count;

  // Too short: probably the EOP of a missed packet, not a keep-alive pulse
  if(diff < USB_CAL_TARGET - USB_CAL_WINDOW) {
    if(++ist->reserved < 4) return;         // resynchronize after some tries
  }
  ist->reserved = 0;
  ist->last_se0_count = cnt;

  // Too long: missed frames, just resynchronize
  if(diff > USB_CAL_TARGET + USB_CAL_WINDOW) return;

  // Frame length is plausible: adjust HSI trim (about 0.1% per step = 12 counts).
  // The windup integrates the error, the low gain keeps the control loop stable
  // even if the trim step size differs.
  int16_t delta = (int16_t)diff - USB_CAL_TARGET;
  int32_t windup = ist->se0_windup + delta;
  int32_t step = windup / 32;
  ist->delta_se0_count = delta;
  if(step) {
    uint32_t icscr = RCC->ICSCR;
    int32_t trim = (icscr & 0x1fff) - step;
    if(trim < 0) trim = 0;
    if(trim > 0x1fff) trim = 0x1fff;
    RCC->ICSCR = (icscr & ~0x1fff) | trim;
    windup -= step * 32;
  }
  if(windup >  1000) windup =  1000;
  if(windup < -1000) windup = -1000;
  ist->se0_windup = windup;
  #endif
}

USB_RAMFUNC void usb_pid_handle_in(uint32_t addr, uint8_t * data, uint32_t endp, uint32_t unused, struct rv003usb_internal * ist) {
  (void)addr; (void)unused;
  ist->current_endpoint = endp;
  struct usb_endpoint * e = &ist->eps[endp];

  int tosend = 0;
  uint8_t * sendnow;
  int sendtok = e->toggle_in?0b01001011:0b11000011;

  #if RV003USB_HANDLE_IN_REQUEST
  if(e->custom || endp) {
    // Can re-use data-stack as scratchpad.
    sendnow = __builtin_assume_aligned(data, 4);
    usb_handle_user_in_request(e, sendnow, endp, sendtok, ist);
    return;
  }
  #endif

  tosend = 0;

  // Handle IN (sending data back to PC)
  // Do this down here.
  // We do this because we are required to have an in-endpoint.  We don't
  // have to do anything with it, though.
  uint8_t * tsend = e->opaque;

  int offset = (e->count)<<3;
  tosend = (int)e->max_len - offset;
  if(tosend > ENDPOINT0_SIZE) tosend = ENDPOINT0_SIZE;
  sendnow = tsend + offset;
  if(tosend <= 0) usb_send_empty(sendtok);
  else usb_send_data(sendnow, tosend, 0, sendtok);
}

USB_RAMFUNC void usb_pid_handle_out(uint32_t addr, uint8_t * data, uint32_t endp, uint32_t unused, struct rv003usb_internal * ist) {
  ist->current_endpoint = endp;

  // We need to handle this here because we could have an interrupt in the middle of a control or big transfer.
  // This will correctly swap back the endpoint.
}

USB_RAMFUNC void usb_pid_handle_data(uint32_t this_token, uint8_t * data, uint32_t which_data, uint32_t length, struct rv003usb_internal * ist) {
  //Received data from host.
  int epno = ist->current_endpoint;
  struct usb_endpoint * e = &ist->eps[epno];

  length -= 3;
  uint8_t * data_in = __builtin_assume_aligned( data, 4 );

  // Alrady received this packet.
  if(e->toggle_out != which_data) goto just_ack;

  e->toggle_out = !e->toggle_out;

  #if RV003USB_HANDLE_USER_DATA
  if(epno || (!ist->setup_request && length > 0)) {   // fix from rv003usb upstream (eeba5cf)
    usb_handle_user_data( e, epno, data_in, length, ist );
    #if RV003USB_USER_DATA_HANDLES_TOKEN
    return;
    #endif
  }
  else
  #endif

  if(ist->setup_request) {
    // For receiving CONTROL-OUT messages into RAM.  NOTE: MUST be ALIGNED to 4, and be allocated round_up( payload, 8 ) + 4
    // opaque points to [length received, but uninitialized before complete][data...]
    #if RV003USB_SUPPORT_CONTROL_OUT
    if(ist->setup_request == 2) {
      // This mode is where we record control-in data into a pointer and mark it as 
      int offset = e->count << 3;
      uint32_t * base = __builtin_assume_aligned( e->opaque, 4 );
      uint32_t * dout = __builtin_assume_aligned( ((uint8_t*)base) + offset + 4, 4 );
      uint32_t * din = __builtin_assume_aligned( data_in, 4 );
      if(offset < e->max_len) {
        dout[0] = din[0];
        dout[1] = din[1];
        e->count++;
        if(offset + 8 >= e->max_len) base[0] = e->max_len;
      }
      goto just_ack;
    }
    #endif

    struct usb_urb * s = __builtin_assume_aligned( (struct usb_urb *)(data_in), 4 );

    uint32_t wvi = s->lValueLSBIndexMSB;
    uint32_t wLength = s->wLength;
    //Send just a data packet.
    e->count = 0;
    e->opaque = 0;
    e->custom = 0;
    e->max_len = 0;
    ist->setup_request = 0;

    //int bRequest = s->wRequestTypeLSBRequestMSB >> 8;

    // We shift down because we don't care if USB_RECIP_INTERFACE is set or not.
    // Otherwise we have to write extra code to handle each case if it's set or
    // not set, but in general, there's never a situation where we realy care.
    uint32_t reqShl = s->wRequestTypeLSBRequestMSB >> 1;

    //LogUEvent( 0, s->wRequestTypeLSBRequestMSB, wvi, s->wLength );
    #if RV003USB_HID_FEATURES
    if(reqShl == (0x01a1>>1)) {
      // Class read request.
      // The host wants to read back from us. hid_get_feature_report
      usb_handle_hid_get_report_start( e, wLength, wvi );
    }
    else if(reqShl == (0x0921>>1)) {
      // Class request (Will be writing)  This is hid_send_feature_report
      usb_handle_hid_set_report_start( e, wLength, wvi );
    }
    else
    #endif

    if(reqShl == (0x0680>>1)) {       // GET_DESCRIPTOR = 6 (msb)
      int i;
      const struct descriptor_list_struct * dl;
      for(i = 0; i < DESCRIPTOR_LIST_ENTRIES; i++) {
        dl = &descriptor_list[i];
        if(dl->lIndexValue == wvi) {
          e->opaque = (uint8_t*)dl->addr;
          uint16_t swLen = wLength;
          uint16_t elLen = dl->length;
          e->max_len = (swLen < elLen)?swLen:elLen;
          break;                              // found (faster response)
        }
      }
    }
    else if(reqShl == (0x0500>>1)) {  // SET_ADDRESS = 0x05
      ist->my_address = wvi;
    }

    //  You could handle SET_CONFIGURATION == 0x0900 here if you wanted.
    //  Can also handle GET_CONFIGURATION == 0x0880 to which we send back { 0x00 }, or the interface number.  (But no one does this).
    //  You could handle SET_INTERFACE == 0x1101 here if you wanted.
    //   or
    //  USB_REQ_GET_INTERFACE to which we would send 0x00, or the interface #
    else {
      #if RV003USB_OTHER_CONTROL
      usb_handle_other_control_message( e, s, ist );
      #endif
    }
  }
just_ack:
  {
    //Got the right data.  Acknowledge.
    usb_send_data( 0, 0, 2, 0xD2 ); // Send ACK
  }
  return;
}

#if defined( RV003USB_OPTIMIZE_FLASH ) && RV003USB_OPTIMIZE_FLASH

// Do not compile ACK commands.

#else

USB_RAMFUNC void usb_pid_handle_ack(uint32_t dummy, uint8_t * data, uint32_t dummy1, uint32_t dummy2, struct rv003usb_internal * ist) {
  struct usb_endpoint * e = &ist->eps[ist->current_endpoint];
  e->toggle_in = !e->toggle_in;
  e->count++;
}

//Received a setup for a specific endpoint.
USB_RAMFUNC void usb_pid_handle_setup(uint32_t addr, uint8_t * data, uint32_t endp, uint32_t unused, struct rv003usb_internal * ist) {
  struct usb_endpoint * e = &ist->eps[endp];
  ist->current_endpoint = endp;
  ist->setup_request = 1;
  e->toggle_in = 1;
  e->toggle_out = 0;
  e->count = 0;
  e->opaque = 0;
}

#endif
