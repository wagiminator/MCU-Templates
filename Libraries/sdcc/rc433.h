// ===================================================================================
// Basic 433MHz Remote Control Transmitter Functions                          * v1.0 *
// ===================================================================================
//
// Sends codes for common 433MHz radio sockets and receivers (EV1527, PT2262, HS1527
// and compatible, "RCSwitch protocol 1") via a simple ASK/OOK transmitter module
// (e.g. FS1000A, STX882).
//
// Functions available:
// --------------------
// RC_init()                init data pin
// RC_send(code, bits)      send code with number of bits (1 - 32), blocking,
//                          repeated RC_REPEAT times
//
// Notes:
// ------
// - Protocol: bit 0 = 1T high, 3T low; bit 1 = 3T high, 1T low; sync = 1T high,
//   31T low; T = RC_PULSE microseconds.
// - The codes of existing remote controls can be found with an RCSwitch receiver
//   sketch, a logic analyzer or a sniffer.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "gpio.h"
#include "system.h"
#ifdef __SDCC
#include "delay.h"
#endif

// RC parameters
#define RC_PIN                P14     // pin connected to DATA of transmitter module
#define RC_PULSE              350     // pulse length T in microseconds
#define RC_REPEAT             10      // number of transmissions per code

// RC functions
void RC_init(void);                   // init data pin
void RC_send(uint32_t code, uint8_t bits); // send code (blocking)

#ifdef __cplusplus
}
#endif
