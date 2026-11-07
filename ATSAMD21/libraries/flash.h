// ===================================================================================
// Basic FLASH Functions (NVMCTRL) for SAMD21                                 * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// FLASH_read(a)            Read 32-bit word from FLASH address (a)
// FLASH_write(a, d)        Write 32-bit word (d) to FLASH address (a), word aligned
// FLASH_PAGE_write(a, b)   Write FLASH page (64 bytes) at address (a) from buffer (b)
// FLASH_ROW_erase(a)       Erase FLASH row (256 bytes = 4 pages) containing address (a)
//
// FLASH_END_erase()        Erase last row of FLASH (256 bytes)
// FLASH_END_read(a)        Read 32-bit word from FLASH END offset (a = 0..252)
// FLASH_END_write(a, d)    Write 32-bit word (d) to FLASH END offset (a = 0..252)
//
// FLASH_REGION_lock(a)     Lock region (FLASH/16) containing address (a) until reset
// FLASH_REGION_unlock(a)   Unlock region (FLASH/16) containing address (a)
// FLASH_locked(a)          Check if region containing address (a) is locked
//
// FLASH_USER_read(n)       Read 32-bit word n (0, 1) of the NVM user row (fuses)
// FLASH_BOOTPROT_read()    Read size of bootloader protection in bytes (BOOTPROT)
// FLASH_protect()          Set security bit (be careful!!!, see below)
// FLASH_protected()        Check if security bit is set
// FLASH_error()            Check if last operation caused an error
//
// Notes:
// ------
// - The FLASH (32KB..256KB) is organized in rows of 256 bytes, each consisting of 4 pages of
//   64 bytes. Erasing is done row by row, writing page by page (via page buffer).
//   Bits can only be changed from 1 to 0 by writing, an erase sets all bits to 1.
// - Each row can be erased about 25,000 times (see datasheet).
// - The functions must not be executed from the FLASH region which is written.
// - Write operations to regions protected by BOOTPROT or a lock bit are ignored.
// - FLASH_protect() prevents any debugger access to the FLASH. This can only be undone
//   by a chip erase (e.g. 'tinyocd -u' or 'tinyocd -e'), which erases the entire FLASH.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// FLASH parameters
#define FLASH_PAGESIZE        FLASH_PAGE_SIZE                     // 64 bytes
#define FLASH_ROWSIZE         (FLASH_PAGE_SIZE * NVMCTRL_ROW_PAGES) // 256 bytes
#define FLASH_END_ADDR        (FLASH_ADDR + FLASH_SIZE - FLASH_ROWSIZE) // last row

// FLASH functions
void FLASH_write(uint32_t addr, uint32_t data);
void FLASH_PAGE_write(uint32_t addr, const uint32_t* buf);
void FLASH_ROW_erase(uint32_t addr);
void FLASH_REGION_lock(uint32_t addr);
void FLASH_REGION_unlock(uint32_t addr);
void FLASH_protect(void);

#define FLASH_read(a)         (*(const __I uint32_t*)(a))
#define FLASH_error()         (NVMCTRL->STATUS.reg & (NVMCTRL_STATUS_PROGE           \
                                                    | NVMCTRL_STATUS_LOCKE           \
                                                    | NVMCTRL_STATUS_NVME))
#define FLASH_protected()     (NVMCTRL->STATUS.reg & NVMCTRL_STATUS_SB)
#define FLASH_REGIONSIZE      (FLASH_SIZE / 16)                   // lock region size
#define FLASH_locked(a)       (!(NVMCTRL->LOCK.reg & ((uint32_t)1 << ((a) / FLASH_REGIONSIZE))))

#define FLASH_END_erase()     FLASH_ROW_erase(FLASH_END_ADDR)
#define FLASH_END_read(a)     FLASH_read(FLASH_END_ADDR + ((a) & 0xfc))
#define FLASH_END_write(a, d) FLASH_write(FLASH_END_ADDR + ((a) & 0xfc), d)

#define FLASH_USER_read(n)    (*(const __I uint32_t*)(NVMCTRL_USER + ((n) & 1) * 4))
#define FLASH_BOOTPROT_read() (((FLASH_USER_read(0) & 7) == 7) ? 0                    \
                              : (256 << (7 - (FLASH_USER_read(0) & 7))))

#ifdef __cplusplus
};
#endif
