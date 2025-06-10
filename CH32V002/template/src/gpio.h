// ===================================================================================
// Basic GPIO Functions for CH32V002, V004, V005, V006, V007, M007            * v0.1 *
// ===================================================================================
//
// Pins must be defined as PA0, PA1, .., PC0, PC1, etc. - e.g.:
// #define PIN_LED PC0      // LED on pin PC0
//
// PIN functions available:
// ------------------------
// PIN_input(PIN)           Set PIN as INPUT (floating, no pullup/pulldown) (*)
// PIN_input_PU(PIN)        Set PIN as INPUT with internal PULLUP resistor
// PIN_input_PD(PIN)        Set PIN as INPUT with internal PULLDOWN resistor
// PIN_input_AN(PIN)        Set PIN as INPUT for analog peripherals (e.g. ADC)
// PIN_output(PIN)          Set PIN as OUTPUT (push-pull)
// PIN_output_OD(PIN)       Set PIN as OUTPUT (open-drain)
// PIN_alternate(PIN)       Set PIN as alternate output (push-pull)
// PIN_alternate_OD(PIN)    Set PIN as alternate output (open-drain)
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
// PIN_INT_enable()         Enable PIN interrupts
// PIN_INT_disable()        Disable PIN interrupts
// PIN_INTFLAG_read(PIN)    Read interrupt flag of PIN
// PIN_INTFLAG_clear(PIN)   Clear interrupt flag of PIN
// PIN_INT_ISR { }          Pin interrupt service routine
//
// PORT functions available:
// -------------------------
// PORT_enable(PIN)         Enable GPIO PORT of PIN
// PORTA_enable()           Enable GPIO PORT A
// PORTB_enable()           Enable GPIO PORT B
// PORTC_enable()           Enable GPIO PORT C
// PORTD_enable()           Enable GPIO PORT D
// PORTS_enable()           Enable all GPIO PORTS
//
// PORT_disable(PIN)        Disable GPIO PORT of PIN
// PORTA_disable()          Disable GPIO PORT A
// PORTA_disable()          Disable GPIO PORT B
// PORTC_disable()          Disable GPIO PORT C
// PORTD_disable()          Disable GPIO PORT D
// PORTS_disable()          Disable all GPIO PORTS
//
// Analog-to-Digital Converter (ADC) functions available:
// ------------------------------------------------------
// ADC_init()               Init, enable and calibrate ADC (must be called first)
// ADC_enable()             Enable ADC (power-up)
// ADC_disable()            Disable ADC (power-down)
// ADC_calibrate()          Calibrate ADC
//
// ADC_fast()               Set fast mode   ( 28 clock cycles, least accurate) (*)
// ADC_medium()             Set medium mode (168 clock cycles, medium accurate)
// ADC_slow()               Set slow mode   (504 clock cycles, most accurate)
//
// ADC_input(PIN)           Set PIN as ADC input
// ADC_input_VREF()         Set internal voltage referece (Vref) as ADC input
// ADC_input_VCAL()         Set calibration voltage (Vcal) as ADC input
// ADC_input_OPA()          Set OPA internal ouput channel as ADC input
//
// ADC_read()               Sample and read ADC value (0..4095)
// ADC_read_VDD()           Sample and read supply voltage (VDD) in millivolts (mV)
//
// Touch Key (TKEY) functions available:
// ------------------------------------------------------
// not yet implemented
//
// Operational Amplifier (OPA) and Comparator (CMP) functions available:
// ---------------------------------------------------------------------
// not yet implemented
//
// Notes:
// ------
// - (*) default state
// - For interrupts and events: Each PIN number can only be used once simultaneously.
//   (For example, PA1 and PC1 cannot be used simultaneously, but PA1 and PC2).
// - Pins used for ADC must be set with PIN_input_AN beforehand. Only the following 
//   pins can be used as INPUT for the ADC: PA1, PA2, PC4, PD2, PD3, PD4, PD5, PD6.
//
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// ===================================================================================
// Enumerate PIN designators (use these designators to define pins)
// ===================================================================================
enum{ PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7,
      PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7,
      PC0, PC1, PC2, PC3, PC4, PC5, PC6, PC7,
      PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7};

