// ===================================================================================
// Software USB Handler for PY32F0xx                                          * v1.0 *
// ===================================================================================
//
// Low-speed USB device in software (bit-banging) for PY32F002A/F003/F030 running at
// 48MHz. The C part is a copy of rv003usb.c (https://github.com/cnlohr/rv003usb),
// copyright (c) 2023 CNLohr (MIT License), with some minor changes. The assembler
// part (usb_handler.S) was rewritten for the Cortex-M0+ core.
//
// Notes:
// ------
// - The system clock must be 48MHz (F_CPU = 48000000, HSI 24MHz x PLL).
// - The timing critical functions are executed from SRAM (about 1KB). Each USB bit
//   takes exactly 32 clock cycles, the GPIO is accessed via the single-cycle IOPORT.
// - The HSI oscillator is calibrated using the USB keep-alive pulses (1ms) and
//   TIM16 (if USB_CALIBRATE is set in config.h). TIM16 cannot be used otherwise.
// - The USB pin interrupt (EXTI of D-) gets the highest priority, all other
//   interrupts are set to lower priority by USB_init(). Interrupts must not be
//   disabled for longer than a few microseconds.
// - Functions available to the user are defined in the device library (e.g.
//   usb_keyboard.h). USB_init() (or e.g. KBD_init()) must be called first.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#include "usb_descr.h"

// ===================================================================================
// Default Settings
// ===================================================================================
#ifndef USB_PORT
  #define USB_PORT            A
#endif
#ifndef USB_PIN_DP
  #define USB_PIN_DP          3
#endif
#ifndef USB_PIN_DM
  #define USB_PIN_DM          1
#endif
#ifndef USB_CALIBRATE
  #define USB_CALIBRATE       1
#endif

// ===================================================================================
// Hardware Definitions (also used by the assembler file)
// ===================================================================================
#define LOCAL_CONCAT(A, B)    A##B
#define LOCAL_EXP(A, B)       LOCAL_CONCAT(A,B)

// GPIO port base addresses (IOPORT, single-cycle access) and EXTI port codes
#define USB_GPIOBASE_A        0x50000000
#define USB_GPIOBASE_B        0x50000400
#define USB_GPIOBASE_F        0x50001400
#define USB_EXTIPORT_A        0
#define USB_EXTIPORT_B        1
#define USB_EXTIPORT_F        2
#define USB_GPIO_BASE         LOCAL_EXP(USB_GPIOBASE_, USB_PORT)
#define USB_EXTI_PORT         LOCAL_EXP(USB_EXTIPORT_, USB_PORT)

// GPIO register offsets
#define USB_GPIO_MODER        0x00
#define USB_GPIO_IDR          0x10
#define USB_GPIO_BSRR         0x18

// EXTI and NVIC registers
#define USB_EXTI_PR           0x4002180C
#define USB_NVIC_ICPR         0xE000E280

// EXTI interrupt for D- pin
#if   USB_PIN_DM <= 1
  #define USB_IRQ_HANDLER     EXTI0_1_IRQHandler
  #define USB_IRQ_NUM         5
#elif USB_PIN_DM <= 3
  #define USB_IRQ_HANDLER     EXTI2_3_IRQHandler
  #define USB_IRQ_NUM         6
#else
  #define USB_IRQ_HANDLER     EXTI4_15_IRQHandler
  #define USB_IRQ_NUM         7
#endif

// Pin masks and line states (low-speed: J = D- high, K = D+ high)
#define USB_DMASK             ((1<<USB_PIN_DP) | (1<<USB_PIN_DM))
#define USB_KSTATE            (1<<USB_PIN_DP)
#define USB_BSRR_K            ((1<<USB_PIN_DP) | (1<<(USB_PIN_DM+16)))
#define USB_BSRR_J            ((1<<USB_PIN_DM) | (1<<(USB_PIN_DP+16)))
#define USB_BSRR_SE0          ((1<<(USB_PIN_DP+16)) | (1<<(USB_PIN_DM+16)))
#define USB_BSRR_TOGGLE       (USB_BSRR_K ^ USB_BSRR_J)
#define USB_MODER_MASK        ((3<<(USB_PIN_DP*2)) | (3<<(USB_PIN_DM*2)))
#define USB_MODER_OUT         ((1<<(USB_PIN_DP*2)) | (1<<(USB_PIN_DM*2)))

