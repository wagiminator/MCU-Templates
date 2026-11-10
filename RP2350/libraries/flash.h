// ===================================================================================
// Basic FLASH Functions for RP2350 (external QSPI flash via boot ROM)       * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// FLASH_read(a)            Read 32-bit word from FLASH address (a)
// FLASH_PAGE_write(a, b)   Write FLASH page (256 bytes) at address (a) from buffer (b)
// FLASH_SECTOR_erase(a)    Erase FLASH sector (4096 bytes) containing address (a)
//
// FLASH_END_erase()        Erase last sector of FLASH (4096 bytes)
// FLASH_END_read(a)        Read 32-bit word from FLASH END offset (a = 0..4092)
// FLASH_END_write(a, d)    Write 32-bit word (d) to FLASH END offset (a = 0..4092)
//
// FLASH_getUID(b)          Read 64-bit unique ID of the flash chip into buffer (b, 8 bytes)
// FLASH_getJEDEC()         Read JEDEC ID of the flash chip (manufacturer, type, capacity)
//
// FLASH_USER_START         Start address of the free FLASH behind the program
//
// Notes:
// ------
// - Addresses are XIP addresses (0x10000000 + offset), e.g. FLASH_USER_START.
// - The FLASH is organized in sectors of 4096 bytes, each consisting of 16 pages of
//   256 bytes. Erasing is done sector by sector, writing page by page. Bits can only
//   be changed from 1 to 0 by writing, an erase sets all bits to 1.
// - Each sector can be erased about 100,000 times (W25Q32RV, see datasheet).
// - During erasing and writing the flash cannot be read, so the functions run from
//   SRAM with interrupts disabled. Core 1 must not execute code from flash or access
//   the flash at that time (e.g. let it wait in a function placed in SRAM).
// - The XIP cache is flushed after each operation. Afterwards the fast XIP mode is
//   restored by the XIP setup function that the boot ROM stored in the boot RAM.
// - The flash commands (UID, JEDEC) are sent via the direct mode of the QSPI memory
//   interface (QMI).
// - The RP2350 has its own 64-bit unique ID in the OTP (ESIG_UID64, see system.h),
//   which is used as unique ID of the board by the Raspberry Pi Pico SDK.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// FLASH parameters
#define FLASH_SIZE        (4 * 1024 * 1024)       // flash size (Raspberry Pi Pico 2: 4MB)

// FLASH definitions
#define FLASH_END_ADDR    (XIP_BASE + FLASH_SIZE - FLASH_SECTOR_SIZE) // last sector
extern uint32_t _flash_end;                       // end of program (linker script)
#define FLASH_USER_START  (((uint32_t)&_flash_end + FLASH_SECTOR_SIZE - 1) & ~(FLASH_SECTOR_SIZE - 1))

// FLASH functions
void FLASH_PAGE_write(uint32_t addr, const void* buf);  // write page (256 bytes)
void FLASH_SECTOR_erase(uint32_t addr);                 // erase sector (4096 bytes)
void FLASH_getUID(uint8_t* buf);                        // read 64-bit unique ID
uint32_t FLASH_getJEDEC(void);                          // read JEDEC ID
void FLASH_write(uint32_t addr, uint32_t data);         // write single word

#define FLASH_read(a)           (*(const volatile uint32_t*)(a))
#define FLASH_END_erase()       FLASH_SECTOR_erase(FLASH_END_ADDR)
#define FLASH_END_read(a)       FLASH_read(FLASH_END_ADDR + (a))
#define FLASH_END_write(a, d)   FLASH_write(FLASH_END_ADDR + (a), d)

#ifdef __cplusplus
};
#endif
