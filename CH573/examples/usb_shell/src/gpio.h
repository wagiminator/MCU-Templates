// ===================================================================================
// Basic GPIO, ADC and TouchKey Functions for CH573, CH571                    * v1.0 *
// ===================================================================================
//
// Pins must be defined as PA0, PA1, .., PB0, PB1, .. - e.g.:
// #define PIN_LED PA8      // LED on pin PA8
//
// PIN functions available:
// ------------------------
// PIN_input(PIN)           set PIN as INPUT (floating, no pullup/pulldown) (*)
// PIN_input_PU(PIN)        set PIN as INPUT with internal PULLUP resistor
// PIN_input_PD(PIN)        set PIN as INPUT with internal PULLDOWN resistor
// PIN_input_AN(PIN)        set PIN as INPUT for analog peripherals (e.g. ADC)
// PIN_output(PIN)          set PIN as OUTPUT (push-pull, 5mA)
// PIN_output_HD(PIN)       set PIN as OUTPUT (push-pull, high drive 20mA)
//
// PIN_low(PIN)             set PIN output value to LOW (*)
// PIN_high(PIN)            set PIN output value to HIGH
// PIN_toggle(PIN)          TOGGLE PIN output value
// PIN_read(PIN)            read PIN input value
// PIN_write(PIN, val)      write PIN output value (0 = LOW / 1 = HIGH)
//
// PIN interrupt functions available:
// ----------------------------------
// PIN_INT_set(PIN, TYPE)   setup PIN interrupt TYPE: PIN_INT_OFF, PIN_INT_RISING,
//                          PIN_INT_FALLING, PIN_INT_HIGH, PIN_INT_LOW
//                          (PIN must be set as input before)
// PIN_INT_enable(PIN)      enable interrupt vector of PIN's port (A or B)
// PIN_INT_disable(PIN)     disable interrupt vector of PIN's port
// PIN_INTFLAG_read(PIN)    read interrupt flag of PIN
// PIN_INTFLAG_clear(PIN)   clear interrupt flag of PIN
// PINA_INT_ISR { }         pin interrupt service routine for port A
// PINB_INT_ISR { }         pin interrupt service routine for port B
//
// Analog-to-Digital Converter (ADC) functions available:
// ------------------------------------------------------
// ADC_init()               init and enable ADC (must be called first)
// ADC_enable()             enable ADC (power-up)
// ADC_disable()            disable ADC (power-down) (*)
// ADC_input(PIN)           set PIN as ADC input (AIN0..AIN5, AIN8, AIN9, AIN12, AIN13)
// ADC_gain_1_4()           set PGA gain 1/4 (0V .. VDD)
// ADC_gain_1_2()           set PGA gain 1/2 (0V .. 3.15V)
// ADC_gain_1()             set PGA gain 1   (0V .. 2.1V) (*)
// ADC_gain_2()             set PGA gain 2   (0.525V .. 1.575V)
//
// ADC_read()               sample and read 12-bit ADC value (0..4095)
// ADC_read_mV()            sample and read voltage in millivolts (considers gain)
// ADC_read_VDD()           sample and read supply voltage (VDD) in millivolts (mV)
// ADC_read_TEMP()          sample and read chip temperature in degrees Celsius
// ADC_read_TEMP_raw()      sample and read raw value of temperature sensor
//
// TouchKey (TOUCH) functions available:
// -------------------------------------
// TOUCH_init()             init ADC for TouchKey
// TOUCH_read(PIN)          sample TouchKey PIN (ADC pin), lower value when touched
//
// Notes:
// ------
// - Pins on the CH573F: PA4, PA5, PA8 - PA15, PB4, PB7, PB10 - PB15, PB22, PB23.
// - Interrupts are triggered by the selected edge or level (no "both edges").
// - The ADC uses the internal reference VINTA (~1.05V), the range depends on gain.
// - TouchKey and the temperature sensor change the ADC settings, call ADC_init()
//   again before using normal ADC conversions.
// - (*) default state
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// ===================================================================================
// Enumerate PIN designators (use these designators to define pins)
// ===================================================================================
enum{ PA0,  PA1,  PA2,  PA3,  PA4,  PA5,  PA6,  PA7,  PA8,  PA9,  PA10, PA11, PA12, PA13, PA14, PA15,
      PA16, PA17, PA18, PA19, PA20, PA21, PA22, PA23, PA24, PA25, PA26, PA27, PA28, PA29, PA30, PA31,
      PB0,  PB1,  PB2,  PB3,  PB4,  PB5,  PB6,  PB7,  PB8,  PB9,  PB10, PB11, PB12, PB13, PB14, PB15,
      PB16, PB17, PB18, PB19, PB20, PB21, PB22, PB23, PIN_NONE };

