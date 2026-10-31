// ===================================================================================
// Basic GPIO Functions for STM32L0xx                                         * v1.0 *
// ===================================================================================
//
// Pins must be defined as PA0, PA1, .., PH14, PH15 - e.g.:
// #define PIN_LED PC0      // LED on pin PC0
//
// PIN functions available:
// ------------------------
// PIN_input(PIN)           Set PIN as INPUT (floating, no pullup/pulldown)
// PIN_input_PU(PIN)        Set PIN as INPUT with internal PULLUP resistor
// PIN_input_PD(PIN)        Set PIN as INPUT with internal PULLDOWN resistor
// PIN_input_AN(PIN)        Set PIN as INPUT for analog peripherals (e.g. ADC) (*)
// PIN_output(PIN)          Set PIN as OUTPUT (push-pull)
// PIN_output_OD(PIN)       Set PIN as OUTPUT (open-drain)
// PIN_output_OD_PU(PIN)    Set PIN as OUTPUT (open-drain, pullup)
//
// PIN_pullup(PIN)          Enable PULLUP resistor on PIN
// PIN_pulldown(PIN)        Enable PULLDOWN resistor on PIN
// PIN_pulloff(PIN)         Disable PULLUP/PULLDOWN resistor on PIN (*)
// PIN_pushpull(PIN)        Set PIN output type to push-pull (*)
// PIN_opendrain(PIN)       Set PIN output type to open-drain
// PIN_speed(PIN, s)        Set PIN output SPEED (0: very low (*) .. 3: very high)
// PIN_alternate(PIN, AF)   Set alternate function AF (0..15) for PIN
//
// PIN_low(PIN)             Set PIN output value to LOW (*)
// PIN_high(PIN)            Set PIN output value to HIGH
// PIN_toggle(PIN)          TOGGLE PIN output value
// PIN_read(PIN)            Read PIN input value
// PIN_write(PIN, val)      Write PIN output value (0 = LOW / 1 = HIGH)
//
// PIN interrupt and event functions available:
// --------------------------------------------
// PIN_EVT_set(PIN,TYPE)    Setup PIN event TYPE:
//                          PIN_EVT_OFF, PIN_EVT_RISING, PIN_EVT_FALLING, PIN_EVT_BOTH
// PIN_INT_set(PIN,TYPE)    Setup PIN interrupt TYPE:
//                          PIN_INT_OFF, PIN_INT_RISING, PIN_INT_FALLING, PIN_INT_BOTH
// PIN_INT_enable(PIN)      Enable interrupt (NVIC) of the EXTI line of PIN
// PIN_INT_disable(PIN)     Disable interrupt (NVIC) of the EXTI line of PIN
// PIN_INTFLAG_read(PIN)    Read interrupt flag of PIN
// PIN_INTFLAG_clear(PIN)   Clear interrupt flag of PIN
// PIN_INT_ISR(handler) { } Interrupt service routine, handler must match the EXTI line:
//                          EXTI0_1_IRQHandler (line 0, 1), EXTI2_3_IRQHandler (2, 3),
//                          EXTI4_15_IRQHandler (line 4..15)
//
// PORT functions available:
// -------------------------
// PORT_enable(PIN)         Enable GPIO PORT of PIN
// PORTA_enable()           Enable GPIO PORT A
// PORTB_enable()           Enable GPIO PORT B
// PORTC_enable()           Enable GPIO PORT C
// PORTD_enable()           Enable GPIO PORT D
// PORTE_enable()           Enable GPIO PORT E
// PORTH_enable()           Enable GPIO PORT H
// PORTS_enable()           Enable all GPIO PORTS
//
// PORT_disable(PIN)        Disable GPIO PORT of PIN
// PORTA_disable()          Disable GPIO PORT A
// PORTB_disable()          Disable GPIO PORT B
// PORTC_disable()          Disable GPIO PORT C
// PORTD_disable()          Disable GPIO PORT D
// PORTE_disable()          Disable GPIO PORT E
// PORTH_disable()          Disable GPIO PORT H
// PORTS_disable()          Disable all GPIO PORTS
//
// Analog-to-Digital Converter (ADC) functions available:
// ------------------------------------------------------
// ADC_init()               Init, enable and calibrate ADC (must be called first)
// ADC_enable()             Enable ADC (power-up)
// ADC_disable()            Disable ADC (power-down)
// ADC_calibrate()          Calibrate ADC (ADC must be disabled)
//
// ADC_fast()               Set fast mode   (  1.5 ADC clock cycles, least accurate)
// ADC_medium()             Set medium mode ( 79.5 ADC clock cycles, medium accurate) (*)
// ADC_slow()               Set slow mode   (160.5 ADC clock cycles, most accurate)
//
// ADC_input(PIN)           Set PIN as ADC input
// ADC_input_VREF()         Set internal voltage reference (Vref) as ADC input
// ADC_input_TEMP()         Set internal temperature sensor as ADC input (**)
//
// ADC_read()               Sample and read 12-bit ADC value (0..4095)
// ADC_read_VDD()           Sample and calculate supply voltage (VDD) in millivolts (mV)
// ADC_read_TEMP()          Sample and calculate internal temperature in °C (**)
//
// Notes:
// ------
// - (*) reset state; ADC_init() sets medium mode.
// - (**) only STM32L011/L021/L031 (set ADC_TEMP below), the STM32L010 has no
//   temperature sensor (RM0451, chapter 13).
// - Pins used for ADC must be set with PIN_input_AN beforehand. ADC input pins are:
//   PA0..PA7 (IN0..IN7), PB0, PB1 (IN8, IN9), PC0..PC5 (IN10..IN15, larger packages).
//   VREFINT is IN17. The ADC clock is PCLK/2; VREF is sampled with 160.5 cycles.
// - For interrupts and events: each line number can only be used once at a time
//   (e.g. PA1 and PB1 cannot be used simultaneously, but PA1 and PB2 can).
// - Ports D, E and H are only available on larger devices (category 2, 3 and 5).
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// ===================================================================================
// Enumerate PIN Designators (use these designators to define pins)
// ===================================================================================
enum{
  PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PA8, PA9, PA10, PA11, PA12, PA13, PA14, PA15,
  PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7, PB8, PB9, PB10, PB11, PB12, PB13, PB14, PB15,
  PC0, PC1, PC2, PC3, PC4, PC5, PC6, PC7, PC8, PC9, PC10, PC11, PC12, PC13, PC14, PC15,
  PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7, PD8, PD9, PD10, PD11, PD12, PD13, PD14, PD15,
  PE0, PE1, PE2, PE3, PE4, PE5, PE6, PE7, PE8, PE9, PE10, PE11, PE12, PE13, PE14, PE15,
  PH0, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10, PH11, PH12, PH13, PH14, PH15
};

