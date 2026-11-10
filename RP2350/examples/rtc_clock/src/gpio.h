// ===================================================================================
// Basic GPIO Functions for RP2350                                            * v1.0 *
// ===================================================================================
//
// Pins must be defined as GP0, GP1, .., GP29 - e.g.:
// #define PIN_LED GP25     // LED on pin GP25 (Raspberry Pi Pico 2)
//
// PIN functions available:
// ------------------------
// PIN_input(PIN)           Set PIN as INPUT (floating, no pullup/pulldown)
// PIN_input_PU(PIN)        Set PIN as INPUT with internal PULLUP resistor
// PIN_input_PD(PIN)        Set PIN as INPUT with internal PULLDOWN resistor
// PIN_input_AN(PIN)        Set PIN as INPUT for ADC (digital input disabled)
// PIN_output(PIN)          Set PIN as OUTPUT (push-pull)
// PIN_disable(PIN)         Disable PIN (output and input disabled, no function) (*)
//
// PIN_pullup(PIN)          Enable PULLUP resistor on PIN
// PIN_pulldown(PIN)        Enable PULLDOWN resistor on PIN (*)
// PIN_pulloff(PIN)         Disable PULLUP/PULLDOWN resistor on PIN
// PIN_drive(PIN, s)        Set PIN drive strength (0: 2mA, 1: 4mA (*), 2: 8mA, 3: 12mA)
// PIN_slew(PIN, s)         Set PIN slew rate (0: slow (*), 1: fast)
// PIN_alternate(PIN, F)    Set peripheral function F for PIN (GPIO_FUNC_SPI, _UART,
//                          _I2C, _PWM, _PIO0, _PIO1, _PIO2, _GPCK, _USB, _UART_AUX,
//                          _HSTX, see rp2350.h)
// PIN_alternate_off(PIN)   Disable peripheral function for PIN (back to GPIO)
//
// PIN_low(PIN)             Set PIN output value to LOW (*)
// PIN_high(PIN)            Set PIN output value to HIGH
// PIN_toggle(PIN)          TOGGLE PIN output value
// PIN_read(PIN)            Read PIN input value
// PIN_write(PIN, val)      Write PIN output value (0 = LOW / 1 = HIGH)
//
// PIN interrupt functions available:
// ----------------------------------
// PIN_INT_set(PIN,TYPE)    Setup PIN interrupt TYPE:
//                          PIN_INT_OFF, PIN_INT_RISING, PIN_INT_FALLING, PIN_INT_BOTH,
//                          PIN_INT_HIGH, PIN_INT_LOW
// PIN_INT_enable(PIN)      Enable GPIO interrupt (NVIC)
// PIN_INT_disable(PIN)     Disable GPIO interrupt (NVIC)
// PIN_INTFLAG_read(PIN)    Read interrupt flag of PIN
// PIN_INTFLAG_clear(PIN)   Clear interrupt flag of PIN (edge events)
// PIN_INT_ISR { }          Pin interrupt service routine (IO_IRQ_BANK0_IRQHandler)
//
// PORT functions available:
// -------------------------
// PORTS_enable()           Release GPIO bank (IO_BANK0, PADS_BANK0) from reset (*)
// PORTS_disable()          Hold GPIO bank in reset
// PORT_read()              Read input values of all pins (bit n = GPn)
// PORT_write(val)          Write output values of all pins
// PORT_set(mask)           Set output pins in mask HIGH
// PORT_clear(mask)         Set output pins in mask LOW
//
// Analog-to-Digital Converter (ADC) functions available:
// ------------------------------------------------------
// ADC_init()               Init and enable ADC (must be called first)
// ADC_enable()             Enable ADC
// ADC_disable()            Disable ADC (*)
//
// ADC_input(PIN)           Set PIN as ADC input (GP26..GP29 = AIN0..AIN3, range 0..ADC_VREF)
// ADC_input_TEMP()         Set internal temperature sensor as ADC input
//
// ADC_read()               Sample and read 12-bit ADC value (0..4095)
// ADC_read_mV()            Sample and read voltage in millivolts (mV)
// ADC_read_TEMP()          Sample and read temperature sensor in °C
// ADC_read_VSYS()          Sample and read VSYS (Raspberry Pi Pico 2, GP29 = VSYS/3) in mV
//
// Notes:
// ------
// - (*) reset state; GP0..GP29 after SYS_init(): no function (FUNC_NULL), output
//   disabled, input disabled, pull-down enabled, pad isolated (ISO).
// - On the RP2350 all pads are isolated (ISO bit) after reset, i.e. the pad keeps its
//   state independent of the peripherals. The PIN functions clear the ISO bit.
// - Only the 30 GPIOs of the RP2350A (QFN60) are supported here. The RP2350B (QFN80)
//   has 48 GPIOs, GP32..GP47 are accessed via the SIO->GPIO_HI_xxx registers.
// - Raspberry Pi Pico 2: GP23 = SMPS power save (low: PFM, high: PWM, less ripple),
//   GP24 = VBUS sense, GP25 = LED, GP29 = VSYS/3 (ADC3). GP0..GP22 and GP26..GP28 are
//   available on the pin headers.
// - The RP2350 has no open-drain outputs, they can be emulated by switching between
//   PIN_input() and PIN_output() with output value LOW.
// - Erratum RP2350-E9: a GPIO input with enabled pull-down resistor may latch at
//   about 2.2V if it was driven high by an external source. Use an external pull-down
//   (< 8.2kOhm) or disable the input buffer (PIN_disable) when this is an issue.
// - The GPIO interrupts are routed to the core which calls PIN_INT_set().
// - Pins used for ADC must be set with PIN_input_AN beforehand. The ADC reference is
//   ADC_VREF (3.3V on the Raspberry Pi Pico 2). ADC_read_TEMP() uses the typical values
//   of the datasheet (27°C at 0.706V, -1.721mV/°C), accuracy approx. +/- 5°C.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// ===================================================================================
// GPIO Register Access (pad and function via APB, input/output via single-cycle SIO)
// ===================================================================================
#define GPIO_BIT(PIN)         ((uint32_t)1 << (PIN))
#define GPIO_PAD(PIN)         (PADS_BANK0->GPIO[PIN])
#define GPIO_FUNC(PIN, F)     (IO_BANK0->GPIO[PIN].CTRL = (F))

