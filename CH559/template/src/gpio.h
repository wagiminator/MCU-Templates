// ===================================================================================
// Basic GPIO, PWM and ADC Functions for CH559                                * v1.0 *
// ===================================================================================
//
// Pins must be defined as P10, P11, P12, etc. - e.g.:
// #define PIN_LED P33      // LED on pin P3.3
//
// Functions available:
// --------------------
// PIN_input(PIN)           set PIN as INPUT (high impedance, no pullup, P0-P4)
// PIN_input_PU(PIN)        set PIN as INPUT with internal PULLUP (P0-P4)
// PIN_input_AN(PIN)        set PIN as INPUT for ADC (P1 only)
// PIN_output(PIN)          set PIN as OUTPUT (push-pull, P0-P4)
//
// PIN_output_OD(PIN)       set PIN as OUTPUT (open-drain, also INPUT, P0-P3)
// PIN_output_PU_OD(PIN)    set PIN as OUTPUT (open-drain, pullup, also INPUT, P0-P3)
// PIN_output_BI(PIN)       set PIN as 8051 bi-directional (open-drain, pullup, P0-P3)
//
// PIN_current_low(PIN)     set PIN output current drive capabilty to 5mA (default)
// PIN_current_high(PIN)    set PIN output current to 20mA for P0, P2, P3 / 10mA for P1
//
// PIN_low(PIN)             set PIN output value to LOW
// PIN_high(PIN)            set PIN output value to HIGH
// PIN_toggle(PIN)          TOGGLE PIN output value
// PIN_read(PIN)            read PIN input value
// PIN_write(PIN, val)      write PIN output value (0 = LOW / 1 = HIGH)
//
// PIN_asm(PIN)             convert PIN for inline assembly: setb PIN_asm(PIN_LED)
// PIN_set(PIN)             convert PIN for direct manipulation: PIN_set(PIN_LED) = 1;
//
// PIN_WAKE_enable(PIN)     enable wake-up from sleep by PIN low (P03, P14, P15 only)
// PIN_WAKE_disable(PIN)    disable wake-up from sleep by PIN low
//
// PWM_start(PIN)           start PWM output on PIN, can be (P24 or P43) and (P25 or P45)
// PWM_stop(PIN)            stop PWM output on PIN
// PWM_write(PIN, val)      set PWM output active level duty cycle on PIN
//
// PWM_pol_normal(PIN)      set normal PWM polarity on PIN (default low and active high)
// PWM_pol_reverse(PIN)     set reverse PWM polarity on PIN (default high and active low)
// PWM_set_freq(FREQ)       set global PWM frequency (in Hertz)
//
// ADC_init()               init and enable ADC
// ADC_enable()             enable ADC
// ADC_disable()            disable ADC
// ADC_input(PIN)           set ADC input pin (P1 only)
// ADC_read()               sample and read ADC value (0..1023)
//
// Notes:
// ------
// P1 means port 1, P13 means pin 3 of port 1.
// If one pin of a port is set with an open-drain or bidirectional mode, then all other
// pins of the same port must be set with one of these modes as well.
// Setting the current drive capability of one pin affects the whole port.
// Reset defaults P0-P3: PIN_output_PU_OD, HIGH, 5mA drive capability
// Reset defaults P4:    PIN_input_PU
// Reset defaults P5:    PIN_input (not changable, but PULL-DOWN available)
// Pins used for PWM should be set as OUTPUT beforehand.
// Pins used for ADC must have been set with PIN_INPUT_AN beforehand.
//
// 2022 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "ch559.h"

// ===================================================================================
// Enumerate pin designators (use these designators to define pins)
// ===================================================================================

