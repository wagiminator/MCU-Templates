// ===================================================================================
// USB Host Keyboard Functions for SAMD21                                     * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "host_keyboard.h"

// ===================================================================================
// Variables
// ===================================================================================
uint8_t KBD_report[8];                        // current report
static uint8_t KBD_last[6];                   // keycodes of last report
static uint8_t KBD_buffer[KBD_BUF_SIZE];      // input ring buffer
static uint8_t KBD_rptr, KBD_wptr;            // buffer pointers
static uint8_t KBD_leds;                      // LED state (Num/Caps/Scroll)

// ===================================================================================
// HID Keycode to ASCII Tables (US layout, keycodes 0x04..0x38)
// ===================================================================================
static const char KBD_ascii[] =
  "abcdefghijklmnopqrstuvwxyz1234567890\n\x1B\b\t -=[]\\#;'`,./";
static const char KBD_ascii_shift[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()\n\x1B\b\t _+{}|~:\"~<>?";
static const char KBD_keypad[] = "/*-+\n1234567890.";   // keycodes 0x54..0x63

static void KBD_sendLEDs(void);

// ===================================================================================
// Class Driver Interface
// ===================================================================================

// Match HID boot keyboard interface (class 3, subclass 1, protocol 1)
uint8_t USBH_CLASS_match(uint8_t* itf) {
  return (itf[5] == 3) && (itf[6] == 1) && (itf[7] == 1);
}

// Init keyboard: set boot protocol and idle rate, switch LEDs off
uint8_t USBH_CLASS_init(void) {
  uint8_t i;
  USBH_setup(USB_REQ_TYP_CLASS | USB_REQ_RECIP_INTERF, HID_SET_PROTOCOL, 0, USBH_dev.itf, 0);
  if(USBH_control(0)) return USBH_ERR_PROTOCOL;
  USBH_setup(USB_REQ_TYP_CLASS | USB_REQ_RECIP_INTERF, HID_SET_IDLE, 0, USBH_dev.itf, 0);
  USBH_control(0);                            // optional, may be stalled
  for(i = 0; i < 8; i++) KBD_report[i] = 0;
  for(i = 0; i < 6; i++) KBD_last[i] = 0;
  KBD_rptr = KBD_wptr = 0;
  KBD_leds = 0;
  KBD_sendLEDs();                             // all LEDs off (optional)
  return USBH_OK;
}

// ===================================================================================
// Front End Functions
// ===================================================================================

// Send LED state to keyboard (output report)
static void KBD_sendLEDs(void) {
  USBH_buffer[0] = KBD_leds;
  USBH_setup(USB_REQ_TYP_CLASS | USB_REQ_RECIP_INTERF, HID_SET_REPORT, 0x0200, USBH_dev.itf, 1);
  USBH_control(USBH_buffer);
}

// Set keyboard LEDs
void KBD_setLEDs(uint8_t leds) {
  KBD_leds = leds;
  if(USBH_ready()) KBD_sendLEDs();
}

// Check if key is currently pressed
uint8_t KBD_isPressed(uint8_t key) {
  uint8_t i;
  for(i = 2; i < 8; i++) if(KBD_report[i] == key) return 1;
  return 0;
}

// Check if characters are in the buffer
uint8_t KBD_available(void) {
  return KBD_rptr != KBD_wptr;
}

// Read character from buffer
char KBD_read(void) {
  char c;
  if(KBD_rptr == KBD_wptr) return 0;
  c = KBD_buffer[KBD_rptr];
  KBD_rptr = (KBD_rptr + 1) & (KBD_BUF_SIZE - 1);
  return c;
}

// Convert keycode to character and put it into buffer (internal)
static void KBD_put(uint8_t key) {
  uint8_t next;
  char c;
  uint8_t shift = KBD_report[0] & (KBD_MOD_LSHIFT | KBD_MOD_RSHIFT);
  if(key == 0x39) {                           // CapsLock
    KBD_setLEDs(KBD_leds ^ KBD_LED_CAPS);
    return;
  }
  if(key == 0x53) {                           // NumLock
    KBD_setLEDs(KBD_leds ^ KBD_LED_NUM);
    return;
  }
  if(key >= 0x04 && key <= 0x38) {            // printable key
    if(key <= 0x1D && (KBD_leds & KBD_LED_CAPS)) shift = !shift; // CapsLock for letters
    c = shift ? KBD_ascii_shift[key - 4] : KBD_ascii[key - 4];
  }
  else if(key == 0x4C) c = 0x7F;              // Delete
  else if(key >= 0x54 && key <= 0x63 && ((KBD_leds & KBD_LED_NUM) || key <= 0x58))
    c = KBD_keypad[key - 0x54];               // keypad
  else c = 0x80 | key;                        // special key
  next = (KBD_wptr + 1) & (KBD_BUF_SIZE - 1);
  if(next != KBD_rptr) {                      // buffer not full?
    KBD_buffer[KBD_wptr] = c;
    KBD_wptr = next;
  }
}

// Handle USB host and poll keyboard
uint8_t KBD_update(void) {
  uint8_t i, j, key;
  USBH_task();
  if(USBH_poll() < 8) return 0;
  for(i = 0; i < 8; i++) KBD_report[i] = USBH_RX_buffer[i];
  if(KBD_report[2] == 0x01) return 1;         // rollover error: ignore keys
  for(i = 2; i < 8; i++) {                    // find newly pressed keys
    key = KBD_report[i];
    if(!key) continue;
    for(j = 0; j < 6; j++) if(KBD_last[j] == key) break;
    if(j == 6) KBD_put(key);
  }
  for(i = 0; i < 6; i++) KBD_last[i] = KBD_report[i + 2];
  return 1;
}
