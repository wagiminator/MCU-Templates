// ===================================================================================
// USB HID Composite Device Functions for tinyAVR 0/1-Series                  * v1.1 *
// ===================================================================================

#include <avr/pgmspace.h>
#include "usb_composite.h"

// ===================================================================================
// HID reports
// ===================================================================================
volatile uint8_t KBD_report[]   = {1,0,0,0,0,0,0,0};
volatile uint8_t CON_report[]   = {2,0,0};
volatile uint8_t MOUSE_report[] = {3,0,0,0,0};
volatile uint8_t KBD_state;

// ===================================================================================
// ASCII to keycode mapping table
// ===================================================================================
static const uint8_t KBD_map[128] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2a, 0x2b, 0x28, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x2c, 0x9e, 0xb4, 0xa0, 0xa1, 0xa2, 0xa4, 0x34, 0xa6, 0xa7,
  0xa5, 0xae, 0x36, 0x2d, 0x37, 0x38, 0x27, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24,
  0x25, 0x26, 0xb3, 0x33, 0xb6, 0x2e, 0xb7, 0xb8, 0x9f, 0x84, 0x85, 0x86, 0x87, 0x88,
  0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96,
  0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x2f, 0x31, 0x30, 0xa3, 0xad, 0x35, 0x04,
  0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12,
  0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0xaf, 0xb1, 0xb0,
  0xb5, 0x00
};

// ===================================================================================
// Standard Keyboard Functions
// ===================================================================================

// Press a key on keyboard
void KBD_press(uint8_t key) {
  uint8_t i;

  // Convert key for HID report
  if(key >= 136) key -= 136;                    // non-printing key/not a modifier?
  else if(key >= 128) {                         // modifier key?
    KBD_report[1] |= (1<<(key-128));            // add modifier to report
    key = 0;
  }
  else {                                        // printing key?
    key = pgm_read_byte(&KBD_map[key]);                         // convert ascii to keycode for report
    if(!key) return;                            // no valid key
    if(key & 0x80) {                            // capital letter/shift character?
      KBD_report[1] |= 0x02;                    // add left shift modifier
      key &= 0x7F;                              // remove shift from key itself
    }
  }

  // Check if key is already present in report
  for(i=3; i<8; i++) {
    if(KBD_report[i] == key) return;            // return if already in report
  }

  // Find an empty slot, insert key and transmit report
  for(i=3; i<8; i++) {
    if(KBD_report[i] == 0) {                    // empty slot?
      KBD_report[i] = key;                      // insert key
      return;                                   // and return
    }
  }
}

// Release a key on keyboard
void KBD_release(uint8_t key) {
  uint8_t i;

  // Convert key for HID report
  if(key >= 136) key -= 136;                    // non-printing key/not a modifier?
  else if(key >= 128) {                         // modifier key?
    KBD_report[1] &= ~(1<<(key-128));           // delete modifier in report
    key = 0;
  }
  else {                                        // printing key?
    key = pgm_read_byte(&KBD_map[key]);                         // convert ascii to keycode for report
    if(!key) return;                            // no valid key
    if(key & 0x80) {                            // capital letter/shift character?
      KBD_report[1] &= ~0x02;                   // remove shift modifier
      key &= 0x7F;                              // remove shift from key itself
    }
  }

  // Delete key in report
  for(i=3; i<8; i++) {
    if(KBD_report[i] == key) KBD_report[i] = 0; // delete key in report
  }
}

// Press and release a key on keyboard
void KBD_type(uint8_t key) {
  KBD_press(key);
  DLY_ms(6);
  KBD_release(key);
  DLY_ms(6);
}

// Release all keys on keyboard
void KBD_releaseAll(void) {
  uint8_t i;
  for(i=7; i; i--) KBD_report[i] = 0;           // delete all keys in report
}

// Write text with keyboard
void KBD_print(char* str) {
  while(*str) KBD_type(*str++);
}

// ===================================================================================
// Consumer Multimedia Keyboard Functions
// ===================================================================================

// Press a consumer key on keyboard
void CON_press(uint16_t key) {
  CON_report[1] = key & 0xff;
  CON_report[2] = key >> 8;
}

// Release a consumer key on keyboard
void CON_release(void) {
  CON_report[1] = 0;
  CON_report[2] = 0;
}