// ===================================================================================
// Set PIN as INPUT (high impedance, no pullup/pulldown)
// ===================================================================================
#define PIN_input(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOA->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOB->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOC->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOD->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOE->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOH->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
(0)))))))
#define PIN_input_HI PIN_input
#define PIN_input_FL PIN_input

// ===================================================================================
// Set PIN as INPUT with internal PULLUP resistor
// ===================================================================================
#define PIN_input_PU(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOA->PUPDR  =  (GPIOA->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOB->PUPDR  =  (GPIOB->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOC->PUPDR  =  (GPIOC->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOD->PUPDR  =  (GPIOD->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOE->PUPDR  =  (GPIOE->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOH->PUPDR  =  (GPIOH->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
(0)))))))

// ===================================================================================
// Set PIN as INPUT with internal PULLDOWN resistor
// ===================================================================================
#define PIN_input_PD(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOA->PUPDR  =  (GPIOA->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOB->PUPDR  =  (GPIOB->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOC->PUPDR  =  (GPIOC->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOD->PUPDR  =  (GPIOD->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOE->PUPDR  =  (GPIOE->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOH->PUPDR  =  (GPIOH->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
(0)))))))

// ===================================================================================
// Set PIN as INPUT for analog peripherals (e.g. ADC)
// ===================================================================================
#define PIN_input_AN(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->MODER |=  ((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOA->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->MODER |=  ((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOB->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->MODER |=  ((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOC->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->MODER |=  ((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOD->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->MODER |=  ((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOE->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->MODER |=  ((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOH->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
(0)))))))
#define PIN_input_AD  PIN_input_AN
#define PIN_input_ADC PIN_input_AN

