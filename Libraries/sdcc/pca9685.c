// ===================================================================================
// Basic PCA9685 16-Channel 12-Bit PWM/Servo Driver Functions                 * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "pca9685.h"

// PCA9685 registers
#define PCA_REG_MODE1         0x00    // mode register 1
#define PCA_REG_MODE2         0x01    // mode register 2
#define PCA_REG_LED0          0x06    // LED0_ON_L (4 registers per channel)
#define PCA_REG_PRESCALE      0xFE    // prescaler for PWM frequency

// Prescaler value = round(25MHz / (4096 * f)) - 1
#define PCA_PRESCALE          ((25000000UL + 2048UL * PCA_FREQ) / (4096UL * PCA_FREQ) - 1)

// Write register (internal)
static void PCA_writeReg(uint8_t reg, uint8_t value) {
  I2C_start((uint8_t)(PCA_ADDR << 1));
  I2C_write(reg);
  I2C_write(value);
  I2C_stop();
}

// Init driver
void PCA_init(void) {
  #if PCA_INIT_I2C > 0
  I2C_init();
  #endif
  PCA_writeReg(PCA_REG_MODE1, 0x10);              // sleep (required to set prescaler)
  PCA_writeReg(PCA_REG_PRESCALE, PCA_PRESCALE);   // set PWM frequency
  PCA_writeReg(PCA_REG_MODE2, 0x04);              // totem-pole outputs
  PCA_sleep(0);                                   // wake up with auto-increment
}

// Set sleep state (0: wake up, 1: sleep)
void PCA_sleep(uint8_t slp) {
  if(slp) PCA_writeReg(PCA_REG_MODE1, 0x30);      // sleep, auto-increment
  else {
    PCA_writeReg(PCA_REG_MODE1, 0x20);            // wake up, auto-increment
    DLY_us(500);                                  // oscillator start-up time
    PCA_writeReg(PCA_REG_MODE1, 0xA0);            // restart PWM channels
  }
}

// Set duty cycle of channel (0: off, 4096: on)
void PCA_setPWM(uint8_t ch, uint16_t value) {
  I2C_start((uint8_t)(PCA_ADDR << 1));
  I2C_write(PCA_REG_LED0 + ((ch & 15) << 2));
  if(value >= 4096) {                             // full on
    I2C_write(0x00); I2C_write(0x10);
    I2C_write(0x00); I2C_write(0x00);
  }
  else if(value == 0) {                           // full off
    I2C_write(0x00); I2C_write(0x00);
    I2C_write(0x00); I2C_write(0x10);
  }
  else {                                          // on at 0, off at value
    I2C_write(0x00); I2C_write(0x00);
    I2C_write(value); I2C_write(value >> 8);
  }
  I2C_stop();
}

// Set pulse width of channel in microseconds
void PCA_setPulse(uint8_t ch, uint16_t us) {
  PCA_setPWM(ch, ((uint32_t)us * (4096UL * PCA_FREQ) + 500000UL) / 1000000UL);
}
