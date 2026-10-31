// ===================================================================================
// Basic EEPROM Functions for STM32L0xx                                       * v1.1 *
// ===================================================================================
//
// Functions available:
// --------------------
// EEPROM_read32(a)         Read 32-bit word (address must be 4-byte-aligned)
// EEPROM_read16(a)         Read 16-bit half-word (address must be 2-byte-aligned)
// EEPROM_read8(a)          Read 8-bit byte
//
// EEPROM_write32(a, w)     Write 32-bit word (address must be 4-byte-aligned)
// EEPROM_write16(a, h)     Write 16-bit half-word (address must be 2-byte-aligned)
// EEPROM_write8(a, b)      Write 8-bit byte
// EEPROM_erase(a)          Erase 32-bit word (set to 0x00000000)
//
// EEPROM_size()            Size of the data EEPROM in bytes
// EEPROM_unlock()          Unlock data EEPROM (done automatically by the functions)
// EEPROM_lock()            Lock data EEPROM (done automatically by the functions)
//
// Notes:
// ------
// - Addresses are offsets from the start of the data EEPROM (0x08080000).
// - EEPROM size: 128 bytes (STM32L010x4), 256 bytes (STM32L010x6/x8), 512 bytes
//   (STM32L010xB). It is derived from the flash size (RM0451, Tables 6 to 9).
//   Writes outside the EEPROM are ignored, the write functions return 0 then.
// - The erased state of the data EEPROM is 0x00. A value is only written if it
//   differs. A write takes up to 3.94ms per erase/program step (datasheet, tprog);
//   code execution from flash stalls during this time.
// - Data EEPROM write/erase is not possible in voltage range 3 (RM0451, chapter 6.1.4).
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// EEPROM size in bytes, derived from the flash size (STM32L010)
#define EEPROM_size()       ((ESIG_FLASHSIZE <= 16) ? 128 : ((ESIG_FLASHSIZE <= 64) ? 256 : 512))

// EEPROM functions and macros
#define EEPROM_read32(addr) (*(__IO uint32_t *)(DATA_EEPROM_BASE + (addr)))
#define EEPROM_read16(addr) (*(__IO uint16_t *)(DATA_EEPROM_BASE + (addr)))
#define EEPROM_read8(addr)  (*(__IO uint8_t  *)(DATA_EEPROM_BASE + (addr)))

uint8_t EEPROM_write32(uint16_t addr, uint32_t value);  // returns 1 on success
uint8_t EEPROM_write16(uint16_t addr, uint16_t value);  // returns 1 on success
uint8_t EEPROM_write8(uint16_t addr, uint8_t value);    // returns 1 on success
uint8_t EEPROM_erase(uint16_t addr);                    // returns 1 on success
void EEPROM_unlock(void);
void EEPROM_lock(void);

#ifdef __cplusplus
};
#endif