// ===================================================================================
// Set PIN as OUTPUT PUSH-PULL
// ===================================================================================
#define PIN_output(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->MODER  =   (GPIOA->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOA->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOA->OTYPER &= ~((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->MODER  =   (GPIOB->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOB->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOB->OTYPER &= ~((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->MODER  =   (GPIOC->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOC->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOC->OTYPER &= ~((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->MODER  =   (GPIOD->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOD->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOD->OTYPER &= ~((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->MODER  =   (GPIOE->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOE->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOE->OTYPER &= ~((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->MODER  =   (GPIOH->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOH->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOH->OTYPER &= ~((uint32_t)1<<((PIN)&15)))        : \
(0)))))))
#define PIN_output_PP PIN_output

// ===================================================================================
// Set PIN as OUTPUT OPEN-DRAIN
// ===================================================================================
#define PIN_output_OD(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->MODER  =   (GPIOA->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOA->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOA->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->MODER  =   (GPIOB->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOB->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOB->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->MODER  =   (GPIOC->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOC->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOC->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->MODER  =   (GPIOD->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOD->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOD->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->MODER  =   (GPIOE->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOE->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOE->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->MODER  =   (GPIOH->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOH->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOH->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
(0)))))))

// ===================================================================================
// Set PIN as OUTPUT OPEN-DRAIN PULLUP
// ===================================================================================
#define PIN_output_OD_PU(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->MODER  =   (GPIOA->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOA->PUPDR  =   (GPIOA->PUPDR                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOA->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->MODER  =   (GPIOB->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOB->PUPDR  =   (GPIOB->PUPDR                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOB->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->MODER  =   (GPIOC->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOC->PUPDR  =   (GPIOC->PUPDR                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOC->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->MODER  =   (GPIOD->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOD->PUPDR  =   (GPIOD->PUPDR                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOD->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->MODER  =   (GPIOE->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOE->PUPDR  =   (GPIOE->PUPDR                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOE->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->MODER  =   (GPIOH->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOH->PUPDR  =   (GPIOH->PUPDR                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOH->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
(0)))))))

// ===================================================================================
// Enable PULLUP resistor on PIN
// ===================================================================================
#define PIN_pullup(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->PUPDR =  (GPIOA->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->PUPDR =  (GPIOB->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->PUPDR =  (GPIOC->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->PUPDR =  (GPIOD->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->PUPDR =  (GPIOE->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->PUPDR =  (GPIOH->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
(0)))))))

// ===================================================================================
// Enable PULLDOWN resistor on PIN
// ===================================================================================
#define PIN_pulldown(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->PUPDR =  (GPIOA->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->PUPDR =  (GPIOB->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->PUPDR =  (GPIOC->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->PUPDR =  (GPIOD->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->PUPDR =  (GPIOE->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->PUPDR =  (GPIOH->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
(0)))))))

// ===================================================================================
// Disable PULLUP/PULLDOWN resistor on PIN
// ===================================================================================
#define PIN_pulloff(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1)) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1)) ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1)) ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1)) ) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1)) ) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1)) ) : \
(0)))))))

// ===================================================================================
// Set PIN output type to push-pull
// ===================================================================================
#define PIN_pushpull(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->OTYPER &= ~((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->OTYPER &= ~((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->OTYPER &= ~((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->OTYPER &= ~((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->OTYPER &= ~((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->OTYPER &= ~((uint32_t)1<<((PIN)&15)) ) : \
(0)))))))

// ===================================================================================
// Set PIN output type to open-drain
// ===================================================================================
#define PIN_opendrain(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->OTYPER |=  ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->OTYPER |=  ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->OTYPER |=  ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->OTYPER |=  ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->OTYPER |=  ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->OTYPER |=  ((uint32_t)1<<((PIN)&15)) ) : \
(0)))))))