// ===================================================================================
// Helper Defines (register of PIN's port, bit mask of PIN)
// ===================================================================================
#define PIN_PORT_BASE(PIN)    ((PIN) & 32 ? 0x400010C0 : 0x400010A0)
#define PIN_REG(PIN, OFS)     (*(volatile uint32_t*)(PIN_PORT_BASE(PIN) + (OFS)))
#define PIN_BIT(PIN)          ((uint32_t)1 << ((PIN) & 31))

#define PIN_DIR(PIN)          PIN_REG(PIN, GPIO_DIR)
#define PIN_IN(PIN)           PIN_REG(PIN, GPIO_PIN)
#define PIN_OUT(PIN)          PIN_REG(PIN, GPIO_OUT)
#define PIN_CLR(PIN)          PIN_REG(PIN, GPIO_CLR)
#define PIN_PU(PIN)           PIN_REG(PIN, GPIO_PU)
#define PIN_PD(PIN)           PIN_REG(PIN, GPIO_PD_DRV)

// ===================================================================================
// Set PIN to INPUT/OUTPUT mode
// ===================================================================================
#define PIN_input(PIN)        {PIN_PD(PIN) &= ~PIN_BIT(PIN); PIN_PU(PIN)  &= ~PIN_BIT(PIN); \
                               PIN_DIR(PIN) &= ~PIN_BIT(PIN);}
#define PIN_input_PU(PIN)     {PIN_PD(PIN) &= ~PIN_BIT(PIN); PIN_PU(PIN)  |=  PIN_BIT(PIN); \
                               PIN_DIR(PIN) &= ~PIN_BIT(PIN);}
#define PIN_input_PD(PIN)     {PIN_PD(PIN) |=  PIN_BIT(PIN); PIN_PU(PIN)  &= ~PIN_BIT(PIN); \
                               PIN_DIR(PIN) &= ~PIN_BIT(PIN);}
#define PIN_input_AN(PIN)     {PIN_input(PIN); R16_PIN_ANALOG_IE |= ADC_PIN_IE(PIN);}
#define PIN_output(PIN)       {PIN_PD(PIN) &= ~PIN_BIT(PIN); PIN_DIR(PIN) |=  PIN_BIT(PIN);}
#define PIN_output_HD(PIN)    {PIN_PD(PIN) |=  PIN_BIT(PIN); PIN_DIR(PIN) |=  PIN_BIT(PIN);}

// ===================================================================================
// Set/Clear/Toggle/Read PIN
// ===================================================================================
#define PIN_low(PIN)          PIN_CLR(PIN) = PIN_BIT(PIN)
#define PIN_high(PIN)         PIN_OUT(PIN) |= PIN_BIT(PIN)
#define PIN_toggle(PIN)       PIN_OUT(PIN) ^= PIN_BIT(PIN)
#define PIN_read(PIN)         ((PIN_IN(PIN) & PIN_BIT(PIN)) != 0)
#define PIN_write(PIN, val)   ((val) ? (PIN_high(PIN)) : (PIN_low(PIN)))

// ===================================================================================
// Pin Interrupt Functions
// ===================================================================================
#define PIN_INT_OFF           0
#define PIN_INT_RISING        1
#define PIN_INT_FALLING       2
#define PIN_INT_HIGH          3
#define PIN_INT_LOW           4

// Interrupt bit of PIN (PB22 and PB23 use bits 8 and 9 of the interrupt registers)
#define PIN_INT_BIT(PIN)      ((uint16_t)((((PIN) == PB22) || ((PIN) == PB23)) ? \
                               (1 << (((PIN) & 31) - 14)) : (1 << ((PIN) & 15))))
#define PIN_INT_EN(PIN)       (*(volatile uint16_t*)((PIN) & 32 ? 0x40001092 : 0x40001090))
#define PIN_INT_MODE(PIN)     (*(volatile uint16_t*)((PIN) & 32 ? 0x40001096 : 0x40001094))
#define PIN_INT_IF(PIN)       (*(volatile uint16_t*)((PIN) & 32 ? 0x4000109E : 0x4000109C))

