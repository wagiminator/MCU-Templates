// ===================================================================================
// Software USB Handler for CH32V003                                          * v1.0 *
// ===================================================================================
//
// This file contains a copy of rv003usb.h (https://github.com/cnlohr/rv003usb),
// copyright (c) 2023 CNLohr (MIT License), with some minor changes by Stefan Wagner.

#pragma once

#include "usb_descr.h"

// USB handler checks
#ifndef __ASSEMBLER__
#include "system.h"
#if SYS_USE_VECTORS == 0
  #error Interrupt vector table must be enabled (SYS_USE_VECTORS in system.h)!
#endif
#if SYS_TICK_INIT == 0
  #error SysTick must be enabled (SYS_TICK_INIT in system.h)!
#endif
#if F_CPU != 48000000
  #error System clock (F_CPU) must be 48MHz!
#endif
#endif

#define LOCAL_CONCAT_BASE(A, B) A##B##_BASE
#define LOCAL_EXP_BASE(A, B) LOCAL_CONCAT_BASE(A,B)

#define USB_GPIO_BASE LOCAL_EXP_BASE( GPIO, USB_PORT )



#ifndef __ASSEMBLER__

extern uint32_t * always0;

struct usb_endpoint;
struct rv003usb_internal;
struct usb_urb;

// usb_hande_interrupt_in is OBLIGATED to call usb_send_data or usb_send_empty.
// Enable with RV003USB_HANDLE_IN_REQUEST=1
void usb_handle_user_in_request( struct usb_endpoint * e, uint8_t * scratchpad, int endp, uint32_t sendtok, struct rv003usb_internal * ist );

// Enable with RV003USB_HID_FEATURES=1
void usb_handle_hid_set_report_start( struct usb_endpoint * e, int reqLen, uint32_t lValueLSBIndexMSB );
void usb_handle_hid_get_report_start( struct usb_endpoint * e, int reqLen, uint32_t lValueLSBIndexMSB );

// Enable with RV003USB_OTHER_CONTROL=1
void usb_handle_other_control_message( struct usb_endpoint * e, struct usb_urb * s, struct rv003usb_internal * ist );

// Received data from the host which is not an internal control message, i.e.
// this could be going to an endpoint or be data coming in for an unidentified
// control message.
// Enable with RV003USB_HANDLE_USER_DATA=1
void usb_handle_user_data( struct usb_endpoint * e, int current_endpoint, uint8_t * data, int len, struct rv003usb_internal * ist );

// If you want to use custom functions for the level 2 stack, then say
// RV003USB_CUSTOM_C
// This is mostly useful on things like bootloaders.

// Note: This checks addr & endp to make sure they are valid.
void usb_pid_handle_setup( uint32_t addr, uint8_t * data, uint32_t endp, uint32_t unused, struct rv003usb_internal * ist );
void usb_pid_handle_in( uint32_t addr, uint8_t * data, uint32_t endp, uint32_t unused, struct rv003usb_internal * ist );
void usb_pid_handle_out( uint32_t addr, uint8_t * data, uint32_t endp, uint32_t unused, struct rv003usb_internal * ist );
void usb_pid_handle_data( uint32_t this_token, uint8_t * data, uint32_t which_data, uint32_t length, struct rv003usb_internal * ist );
void usb_pid_handle_ack( uint32_t dummy, uint8_t * data, uint32_t dummy2, uint32_t dummy3, struct rv003usb_internal * ist  );

//poly_function = 0 to include CRC.
//poly_function = 2 to exclude CRC.
//This function is provided in assembly
void usb_send_data( const void * data, uint32_t length, uint32_t poly_function, uint32_t token );
void usb_send_empty( uint32_t token );

void usb_setup();

#define LogUEvent( a, b, c,  d )
#define GetUEvent() 0

#endif    // __ASSEMBLER__


// Internal stuff.

// Packet Type + 8 + CRC + Buffer
#define USB_BUFFER_SIZE 12

#define USB_DMASK ((1<<(USB_PIN_DP)) | 1<<(USB_PIN_DM))

#ifdef  RV003USB_OPTIMIZE_FLASH
#define MY_ADDRESS_OFFSET_BYTES 4
#define LAST_SE0_OFFSET         16
#define DELTA_SE0_OFFSET        20
#define SE0_WINDUP_OFFSET       24
#define ENDP_OFFSET             28
#define SETUP_REQUEST_OFFSET    8

#define EP_COUNT_OFFSET         0
#define EP_TOGGLE_IN_OFFSET     4
#define EP_TOGGLE_OUT_OFFSET    8
#define EP_IS_CUSTOM_OFFSET     12
#define EP_MAX_LEN_OFFSET       16
#define EP_OPAQUE_OFFSET        28
#else
#define MY_ADDRESS_OFFSET_BYTES 1
#define LAST_SE0_OFFSET         4
#define DELTA_SE0_OFFSET        8
#define SE0_WINDUP_OFFSET       12
#endif

#ifndef __ASSEMBLER__

#define EMPTY_SEND_BUFFER (uint8_t*)1

// This can be undone once support for fast-c.lbu / c.sbu is made available.
#ifdef  RV003USB_OPTIMIZE_FLASH
#define TURBO8TYPE uint32_t
#define TURBO16TYPE uint32_t
#else
#define TURBO8TYPE uint8_t
#define TURBO16TYPE uint16_t
#endif

struct usb_endpoint {
  TURBO8TYPE count;	      // ack count / in count
  TURBO8TYPE toggle_in;   // DATA0 or DATA1?
  TURBO8TYPE toggle_out;  // Out PC->US
  TURBO8TYPE custom;      // Anything nonzero will incur the custom call.
  TURBO16TYPE max_len;
  TURBO16TYPE reserved1;
  uint32_t    reserved2; 
  uint8_t *   opaque;     // For user.
};  // CAREFUL! sizeof pacekt 

// Make the size of this a power of 2, otherwise it will be slow to access.
#ifdef RV003USB_OPTIMIZE_FLASH
_Static_assert( (sizeof(struct usb_endpoint) == 32), "usb_endpoint must be pow2 sized" );
#else
_Static_assert( (sizeof(struct usb_endpoint) == 16), "usb_endpoint must be pow2 sized" );
#endif

struct rv003usb_internal {
  TURBO8TYPE current_endpoint; // Can this be combined with setup_request?
  TURBO8TYPE my_address;       // Will be 0 until set up.
  TURBO8TYPE setup_request;    // 0 for non-setup request, 1 after setup token, is allowed to be 2 for control-out if RV003USB_SUPPORT_CONTROL_OUT is set.
  TURBO8TYPE reserved;
  uint32_t last_se0_cyccount;
  int32_t delta_se0_cyccount;
  uint32_t se0_windup;
  // 5 bytes + 6 * ENDPOINTS

  struct usb_endpoint eps[ENDPOINTS];
};

//Detailed analysis of some useful stuff and performance tweaking: http://naberius.de/2015/05/14/esp8266-gpio-output-performance/
//Reverse engineerd boot room can be helpful, too: http://cholla.mmto.org/esp8266/bootrom/boot.txt
//USB Protocol read from wikipedia: https://en.wikipedia.org/wiki/USB
// Neat stuff: http://www.usbmadesimple.co.uk/ums_3.htm
// Neat stuff: http://www.beyondlogic.org/usbnutshell/usb1.shtml

struct usb_urb {
  uint16_t wRequestTypeLSBRequestMSB;
  uint32_t lValueLSBIndexMSB;
  uint16_t wLength;
} __attribute__((packed));


extern struct rv003usb_internal rv003usb_internal_data;

#endif    // __ASSEMBLER__