// ===================================================================================
// Set PIN output SPEED (0: very low ... 3: very high)
// ===================================================================================
#define PIN_speed(PIN, s) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->OSPEEDR =  (GPIOA->OSPEEDR                         \
                                              & ~((uint32_t)   0b11<<(((PIN)&15)<<1)))   \
                                              |  ((uint32_t)((s)&3)<<(((PIN)&15)<<1))) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->OSPEEDR =  (GPIOB->OSPEEDR                         \
                                              & ~((uint32_t)   0b11<<(((PIN)&15)<<1)))   \
                                              |  ((uint32_t)((s)&3)<<(((PIN)&15)<<1))) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->OSPEEDR =  (GPIOC->OSPEEDR                         \
                                              & ~((uint32_t)   0b11<<(((PIN)&15)<<1)))   \
                                              |  ((uint32_t)((s)&3)<<(((PIN)&15)<<1))) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->OSPEEDR =  (GPIOD->OSPEEDR                         \
                                              & ~((uint32_t)   0b11<<(((PIN)&15)<<1)))   \
                                              |  ((uint32_t)((s)&3)<<(((PIN)&15)<<1))) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->OSPEEDR =  (GPIOE->OSPEEDR                         \
                                              & ~((uint32_t)   0b11<<(((PIN)&15)<<1)))   \
                                              |  ((uint32_t)((s)&3)<<(((PIN)&15)<<1))) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->OSPEEDR =  (GPIOH->OSPEEDR                         \
                                              & ~((uint32_t)   0b11<<(((PIN)&15)<<1)))   \
                                              |  ((uint32_t)((s)&3)<<(((PIN)&15)<<1))) : \
(0)))))))

// ===================================================================================
// Set alternate function AF (0..15) for PIN
// ===================================================================================
#define PIN_alternate(PIN, AF) \
  ((PIN>=PA0)&&(PIN<=PA7)  ? ( GPIOA->AFR[0] =  (GPIOA->AFR[0]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PA8)&&(PIN<=PA15) ? ( GPIOA->AFR[1] =  (GPIOA->AFR[1]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PB0)&&(PIN<=PB7)  ? ( GPIOB->AFR[0] =  (GPIOB->AFR[0]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PB8)&&(PIN<=PB15) ? ( GPIOB->AFR[1] =  (GPIOB->AFR[1]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PC0)&&(PIN<=PC7)  ? ( GPIOC->AFR[0] =  (GPIOC->AFR[0]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PC8)&&(PIN<=PC15) ? ( GPIOC->AFR[1] =  (GPIOC->AFR[1]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PD0)&&(PIN<=PD7)  ? ( GPIOD->AFR[0] =  (GPIOD->AFR[0]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PD8)&&(PIN<=PD15) ? ( GPIOD->AFR[1] =  (GPIOD->AFR[1]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PE0)&&(PIN<=PE7)  ? ( GPIOE->AFR[0] =  (GPIOE->AFR[0]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PE8)&&(PIN<=PE15) ? ( GPIOE->AFR[1] =  (GPIOE->AFR[1]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PH0)&&(PIN<=PH7)  ? ( GPIOH->AFR[0] =  (GPIOH->AFR[0]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PH8)&&(PIN<=PH15) ? ( GPIOH->AFR[1] =  (GPIOH->AFR[1]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
(0)))))))))))))

// ===================================================================================
// Set PIN output value to LOW
// ===================================================================================
#define PIN_low(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->BRR = 1<<((PIN)&15) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->BRR = 1<<((PIN)&15) ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->BRR = 1<<((PIN)&15) ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->BRR = 1<<((PIN)&15) ) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->BRR = 1<<((PIN)&15) ) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->BRR = 1<<((PIN)&15) ) : \
(0)))))))