// ===================================================================================
// Set PIN as INPUT (high impedance, no pullup/pulldown)
// ===================================================================================
#define PIN_input(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ( GPIOA->CFGLR =  (GPIOA->CFGLR                          \
                                           & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))    \
                                           |  ((uint32_t)0b0100<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PB0)&&(PIN<=PB7) ? ( GPIOB->CFGLR =  (GPIOB->CFGLR                          \
                                           & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))    \
                                           |  ((uint32_t)0b0100<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ( GPIOC->CFGLR =  (GPIOC->CFGLR                          \
                                           & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))    \
                                           |  ((uint32_t)0b0100<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ( GPIOD->CFGLR =  (GPIOD->CFGLR                          \
                                           & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))    \
                                           |  ((uint32_t)0b0100<<(((PIN)&7)<<2)) ) : \
(0)))))
#define PIN_input_HI PIN_input
#define PIN_input_FL PIN_input

// ===================================================================================
// Set PIN as INPUT with internal PULLUP resistor
// ===================================================================================
#define PIN_input_PU(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ({GPIOA->CFGLR  =  (GPIOA->CFGLR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b1000<<(((PIN)&7)<<2));   \
                              GPIOA->BSHR   =  ((uint32_t)1<<((PIN)&7));        }) : \
  ((PIN>=PB0)&&(PIN<=PB7) ? ({GPIOB->CFGLR  =  (GPIOB->CFGLR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b1000<<(((PIN)&7)<<2));   \
                              GPIOB->BSHR   =  ((uint32_t)1<<((PIN)&7));        }) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ({GPIOC->CFGLR  =  (GPIOC->CFGLR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b1000<<(((PIN)&7)<<2));   \
                              GPIOC->BSHR   =  ((uint32_t)1<<((PIN)&7));        }) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ({GPIOD->CFGLR  =  (GPIOD->CFGLR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b1000<<(((PIN)&7)<<2));   \
                              GPIOD->BSHR   =  ((uint32_t)1<<((PIN)&7));        }) : \
(0)))))

// ===================================================================================
// Set PIN as INPUT with internal PULLDOWN resistor
// ===================================================================================
#define PIN_input_PD(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ({GPIOA->CFGLR  =  (GPIOA->CFGLR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b1000<<(((PIN)&7)<<2));   \
                              GPIOA->BCR    =  ((uint32_t)1<<((PIN)&7));        }) : \
  ((PIN>=PB0)&&(PIN<=PB7) ? ({GPIOB->CFGLR  =  (GPIOB->CFGLR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b1000<<(((PIN)&7)<<2));   \
                              GPIOB->BCR    =  ((uint32_t)1<<((PIN)&7));        }) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ({GPIOC->CFGLR  =  (GPIOC->CFGLR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b1000<<(((PIN)&7)<<2));   \
                              GPIOC->BCR    =  ((uint32_t)1<<((PIN)&7));        }) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ({GPIOD->CFGLR  =  (GPIOD->CFGLR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b1000<<(((PIN)&7)<<2));   \
                              GPIOD->BCR    =  ((uint32_t)1<<((PIN)&7));        }) : \
(0)))))

// ===================================================================================
// Set PIN as INPUT for analog peripherals (e.g. ADC)
// ===================================================================================
#define PIN_input_AN(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ( GPIOA->CFGLR &= ~((uint32_t)0b1111<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PB0)&&(PIN<=PB7) ? ( GPIOB->CFGLR &= ~((uint32_t)0b1111<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ( GPIOC->CFGLR &= ~((uint32_t)0b1111<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ( GPIOD->CFGLR &= ~((uint32_t)0b1111<<(((PIN)&7)<<2)) ) : \
(0)))))
#define PIN_input_AD  PIN_input_AN
#define PIN_input_ADC PIN_input_AN

