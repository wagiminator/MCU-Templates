// ===================================================================================
// Data Flash Functions for CH559                                             * v1.0 *
// ===================================================================================
//
// The CH559 has 1KB data flash at 0xF000 (one erase block). Erasing sets all bytes
// to 0xFF, writing can only change bits from 1 to 0 (16-bit words at even addresses).
//
// Functions available:
// --------------------
// FLASH_read(a)            read byte at data flash address a (0..1023)
// FLASH_read16(a)          read 16-bit word at even data flash address a
// FLASH_erase()            erase complete data flash (all bytes 0xFF)
// FLASH_write(a,w)         write 16-bit word w at even data flash address a
//
// Notes:
// ------
// - A word can be written several times without erase as long as only bits are
//   cleared (e.g. for counters or "append only" logs).
// - FLASH_erase() and FLASH_write() return 0 on success.
// - The CPU is halted during erase/write. Interrupts are disabled meanwhile.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "ch559.h"

#define FLASH_SIZE          1024
#define FLASH_read(a)       (*(__code uint8_t*)(DATA_FLASH_ADDR + (a)))
#define FLASH_read16(a)     (*(__code uint16_t*)(DATA_FLASH_ADDR + (a)))

uint8_t FLASH_erase(void);                    // erase data flash
uint8_t FLASH_write(uint16_t addr, uint16_t data); // write 16-bit word