// ===================================================================================
// Set PIN output value to HIGH
// ===================================================================================
#define PIN_high(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->BSRR = 1<<((PIN)&15) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->BSRR = 1<<((PIN)&15) ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->BSRR = 1<<((PIN)&15) ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->BSRR = 1<<((PIN)&15) ) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->BSRR = 1<<((PIN)&15) ) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->BSRR = 1<<((PIN)&15) ) : \
(0)))))))

// ===================================================================================
// Toggle PIN output value (single write to BSRR, other pins are not affected)
// ===================================================================================
#define PIN_toggle(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->BSRR = (GPIOA->ODR & ((uint32_t)1<<((PIN)&15))) \
                                         ? ((uint32_t)1<<(((PIN)&15)+16)) : ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->BSRR = (GPIOB->ODR & ((uint32_t)1<<((PIN)&15))) \
                                         ? ((uint32_t)1<<(((PIN)&15)+16)) : ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->BSRR = (GPIOC->ODR & ((uint32_t)1<<((PIN)&15))) \
                                         ? ((uint32_t)1<<(((PIN)&15)+16)) : ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->BSRR = (GPIOD->ODR & ((uint32_t)1<<((PIN)&15))) \
                                         ? ((uint32_t)1<<(((PIN)&15)+16)) : ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->BSRR = (GPIOE->ODR & ((uint32_t)1<<((PIN)&15))) \
                                         ? ((uint32_t)1<<(((PIN)&15)+16)) : ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->BSRR = (GPIOH->ODR & ((uint32_t)1<<((PIN)&15))) \
                                         ? ((uint32_t)1<<(((PIN)&15)+16)) : ((uint32_t)1<<((PIN)&15)) ) : \
(0)))))))

// ===================================================================================
// Read PIN input value (returns 0 for LOW, 1 for HIGH)
// ===================================================================================
#define PIN_read(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( (GPIOA->IDR>>((PIN)&15))&1 ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( (GPIOB->IDR>>((PIN)&15))&1 ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( (GPIOC->IDR>>((PIN)&15))&1 ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( (GPIOD->IDR>>((PIN)&15))&1 ) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( (GPIOE->IDR>>((PIN)&15))&1 ) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( (GPIOH->IDR>>((PIN)&15))&1 ) : \
(0)))))))

// ===================================================================================
// Write PIN output value (0 = LOW / 1 = HIGH)
// ===================================================================================
#define PIN_write(PIN, val) ((val)?(PIN_high(PIN)):(PIN_low(PIN)))

// ===================================================================================
// Setup PIN interrupt / event (EXTI)
// ===================================================================================
enum{PIN_INT_OFF, PIN_INT_RISING, PIN_INT_FALLING, PIN_INT_BOTH};
enum{PIN_EVT_OFF, PIN_EVT_RISING, PIN_EVT_FALLING, PIN_EVT_BOTH};

// Connect EXTI line (PIN&15) to the port of PIN (SYSCFG_EXTICRx, 4 bits per line,
// port code PA:0, PB:1, PC:2, PD:3, PE:4, PH:5 = PIN>>4)
#define PIN_EXTI_set(PIN) \
  (RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN, \
   SYSCFG->EXTICR[((PIN)&15)>>2] = (SYSCFG->EXTICR[((PIN)&15)>>2]      \
                                 & ~((uint32_t)0xf << (((PIN)&3)<<2))) \
                                 |  ((uint32_t)((PIN)>>4) << (((PIN)&3)<<2)))

#define PIN_INT_set(PIN, TYPE) \
  (PIN_EXTI_set(PIN), \
  ((TYPE) & 1) ? (EXTI->RTSR |=  (uint32_t)1<<((PIN)&15)) : \
                 (EXTI->RTSR &= ~((uint32_t)1<<((PIN)&15))), \
  ((TYPE) & 2) ? (EXTI->FTSR |=  (uint32_t)1<<((PIN)&15)) : \
                 (EXTI->FTSR &= ~((uint32_t)1<<((PIN)&15))), \
  EXTI->PR = (uint32_t)1<<((PIN)&15), \
  ((TYPE) & 3) ? (EXTI->IMR  |=  (uint32_t)1<<((PIN)&15)) : \
                 (EXTI->IMR  &= ~((uint32_t)1<<((PIN)&15))))