enum{ P00, P01, P02, P03, P04, P05, P06, P07,   // port 0 input & output
      P10, P11, P12, P13, P14, P15, P16, P17,   // port 1 input & output, ADC, not 5VT
      P20, P21, P22, P23, P24, P25, P26, P27,   // port 2 input & output
      P30, P31, P32, P33, P34, P35, P36, P37,   // port 3 input & output
      P40, P41, P42, P43, P44, P45, P46, P47,   // port 4 input & output, no OD, no input SBIT
      P50, P51, P52, P53, P54, P55, P56, P57    // port 5 input, no SBIT, not 5VT
};

// ===================================================================================
// Helper Defines (these are for internal use only)
// ===================================================================================

// Define pins for direct bit manipulation
SBIT(PP00, 0x80, 0);
SBIT(PP01, 0x80, 1);
SBIT(PP02, 0x80, 2);
SBIT(PP03, 0x80, 3);
SBIT(PP04, 0x80, 4);
SBIT(PP05, 0x80, 5);
SBIT(PP06, 0x80, 6);
SBIT(PP07, 0x80, 7);

SBIT(PP10, 0x90, 0);
SBIT(PP11, 0x90, 1);
SBIT(PP12, 0x90, 2);
SBIT(PP13, 0x90, 3);
SBIT(PP14, 0x90, 4);
SBIT(PP15, 0x90, 5);
SBIT(PP16, 0x90, 6);
SBIT(PP17, 0x90, 7);

SBIT(PP20, 0xA0, 0);
SBIT(PP21, 0xA0, 1);
SBIT(PP22, 0xA0, 2);
SBIT(PP23, 0xA0, 3);
SBIT(PP24, 0xA0, 4);
SBIT(PP25, 0xA0, 5);
SBIT(PP26, 0xA0, 6);
SBIT(PP27, 0xA0, 7);

SBIT(PP30, 0xB0, 0);
SBIT(PP31, 0xB0, 1);
SBIT(PP32, 0xB0, 2);
SBIT(PP33, 0xB0, 3);
SBIT(PP34, 0xB0, 4);
SBIT(PP35, 0xB0, 5);
SBIT(PP36, 0xB0, 6);
SBIT(PP37, 0xB0, 7);

SBIT(PP40, 0xC0, 0);
SBIT(PP41, 0xC0, 1);
SBIT(PP42, 0xC0, 2);
SBIT(PP43, 0xC0, 3);
SBIT(PP44, 0xC0, 4);
SBIT(PP45, 0xC0, 5);
SBIT(PP46, 0xC0, 6);
SBIT(PP47, 0xC0, 7);

SFR(P4, 0xC0);

// 2nd-stage glue defines
#define PIN_h_a(PIN) _P##PIN
#define PIN_h_s(PIN) P##PIN

#define PORT_h_l(PORT, PIN)       PP##PORT##PIN = 0
#define PORT_h_h(PORT, PIN)       PP##PORT##PIN = 1
#define PORT_h_t(PORT, PIN)       PP##PORT##PIN = !PP##PORT##PIN
#define PORT_h_r(PORT, PIN)       (PP##PORT##PIN)
#define PORT_h_w(PORT,PIN,val)    PP##PORT##PIN = val