// Pad settings (writing the pad register also clears the isolation bit ISO)
#define GPIO_PAD_IN           (PADS_BANK0_GPIO_IE | PADS_BANK0_GPIO_SCHMITT | ((uint32_t)1 << PADS_BANK0_GPIO_DRIVE_Pos))
#define GPIO_PAD_DRIVE(s)     ((uint32_t)((s) & 3) << PADS_BANK0_GPIO_DRIVE_Pos)

// ===================================================================================
// Set PIN as INPUT (high impedance, no pullup/pulldown)
// ===================================================================================
#define PIN_input(PIN)        ( SIO->GPIO_OE_CLR = GPIO_BIT(PIN),                      \
                                GPIO_PAD(PIN)    = GPIO_PAD_IN,                        \
                                GPIO_FUNC(PIN, GPIO_FUNC_SIO) )
#define PIN_input_HI          PIN_input
#define PIN_input_FL          PIN_input

// ===================================================================================
// Set PIN as INPUT with internal PULLUP resistor
// ===================================================================================
#define PIN_input_PU(PIN)     ( SIO->GPIO_OE_CLR = GPIO_BIT(PIN),                      \
                                GPIO_PAD(PIN)    = GPIO_PAD_IN | PADS_BANK0_GPIO_PUE,  \
                                GPIO_FUNC(PIN, GPIO_FUNC_SIO) )

// ===================================================================================
// Set PIN as INPUT with internal PULLDOWN resistor
// ===================================================================================
#define PIN_input_PD(PIN)     ( SIO->GPIO_OE_CLR = GPIO_BIT(PIN),                      \
                                GPIO_PAD(PIN)    = GPIO_PAD_IN | PADS_BANK0_GPIO_PDE,  \
                                GPIO_FUNC(PIN, GPIO_FUNC_SIO) )

// ===================================================================================
// Set PIN as INPUT for analog peripherals (ADC): digital input and pulls disabled
// ===================================================================================
#define PIN_input_AN(PIN)     ( SIO->GPIO_OE_CLR = GPIO_BIT(PIN),                      \
                                GPIO_PAD(PIN)    = PADS_BANK0_GPIO_OD,                 \
                                GPIO_FUNC(PIN, GPIO_FUNC_NULL) )
#define PIN_input_AD          PIN_input_AN
#define PIN_input_ADC         PIN_input_AN

// ===================================================================================
// Set PIN as OUTPUT PUSH-PULL (input buffer stays enabled for reading back)
// ===================================================================================
#define PIN_output(PIN)       ( GPIO_PAD(PIN)    = GPIO_PAD_IN,                        \
                                GPIO_FUNC(PIN, GPIO_FUNC_SIO),                         \
                                SIO->GPIO_OE_SET = GPIO_BIT(PIN) )