#define PIN_EVT_set(PIN, TYPE) \
  (PIN_EXTI_set(PIN), \
  ((TYPE) & 1) ? (EXTI->RTSR |=  (uint32_t)1<<((PIN)&15)) : \
                 (EXTI->RTSR &= ~((uint32_t)1<<((PIN)&15))), \
  ((TYPE) & 2) ? (EXTI->FTSR |=  (uint32_t)1<<((PIN)&15)) : \
                 (EXTI->FTSR &= ~((uint32_t)1<<((PIN)&15))), \
  ((TYPE) & 3) ? (EXTI->EMR  |=  (uint32_t)1<<((PIN)&15)) : \
                 (EXTI->EMR  &= ~((uint32_t)1<<((PIN)&15))))

// NVIC interrupt number of the EXTI line of PIN
#define PIN_IRQn(PIN) \
  ((((PIN)&15) <= 1) ? EXTI0_1_IRQn : (((PIN)&15) <= 3) ? EXTI2_3_IRQn : EXTI4_15_IRQn)

#define PIN_INT_enable(PIN)     NVIC_EnableIRQ(PIN_IRQn(PIN))
#define PIN_INT_disable(PIN)    NVIC_DisableIRQ(PIN_IRQn(PIN))

#define PIN_INTFLAG_read(PIN)   (EXTI->PR & ((uint32_t)1 << ((PIN) & 15)))
#define PIN_INTFLAG_clear(PIN)  (EXTI->PR = ((uint32_t)1 << ((PIN) & 15)))

#define PIN_INT_ISR(handler)    void handler(void) __attribute__((interrupt)); \
                                void handler(void)

// ===================================================================================
// Enable GPIO PORTS
// ===================================================================================
#define PORTA_enable()   (RCC->IOPENR |= RCC_IOPENR_GPIOAEN)
#define PORTB_enable()   (RCC->IOPENR |= RCC_IOPENR_GPIOBEN)
#define PORTC_enable()   (RCC->IOPENR |= RCC_IOPENR_GPIOCEN)
#define PORTD_enable()   (RCC->IOPENR |= RCC_IOPENR_GPIODEN)
#define PORTE_enable()   (RCC->IOPENR |= RCC_IOPENR_GPIOEEN)
#define PORTH_enable()   (RCC->IOPENR |= RCC_IOPENR_GPIOHEN)
#define PORTS_enable()    (RCC->IOPENR  = RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN \
                                        | RCC_IOPENR_GPIOCEN | RCC_IOPENR_GPIODEN \
                                        | RCC_IOPENR_GPIOEEN | RCC_IOPENR_GPIOHEN)

#define PORT_enable(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( RCC->IOPENR |= RCC_IOPENR_GPIOAEN ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( RCC->IOPENR |= RCC_IOPENR_GPIOBEN ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( RCC->IOPENR |= RCC_IOPENR_GPIOCEN ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( RCC->IOPENR |= RCC_IOPENR_GPIODEN ) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( RCC->IOPENR |= RCC_IOPENR_GPIOEEN ) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( RCC->IOPENR |= RCC_IOPENR_GPIOHEN ) : \
(0)))))))

// ===================================================================================
// Disable GPIO PORTS
// ===================================================================================
#define PORTA_disable()  (RCC->IOPENR &= ~RCC_IOPENR_GPIOAEN)
#define PORTB_disable()  (RCC->IOPENR &= ~RCC_IOPENR_GPIOBEN)
#define PORTC_disable()  (RCC->IOPENR &= ~RCC_IOPENR_GPIOCEN)
#define PORTD_disable()  (RCC->IOPENR &= ~RCC_IOPENR_GPIODEN)
#define PORTE_disable()  (RCC->IOPENR &= ~RCC_IOPENR_GPIOEEN)
#define PORTH_disable()  (RCC->IOPENR &= ~RCC_IOPENR_GPIOHEN)
#define PORTS_disable()  (RCC->IOPENR  = 0)

