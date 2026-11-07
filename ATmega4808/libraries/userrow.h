// ===================================================================================
// User Row (USERROW) Functions for megaAVR 0-Series                          * v1.0 *
// ===================================================================================
//
// The user row is an additional 64-byte EEPROM page which is not erased by a chip
// erase. It can be used to store calibration values, serial numbers, etc.
//
// Functions available:
// --------------------
// USERROW_read(a)              read byte from user row address a (0..63)
// USERROW_write(a,b)           write (update) byte b to user row address a
// USERROW_read_block(p,a,l)    read block of length l from address a to pointer p
// USERROW_write_block(p,a,l)   write block of length l from pointer p to address a
// USERROW_busy()               check if a write operation is in progress
//
// Notes:
// ------
// - Writing a byte takes about 4ms. The functions wait until the NVM is ready.
// - The user row is not erased by a chip erase, but it is erased if a locked device
//   is unlocked.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

#define USERROW_SIZE          64

#define USERROW_read(a)       ((&USERROW.USERROW0)[(a) & (USERROW_SIZE - 1)])
#define USERROW_busy()        (NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm)

void USERROW_write(uint8_t addr, uint8_t data);
void USERROW_read_block(void* ptr, uint8_t addr, uint8_t len);
void USERROW_write_block(const void* ptr, uint8_t addr, uint8_t len);

#ifdef __cplusplus
}
#endif