// ===================================================================================
// Set PIN as OUTPUT (push-pull, maximum speed 30MHz)
// ===================================================================================
#define PIN_output(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ( GPIOA->CFGLR =  (GPIOA->CFGLR                          \
                                           & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))    \
                                           |  ((uint32_t)0b0001<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PB0)&&(PIN<=PB7) ? ( GPIOB->CFGLR =  (GPIOB->CFGLR                          \
                                           & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))    \
                                           |  ((uint32_t)0b0001<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ( GPIOC->CFGLR =  (GPIOC->CFGLR                          \
                                           & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))    \
                                           |  ((uint32_t)0b0001<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ( GPIOD->CFGLR =  (GPIOD->CFGLR                          \
                                           & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))    \
                                           |  ((uint32_t)0b0001<<(((PIN)&7)<<2)) ) : \
(0)))))
#define PIN_output_PP PIN_output

// ===================================================================================
// Set PIN as OUTPUT OPEN-DRAIN (maximum speed 30MHz)
// ===================================================================================
#define PIN_output_OD(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ( GPIOA->CFGLR =  (GPIOA->CFGLR                          \
                                           & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))    \
                                           |  ((uint32_t)0b0101<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PB0)&&(PIN<=PB7) ? ( GPIOB->CFGLR =  (GPIOB->CFGLR                          \
                                           & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))    \
                                           |  ((uint32_t)0b0101<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ( GPIOC->CFGLR =  (GPIOC->CFGLR                          \
                                           & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))    \
                                           |  ((uint32_t)0b0101<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ( GPIOD->CFGLR =  (GPIOD->CFGLR                          \
                                           & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))    \
                                           |  ((uint32_t)0b0101<<(((PIN)&7)<<2)) ) : \
(0)))))

// ===================================================================================
// Set PIN as alternate output (push-pull, maximum speed 30MHz)
// ===================================================================================
#define PIN_alternate(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ( GPIOA->CFGLR =  (GPIOA->CFGLR                          \
                                           & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))    \
                                           |  ((uint32_t)0b1001<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PB0)&&(PIN<=PB7) ? ( GPIOB->CFGLR =  (GPIOB->CFGLR                          \
                                           & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))    \
                                           |  ((uint32_t)0b1001<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ( GPIOC->CFGLR =  (GPIOC->CFGLR                          \
                                           & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))    \
                                           |  ((uint32_t)0b1001<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ( GPIOD->CFGLR =  (GPIOD->CFGLR                          \
                                           & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))    \
                                           |  ((uint32_t)0b1001<<(((PIN)&7)<<2)) ) : \
(0)))))
#define PIN_alternate_PP PIN_alternate

// ===================================================================================
// Set PIN as alternate output (open-drain, maximum speed 30MHz)
// ===================================================================================
#define PIN_alternate_OD(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ( GPIOA->CFGLR =  (GPIOA->CFGLR                          \
                                           & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))    \
                                           |  ((uint32_t)0b1101<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PB0)&&(PIN<=PB7) ? ( GPIOB->CFGLR =  (GPIOB->CFGLR                          \
                                           & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))    \
                                           |  ((uint32_t)0b1101<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ( GPIOC->CFGLR =  (GPIOC->CFGLR                          \
                                           & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))    \
                                           |  ((uint32_t)0b1101<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ( GPIOD->CFGLR =  (GPIOD->CFGLR                          \
                                           & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))    \
                                           |  ((uint32_t)0b1101<<(((PIN)&7)<<2)) ) : \
(0)))))

// ===================================================================================
// Set PIN output value to LOW
// ===================================================================================
#define PIN_low(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ( GPIOA->BCR = 1<<((PIN)&7) ) : \
  ((PIN>=PB0)&&(PIN<=PB7) ? ( GPIOB->BCR = 1<<((PIN)&7) ) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ( GPIOC->BCR = 1<<((PIN)&7) ) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ( GPIOD->BCR = 1<<((PIN)&7) ) : \
(0)))))

