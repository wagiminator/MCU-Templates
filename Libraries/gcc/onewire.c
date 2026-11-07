// ===================================================================================
// Basic 1-Wire Bus Functions (Software)                                      * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "onewire.h"

// Pin control (open-drain emulation)
#define OW_low()          do {PIN_low(OW_PIN); PIN_output(OW_PIN);} while(0)
#define OW_release()      PIN_input(OW_PIN)

#if OW_INT_LOCK > 0
  #define OW_lock()       INT_disable()
  #define OW_unlock()     INT_enable()
#else
  #define OW_lock()
  #define OW_unlock()
#endif

// Init 1-Wire pin
void OW_init(void) {
  OW_release();
}

// Send reset pulse, returns 1 if a device answered with a presence pulse
uint8_t OW_reset(void) {
  uint8_t presence;
  OW_low();
  DLY_us(480);                                    // reset pulse
  OW_lock();
  OW_release();
  DLY_us(70);                                     // wait for presence pulse
  presence = !PIN_read(OW_PIN);                   // device pulls line low
  OW_unlock();
  DLY_us(410);                                    // end of presence time slot
  return presence;
}

// Write one bit
void OW_writeBit(uint8_t bit) {
  OW_lock();
  OW_low();
  if(bit) {
    DLY_us(6);                                    // write 1: short low pulse
    OW_release();
    OW_unlock();
    DLY_us(64);
  }
  else {
    DLY_us(60);                                   // write 0: long low pulse
    OW_release();
    OW_unlock();
    DLY_us(10);
  }
}

// Read one bit
uint8_t OW_readBit(void) {
  uint8_t bit;
  OW_lock();
  OW_low();
  DLY_us(3);                                      // start of read time slot
  OW_release();
  DLY_us(10);                                     // wait before sampling
  bit = PIN_read(OW_PIN) ? 1 : 0;                 // sample within 15us
  OW_unlock();
  DLY_us(53);                                     // end of time slot
  return bit;
}

// Write one byte (LSB first)
void OW_write(uint8_t data) {
  uint8_t i;
  for(i = 8; i; i--, data >>= 1) OW_writeBit(data & 1);
}

// Read one byte (LSB first)
uint8_t OW_read(void) {
  uint8_t i, data = 0;
  for(i = 8; i; i--) {
    data >>= 1;
    if(OW_readBit()) data |= 0x80;
  }
  return data;
}

// Calculate 1-Wire CRC8 (polynomial x^8 + x^5 + x^4 + 1), returns 0 if the last byte
// of the data is the valid CRC
uint8_t OW_crc8(const uint8_t* ptr, uint8_t len) {
  uint8_t i, crc = 0;
  while(len--) {
    crc ^= *ptr++;
    for(i = 8; i; i--) crc = (crc & 1) ? (crc >> 1) ^ 0x8C : (crc >> 1);
  }
  return crc;
}