#define PORT_disable(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( RCC->IOPENR &= ~RCC_IOPENR_GPIOAEN ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( RCC->IOPENR &= ~RCC_IOPENR_GPIOBEN ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( RCC->IOPENR &= ~RCC_IOPENR_GPIOCEN ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( RCC->IOPENR &= ~RCC_IOPENR_GPIODEN ) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( RCC->IOPENR &= ~RCC_IOPENR_GPIOEEN ) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( RCC->IOPENR &= ~RCC_IOPENR_GPIOHEN ) : \
(0)))))))

// ===================================================================================
// ADC Functions
// ===================================================================================

// ADC parameters
#define ADC_TEMP            0     // 1: temperature sensor functions (only STM32L011,
                                  //    STM32L021, STM32L031 - the STM32L010 has none!)

// ADC calibration registers (factory calibration values, see datasheet)
#define ADC_VREFCAL         (*(__I uint16_t*)(0x1FF80078))    // VREFINT at 3000mV, 25°C
#define ADC_TSCAL1          (*(__I uint16_t*)(0x1FF8007A))    // TEMP at 30°C (L011/21/31)
#define ADC_TSCAL2          (*(__I uint16_t*)(0x1FF8007E))    // TEMP at 130°C (L011/21/31)

// ADC calibration values
#define ADC_CVOLT           3000  // calibration voltage of VREF and TEMP sensor
#define ADC_CTEMP1          30    // calibration temperature of TSCAL1
#define ADC_CTEMP2          130   // calibration temperature of TSCAL2

// ADC clock: PCLK/2 (synchronous, max. 16MHz in range 1, 8MHz in range 2), low
// frequency mode below 3.5MHz (RM0451, chapter 13.3.5), min. 140kHz (datasheet)
#define ADC_CKMODE          (0b01 << ADC_CFGR2_CKMODE_Pos)  // ADC clock = PCLK / 2
#if F_CPU / 2 < 3500000
  #define ADC_LFMEN         ADC_CCR_LFMEN                   // low frequency mode
#else
  #define ADC_LFMEN         0
#endif
// Note: the ADC needs fADC >= 140kHz, i.e. F_CPU >= 280kHz (datasheet)

// Set ADC sampling time (applies to all channels)
#define ADC_fast()          (ADC1->SMPR =  ADC1->SMPR & ~ADC_SMPR_SMP)
#define ADC_medium()        (ADC1->SMPR = (ADC1->SMPR & ~ADC_SMPR_SMP) | 0b110)
#define ADC_slow()          (ADC1->SMPR =  ADC1->SMPR |  ADC_SMPR_SMP)

// Select ADC channel (0..18)
static inline void ADC_setChannel(uint8_t ch) {
  if(ch > 18) return;                           // invalid channel
  ADC1->CHSELR = (uint32_t)1 << ch;             // set ADC channel
}

// Set GPIO pin as ADC input
#define ADC_input(PIN) ADC_setChannel( \
  ((PIN>=PA0 )&&(PIN<=PA7 ) ? ( (PIN)&7      ) : \
  ((PIN>=PB0 )&&(PIN<=PB1 ) ? (((PIN)&7)+8   ) : \
  ((PIN>=PC0 )&&(PIN<=PC5 ) ? (((PIN)&7)+10  ) : \
  (0xff)))))

// Set VREF (and temperature sensor) as ADC input
#define ADC_input_VREF()    ADC_setChannel(17)      // internal reference (IN17)
#if ADC_TEMP > 0
#define ADC_input_TEMP()    ADC_setChannel(18)      // temperature sensor (IN18)
#endif

// Enable ADC
static inline void ADC_enable(void) {
  #if ADC_TEMP > 0
  ADC->CCR |= ADC_CCR_TSEN | ADC_CCR_VREFEN;    // enable TEMP and VREF
  #else
  ADC->CCR |= ADC_CCR_VREFEN;                   // enable VREF
  #endif
  ADC1->ISR = ADC_ISR_ADRDY;                    // clear ready flag
  do {
    ADC1->CR |= ADC_CR_ADEN;                    // enable ADC (may be ignored right
  } while(!(ADC1->ISR & ADC_ISR_ADRDY));        // after calibration -> repeat)
  #if ADC_TEMP > 0
  DLY_us(10);                                   // TEMP sensor start-up time (max)
  #endif
}