// Press and release a consumer key on keyboard
void CON_type(uint16_t key) {
  CON_press(key);
  DLY_ms(6);
  CON_release();
  DLY_ms(6);
}

// ===================================================================================
// Mouse Functions
// ===================================================================================

// Add relative movement with saturation (-127..127)
static int8_t MOUSE_add(int8_t a, int8_t b) {
  int16_t r = (int16_t)a + b;
  return (r > 127) ? 127 : (r < -127) ? -127 : (int8_t)r;
}

// Press mouse button(s)
void MOUSE_press(uint8_t buttons) {
  MOUSE_report[1] |= buttons;                   // press button(s)
}

// Release mouse button(s)
void MOUSE_release(uint8_t buttons) {
  MOUSE_report[1] &= ~buttons;                  // release button(s)
}

// Move mouse pointer
void MOUSE_move(int8_t xrel, int8_t yrel) {
  INT_ATOMIC_BLOCK {
    MOUSE_report[2] = (uint8_t)MOUSE_add((int8_t)MOUSE_report[2], xrel); // add x-movement
    MOUSE_report[3] = (uint8_t)MOUSE_add((int8_t)MOUSE_report[3], yrel); // add y-movement
  }
}

// Move mouse wheel
void MOUSE_wheel(int8_t rel) {
  INT_ATOMIC_BLOCK {
    MOUSE_report[4] = (uint8_t)MOUSE_add((int8_t)MOUSE_report[4], rel);  // add wheel movement
  }
}

// ===================================================================================
// V-USB Software USB User Handle Functions
// ===================================================================================
// Copy report if it has changed (internal)
static uint8_t HID_changed(uint8_t* last, volatile uint8_t* report, uint8_t len) {
  uint8_t changed = 0;
  while(len--) {
    if(*last != *report) {
      *last = *report;
      changed = 1;
    }
    last++; report++;
  }
  return changed;
}

// Send changed reports alternately (called after each USB poll)
void usb_handle_user_in(void) {
  static uint8_t dev = 0;
  static uint8_t last_kbd[sizeof(KBD_report)];
  static uint8_t last_con[sizeof(CON_report)];
  static uint8_t last_buttons;
  uint8_t i, buf[5];
  if(!USB_EP1_ready()) return;
  for(i=3; i; i--) {
    if(++dev > 3) dev = 1;
    switch(dev) {
      case 1: // keyboard
        if(HID_changed(last_kbd, KBD_report, sizeof(KBD_report))) {
          USB_EP1_send(last_kbd, sizeof(KBD_report));
          return;
        }
        break;
      case 2: // consumer keys
        if(HID_changed(last_con, CON_report, sizeof(CON_report))) {
          USB_EP1_send(last_con, sizeof(CON_report));
          return;
        }
        break;
      case 3: // mouse
        if((MOUSE_report[1] != last_buttons) || MOUSE_report[2]
          || MOUSE_report[3] || MOUSE_report[4]) {
          buf[0] = 3;
          buf[1] = last_buttons = MOUSE_report[1];
          buf[2] = MOUSE_report[2]; MOUSE_report[2] = 0;
          buf[3] = MOUSE_report[3]; MOUSE_report[3] = 0;
          buf[4] = MOUSE_report[4]; MOUSE_report[4] = 0;
          USB_EP1_send(buf, 5);
          return;
        }
        break;
    }
  }
}

// LED output report with report ID 1: [ID, LEDs] (OUT endpoint 2 or SET_REPORT)
void usb_handle_user_data(uint8_t ep, uint8_t* data, uint8_t len) {
  (void)ep;
  if((len >= 2) && (data[0] == 1)) KBD_state = data[1];
}

// GET_REPORT request
usbMsgLen_t usb_handle_get_report(uint8_t id) {
  switch(id) {
    case 1:  usbMsgPtr = (usbMsgPtr_t)KBD_report;   return sizeof(KBD_report);
    case 2:  usbMsgPtr = (usbMsgPtr_t)CON_report;   return sizeof(CON_report);
    case 3:  usbMsgPtr = (usbMsgPtr_t)MOUSE_report; return sizeof(MOUSE_report);
  }
  return 0;
}
