// ===================================================================================
// Basic FLASH Functions for CH32X035/X034/X033                               * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// FLASH_unlock()           Unlock FLASH incl. fast mode (remove write protection)
// FLASH_lock()             Lock FLASH incl. fast mode (set write protection)
// FLASH_locked()           Check if FLASH is locked (write protected)
//
// FLASH_read(a)            Read 32-bit word from FLASH address (a)
// FLASH_PAGE_write(a, b)   Write FLASH page (256 bytes) at address (a) from buffer (b)
// FLASH_PAGE_erase(a)      Erase FLASH page (256 bytes) containing address (a)
// FLASH_error()            Check if last operation caused an error
//
// FLASH_END_erase()        Erase last page of FLASH (256 bytes)
// FLASH_END_read(a)        Read 32-bit word from FLASH END offset (a = 0..252)
// FLASH_END_write(b)       Write buffer (b, 256 bytes) to the last page of FLASH
//
// Notes:
// ------
// - The CH32X035 only supports fast page programming and erasing (256 bytes). The
//   FLASH must be unlocked prior to writing or erasing. FLASH areas must be erased
//   before being overwritten.
// - The END functions use the last page of the user FLASH (62KB), so they can be
//   used to store user data without affecting the firmware code (if there is enough
//   space left).
// - The buffer (b) must be word-aligned (e.g. uint32_t array).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// FLASH parameters
#define FLASH_SIZE        (62*1024)   // user FLASH size in bytes
#define FLASH_PAGE_SIZE   256         // FLASH page size in bytes

// FLASH unlock keys
#define FLASH_KEY_1         0x45670123
#define FLASH_KEY_2         0xCDEF89AB
#define FLASH_END_ADDR      (FLASH_BASE + FLASH_SIZE)

// FLASH functions
void FLASH_unlock(void);
void FLASH_PAGE_write(uint32_t addr, const uint32_t* buf);
void FLASH_PAGE_erase(uint32_t addr);

#define FLASH_lock()        (FLASH->CTLR |= FLASH_CTLR_LOCK | FLASH_CTLR_FLOCK)
#define FLASH_locked()      (FLASH->CTLR & (FLASH_CTLR_LOCK | FLASH_CTLR_FLOCK))
#define FLASH_error()       (FLASH->STATR & FLASH_STATR_WRPRTERR)
#define FLASH_read(a)       (*(__IO uint32_t*)(a))

#define FLASH_END_erase()   FLASH_PAGE_erase(FLASH_END_ADDR - FLASH_PAGE_SIZE)
#define FLASH_END_read(a)   FLASH_read(FLASH_END_ADDR - FLASH_PAGE_SIZE + (a))
#define FLASH_END_write(b)  FLASH_PAGE_write(FLASH_END_ADDR - FLASH_PAGE_SIZE, b)

#ifdef __cplusplus
};
#endif
