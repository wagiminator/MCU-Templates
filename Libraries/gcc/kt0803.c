// ===================================================================================
// Basic KT0803 K/L FM Transmitter Functions                                  * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "kt0803.h"

// KT0803 registers 0x00 - 0x02
uint8_t KT_regs[] = { 0x81, 0xC3, 0x41 };

// Update KT0803 registers 0x00 - 0x02
void KT_update(void) {
  uint8_t i;
  I2C_start((KT_I2C_ADDR << 1) | 0);
  I2C_write(0);
  for(i=0; i<3; i++) I2C_write(KT_regs[i]);
  I2C_stop();
}

// Setup KT0803
void KT_init(void) {
  KT_update();
}

// Set frequency (in 100kHz steps, 885 means 88.5Mhz)
void KT_setFreq(uint16_t freq) {
  KT_regs[0] = (uint8_t)freq;
  KT_regs[1] = (KT_regs[1] & 0xf8) | ((freq >> 8) & 0x07);
  KT_update();
}

// Get current frequency (in 100kHz steps, 885 means 88.5Mhz)
uint16_t KT_getFreq(void) {
  return( (((uint16_t)KT_regs[1] & 0x0007) << 8) | KT_regs[0] );
}

// Set gain (0: -12dB, ... , 3: 0dB, ... , 6: +12dB) 
void KT_setGain(uint8_t gain) {
  (gain <= 3) ? (gain = 3 - gain) : (gain++);
  KT_regs[1] = (KT_regs[1] & 0xc7) | ((gain & 0x07) << 3);
  KT_update();
}

// Set mude (0: unmute, 1: mute)
void KT_setMute(uint8_t mute) {
  (mute) ? (KT_regs[2] |= 0x08) : (KT_regs[2] &= 0xf7);
  KT_update();
}

// Set pre-emphasis time-constant depending on region
void KT_setRegion(uint8_t region) {
  (region) ? (KT_regs[2] |= 0x01) : (KT_regs[2] &= 0xfe);
  KT_update();
}