// ===================================================================================
// Set PIN output value to HIGH
// ===================================================================================
#define PIN_high(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ( GPIOA->BSHR = 1<<((PIN)&7) ) : \
  ((PIN>=PB0)&&(PIN<=PB7) ? ( GPIOB->BSHR = 1<<((PIN)&7) ) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ( GPIOC->BSHR = 1<<((PIN)&7) ) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ( GPIOD->BSHR = 1<<((PIN)&7) ) : \
(0)))))

// ===================================================================================
// Toggle PIN output value
// ===================================================================================
#define PIN_toggle(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ( GPIOA->OUTDR ^= 1<<((PIN)&7) ) : \
  ((PIN>=PB0)&&(PIN<=PB7) ? ( GPIOB->OUTDR ^= 1<<((PIN)&7) ) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ( GPIOC->OUTDR ^= 1<<((PIN)&7) ) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ( GPIOD->OUTDR ^= 1<<((PIN)&7) ) : \
(0)))))

// ===================================================================================
// Read PIN input value (returns 0 for LOW, 1 for HIGH)
// ===================================================================================
#define PIN_read(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ( (GPIOA->INDR>>((PIN)&7))&1 ) : \
  ((PIN>=PB0)&&(PIN<=PB7) ? ( (GPIOB->INDR>>((PIN)&7))&1 ) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ( (GPIOC->INDR>>((PIN)&7))&1 ) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ( (GPIOD->INDR>>((PIN)&7))&1 ) : \
(0)))))

// ===================================================================================
// Write PIN output value (0 = LOW / 1 = HIGH)
// ===================================================================================
#define PIN_write(PIN, val) (val)?(PIN_high(PIN)):(PIN_low(PIN))

// ===================================================================================
// Setup PIN interrupt
// ===================================================================================
enum{PIN_INT_OFF, PIN_INT_RISING, PIN_INT_FALLING, PIN_INT_BOTH};

#define PIN_INT_set(PIN, TYPE) { \
  ((PIN>=PA0)&&(PIN<=PA7) ? ({RCC->PB2PCENR |=  RCC_AFIOEN | RCC_IOPAEN;            \
                              AFIO->EXTICR  &= ~((uint32_t)3<<(((PIN)&7)<<1)); }) : \
  ((PIN>=PB0)&&(PIN<=PB7) ? ({RCC->PB2PCENR |=  RCC_AFIOEN | RCC_IOPBEN;            \
                              AFIO->EXTICR   =  (AFIO->EXTICR                       \
                                             & ~((uint32_t)3<<(((PIN)&7)<<1)))      \
                                             |  ((uint32_t)1<<(((PIN)&7)<<1)); }) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ({RCC->PB2PCENR |=  RCC_AFIOEN | RCC_IOPCEN;            \
                              AFIO->EXTICR   =  (AFIO->EXTICR                       \
                                             & ~((uint32_t)3<<(((PIN)&7)<<1)))      \
                                             |  ((uint32_t)2<<(((PIN)&7)<<1)); }) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ({RCC->PB2PCENR |=  RCC_AFIOEN | RCC_IOPDEN;            \
                              AFIO->EXTICR  |=  ((uint32_t)3<<(((PIN)&7)<<1)); }) : \
  (0))))); \
  (TYPE & 3) ? (EXTI->INTENR |=   (uint32_t)1<<((PIN)&7)) : \
               (EXTI->INTENR &= ~((uint32_t)1<<((PIN)&7))); \
  (TYPE & 1) ? (EXTI->RTENR  |=   (uint32_t)1<<((PIN)&7)) : \
               (EXTI->RTENR  &= ~((uint32_t)1<<((PIN)&7))); \
  (TYPE & 2) ? (EXTI->FTENR  |=   (uint32_t)1<<((PIN)&7)) : \
               (EXTI->FTENR  &= ~((uint32_t)1<<((PIN)&7))); \
}

