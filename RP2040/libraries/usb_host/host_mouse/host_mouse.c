// ===================================================================================
// USB Host Mouse Functions for RP2040                                        * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "host_mouse.h"

// ===================================================================================
// Variables
// ===================================================================================
static uint8_t MOUSE_buttons;                 // current buttons
static int16_t MOUSE_x, MOUSE_y;              // accumulated movement
static int8_t  MOUSE_wheel;                   // accumulated wheel movement

// ===================================================================================
// Class Driver Interface
// ===================================================================================

// Match HID boot mouse interface (class 3, subclass 1, protocol 2)
uint8_t USBH_CLASS_match(uint8_t* itf) {
  return (itf[5] == 3) && (itf[6] == 1) && (itf[7] == 2);
}

// Init mouse: set boot protocol and idle rate
uint8_t USBH_CLASS_init(void) {
  USBH_setup(USB_REQ_TYP_CLASS | USB_REQ_RECIP_INTERF, HID_SET_PROTOCOL, 0, USBH_dev.itf, 0);
  if(USBH_control(0)) return USBH_ERR_PROTOCOL;
  USBH_setup(USB_REQ_TYP_CLASS | USB_REQ_RECIP_INTERF, HID_SET_IDLE, 0, USBH_dev.itf, 0);
  USBH_control(0);                            // optional, may be stalled
  MOUSE_buttons = 0;
  MOUSE_x = MOUSE_y = 0;
  MOUSE_wheel = 0;
  return USBH_OK;
}

// ===================================================================================
// Front End Functions
// ===================================================================================

// Handle USB host and poll mouse
uint8_t MOUSE_update(void) {
  uint8_t len;
  USBH_task();
  if(!USBH_ready()) MOUSE_buttons = 0;
  len = USBH_poll();
  if(len < 3) return 0;
  MOUSE_buttons = USBH_RX_buffer[0];
  MOUSE_x += (int8_t)USBH_RX_buffer[1];
  MOUSE_y += (int8_t)USBH_RX_buffer[2];
  if(len > 3) MOUSE_wheel += (int8_t)USBH_RX_buffer[3];
  return 1;
}

// Get current buttons
uint8_t MOUSE_getButtons(void) {
  return MOUSE_buttons;
}

// Get accumulated movement and reset it
int16_t MOUSE_getX(void) {
  int16_t v = MOUSE_x;
  MOUSE_x = 0;
  return v;
}

int16_t MOUSE_getY(void) {
  int16_t v = MOUSE_y;
  MOUSE_y = 0;
  return v;
}

int8_t MOUSE_getWheel(void) {
  int8_t v = MOUSE_wheel;
  MOUSE_wheel = 0;
  return v;
}