// ===================================================================================
// Set PIN as INPUT (high impedance, no PULLUP, P0-P4 only)
// ===================================================================================
#define PIN_input(PIN) \
  ((PIN>=P00)&&(PIN<=P07) ? (PORT_CFG &= ~bP0_OC,           \
                             P0_DIR   &= ~(1<<((PIN)&7)),   \
                             P0_PU    &= ~(1<<((PIN)&7))) : \
  ((PIN>=P10)&&(PIN<=P17) ? (PORT_CFG &= ~bP1_OC,           \
                             P1_DIR   &= ~(1<<((PIN)&7)),   \
                             P1_PU    &= ~(1<<((PIN)&7)),   \
                             P1_IE    |=  (1<<((PIN)&7))) : \
  ((PIN>=P20)&&(PIN<=P27) ? (PORT_CFG &= ~bP2_OC,           \
                             P2_DIR   &= ~(1<<((PIN)&7)),   \
                             P2_PU    &= ~(1<<((PIN)&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (PORT_CFG &= ~bP3_OC,           \
                             P3_DIR   &= ~(1<<((PIN)&7)),   \
                             P3_PU    &= ~(1<<((PIN)&7))) : \
  ((PIN>=P40)&&(PIN<=P47) ? (P4_DIR   &= ~(1<<((PIN)&7)),   \
                             P4_PU    &= ~(1<<((PIN)&7))) : \
(0))))))

#define PIN_input_HI    PIN_input
#define PIN_input_FL    PIN_input

// ===================================================================================
// Set PIN as INPUT with internal PULLUP resistor (P0-P4 only)
// ===================================================================================
#define PIN_input_PU(PIN) \
  ((PIN>=P00)&&(PIN<=P07) ? (PORT_CFG &= ~bP0_OC,           \
                             P0_DIR   &= ~(1<<((PIN)&7)),   \
                             P0_PU    |=  (1<<((PIN)&7))) : \
  ((PIN>=P10)&&(PIN<=P17) ? (PORT_CFG &= ~bP1_OC,           \
                             P1_DIR   &= ~(1<<((PIN)&7)),   \
                             P1_PU    |=  (1<<((PIN)&7)),   \
                             P1_IE    |=  (1<<((PIN)&7))) : \
  ((PIN>=P20)&&(PIN<=P27) ? (PORT_CFG &= ~bP2_OC,           \
                             P2_DIR   &= ~(1<<((PIN)&7)),   \
                             P2_PU    |=  (1<<((PIN)&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (PORT_CFG &= ~bP3_OC,           \
                             P3_DIR   &= ~(1<<((PIN)&7)),   \
                             P3_PU    |= ~(1<<((PIN)&7))) : \
  ((PIN>=P40)&&(PIN<=P47) ? (P4_DIR   &= ~(1<<((PIN)&7)),   \
                             P4_PU    |= ~(1<<((PIN)&7))) : \
(0))))))

// ===================================================================================
// Set PIN as INPUT (high impedance, no PULLUP) for ADC (P1 only)
// ===================================================================================
#define PIN_input_AN(PIN) \
  ((PIN>=P10)&&(PIN<=P17) ? (PORT_CFG &= ~bP1_OC,           \
                             P1_DIR   &= ~(1<<((PIN)&7)),   \
                             P1_PU    &= ~(1<<((PIN)&7)),   \
                             P1_IE    &= ~(1<<((PIN)&7))) : \
(0))

#define PIN_input_AD  PIN_input_AN
#define PIN_input_ADC PIN_input_AN

// ===================================================================================
// Set PIN as OUTPUT (push-pull, P0-P4 only)
// ===================================================================================
#define PIN_output(PIN) \
  ((PIN>=P00)&&(PIN<=P07) ? (PORT_CFG &= ~bP0_OC,           \
                             P0_DIR   |=  (1<<((PIN)&7))) : \
  ((PIN>=P10)&&(PIN<=P17) ? (PORT_CFG &= ~bP1_OC,           \
                             P1_DIR   |=  (1<<((PIN)&7))) : \
  ((PIN>=P20)&&(PIN<=P27) ? (PORT_CFG &= ~bP2_OC,           \
                             P2_DIR   |=  (1<<((PIN)&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (PORT_CFG &= ~bP3_OC,           \
                             P3_DIR   |=  (1<<((PIN)&7))) : \
  ((PIN>=P40)&&(PIN<=P47) ? (P4_DIR   |=  (1<<((PIN)&7))) : \
(0))))))

#define PIN_output_PP PIN_output

// ===================================================================================
// Set PIN as OPEN-DRAIN OUTPUT (also high-impedance input, no pullup, P0-P3 only)
// ===================================================================================
#define PIN_output_OD(PIN) \
  ((PIN>=P00)&&(PIN<=P07) ? (PORT_CFG |=  bP0_OC,           \
                             P0_DIR   &= ~(1<<((PIN)&7)),   \
                             P0_PU    &= ~(1<<((PIN)&7))) : \
  ((PIN>=P10)&&(PIN<=P17) ? (PORT_CFG |=  bP1_OC,           \
                             P1_DIR   &= ~(1<<((PIN)&7)),   \
                             P1_PU    &= ~(1<<((PIN)&7)),   \
                             P1_IE    |=  (1<<((PIN)&7))) : \
  ((PIN>=P20)&&(PIN<=P27) ? (PORT_CFG |=  bP2_OC,           \
                             P2_DIR   &= ~(1<<((PIN)&7)),   \
                             P2_PU    &= ~(1<<((PIN)&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (PORT_CFG |=  bP3_OC,           \
                             P3_DIR   &= ~(1<<((PIN)&7)),   \
                             P3_PU    &= ~(1<<((PIN)&7))) : \
(0)))))

#define PIN_input_OD PIN_output_OD

// ===================================================================================
// Set PIN as OPEN-DRAIN OUTPUT with internal PULLUP (also input, P0-P3 only)
// ===================================================================================
#define PIN_output_PU_OD(PIN) \
  ((PIN>=P00)&&(PIN<=P07) ? (PORT_CFG |=  bP0_OC,           \
                             P0_DIR   &= ~(1<<((PIN)&7)),   \
                             P0_PU    |=  (1<<((PIN)&7))) : \
  ((PIN>=P10)&&(PIN<=P17) ? (PORT_CFG |=  bP1_OC,           \
                             P1_DIR   &= ~(1<<((PIN)&7)),   \
                             P1_PU    |=  (1<<((PIN)&7)),   \
                             P1_IE    |=  (1<<((PIN)&7))) : \
  ((PIN>=P20)&&(PIN<=P27) ? (PORT_CFG |=  bP2_OC,           \
                             P2_DIR   &= ~(1<<((PIN)&7)),   \
                             P2_PU    |=  (1<<((PIN)&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (PORT_CFG |=  bP3_OC,           \
                             P3_DIR   &= ~(1<<((PIN)&7)),   \
                             P3_PU    |=  (1<<((PIN)&7))) : \
(0)))))

#define PIN_input_PU_OD PIN_output_PU_OD

// ===================================================================================
// Set PIN as OPEN-DRAIN OUTPUT with internal PULLUP and 2 cycle HIGH drive
// (8051 standard bi-direction mode, also input, P0-P3 only)
// ===================================================================================
#define PIN_output_BI(PIN) \
  ((PIN>=P00)&&(PIN<=P07) ? (PORT_CFG |=  bP0_OC,           \
                             P0_DIR   |=  (1<<((PIN)&7)),   \
                             P0_PU    |=  (1<<((PIN)&7))) : \
  ((PIN>=P10)&&(PIN<=P17) ? (PORT_CFG |=  bP1_OC,           \
                             P1_DIR   |=  (1<<((PIN)&7)),   \
                             P1_PU    |=  (1<<((PIN)&7)),   \
                             P1_IE    |=  (1<<((PIN)&7))) : \
  ((PIN>=P20)&&(PIN<=P27) ? (PORT_CFG |=  bP2_OC,           \
                             P2_DIR   |=  (1<<((PIN)&7)),   \
                             P2_PU    |=  (1<<((PIN)&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (PORT_CFG |=  bP3_OC,           \
                             P3_DIR   |=  (1<<((PIN)&7)),   \
                             P3_PU    |=  (1<<((PIN)&7))) : \
(0)))))

#define PIN_input_BI  PIN_output_BI
#define PIN_bidir     PIN_output_BI

// ===================================================================================
// Set PIN output current drive capabilty to 5mA (P0-P3 only, affects whole port)
// ===================================================================================
#define PIN_current_low(PIN) \
  ((PIN>=P00)&&(PIN<=P07) ? (PORT_CFG &= ~bP0_DRV) : \
  ((PIN>=P10)&&(PIN<=P17) ? (PORT_CFG &= ~bP1_DRV) : \
  ((PIN>=P20)&&(PIN<=P27) ? (PORT_CFG &= ~bP2_DRV) : \
  ((PIN>=P30)&&(PIN<=P37) ? (PORT_CFG &= ~bP3_DRV) : \
(0)))))

// ===================================================================================
// Set PIN output current drive capabilty to 20mA (P0, P2, P3 only) or 10mA (P1 only)
// Setting affects the whole port.
// ===================================================================================
#define PIN_current_high(PIN) \
  ((PIN>=P00)&&(PIN<=P07) ? (PORT_CFG |= bP0_DRV) : \
  ((PIN>=P10)&&(PIN<=P17) ? (PORT_CFG |= bP1_DRV) : \
  ((PIN>=P20)&&(PIN<=P27) ? (PORT_CFG |= bP2_DRV) : \
  ((PIN>=P30)&&(PIN<=P37) ? (PORT_CFG |= bP3_DRV) : \
(0)))))

// ===================================================================================
// PIN OUTPUT manipulation macros (P0-P4 only)
// ===================================================================================
#define PIN_low(PIN)          PIN_h_s(PIN) = 0              // set pin to LOW
#define PIN_high(PIN)         PIN_h_s(PIN) = 1              // set pin to HIGH
#define PIN_toggle(PIN)       PIN_h_s(PIN) = !PIN_h_s(PIN)  // TOGGLE pin
#define PIN_write(PIN, val)   PIN_h_s(PIN) = val            // WRITE pin value

// ===================================================================================
// PIN INPUT read macros (P0-P5)
// ===================================================================================
#define PIN_read(PIN) \
  ((PIN>=P00)&&(PIN<=P37) ? (PIN_h_s(PIN))         : \
  ((PIN>=P40)&&(PIN<=P47) ? ((P4_IN>>((PIN)&7))&1) : \
  ((PIN>=P50)&&(PIN<=P57) ? ((P5_IN>>((PIN)&7))&1) : \
(0))))

// ===================================================================================
// Convert PIN for inline assembly and direct manipulation
// ===================================================================================
#define PIN_asm(PIN)        PIN_h_a(PIN)
#define PIN_set(PIN)        PIN_h_s(PIN)

// ===================================================================================
// Enable/disable WAKE-up from sleep by PIN LOW (P03, P14, P15 only)
// ===================================================================================
#define WAKE_PIN_enable(PIN) \
  ((PIN == P03) ? (WAKE_CTRL |= bWAK_P0_3_LO) : \
  ((PIN == P14) ? (WAKE_CTRL |= bWAK_P1_4_LO) : \
  ((PIN == P15) ? (WAKE_CTRL |= bWAK_P1_5_LO) : \
(0)))))

#define WAKE_PIN_disable(PIN) \
  ((PIN == P03) ? (WAKE_CTRL &= ~bWAK_P0_3_LO) : \
  ((PIN == P14) ? (WAKE_CTRL &= ~bWAK_P1_4_LO) : \
  ((PIN == P15) ? (WAKE_CTRL &= ~bWAK_P1_5_LO) : \
(0)))))

// ===================================================================================
// Start PWM on PIN, must be a PWM1/2-capable: (P24 or 43) and (P25 or P45)
// ===================================================================================
#define PWM_start(PIN) \
  ((PIN == P24) ? (PWM_CTRL &= ~bPWM_CLR_ALL, PIN_FUNC &= ~bPWM1_PIN_X, PWM_CTRL |= bPWM1_OUT_EN) : \
  ((PIN == P25) ? (PWM_CTRL &= ~bPWM_CLR_ALL, PIN_FUNC &= ~bPWM2_PIN_X, PWM_CTRL |= bPWM2_OUT_EN) : \
  ((PIN == P43) ? (PWM_CTRL &= ~bPWM_CLR_ALL, PIN_FUNC |=  bPWM1_PIN_X, PWM_CTRL |= bPWM1_OUT_EN) : \
  ((PIN == P45) ? (PWM_CTRL &= ~bPWM_CLR_ALL, PIN_FUNC |=  bPWM2_PIN_X, PWM_CTRL |= bPWM2_OUT_EN) : \
(0)))))

// ===================================================================================
// Set PWM output active level duty cycle on pin
// ===================================================================================
#define PWM_write(PIN, val) \
  ((PIN == P24) ? (PWM_DATA1 = val) : \
  ((PIN == P25) ? (PWM_DATA2 = val) : \
  ((PIN == P43) ? (PWM_DATA1 = val) : \
  ((PIN == P45) ? (PWM_DATA2 = val) : \
(0)))))

// ===================================================================================
// Stop PWM on pin
// ===================================================================================
#define PWM_stop(PIN) \
  ((PIN == P24) ? (PWM_CTRL &= ~bPWM1_OUT_EN) : \
  ((PIN == P25) ? (PWM_CTRL &= ~bPWM2_OUT_EN) : \
  ((PIN == P43) ? (PWM_CTRL &= ~bPWM1_OUT_EN) : \
  ((PIN == P45) ? (PWM_CTRL &= ~bPWM2_OUT_EN) : \
(0)))))

// ===================================================================================
// Set normal PWM polarity on pin (default low and active high)
// ===================================================================================
#define PWM_pol_normal(PIN) \
  ((PIN == P24) ? (PWM_CTRL &= ~bPWM1_POLAR) : \
  ((PIN == P25) ? (PWM_CTRL &= ~bPWM2_POLAR) : \
  ((PIN == P43) ? (PWM_CTRL &= ~bPWM1_POLAR) : \
  ((PIN == P45) ? (PWM_CTRL &= ~bPWM2_POLAR) : \
(0)))))

// ===================================================================================
// Set reverse PWM polarity on pin (default high and active low)
// ===================================================================================
#define PWM_pol_reverse(PIN) \
  ((PIN == P24) ? (PWM_CTRL |= bPWM1_POLAR) : \
  ((PIN == P25) ? (PWM_CTRL |= bPWM2_POLAR) : \
  ((PIN == P43) ? (PWM_CTRL |= bPWM1_POLAR) : \
  ((PIN == P45) ? (PWM_CTRL |= bPWM2_POLAR) : \
(0)))))

// ===================================================================================
// Set global PWM frequency (in Hertz, range: F_CPU/65536 - F_CPU/256)
// ===================================================================================
#define PWM_set_freq(FREQ) \
  (((FREQ) >= F_CPU / 256) ? (PWM_CK_SE = 0)              : \
  (((F_CPU / 256 / (FREQ) - 1) > 255) ? (PWM_CK_SE = 255) : \
  (PWM_CK_SE = (uint8_t)(F_CPU / 256 / (FREQ) - 1))         \
))

// ===================================================================================
// ADC functions (P1 only)
// ===================================================================================
#define ADC_enable()    ADC_SETUP |=  bADC_POWER_EN
#define ADC_disable()   ADC_SETUP &= ~bADC_POWER_EN

inline void ADC_init(void) {
  //uint16_t temp;
  ADC_enable();
  ADC_CK_SE = (uint8_t)((F_CPU + 11999999) / 12000000);
  //temp = ADC_FIFO;
  //ADC_CTRL = 0;
}

#define ADC_input(PIN) \
  ((PIN>=P10)&&(PIN<=P17) ? (ADC_CHANN = (1<<((PIN)&7))) : \
(0))

inline uint16_t ADC_read(void) {
  ADC_CTRL |=  bADC_SAMPLE;
  while(!(ADC_STAT & bADC_IF_ACT));
  ADC_CTRL &= ~bADC_SAMPLE;
  return ADC_FIFO;
}