#define PIN_INT_set(PIN, TYPE) \
  ((TYPE) == PIN_INT_RISING  ? ({PIN_INT_MODE(PIN) |=  PIN_INT_BIT(PIN); PIN_OUT(PIN) |= PIN_BIT(PIN); \
                                 PIN_INT_IF(PIN) = PIN_INT_BIT(PIN); PIN_INT_EN(PIN) |=  PIN_INT_BIT(PIN);}) : \
  ((TYPE) == PIN_INT_FALLING ? ({PIN_INT_MODE(PIN) |=  PIN_INT_BIT(PIN); PIN_CLR(PIN) = PIN_BIT(PIN);  \
                                 PIN_INT_IF(PIN) = PIN_INT_BIT(PIN); PIN_INT_EN(PIN) |=  PIN_INT_BIT(PIN);}) : \
  ((TYPE) == PIN_INT_HIGH    ? ({PIN_INT_MODE(PIN) &= ~PIN_INT_BIT(PIN); PIN_OUT(PIN) |= PIN_BIT(PIN); \
                                 PIN_INT_IF(PIN) = PIN_INT_BIT(PIN); PIN_INT_EN(PIN) |=  PIN_INT_BIT(PIN);}) : \
  ((TYPE) == PIN_INT_LOW     ? ({PIN_INT_MODE(PIN) &= ~PIN_INT_BIT(PIN); PIN_CLR(PIN) = PIN_BIT(PIN);  \
                                 PIN_INT_IF(PIN) = PIN_INT_BIT(PIN); PIN_INT_EN(PIN) |=  PIN_INT_BIT(PIN);}) : \
                               ({PIN_INT_EN(PIN) &= ~PIN_INT_BIT(PIN);})))))

#define PIN_INT_enable(PIN)   NVIC_EnableIRQ((PIN) & 32 ? GPIO_B_IRQn : GPIO_A_IRQn)
#define PIN_INT_disable(PIN)  NVIC_DisableIRQ((PIN) & 32 ? GPIO_B_IRQn : GPIO_A_IRQn)
#define PIN_INTFLAG_read(PIN) (PIN_INT_IF(PIN) & PIN_INT_BIT(PIN))
#define PIN_INTFLAG_clear(PIN) PIN_INT_IF(PIN) = PIN_INT_BIT(PIN)

#define PINA_INT_ISR          void GPIOA_IRQHandler(void) __attribute__((interrupt)); \
                              void GPIOA_IRQHandler(void)
#define PINB_INT_ISR          void GPIOB_IRQHandler(void) __attribute__((interrupt)); \
                              void GPIOB_IRQHandler(void)

// ===================================================================================
// ADC Functions
// ===================================================================================
// ADC channel of PIN
#define ADC_CHANNEL(PIN) \
  ( (PIN) == PA4  ?  0 : (PIN) == PA5  ?  1 : (PIN) == PA12 ?  2 : (PIN) == PA13 ?  3 : \
    (PIN) == PA14 ?  4 : (PIN) == PA15 ?  5 : (PIN) == PB0  ?  8 : (PIN) == PB6  ?  9 : \
    (PIN) == PA8  ? 12 : (PIN) == PA9  ? 13 : 7 )

// Digital input disable bit of ADC PIN
#define ADC_PIN_IE(PIN) \
  ( (PIN) == PA4  ? RB_PIN_ADC0_IE   : (PIN) == PA5  ? RB_PIN_ADC1_IE   : \
    (PIN) == PA12 ? RB_PIN_ADC2_3_IE : (PIN) == PA13 ? RB_PIN_ADC2_3_IE : \
    (PIN) == PA14 ? RB_PIN_ADC4_5_IE : (PIN) == PA15 ? RB_PIN_ADC4_5_IE : \
    (PIN) == PB0  ? RB_PIN_ADC8_9_IE : (PIN) == PB6  ? RB_PIN_ADC8_9_IE : \
    (PIN) == PA8  ? RB_PIN_ADC12_IE  : (PIN) == PA9  ? RB_PIN_ADC13_IE  : 0 )

#define ADC_CH_VBAT           14                  // internal channel: supply voltage
#define ADC_CH_TEMP           15                  // internal channel: temperature sensor
#define ADC_VREF              1050                // internal reference voltage in mV

#define ADC_init()            {R8_TKEY_CFG &= ~RB_TKEY_PWR_ON; R8_TEM_SENSOR = 0; \
                               R8_ADC_CFG = RB_ADC_POWER_ON | RB_ADC_BUF_EN | (0 << 6) | (2 << 4);}
#define ADC_enable()          R8_ADC_CFG |=  RB_ADC_POWER_ON
#define ADC_disable()         R8_ADC_CFG &= ~RB_ADC_POWER_ON
#define ADC_input(PIN)        {PIN_input_AN(PIN); R8_ADC_CHANNEL = ADC_CHANNEL(PIN);}

#define ADC_gain_1_4()        R8_ADC_CFG = (R8_ADC_CFG & ~RB_ADC_PGA_GAIN) | (0 << 4)
#define ADC_gain_1_2()        R8_ADC_CFG = (R8_ADC_CFG & ~RB_ADC_PGA_GAIN) | (1 << 4)
#define ADC_gain_1()          R8_ADC_CFG = (R8_ADC_CFG & ~RB_ADC_PGA_GAIN) | (2 << 4)
#define ADC_gain_2()          R8_ADC_CFG = (R8_ADC_CFG & ~RB_ADC_PGA_GAIN) | (3 << 4)

