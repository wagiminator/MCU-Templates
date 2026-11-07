// ===================================================================================
// Basic 1-Wire Bus Functions (Software)                                      * v1.0 *
// ===================================================================================
//
// Bit-banging 1-Wire master on a single GPIO pin (e.g. for DS18B20, DS2401, DS2431).
//
// Functions available:
// --------------------
// OW_init()                init 1-Wire pin
// OW_reset()               send reset pulse, returns 1 if a device answered
// OW_write(b)              write one byte (LSB first)
// OW_read()                read one byte (LSB first)
// OW_writeBit(b)           write one bit
// OW_readBit()             read one bit
// OW_skip()                send SKIP ROM command (address all devices)
// OW_crc8(p,l)             calculate 1-Wire CRC8 of l bytes at pointer p (0 = valid)
//
// Notes:
// ------
// - The data line needs an external pull-up resistor (4.7kOhm) to VCC.
// - The pin is driven LOW or released (open-drain emulation), never driven HIGH.
// - The timing is generated with DLY_us(). Interrupts during a time slot can corrupt
//   the communication, set OW_INT_LOCK to 1 to disable interrupts during each slot.
// - Requires F_CPU >= 4MHz for correct timing.
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

// 1-Wire parameters
#define OW_PIN            P14       // pin connected to 1-Wire data line
#define OW_INT_LOCK       0         // 1: disable interrupts during time slots

// 1-Wire ROM commands
#define OW_CMD_READ_ROM   0x33      // read ROM (single device only)
#define OW_CMD_SKIP_ROM   0xCC      // address all devices
#define OW_CMD_MATCH_ROM  0x55      // address a specific device

// 1-Wire functions
void OW_init(void);                 // init 1-Wire pin
uint8_t OW_reset(void);             // reset pulse, returns 1 if presence pulse detected
void OW_write(uint8_t data);        // write one byte
uint8_t OW_read(void);              // read one byte
void OW_writeBit(uint8_t bit);      // write one bit
uint8_t OW_readBit(void);           // read one bit
uint8_t OW_crc8(const uint8_t* ptr, uint8_t len); // calculate CRC8
#define OW_skip()         OW_write(OW_CMD_SKIP_ROM)

#ifdef __cplusplus
}
#endif
