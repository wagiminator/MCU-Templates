// ===================================================================================
// USB HID Composite Device Functions for CH32V003                            * v1.0 *
// ===================================================================================

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
const uint8_t KBD_map[128] = {
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
    key = KBD_map[key];                         // convert ascii to keycode for report
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
    key = KBD_map[key];                         // convert ascii to keycode for report
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
void CON_press(uint8_t key) {
  CON_report[1] = key;
}

// Release a consumer key on keyboard
void CON_release(void) {
  CON_report[1] = 0;
}

// Press and release a consumer key on keyboard
void CON_type(uint8_t key) {
  CON_press(key);
  DLY_ms(6);
  CON_release();
  DLY_ms(6);
}

// ===================================================================================
// Mouse Functions
// ===================================================================================

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
    MOUSE_report[2] += (uint8_t)xrel;           // set relative x-movement
    MOUSE_report[3] += (uint8_t)yrel;           // set relative y-movement
  }
}

// Move mouse wheel
void MOUSE_wheel(int8_t rel) {
  INT_ATOMIC_BLOCK {
    MOUSE_report[4] += (uint8_t)rel;            // set relative wheel movement
  }
}

// ===================================================================================
// RV003USB Software USB User Handle Functions
// ===================================================================================
void usb_handle_user_in_request(struct usb_endpoint * e, uint8_t * scratchpad, int endp, uint32_t sendtok, struct rv003usb_internal * ist) {

  // Current device (alternating)
  static uint8_t dev = 3;

  // Mouse
  if(endp == 1) {
    if(!(--dev)) dev = 3;
    switch(dev) {
      case 1: usb_send_data((uint8_t*)&KBD_report, sizeof(KBD_report), 0, sendtok); return;
      case 2: usb_send_data((uint8_t*)&CON_report, sizeof(CON_report), 0, sendtok); return;
      case 3: usb_send_data((uint8_t*)&MOUSE_report, sizeof(MOUSE_report), 0, sendtok);
              MOUSE_report[2] = 0; MOUSE_report[3] = 0; MOUSE_report[4] = 0; return;
    }
  }

  // Control transfer
  usb_send_empty(sendtok);
}

void usb_handle_user_data(struct usb_endpoint * e, int current_endpoint, uint8_t * data, int len, struct rv003usb_internal * ist) {
  if(current_endpoint == 1) KBD_state = data[0];
}