#define PIN_INT_enable()        NVIC_EnableIRQ(EXTI7_0_IRQn)
#define PIN_INT_disable()       NVIC_DisableIRQ(EXTI7_0_IRQn)

#define PIN_INTFLAG_read(PIN)   (EXTI->INTFR & ((uint32_t)1 << ((PIN) & 7)))
#define PIN_INTFLAG_clear(PIN)  EXTI->INTFR = ((uint32_t)1 << ((PIN) & 7))

#define PIN_INT_ISR             void EXTI7_0_IRQHandler(void) __attribute__((interrupt));\
                                void EXTI7_0_IRQHandler(void)

// ===================================================================================
// Setup PIN event
// ===================================================================================
enum{PIN_EVT_OFF, PIN_EVT_RISING, PIN_EVT_FALLING, PIN_EVT_BOTH};

#define PIN_EVT_set(PIN, TYPE) { \
  ((PIN>=PA0)&&(PIN<=PA7) ? ({RCC->PB2PCENR |=  RCC_AFIOEN | RCC_IOPAEN;            \
                              AFIO->EXTICR  &= ~((uint32_t)3<<(((PIN)&7)<<1)); }) : \
  ((PIN>=PB0)&&(PIN<=PB7) ? ({RCC->PB2PCENR |=  RCC_AFIOEN | RCC_IOPBEN;            \
                              AFIO->EXTICR   =  (AFIO->EXTICR                       \
                                             & ~((uint32_t)3<<(((PIN)&7)<<1)))      \
                                             |  ((uint32_t)1<<(((PIN)&7)<<1)); }) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ({RCC->PB2PCENR |=  RCC_AFIOEN | RCC_IOPCEN;            \
                              AFIO->EXTICR   =  (AFIO->EXTICR                       \
                                             & ~((uint32_t)3<<(((PIN)&7)<<1)))      \
                                             |  ((uint32_t)2<<(((PIN)&7)<<1)); }) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ({RCC->PB2PCENR |=  RCC_AFIOEN | RCC_IOPDEN;            \
                              AFIO->EXTICR  |=  ((uint32_t)3<<(((PIN)&7)<<1)); }) : \
  (0))))); \
  (TYPE & 3) ? (EXTI->EVENR |=   (uint32_t)1<<((PIN)&7)) : \
               (EXTI->EVENR &= ~((uint32_t)1<<((PIN)&7))); \
  (TYPE & 1) ? (EXTI->RTENR |=   (uint32_t)1<<((PIN)&7)) : \
               (EXTI->RTENR &= ~((uint32_t)1<<((PIN)&7))); \
  (TYPE & 2) ? (EXTI->FTENR |=   (uint32_t)1<<((PIN)&7)) : \
               (EXTI->FTENR &= ~((uint32_t)1<<((PIN)&7))); \
}

// ===================================================================================
// Enable GPIO PORTS
// ===================================================================================
#define PORTA_enable()      RCC->PB2PCENR |= RCC_IOPAEN;
#define PORTB_enable()      RCC->PB2PCENR |= RCC_IOPBEN;
#define PORTC_enable()      RCC->PB2PCENR |= RCC_IOPCEN;
#define PORTD_enable()      RCC->PB2PCENR |= RCC_IOPDEN;
#define PORTS_enable()      RCC->PB2PCENR |= RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_IOPDEN

#define PORT_enable(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ( RCC->PB2PCENR |= RCC_IOPAEN ) : \
  ((PIN>=PB0)&&(PIN<=PB7) ? ( RCC->PB2PCENR |= RCC_IOPBEN ) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ( RCC->PB2PCENR |= RCC_IOPCEN ) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ( RCC->PB2PCENR |= RCC_IOPDEN ) : \
(0)))))

