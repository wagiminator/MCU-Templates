// ===================================================================================
// USB Host Gamepad/Joystick Functions for CH32X035                           * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "host_gamepad.h"

// ===================================================================================
// Variables
// ===================================================================================
uint8_t  GPD_axis[GPD_AXES];                  // current axis values
uint8_t  GPD_hat;                             // current hat value
uint16_t GPD_buttons;                         // current buttons
uint8_t  GPD_btnCount;                        // number of buttons
uint8_t  GPD_axisMask;                        // available axes

// Field descriptions found in the report descriptor
typedef struct {
  uint16_t pos;                               // bit position in report
  uint8_t  size;                              // size in bits
  uint8_t  bits;                              // effective bits of logical range
  uint8_t  sign;                              // 1: signed value
  int8_t   min;                               // logical minimum (hat only)
} GPD_FIELD_t;

static GPD_FIELD_t GPD_field[GPD_AXES + 1];   // axes and hat (index 6)
static uint16_t GPD_btnPos;                   // bit position of first button
static uint8_t  GPD_rid;                      // report ID (0: none)
static uint8_t  GPD_hatMode;                  // 0: no hat, 1: 8 directions, 2: 4 directions

// ===================================================================================
// HID Report Descriptor Parser
// ===================================================================================
static void GPD_parse(uint16_t len) {
  uint8_t* r = USBH_buffer;
  GPD_FIELD_t* f;
  uint16_t pos = 0;                           // current bit position
  int32_t  data, lmin = 0, lmax = 0, range;
  uint8_t  page = 0, size = 0, count = 0, rid = 0, ridset = 0;
  uint8_t  usages[8], ucnt = 0, umin = 0, urange = 0;
  uint8_t  b, sz, tag, k, n, usage, bits;

  GPD_axisMask = 0;
  GPD_btnCount = 0;
  GPD_hatMode  = 0;
  GPD_rid      = 0;

  while(len) {
    b = *r++; len--;
    if(b == 0xFE) {                           // long item: skip
      if(len < 2) break;
      sz = r[0] + 2;
      if(len < sz) break;
      r += sz; len -= sz;
      continue;
    }
    sz = b & 3;
    if(sz == 3) sz = 4;
    if(len < sz) break;
    data = 0;                                 // read data (little-endian, signed)
    if(sz == 1) data = (int8_t)r[0];
    else if(sz == 2) data = (int16_t)(r[0] | ((uint16_t)r[1] << 8));
    else if(sz == 4) data = r[0] | ((uint32_t)r[1] << 8) | ((uint32_t)r[2] << 16) | ((uint32_t)r[3] << 24);
    r += sz; len -= sz;
    tag = b & 0xFC;

    switch(tag) {
      case 0x04: page = (data & 0xFF00) ? 0xFF : (uint8_t)data; break; // usage page
      case 0x14: lmin  = data; break;         // logical minimum
      case 0x24: lmax  = (sz < 4 && lmin >= 0 && data < 0) ? (data & (sz == 1 ? 0xFF : 0xFFFF)) : data; break;
      case 0x74: size  = (uint8_t)data; break; // report size
      case 0x94: count = (uint8_t)data; break; // report count
      case 0x84:                              // report ID
        rid = (uint8_t)data;
        pos = 0;
        break;
      case 0x08: if(ucnt < 8) usages[ucnt++] = (uint8_t)data; break; // usage
      case 0x18: umin = (uint8_t)data; urange = 1; break; // usage minimum
      case 0x80:                              // input item
        if(!(data & 0x01) && (!ridset || rid == GPD_rid)) { // data field of our report
          for(k = 0; k < count; k++) {
            if(k < ucnt) usage = usages[k];
            else if(urange) usage = umin + k;
            else usage = ucnt ? usages[ucnt - 1] : 0;
            if(page == 0x01 && ((usage >= 0x30 && usage <= 0x35) || usage == 0x39)) {
              n = (usage == 0x39) ? GPD_AXES : usage - 0x30;
              if(n == GPD_AXES ? !GPD_hatMode : !(GPD_axisMask & (1 << n))) {
                f = &GPD_field[n];
                f->pos  = pos;
                f->size = size > 16 ? 16 : size;
                f->sign = lmin < 0;
                f->min  = (int8_t)lmin;
                range   = lmax - lmin;        // effective bits of logical range
                for(bits = 0; range > 0 && bits < 16; bits++) range >>= 1;
                f->bits = bits ? bits : f->size;
                if(n == GPD_AXES) GPD_hatMode = ((lmax - lmin) == 3) ? 2 : 1;
                else GPD_axisMask |= 1 << n;
                if(!ridset) {GPD_rid = rid; ridset = 1;}
              }
            }
            else if(page == 0x09 && !GPD_btnCount && size == 1) { // buttons
              GPD_btnPos   = pos;
              GPD_btnCount = (count - k) > 16 ? 16 : (count - k);
              if(!ridset) {GPD_rid = rid; ridset = 1;}
            }
            pos += size;
          }
        }
        else pos += (uint16_t)size * count;   // constant/padding or other report
        __attribute__((fallthrough));         // clear local items
      case 0x90: case 0xB0: case 0xA0: case 0xC0: // output, feature, collection, end
        ucnt = 0; urange = 0;
        break;
      default: break;
    }
  }
}

