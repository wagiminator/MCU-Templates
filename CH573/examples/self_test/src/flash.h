// ===================================================================================
// Basic FLASH Functions (DataFlash and CodeFlash) for CH573/CH571            * v1.0 *
// ===================================================================================
//
// Functions available (return 0 on success):
// ------------------------------------------
// FLASH_DATA_read(a, b, n)   Read n bytes from DataFlash offset a into buffer b
// FLASH_DATA_write(a, b, n)  Write n bytes from buffer b to DataFlash offset a
// FLASH_DATA_erase(a, n)     Erase DataFlash area (256-byte pages) at offset a
// FLASH_DATA_readByte(a)     Read single byte from DataFlash offset a
//
// FLASH_CODE_erase(a, n)     Erase CodeFlash area (4096-byte blocks) at address a
// FLASH_CODE_write(a, b, n)  Write n bytes (multiple of 4) to CodeFlash address a
// FLASH_CODE_read(a)         Read 32-bit word from CodeFlash address a
//
// Notes:
// ------
// - The DataFlash ("EEPROM") has 32KB (offset 0x0000..0x7FFF, physically located at
//   0x70000), it can be erased in pages of 256 bytes and written byte by byte.
// - The CodeFlash contains the program (CH573: 448KB, CH571: 192KB), it is erased in
//   blocks of 4096 bytes and written in 32-bit words.
// - Bits can only be changed from 1 to 0 by writing, an erase sets all bits to 1.
// - During erasing and writing the flash cannot be read, so the functions run from
//   RAM with interrupts disabled. This may disturb time-critical tasks like USB or
//   Bluetooth connections.
// - The functions are based on the reconstructed command sequence of the WCH ISP
//   library (see low-level flash ROM access in system.c).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// FLASH parameters
#define FLASH_DATA_START    0x70000     // physical address of DataFlash
#define FLASH_DATA_SIZE     0x8000      // size of DataFlash (32KB)
#define FLASH_DATA_PAGE     256         // DataFlash erase page size
#define FLASH_CODE_BLOCK    4096        // CodeFlash erase block size

// FLASH functions
uint8_t FLASH_DATA_read(uint32_t addr, void* buf, uint32_t len);
uint8_t FLASH_DATA_write(uint32_t addr, const void* buf, uint32_t len);
uint8_t FLASH_DATA_erase(uint32_t addr, uint32_t len);
uint8_t FLASH_DATA_readByte(uint32_t addr);
uint8_t FLASH_CODE_erase(uint32_t addr, uint32_t len);
uint8_t FLASH_CODE_write(uint32_t addr, const void* buf, uint32_t len);

#define FLASH_CODE_read(a)  (*(__I uint32_t*)(a))

#ifdef __cplusplus
};
#endif