// Sample and read 12-bit ADC value
static inline uint16_t ADC_read(void) {
  R8_ADC_CONVERT = RB_ADC_START;
  while(R8_ADC_CONVERT & RB_ADC_START);
  return R16_ADC_DATA & RB_ADC_DATA;
}

// Sample and read voltage in millivolts (single-ended, considers PGA gain)
static inline int32_t ADC_read_mV(void) {
  int32_t val = ADC_read();
  switch((R8_ADC_CFG & RB_ADC_PGA_GAIN) >> 4) {
    case 0:  return (val * ADC_VREF + 256) / 512 - 3 * ADC_VREF;     // (ADC/512-3)*Vref
    case 1:  return (val * ADC_VREF + 512) / 1024 - ADC_VREF;        // (ADC/1024-1)*Vref
    case 2:  return (val * ADC_VREF + 1024) / 2048;                  // (ADC/2048)*Vref
    default: return (val * ADC_VREF + 2048) / 4096 + ADC_VREF / 2;   // (ADC/4096+0.5)*Vref
  }
}

// Sample and read supply voltage (VDD/VBAT) in millivolts
static inline uint16_t ADC_read_VDD(void) {
  uint8_t cfg = R8_ADC_CFG;
  uint8_t ch  = R8_ADC_CHANNEL;
  int32_t val;
  R8_ADC_CHANNEL = ADC_CH_VBAT;
  R8_ADC_CFG = RB_ADC_POWER_ON | RB_ADC_BUF_EN | (0 << 6) | (0 << 4);  // gain 1/4
  ADC_read();                                     // discard first value
  val = ADC_read_mV();
  R8_ADC_CHANNEL = ch;
  R8_ADC_CFG = cfg;
  return (val < 0) ? 0 : (uint16_t)val;
}

// Sample and read raw value of temperature sensor
static inline uint16_t ADC_read_TEMP_raw(void) {
  uint8_t  cfg = R8_ADC_CFG;
  uint8_t  ch  = R8_ADC_CHANNEL;
  uint8_t  tk  = R8_TKEY_CFG;
  uint8_t  ts  = R8_TEM_SENSOR;
  uint16_t val;
  R8_TKEY_CFG   &= ~RB_TKEY_PWR_ON;
  R8_TEM_SENSOR  = RB_TEM_SEN_PWR_ON;
  R8_ADC_CHANNEL = ADC_CH_TEMP;
  R8_ADC_CFG     = RB_ADC_POWER_ON | RB_ADC_DIFF_EN | (0 << 6) | (3 << 4);
  ADC_read();                                     // discard first value
  val = ADC_read();
  R8_TEM_SENSOR  = ts;
  R8_ADC_CHANNEL = ch;
  R8_ADC_CFG     = cfg;
  R8_TKEY_CFG    = tk;
  return val;
}

// Sample and read chip temperature in degrees Celsius (uses factory calibration)
static inline int16_t ADC_read_TEMP(void) {
  uint32_t c25  = ESIG_TEMP25;
  int16_t  tref = (c25 >> 16) ? (int16_t)(c25 >> 16) : 25;
  return tref + ((int32_t)ADC_read_TEMP_raw() - (int32_t)(c25 & 0xFFFF)) * 10 / 27;
}

// ===================================================================================
// TouchKey Functions
// ===================================================================================
#define TOUCH_CHARGE          0x10                // charge time (0..31)
#define TOUCH_DISCHARGE       0                   // discharge time (0..7)

#define TOUCH_init()          {R8_TEM_SENSOR = 0; \
                               R8_ADC_CFG = RB_ADC_POWER_ON | RB_ADC_BUF_EN | (2 << 4); \
                               R8_TKEY_CFG |= RB_TKEY_PWR_ON;}

// Sample TouchKey PIN (value decreases when touched)
#define TOUCH_read(PIN)       ({PIN_input_AN(PIN); R8_ADC_CHANNEL = ADC_CHANNEL(PIN); TOUCH_sample();})

static inline uint16_t TOUCH_sample(void) {
  R8_TKEY_COUNT   = (TOUCH_DISCHARGE << 5) | (TOUCH_CHARGE & 0x1F);
  R8_TKEY_CONVERT = RB_TKEY_START;
  while(R8_TKEY_CONVERT & RB_TKEY_START);
  return R16_ADC_DATA & RB_ADC_DATA;
}

#ifdef __cplusplus
};
#endif
