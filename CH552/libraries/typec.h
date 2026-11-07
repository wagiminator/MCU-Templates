// ===================================================================================
// USB Type-C CC Functions for CH552 and CH554                                * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// TYPEC_sink_init()        enable 5.1k pull-down (Rd) on CC1/CC2 (device/sink role)
// TYPEC_source_init(c)     enable pull-up (Rp) on CC1/CC2 (host/source role), c see below
// TYPEC_off()              disable all CC resistors
// TYPEC_read()             sink: read current advertised by the source (see below)
// TYPEC_orientation()      sink: get plug orientation (0: none, 1: CC1, 2: CC2)
//
// Source current (c) / result of TYPEC_read():
// --------------------------------------------
// TYPEC_NONE               no source connected (TYPEC_read only)
// TYPEC_DEFAULT            default USB power (500mA/900mA), Rp = 56k
// TYPEC_1A5                1.5A, Rp = 22k
// TYPEC_3A                 3.0A, Rp = 10k
//
// Notes:
// ------
// - CC1 = P1.4 (UCC1, AIN1), CC2 = P1.5 (UCC2, AIN2). Both pins must be set as input
//   (high impedance). TYPEC_read() uses the ADC and changes the ADC input channel.
// - The ADC measures relative to VCC, set TYPEC_VCC_mV below accordingly.
// - Not available on CH551.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "ch554.h"

// Type-C parameters
#define TYPEC_VCC_mV      5000              // supply voltage of the chip in mV

// Results / source currents
#define TYPEC_NONE        0
#define TYPEC_DEFAULT     1
#define TYPEC_1A5         2
#define TYPEC_3A          3

// CC voltage thresholds at the sink (Rd = 5.1k) in ADC steps
#define TYPEC_TH_NONE     (uint8_t)(200UL  * 255 / TYPEC_VCC_mV)    // < 0.20V: no source
#define TYPEC_TH_1A5      (uint8_t)(660UL  * 255 / TYPEC_VCC_mV)    // > 0.66V: 1.5A
#define TYPEC_TH_3A       (uint8_t)(1230UL * 255 / TYPEC_VCC_mV)    // > 1.23V: 3.0A

// Functions
#define TYPEC_sink_init()   USB_C_CTRL = bUCC1_PD_EN | bUCC2_PD_EN
#define TYPEC_source_init(c) USB_C_CTRL = ((c) | ((c) << 4))
#define TYPEC_off()         USB_C_CTRL = 0

// Read CC voltage (internal): ch = 1 (CC1, AIN1) or 2 (CC2, AIN2)
inline uint8_t TYPEC_readCC(uint8_t ch) {
  ADC_CFG |= bADC_EN;                       // enable ADC
  ADC_CHAN1 = ch >> 1;                      // select channel
  ADC_CHAN0 = ch & 1;
  ADC_START = 1;                            // start conversion
  while(ADC_START);                         // wait for conversion
  return ADC_DATA;
}

// Get plug orientation (0: none, 1: CC1, 2: CC2)
inline uint8_t TYPEC_orientation(void) {
  uint8_t cc1 = TYPEC_readCC(1);
  uint8_t cc2 = TYPEC_readCC(2);
  if(cc1 < TYPEC_TH_NONE && cc2 < TYPEC_TH_NONE) return 0;
  return (cc1 >= cc2) ? 1 : 2;
}

// Read current advertised by the source
inline uint8_t TYPEC_read(void) {
  uint8_t cc1 = TYPEC_readCC(1);
  uint8_t cc2 = TYPEC_readCC(2);
  if(cc2 > cc1) cc1 = cc2;                  // use the connected CC line
  if(cc1 < TYPEC_TH_NONE) return TYPEC_NONE;
  if(cc1 < TYPEC_TH_1A5)  return TYPEC_DEFAULT;
  if(cc1 < TYPEC_TH_3A)   return TYPEC_1A5;
  return TYPEC_3A;
}