#define PIN_output_PP         PIN_output

// ===================================================================================
// Disable PIN (output and input disabled, no function)
// ===================================================================================
#define PIN_disable(PIN)      ( SIO->GPIO_OE_CLR = GPIO_BIT(PIN),                      \
                                GPIO_FUNC(PIN, GPIO_FUNC_NULL),                        \
                                GPIO_PAD(PIN)    = PADS_BANK0_GPIO_OD )

// ===================================================================================
// Enable PULLUP/PULLDOWN resistor on PIN, disable resistor
// ===================================================================================
#define PIN_pullup(PIN)       REG_WRITE_MASK(GPIO_PAD(PIN), PADS_BANK0_GPIO_PUE,       \
                                PADS_BANK0_GPIO_PUE | PADS_BANK0_GPIO_PDE)
#define PIN_pulldown(PIN)     REG_WRITE_MASK(GPIO_PAD(PIN), PADS_BANK0_GPIO_PDE,       \
                                PADS_BANK0_GPIO_PUE | PADS_BANK0_GPIO_PDE)
#define PIN_pulloff(PIN)      REG_CLR(GPIO_PAD(PIN), PADS_BANK0_GPIO_PUE | PADS_BANK0_GPIO_PDE)

// ===================================================================================
// Set PIN drive strength (0: 2mA, 1: 4mA, 2: 8mA, 3: 12mA) and slew rate (1: fast)
// ===================================================================================
#define PIN_drive(PIN, s)     REG_WRITE_MASK(GPIO_PAD(PIN), GPIO_PAD_DRIVE(s),         \
                                PADS_BANK0_GPIO_DRIVE_Msk)
#define PIN_slew(PIN, s)      ( (s) ? REG_SET(GPIO_PAD(PIN), PADS_BANK0_GPIO_SLEWFAST) \
                                    : REG_CLR(GPIO_PAD(PIN), PADS_BANK0_GPIO_SLEWFAST) )
#define PIN_speed             PIN_slew

// ===================================================================================
// Set peripheral function F for PIN (input enabled, output not disabled)
// ===================================================================================
#define PIN_alternate(PIN, F) ( GPIO_FUNC(PIN, F),                                     \
                                REG_WRITE_MASK(GPIO_PAD(PIN), PADS_BANK0_GPIO_IE,      \
                                PADS_BANK0_GPIO_IE | PADS_BANK0_GPIO_OD | PADS_BANK0_GPIO_ISO) )
#define PIN_alternate_off(PIN) GPIO_FUNC(PIN, GPIO_FUNC_SIO)

// ===================================================================================
// Set PIN output value (LOW, HIGH, TOGGLE, WRITE) and read PIN input value
// ===================================================================================
#define PIN_low(PIN)          ( SIO->GPIO_OUT_CLR = GPIO_BIT(PIN) )
#define PIN_high(PIN)         ( SIO->GPIO_OUT_SET = GPIO_BIT(PIN) )
#define PIN_toggle(PIN)       ( SIO->GPIO_OUT_XOR = GPIO_BIT(PIN) )
#define PIN_read(PIN)         ( (SIO->GPIO_IN >> (PIN)) & 1 )
#define PIN_write(PIN, val)   ( (val) ? (PIN_high(PIN)) : (PIN_low(PIN)) )

// ===================================================================================
// Setup PIN interrupt (IO_BANK0, routed to the calling core)
// ===================================================================================
enum{PIN_INT_OFF, PIN_INT_RISING, PIN_INT_FALLING, PIN_INT_BOTH, PIN_INT_HIGH, PIN_INT_LOW};

// Event bits of PIN (4 bits per pin, 8 pins per register)
#define PIN_EVT_REG(PIN)      ((PIN) >> 3)
#define PIN_EVT_POS(PIN)      (((PIN) & 7) << 2)
#define PIN_EVT_LEVEL_LOW     1
#define PIN_EVT_LEVEL_HIGH    2
#define PIN_EVT_EDGE_LOW      4
#define PIN_EVT_EDGE_HIGH     8

static inline void PIN_INT_set(uint8_t pin, uint8_t type) {
  static const uint8_t evt[] = {0, PIN_EVT_EDGE_HIGH, PIN_EVT_EDGE_LOW,
                                PIN_EVT_EDGE_HIGH | PIN_EVT_EDGE_LOW,
                                PIN_EVT_LEVEL_HIGH, PIN_EVT_LEVEL_LOW};
  volatile uint32_t* inte = &IO_BANK0->PROC[CPU_ID].INTE[PIN_EVT_REG(pin)];
  REG_CLR(*inte, (uint32_t)0x0F << PIN_EVT_POS(pin));         // disable all events
  IO_BANK0->INTR[PIN_EVT_REG(pin)] = (uint32_t)0x0C << PIN_EVT_POS(pin); // clear edges
  if(type < 6) REG_SET(*inte, (uint32_t)evt[type] << PIN_EVT_POS(pin));
}