// Disable ADC
static inline void ADC_disable(void) {
  if(ADC1->CR & ADC_CR_ADSTART) {               // conversion in progress?
    ADC1->CR |= ADC_CR_ADSTP;                   // stop conversion
    while(ADC1->CR & ADC_CR_ADSTP);             // wait until stopped
  }
  if(ADC1->CR & ADC_CR_ADEN) {                  // ADC enabled?
    ADC1->CR |= ADC_CR_ADDIS;                   // disable ADC
    while(ADC1->CR & ADC_CR_ADEN);              // wait until disabled
  }
  ADC->CCR &= ~(ADC_CCR_TSEN | ADC_CCR_VREFEN); // disable TEMP and VREF
  ADC1->CR &= ~ADC_CR_ADVREGEN;                 // disable ADC voltage regulator
}

// Calibrate ADC (ADC must be disabled)
static inline void ADC_calibrate(void) {
  ADC1->CR |= ADC_CR_ADCAL;                     // start calibration (sets ADVREGEN)
  while(ADC1->CR & ADC_CR_ADCAL);               // wait until finished
  ADC1->ISR = ADC_ISR_EOCAL;                    // clear flag
}

// Setup, calibrate and enable ADC
static inline void ADC_init(void) {
  RCC->APB2ENR  |=  RCC_APB2ENR_ADCEN;          // enable ADC module clock
  RCC->APB2RSTR |=  RCC_APB2RSTR_ADCRST;        // reset ADC module
  RCC->APB2RSTR &= ~RCC_APB2RSTR_ADCRST;
  ADC1->CFGR2 = ADC_CKMODE;                     // set ADC clock (ADEN must be 0)
  ADC->CCR    = ADC_LFMEN;                      // low frequency mode if fADC < 3.5MHz
  ADC_calibrate();                              // calibrate ADC (enables regulator)
  ADC1->SMPR  = 0b110;                          // medium sampling time (79.5 cycles)
  ADC_enable();                                 // enable ADC
}

// Sample and read ADC value of current input
static inline uint16_t ADC_read(void) {
  ADC1->CR |= ADC_CR_ADSTART;                   // start conversion
  while(!(ADC1->ISR & ADC_ISR_EOC));            // wait until finished
  return ADC1->DR;                              // return result (clears EOC)
}

// Sample internal channel with longest sampling time (VREF/TEMP: >= 10us, datasheet)
static inline uint16_t ADC_read_slow(void) {
  uint32_t smpr = ADC1->SMPR;                   // save sampling time
  uint16_t val;
  ADC1->SMPR = smpr | ADC_SMPR_SMP;             // 160.5 ADC clock cycles
  val = ADC_read();                             // sample and read
  ADC1->SMPR = smpr;                            // restore sampling time
  return val;
}

// Sample and read supply voltage (VDD) in millivolts (mV)
static inline uint16_t ADC_read_VDD(void) {
  uint16_t val;
  ADC_input_VREF();                             // set VREF as ADC input
  val = ADC_read_slow();
  return(val ? (uint32_t)ADC_CVOLT * ADC_VREFCAL / val : 0); // return VDD in mV
}

#if ADC_TEMP > 0
// Sample and read temperature sensor in °C (only STM32L011/L021/L031)
static inline int8_t ADC_read_TEMP(void) {
  int32_t vdd = ADC_read_VDD();                 // read current supply voltage
  ADC_input_TEMP();                             // set temp sensor as ADC input
  int32_t tdata = vdd * ADC_read_slow() / ADC_CVOLT; // vdd-compensated temp sensor val
  return((tdata - ADC_TSCAL1) * (ADC_CTEMP2 - ADC_CTEMP1) / (ADC_TSCAL2 - ADC_TSCAL1) + ADC_CTEMP1);
}
#endif

#ifdef __cplusplus
};
#endif