// ===================================================================================
// Read Bit Field from Report (internal)
// ===================================================================================
static uint16_t GPD_getBits(uint8_t* buf, uint8_t len, uint16_t pos, uint8_t size) {
  uint16_t i = pos >> 3;
  uint32_t v;
  if(!size || i >= len) return 0;
  v = buf[i];
  if(i + 1 < len) v |= (uint32_t)buf[i + 1] << 8;
  if(i + 2 < len) v |= (uint32_t)buf[i + 2] << 16;
  v >>= pos & 7;
  return (uint16_t)(v & ((1UL << size) - 1));
}

// ===================================================================================
// Class Driver Interface
// ===================================================================================

// Match HID interface which is no boot keyboard/mouse (class 3, protocol 0)
uint8_t USBH_CLASS_match(uint8_t* itf) {
  return (itf[5] == 3) && (itf[7] == 0);
}

// Init gamepad: set idle rate, read and parse report descriptor
uint8_t USBH_CLASS_init(void) {
  uint8_t i;
  uint16_t len = USBH_dev.rlen;
  USBH_setup(USB_REQ_TYP_CLASS | USB_REQ_RECIP_INTERF, HID_SET_IDLE, 0, USBH_dev.itf, 0);
  USBH_control(0);                            // optional, may be stalled
  if(!len || len > USBH_BUF_SIZE) len = USBH_BUF_SIZE;
  USBH_setup(USB_REQ_TYP_IN | USB_REQ_RECIP_INTERF, USB_GET_DESCRIPTOR,
             USB_DESCR_TYP_REPORT << 8, USBH_dev.itf, len);
  if(USBH_control(USBH_buffer)) return USBH_ERR_DESCR;
  GPD_parse(len);
  for(i = 0; i < GPD_AXES; i++) GPD_axis[i] = 128;
  GPD_hat = GPD_HAT_CENTER;
  GPD_buttons = 0;
  return (GPD_axisMask || GPD_btnCount) ? USBH_OK : USBH_ERR_DESCR;
}

// ===================================================================================
// Front End Functions
// ===================================================================================

// Handle USB host and poll gamepad
uint8_t GPD_update(void) {
  uint8_t* buf = USBH_RX_buffer;
  GPD_FIELD_t* f;
  uint8_t  len, i;
  uint16_t v;
  USBH_task();
  len = USBH_poll();
  if(!len) return 0;
  if(GPD_rid) {                               // report with ID?
    if(buf[0] != GPD_rid) return 0;           // not our report
    buf++; len--;
  }
  for(i = 0; i < GPD_AXES; i++) {             // axes
    if(!(GPD_axisMask & (1 << i))) continue;
    f = &GPD_field[i];
    v = GPD_getBits(buf, len, f->pos, f->size);
    if(f->sign) v ^= 1 << (f->size - 1);      // signed -> offset binary
    if(f->bits > 8) v >>= f->bits - 8;        // scale to 8 bits
    else if(f->bits < 8) v <<= 8 - f->bits;
    GPD_axis[i] = (v > 255) ? 255 : (uint8_t)v;
  }
  if(GPD_hatMode) {                           // hat switch
    f = &GPD_field[GPD_AXES];
    v = GPD_getBits(buf, len, f->pos, f->size) - f->min;
    if(GPD_hatMode == 2) v <<= 1;             // 4 directions -> 8 directions
    GPD_hat = (v > 7) ? GPD_HAT_CENTER : (uint8_t)v;
  }
  GPD_buttons = GPD_getBits(buf, len, GPD_btnPos, GPD_btnCount); // buttons
  return 1;
}