#define PIN_INT_enable(PIN)     NVIC_EnableIRQ(IO_IRQ_BANK0_IRQn)
#define PIN_INT_disable(PIN)    NVIC_DisableIRQ(IO_IRQ_BANK0_IRQn)

#define PIN_INTFLAG_read(PIN)   ( IO_BANK0->PROC[CPU_ID].INTS[PIN_EVT_REG(PIN)]           \
                                  & ((uint32_t)0x0F << PIN_EVT_POS(PIN)) )
#define PIN_INTFLAG_clear(PIN)  ( IO_BANK0->INTR[PIN_EVT_REG(PIN)]                        \
                                  = ((uint32_t)0x0C << PIN_EVT_POS(PIN)) )

#define PIN_INT_ISR             void IO_IRQ_BANK0_IRQHandler(void)

// ===================================================================================
// PORT Functions (all pins at once, bit n = GPn)
// ===================================================================================
#define PORTS_enable()          RESET_release(RESETS_RESET_IO_BANK0 | RESETS_RESET_PADS_BANK0)
#define PORTS_disable()         RESET_assert(RESETS_RESET_IO_BANK0 | RESETS_RESET_PADS_BANK0)
#define PORT_enable(PIN)        PORTS_enable()
#define PORT_disable(PIN)       PORTS_disable()
#define PORT_read()             (SIO->GPIO_IN)
#define PORT_write(val)         (SIO->GPIO_OUT = (val))
#define PORT_set(mask)          (SIO->GPIO_OUT_SET = (mask))
#define PORT_clear(mask)        (SIO->GPIO_OUT_CLR = (mask))

// ===================================================================================
// ADC Functions
// ===================================================================================
#define ADC_VREF            3300    // ADC reference voltage in mV (ADC_AVDD)
#define ADC_CH_TEMP         4       // temperature sensor channel (RP2350A, RP2350B: 8)

#define ADC_enable()        REG_SET(ADC->CS, ADC_CS_EN)
#define ADC_disable()       REG_CLR(ADC->CS, ADC_CS_EN)
#define ADC_input(PIN)      REG_WRITE_MASK(ADC->CS, (uint32_t)((PIN) - GP26) << ADC_CS_AINSEL_Pos, \
                                          ADC_CS_AINSEL_Msk)
#define ADC_input_TEMP()    REG_WRITE_MASK(ADC->CS, ADC_CS_TS_EN                         \
                                          | ((uint32_t)ADC_CH_TEMP << ADC_CS_AINSEL_Pos),\
                                          ADC_CS_TS_EN | ADC_CS_AINSEL_Msk)

// Init and enable ADC (clk_adc = 48MHz, 500ksps)
static inline void ADC_init(void) {
  RESET_cycle(RESETS_RESET_ADC);
  ADC->CS = ADC_CS_EN;
  while(!(ADC->CS & ADC_CS_READY));
}

// Sample and read ADC value (0..4095)
static inline uint16_t ADC_read(void) {
  REG_SET(ADC->CS, ADC_CS_START_ONCE);
  while(!(ADC->CS & ADC_CS_READY));
  return ADC->RESULT & 0x0FFF;
}

// Sample and read voltage in mV
static inline uint16_t ADC_read_mV(void) {
  return ((uint32_t)ADC_read() * ADC_VREF + 2048) >> 12;
}

// Sample and read temperature sensor in °C
static inline int8_t ADC_read_TEMP(void) {
  int32_t uv;
  ADC_input_TEMP();
  uv = (int32_t)(((uint32_t)ADC_read() * (ADC_VREF * 250)) >> 10);  // voltage in uV
  return (int8_t)(27 - (uv - 706000 + (uv < 706000 ? -860 : 860)) / 1721);
}

// Sample and read VSYS on Raspberry Pi Pico 2 (GP29 = VSYS/3) in mV
static inline uint16_t ADC_read_VSYS(void) {
  PIN_input_AN(GP29);
  ADC_input(GP29);
  return ((uint32_t)ADC_read() * ADC_VREF * 3 + 2048) >> 12;
}

#ifdef __cplusplus
};
#endif
