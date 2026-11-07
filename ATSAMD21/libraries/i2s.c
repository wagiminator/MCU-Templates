// ===================================================================================
// Basic I2S Functions (TX, master) for SAMD21                                * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "i2s.h"
#include "gpio.h"

#define I2S_MASK      ((I2S_BITS == 32) ? 0xffffffff : 0x0000ffff)

// Init I2S master transmitter
void I2S_init(void) {
  // Set GPIO pins (peripheral function G)
  #ifdef I2S_SCK_PIN
  PIN_output(I2S_SCK_PIN); PIN_alternate(I2S_SCK_PIN, 6);
  PIN_output(I2S_FS_PIN);  PIN_alternate(I2S_FS_PIN,  6);
  PIN_output(I2S_SD_PIN);  PIN_alternate(I2S_SD_PIN,  6);
  #else
    #warning No automatic pin mapping for I2S
  #endif

  // Enable I2S clocks (GCLK5 = DFLL48M / I2S_DIV)
  PM->APBCMASK.reg |= PM_APBCMASK_I2S;            // enable I2S module clock
  GCLK_setGEN(GCLK_USER, GCLK_SOURCE_DFLL48M, I2S_DIV);
  GCLK_setCH(I2S_GCLK_ID_0, GCLK_USER);           // GCLK5 -> I2S clock unit 0

  // Setup I2S clock unit 0 and serializer 0
  I2S->CTRLA.reg = I2S_CTRLA_SWRST;               // reset I2S
  while(I2S->SYNCBUSY.reg & I2S_SYNCBUSY_SWRST);
  I2S->CLKCTRL[0].reg = (I2S_BITS == 32 ? I2S_CLKCTRL_SLOTSIZE_32 : I2S_CLKCTRL_SLOTSIZE_16)
                      | I2S_CLKCTRL_NBSLOTS(1)    // 2 slots (stereo)
                      | I2S_CLKCTRL_FSWIDTH_HALF  // frame sync: half frame
                      | I2S_CLKCTRL_BITDELAY_I2S  // I2S format (1 bit delay)
                      | I2S_CLKCTRL_MCKDIV(0);    // SCK = GCLK
  I2S->SERCTRL[0].reg = I2S_SERCTRL_SERMODE_TX    // transmitter
                      | (I2S_BITS == 32 ? I2S_SERCTRL_DATASIZE_32 : I2S_SERCTRL_DATASIZE_16);
  I2S_enable();                                   // enable I2S
}

// Enable I2S (clock unit 0, serializer 0)
void I2S_enable(void) {
  I2S->CTRLA.reg = I2S_CTRLA_ENABLE | I2S_CTRLA_CKEN0 | I2S_CTRLA_SEREN0;
  I2S_sync();
}

// Disable I2S
void I2S_disable(void) {
  I2S->CTRLA.reg = 0;
  I2S_sync();
}

// Write one stereo sample (left, right)
#if I2S_BITS == 32
void I2S_write(int32_t left, int32_t right) {
#else
void I2S_write(int16_t left, int16_t right) {
#endif
  while(!I2S_ready());                            // wait for transmit ready
  while(I2S->SYNCBUSY.reg & I2S_SYNCBUSY_DATA0);
  I2S->DATA[0].reg = (uint32_t)left  & I2S_MASK;  // left channel (slot 0)
  while(!I2S_ready());
  while(I2S->SYNCBUSY.reg & I2S_SYNCBUSY_DATA0);
  I2S->DATA[0].reg = (uint32_t)right & I2S_MASK;  // right channel (slot 1)
}
