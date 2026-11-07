// ===================================================================================
// Basic ADS1115/ADS1015 16/12-Bit ADC Functions                              * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "ads1115.h"

// ADS registers
#define ADS_REG_CONV          0x00    // conversion register
#define ADS_REG_CONFIG        0x01    // configuration register

// Full-scale range in mV
#if   ADS_PGA == 0
  #define ADS_FSR             6144
#elif ADS_PGA == 1
  #define ADS_FSR             4096
#elif ADS_PGA == 2
  #define ADS_FSR             2048
#elif ADS_PGA == 3
  #define ADS_FSR             1024
#elif ADS_PGA == 4
  #define ADS_FSR             512
#else
  #define ADS_FSR             256
#endif

// Read 16-bit register (internal)
static uint16_t ADS_readReg(uint8_t reg) {
  uint16_t value;
  I2C_start((uint8_t)(ADS_ADDR << 1));
  I2C_write(reg);
  I2C_stop();
  I2C_start((uint8_t)((ADS_ADDR << 1) | 1));
  value  = (uint16_t)I2C_read(1) << 8;
  value |= I2C_read(0);
  I2C_stop();
  return value;
}

// Init I2C
void ADS_init(void) {
  #if ADS_INIT_I2C > 0
  I2C_init();
  #endif
}

// Single-shot conversion of input ch, returns raw value
int16_t ADS_read(uint8_t ch) {
  uint8_t timeout = 20;
  I2C_start((uint8_t)(ADS_ADDR << 1));            // write config register:
  I2C_write(ADS_REG_CONFIG);
  I2C_write(0x81 | ((ch & 7) << 4) | (ADS_PGA << 1)); // start, MUX, PGA, single-shot
  I2C_write(0x83);                                // 128SPS (ADS1015: 1600SPS), no comp.
  I2C_stop();
  do {
    DLY_ms(1);                                    // wait for conversion
  } while(!(ADS_readReg(ADS_REG_CONFIG) & 0x8000) && --timeout);
  return (int16_t)ADS_readReg(ADS_REG_CONV);
}

// Single-shot conversion of input ch, returns millivolts
int16_t ADS_readMV(uint8_t ch) {
  return (int16_t)(((int32_t)ADS_read(ch) * ADS_FSR) >> 15);
}
