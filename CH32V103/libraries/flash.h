// ===================================================================================
// Basic FLASH Functions for CH32V103                                         * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// FLASH_unlock()           Unlock FLASH (remove write protection)
// FLASH_lock()             Lock FLASH (set write protection)
// FLASH_locked()           Check if FLASH is locked (write protected)
//
// FLASH_read(a)            Read 16-bit data from FLASH address (a)
// FLASH_write(a, d)        Write 16-bit data (d) to FLASH address (a), half-word aligned
// FLASH_PAGE_erase(a)      Erase FLASH page (1024 bytes) containing address (a)
// FLASH_error()            Check if last operation caused an error
//
// FLASH_END_erase()        Erase last page of FLASH (1024 bytes)
// FLASH_END_read(a)        Read 16-bit data from FLASH END offset (a = 0..1022)
// FLASH_END_write(a, d)    Write 16-bit data (d) to FLASH END offset (a = 0..1022)
//
// Notes:
// ------
// - The FLASH must be unlocked prior to writing or erasing. FLASH areas must be
//   erased before being overwritten (all bits are 1 after erasing, writing changes
//   bits from 1 to 0).
// - The END functions use the last page of the FLASH (FLASH_SIZE, set below:
//   CH32V103C8/R8: 64KB, C6: 32KB), so they can be used to store user data without
//   affecting the firmware code (if there is enough space left). The offset (a) is
//   counted from the end of the FLASH.
// - The CPU is stalled while the FLASH is being written or erased.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// FLASH parameters
#define FLASH_SIZE        (64*1024)     // FLASH size in bytes (CH32V103C8/R8: 64KB, C6: 32KB)
#define FLASH_PAGE_SIZE   1024          // FLASH page size in bytes (erase unit)

// FLASH unlock keys
#define FLASH_KEY_1         0x45670123
#define FLASH_KEY_2         0xCDEF89AB
#define FLASH_END_ADDR      (FLASH_BASE + FLASH_SIZE)

// FLASH functions
void FLASH_write(uint32_t addr, uint16_t data);
void FLASH_PAGE_erase(uint32_t addr);

#define FLASH_unlock()      do{FLASH->KEYR = FLASH_KEY_1; FLASH->KEYR = FLASH_KEY_2;}while(0)
#define FLASH_lock()        (FLASH->CTLR |= FLASH_CTLR_LOCK)
#define FLASH_locked()      (FLASH->CTLR & FLASH_CTLR_LOCK)
#define FLASH_error()       (FLASH->STATR & (FLASH_STATR_PGERR | FLASH_STATR_WRPRTERR))
#define FLASH_read(a)       (*(__IO uint16_t*)(a))

#define FLASH_END_erase()     FLASH_PAGE_erase(FLASH_END_ADDR - FLASH_PAGE_SIZE)
#define FLASH_END_read(a)     FLASH_read(FLASH_END_ADDR - 2 - (a))
#define FLASH_END_write(a, d) FLASH_write(FLASH_END_ADDR - 2 - (a), d)

#ifdef __cplusplus
};
#endif
