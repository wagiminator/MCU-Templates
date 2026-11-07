// ===================================================================================
// Basic MPU-6050 Accelerometer and Gyroscope Functions                       * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "mpu6050.h"

// IMU registers
#define IMU_REG_CONFIG        0x1A    // DLPF configuration
#define IMU_REG_GYRO_CONFIG   0x1B    // gyroscope range
#define IMU_REG_ACCEL_CONFIG  0x1C    // accelerometer range
#define IMU_REG_DATA          0x3B    // first data register (ACCEL_XOUT_H)
#define IMU_REG_PWR_MGMT_1    0x6B    // power management

// IMU variables
int16_t IMU_accel[3];                 // acceleration X, Y, Z
int16_t IMU_gyro[3];                  // angular rate X, Y, Z
int16_t IMU_temp;                     // raw temperature

// Write register (internal)
static void IMU_writeReg(uint8_t reg, uint8_t value) {
  I2C_start((uint8_t)(IMU_ADDR << 1));
  I2C_write(reg);
  I2C_write(value);
  I2C_stop();
}

// Read 16-bit big-endian value (internal)
static int16_t IMU_read16(uint8_t ack) {
  uint16_t v = (uint16_t)I2C_read(1) << 8;
  v |= I2C_read(ack);
  return (int16_t)v;
}

// Init sensor
void IMU_init(void) {
  #if IMU_INIT_I2C > 0
  I2C_init();
  #endif
  DLY_ms(100);                                    // start-up time
  IMU_writeReg(IMU_REG_PWR_MGMT_1, 0x01);         // wake up, clock from gyro X PLL
  IMU_writeReg(IMU_REG_CONFIG, IMU_DLPF);         // digital low-pass filter
  IMU_writeReg(IMU_REG_GYRO_CONFIG,  IMU_GYRO_RANGE  << 3);
  IMU_writeReg(IMU_REG_ACCEL_CONFIG, IMU_ACCEL_RANGE << 3);
}

// Read all sensor values
void IMU_read(void) {
  uint8_t i;
  I2C_start((uint8_t)(IMU_ADDR << 1));
  I2C_write(IMU_REG_DATA);
  I2C_stop();
  I2C_start((uint8_t)((IMU_ADDR << 1) | 1));
  for(i = 0; i < 3; i++) IMU_accel[i] = IMU_read16(1);
  IMU_temp = IMU_read16(1);
  for(i = 0; i < 3; i++) IMU_gyro[i] = IMU_read16(i < 2);
  I2C_stop();
}

// Set sleep state (0: wake up, 1: sleep)
void IMU_sleep(uint8_t slp) {
  IMU_writeReg(IMU_REG_PWR_MGMT_1, slp ? 0x41 : 0x01);
}