// ===================================================================================
// Disable GPIO PORTS
// ===================================================================================
#define PORTA_disable()     RCC->PB2PCENR &= ~RCC_IOPAEN
#define PORTB_disable()     RCC->PB2PCENR &= ~RCC_IOPBEN
#define PORTC_disable()     RCC->PB2PCENR &= ~RCC_IOPCEN
#define PORTD_disable()     RCC->PB2PCENR &= ~RCC_IOPDEN
#define PORTS_disable()     RCC->PB2PCENR &= ~(RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_IOPDEN)

#define PORT_disable(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ( RCC->PB2PCENR &= ~RCC_IOPAEN ) : \
  ((PIN>=PB0)&&(PIN<=PB7) ? ( RCC->PB2PCENR &= ~RCC_IOPBEN ) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ( RCC->PB2PCENR &= ~RCC_IOPCEN ) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ( RCC->PB2PCENR &= ~RCC_IOPDEN ) : \
(0)))))

// ===================================================================================
// ADC Functions
// ===================================================================================
#define ADC_enable()        ADC1->CTLR2  |=  ADC_ADON
#define ADC_disable()       ADC1->CTLR2  &= ~ADC_ADON
#define ADC_fast()          ADC1->SAMPTR2 = 0b00000000000000000000000000000000
#define ADC_slow()          ADC1->SAMPTR2 = 0b00111111111111111111111111111111
#define ADC_medium()        ADC1->SAMPTR2 = 0b00110110110110110110110110110110

#define ADC_input_VREF()    ADC1->RSQR3 = 8
#define ADC_input_OPA()     ADC1->RSQR3 = 9
#define ADC_input_VCAL()    ADC1->RSQR3 = 10

#define ADC_input(PIN) \
  (PIN == PA1 ? (ADC1->RSQR3 = 1) : \
  (PIN == PA2 ? (ADC1->RSQR3 = 0) : \
  (PIN == PC4 ? (ADC1->RSQR3 = 2) : \
  (PIN == PD2 ? (ADC1->RSQR3 = 3) : \
  (PIN == PD3 ? (ADC1->RSQR3 = 4) : \
  (PIN == PD4 ? (ADC1->RSQR3 = 7) : \
  (PIN == PD5 ? (ADC1->RSQR3 = 5) : \
  (PIN == PD6 ? (ADC1->RSQR3 = 6) : \
(0)))))))))

static inline void ADC_calibrate(void) {
  ADC1->CTLR2 |= ADC_RSTCAL;                    // reset calibration
  while(ADC1->CTLR2 & ADC_RSTCAL);              // wait until finished
  ADC1->CTLR2 |= ADC_CAL;                       // start calibration
  while(ADC1->CTLR2 & ADC_CAL);                 // wait until finished
}

static inline void ADC_init(void) {
  RCC->PB2PCENR |= RCC_ADC1EN | RCC_AFIOEN;     // enable ADC and AFIO
  ADC1->CTLR2 = ADC_ADON | ADC_EXTSEL;          // turn on ADC, software triggering
  DLY_us(10);                                   // wait to settle
  ADC_calibrate();                              // calibrate ADC
}

static inline uint16_t ADC_read(void) {
  ADC1->CTLR2 |= ADC_SWSTART;                   // start conversion
  while(!(ADC1->STATR & ADC_EOC));              // wait until finished
  return ADC1->RDATAR;                          // return result
}

static inline uint16_t ADC_read_VDD(void) {
  ADC_input_VREF();                             // set VREF as ADC input
  return((uint32_t)1200 * 4095 / ADC_read());   // return VDD im mV
}

// ===================================================================================
// TKEY Functions
// ===================================================================================

// not yet implemented

// ===================================================================================
// OPA Functions
// ===================================================================================

// not yet implemented

// ===================================================================================
// CMP Functions (alias)
// ===================================================================================

// not yet implemented

#ifdef __cplusplus
};
#endif