// Packet Type + 8 + CRC + Buffer
#define USB_BUFFER_SIZE       12

// Offset of my_address in struct rv003usb_internal
#define MY_ADDRESS_OFFSET_BYTES 1

#ifndef __ASSEMBLER__

// ===================================================================================
// USB Handler Checks
// ===================================================================================
#include "system.h"
#if SYS_USE_VECTORS == 0
  #error Interrupt vector table must be enabled (SYS_USE_VECTORS in system.h)!
#endif
#if F_CPU != 48000000
  #error System clock (F_CPU) must be 48MHz!
#endif
#if USB_PIN_DP == USB_PIN_DM
  #error USB_PIN_DP and USB_PIN_DM must be different!
#endif

// ===================================================================================
// USB Handler Functions
// ===================================================================================
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

// Functions in SRAM (faster than flash with wait states)
#define USB_RAMFUNC __attribute__((section(".RamFunc"), noinline))

// Called by the assembler part
void usb_pid_handle_setup( uint32_t addr, uint8_t * data, uint32_t endp, uint32_t unused, struct rv003usb_internal * ist );
void usb_pid_handle_in( uint32_t addr, uint8_t * data, uint32_t endp, uint32_t unused, struct rv003usb_internal * ist );
void usb_pid_handle_out( uint32_t addr, uint8_t * data, uint32_t endp, uint32_t unused, struct rv003usb_internal * ist );
void usb_pid_handle_data( uint32_t this_token, uint8_t * data, uint32_t which_data, uint32_t length, struct rv003usb_internal * ist );
void usb_pid_handle_ack( uint32_t dummy, uint8_t * data, uint32_t dummy2, uint32_t dummy3, struct rv003usb_internal * ist  );
void usb_handle_se0(void);

// poly_function = 0 to include CRC.
// poly_function = 2 to exclude CRC.
// These functions are provided in assembly and are located in SRAM.
__attribute__((long_call)) void usb_send_data( const void * data, uint32_t length, uint32_t poly_function, uint32_t token );
__attribute__((long_call)) void usb_send_empty( uint32_t token );

// Init and connect USB
void usb_setup(void);
#define USB_init              usb_setup

#define LogUEvent( a, b, c,  d )
#define GetUEvent() 0

// ===================================================================================
// Internal Structures
// ===================================================================================
#define EMPTY_SEND_BUFFER (uint8_t*)1

struct usb_endpoint {
  uint8_t   count;            // ack count / in count
  uint8_t   toggle_in;        // DATA0 or DATA1?
  uint8_t   toggle_out;       // Out PC->US
  uint8_t   custom;           // Anything nonzero will incur the custom call.
  uint16_t  max_len;
  uint16_t  reserved1;
  uint32_t  reserved2;
  uint8_t * opaque;           // For user.
};  // CAREFUL! sizeof pacekt

// Make the size of this a power of 2, otherwise it will be slow to access.
_Static_assert( (sizeof(struct usb_endpoint) == 16), "usb_endpoint must be pow2 sized" );

struct rv003usb_internal {
  uint8_t  current_endpoint;  // Can this be combined with setup_request?
  uint8_t  my_address;        // Will be 0 until set up.
  uint8_t  setup_request;     // 0 for non-setup request, 1 after setup token
  uint8_t  reserved;          // used for HSI calibration
  uint16_t last_se0_count;    // TIM16 count at last keep-alive pulse
  int16_t  delta_se0_count;   // measured frame length - 48000
  int32_t  se0_windup;
  struct usb_endpoint eps[ENDPOINTS];
};

struct usb_urb {
  uint16_t wRequestTypeLSBRequestMSB;
  uint32_t lValueLSBIndexMSB;
  uint16_t wLength;
} __attribute__((packed));

extern struct rv003usb_internal rv003usb_internal_data;

#endif // __ASSEMBLER__
