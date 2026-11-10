// ===================================================================================
// Basic FLASH Functions for STM32G0xx                                        * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// FLASH_unlock()           Unlock FLASH (remove write protection)
// FLASH_lock()             Lock FLASH (set write protection)
// FLASH_locked()           Check if FLASH is locked (write protected)
//
// FLASH_read(a)            Read 32-bit word from FLASH address (a)
// FLASH_write(a, l, h)     Write 64-bit double word (low word l, high word h) to FLASH
//                          address (a), double-word aligned
// FLASH_PAGE_erase(a)      Erase FLASH page (2048 bytes) containing address (a)
// FLASH_error()            Check if last operation caused an error
//
// FLASH_END_erase()        Erase last page of FLASH (2048 bytes)
// FLASH_END_read(a)        Read 32-bit word from FLASH END offset (a = 0..2044)
// FLASH_END_write(a, l, h) Write double word to FLASH END offset (a = 0..2040)
//
// Notes:
// ------
// - The FLASH must be unlocked prior to writing or erasing. FLASH areas must be
//   erased before being overwritten. A double word can only be written once after
//   erasing (ECC).
// - The END functions use the last page of the FLASH (FLASH_SIZE, set below:
//   STM32G030x6: 32KB, x8: 64KB), so they can be used to store user data without
//   affecting the firmware code (if there is enough space left). The offset (a) is
//   counted from the start of the last page.
// - The CPU is stalled while the FLASH is being written or erased.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// FLASH parameters
#define FLASH_SIZE        (32*1024)     // FLASH size in bytes (STM32G030x6: 32KB, x8: 64KB)
#define FLASH_PAGE_SIZE   2048       // FLASH page size in bytes (erase unit)

// FLASH unlock keys
#define FLASH_KEY_1         0x45670123
#define FLASH_KEY_2         0xCDEF89AB
#define FLASH_END_ADDR      (FLASH_BASE + FLASH_SIZE)
#define FLASH_ERRORS        (FLASH_SR_OPERR | FLASH_SR_PROGERR | FLASH_SR_WRPERR | FLASH_SR_PGAERR \
                            | FLASH_SR_SIZERR | FLASH_SR_PGSERR | FLASH_SR_MISERR | FLASH_SR_FASTERR)

// FLASH functions
void FLASH_write(uint32_t addr, uint32_t low, uint32_t high);
void FLASH_PAGE_erase(uint32_t addr);

#define FLASH_unlock()      do{FLASH->KEYR = FLASH_KEY_1; FLASH->KEYR = FLASH_KEY_2;}while(0)
#define FLASH_lock()        (FLASH->CR |= FLASH_CR_LOCK)
#define FLASH_locked()      (FLASH->CR & FLASH_CR_LOCK)
#define FLASH_error()       (FLASH->SR & FLASH_ERRORS)
#define FLASH_read(a)       (*(__IO uint32_t*)(a))

#define FLASH_END_erase()         FLASH_PAGE_erase(FLASH_END_ADDR - FLASH_PAGE_SIZE)
#define FLASH_END_read(a)         FLASH_read(FLASH_END_ADDR - FLASH_PAGE_SIZE + (a))
#define FLASH_END_write(a, l, h)  FLASH_write(FLASH_END_ADDR - FLASH_PAGE_SIZE + (a), l, h)

#ifdef __cplusplus
};
#endif
