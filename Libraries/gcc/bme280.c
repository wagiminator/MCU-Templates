// ===================================================================================
// Basic BME280 Temperature, Pressure and Humidity Sensor Functions           * v1.1 *
// ===================================================================================
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#include "bme280.h"

// Calibration data
static uint16_t BME_T1, BME_P1;
static int16_t  BME_T2, BME_T3, BME_P2, BME_P3, BME_P4, BME_P5, BME_P6, BME_P7, BME_P8, BME_P9;
static uint8_t  BME_H1, BME_H3;
static int16_t  BME_H2, BME_H4, BME_H5;
static int8_t   BME_H6;

static int32_t BME_t_fine;                        // fine temperature for compensation
static int32_t BME_t_fine_adjust = 0;             // user temperature adjustment

// Set register pointer and start reading (internal)
static void BME_readStart(uint8_t reg) {
  I2C_start((uint8_t)(BME_ADDR << 1));
  I2C_write(reg);
  I2C_stop();
  I2C_start((uint8_t)((BME_ADDR << 1) | 1));
}

// Write register (internal)
static void BME_writeReg(uint8_t reg, uint8_t value) {
  I2C_start((uint8_t)(BME_ADDR << 1));
  I2C_write(reg);
  I2C_write(value);
  I2C_stop();
}

// Read 16-bit little-endian value (internal)
static uint16_t BME_read16(uint8_t ack) {
  uint16_t v = I2C_read(1);
  return v | ((uint16_t)I2C_read(ack) << 8);
}

// Read 20-bit ADC value (msb, lsb, xlsb[7:4]) (internal)
static int32_t BME_read20(uint8_t reg) {
  uint32_t v;
  BME_readStart(reg);
  v  = (uint32_t)I2C_read(1) << 12;
  v |= (uint32_t)I2C_read(1) << 4;
  v |= I2C_read(0) >> 4;
  I2C_stop();
  return (int32_t)v;
}

// Init BME280
void BME_init(void) {
  #if BME_INIT_I2C > 0
  I2C_init();                                     // initialize I2C first
  #endif

  #if BME_BOOT_TIME > 0
  DLY_ms(BME_BOOT_TIME);                          // time for the sensor to boot up
  #endif

  #if BME_INIT_RESET > 0
  BME_writeReg(BME_REG_SOFTRESET, 0xB6);          // software reset
  {
    uint8_t status;
    do {
      DLY_ms(10);
      BME_readStart(BME_REG_STATUS);
      status = I2C_read(0);
      I2C_stop();
    } while(status & 1);                          // wait for calibration data copied
  }
  #endif

  BME_writeReg(BME_REG_CONTROLHUMID, 0b00000100); // humidity: 8x oversampling
  BME_writeReg(BME_REG_CONTROL,      0b10010011); // pressure/temp: 8x oversampling, normal

  BME_readStart(BME_REG_CALIB_T1);                // read calibration data 0x88..0xA1
  BME_T1 = BME_read16(1); BME_T2 = BME_read16(1); BME_T3 = BME_read16(1);
  BME_P1 = BME_read16(1); BME_P2 = BME_read16(1); BME_P3 = BME_read16(1);
  BME_P4 = BME_read16(1); BME_P5 = BME_read16(1); BME_P6 = BME_read16(1);
  BME_P7 = BME_read16(1); BME_P8 = BME_read16(1); BME_P9 = BME_read16(1);
  I2C_read(1);                                    // 0xA0: not used
  BME_H1 = I2C_read(0);
  I2C_stop();

  BME_readStart(BME_REG_CALIB_H2);                // read calibration data 0xE1..0xE7
  {
    uint8_t e4, e5, e6;
    BME_H2 = BME_read16(1);
    BME_H3 = I2C_read(1);
    e4 = I2C_read(1); e5 = I2C_read(1); e6 = I2C_read(1);
    BME_H6 = (int8_t)I2C_read(0);
    I2C_stop();
    BME_H4 = ((int16_t)(int8_t)e4 << 4) | (e5 & 0x0F);
    BME_H5 = ((int16_t)(int8_t)e6 << 4) | (e5 >> 4);
  }

  #if BME_INIT_TEMP > 0
  BME_getTemp();
  #endif
}

// Returns temperature in DegC, resolution is 0.01 DegC.
// Output value of "5123" equals 51.23 DegC.
int32_t BME_getTemp(void) {
  int32_t adc_T = BME_read20(BME_REG_TEMPDATA);
  int32_t var1, var2;
  var1 = ((adc_T >> 3) - ((int32_t)BME_T1 << 1));
  var1 = (var1 * (int32_t)BME_T2) >> 11;
  var2 = ((adc_T >> 4) - (int32_t)BME_T1);
  var2 = (((var2 * var2) >> 12) * (int32_t)BME_T3) >> 14;
  BME_t_fine = var1 + var2 + BME_t_fine_adjust;
  return (BME_t_fine * 5 + 128) >> 8;
}

// Returns pressure in Pa as unsigned 32 bit integer.
// Output value of "96386" equals 96386 Pa = 963.86 hPa.
uint32_t BME_getPressure(void) {
  int32_t adc_P = BME_read20(BME_REG_PRESSUREDATA);
  int32_t var1, var2;
  uint32_t p;
  var1 = (BME_t_fine >> 1) - 64000;
  var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * (int32_t)BME_P6;
  var2 = var2 + ((var1 * (int32_t)BME_P5) << 1);
  var2 = (var2 >> 2) + ((int32_t)BME_P4 << 16);
  var1 = ((((int32_t)BME_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3)
         + (((int32_t)BME_P2 * var1) >> 1)) >> 18;
  var1 = ((32768 + var1) * (int32_t)BME_P1) >> 15;
  if(var1 == 0) return 0;                         // avoid division by zero
  p = ((uint32_t)(1048576 - adc_P) - (uint32_t)(var2 >> 12)) * 3125;
  if(p < 0x80000000) p = (p << 1) / (uint32_t)var1;
  else p = (p / (uint32_t)var1) * 2;
  var1 = ((int32_t)BME_P9 * (int32_t)(((p >> 3) * (p >> 3)) >> 13)) >> 12;
  var2 = ((int32_t)(p >> 2) * (int32_t)BME_P8) >> 13;
  p = (uint32_t)((int32_t)p + ((var1 + var2 + BME_P7) >> 4));
  return p;
}

// Humidity in %RH, resolution is 0.01%RH.
// Output value of "4653" represents 46.53 %RH.
uint32_t BME_getHumidity(void) {
  int32_t adc_H, var1;
  BME_readStart(BME_REG_HUMIDDATA);
  adc_H  = (int32_t)I2C_read(1) << 8;
  adc_H |= I2C_read(0);
  I2C_stop();

  var1 = BME_t_fine - 76800;
  var1 = (((((adc_H << 14) - ((int32_t)BME_H4 << 20) - ((int32_t)BME_H5 * var1))
         + 16384) >> 15) * (((((((var1 * (int32_t)BME_H6) >> 10)
         * (((var1 * (int32_t)BME_H3) >> 11) + 32768)) >> 10)
         + 2097152) * (int32_t)BME_H2 + 8192) >> 14));
  var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * (int32_t)BME_H1) >> 4));
  if(var1 < 0) var1 = 0;
  if(var1 > 419430400) var1 = 419430400;
  return (((uint32_t)var1 >> 12) * 25) >> 8;
}

// Put device into sleep mode (1: sleep, 0: wake up)
void BME_sleep(uint8_t slp) {
  BME_writeReg(BME_REG_CONTROL, slp ? 0 : 0b10010011);
}

// Set user temperature adjustment ("-127" means -1.27 DegC)
void BME_adjustTemp(int32_t adjust) {
  BME_t_fine_adjust = (adjust << 8) / 5;
}
