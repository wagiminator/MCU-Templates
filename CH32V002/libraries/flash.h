// ===================================================================================
// Basic FLASH Functions for CH32V002, V004, V005, V006, V007, M007           * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// FLASH_lock()             Lock FLASH (set write protection)
// FLASH_unlock()           Unlock FLASH (remove write protection)
// FLASH_locked()           Check if FLASH is locked (write protected)
//
// FLASH_read(a)            Read 16-bit data from FLASH address (a)
// FLASH_write(a, d)        Write 16-bit data (d) to FLASH address (a) (area must be erased)
// FLASH_PAGE_erase(p)      Erase CODE FLASH page (256 bytes each)
// FLASH_PAGE_write(p, buf) Write 256 bytes (buf) to CODE FLASH page (p) (page must be erased)
//
// FLASH_END_erase()        Erase last page of CODE FLASH (256 bytes)
// FLASH_END_read(a)        Read 16-bit data from CODE FLASH END address (a)
// FLASH_END_write(a, d)    Write 16-bit data (d) to CODE FLASH END address (a)
//
// FLASH_OB_protect()       Set FLASH read/write protection (be careful!!!)
// FLASH_OB_unprotect()     Remove FLASH read/write protection (erases the whole CODE FLASH!)
// FLASH_OB_protected()     Check if FLASH is read/write protected
//
// FLASH_OB_erase()         Erase OPTION BYTES and remove read/write protection
// FLASH_OB_DATA_read()     Read OPTION BYTES user data (16-bit)
// FLASH_OB_DATA_write(d)   Write OPTION BYTES user data (16-bit)
// FLASH_OB_USER_read()     Read OPTION BYTES user flags (see below)
// FLASH_OB_USER_write(f)   Write OPTION BYTES user flags (see below)
//
// FLASH_OB_RESET2GPIO()    Make the RESET pin a normal GPIO pin (PD7)
// FLASH_OB_DEFAULT()       Set OPTION BYTES user flags to default
//
// Option bytes user flags (combine by OR):
// ----------------------------------------
// FLASH_OB_IWDGSW          IWDG is started by software (default, otherwise by hardware)
// FLASH_OB_STBYRST         No system reset when entering standby mode (default)
// FLASH_OB_RST_128US       PD7 is RESET pin, ignore pin state for 128us after startup
// FLASH_OB_RST_1MS         PD7 is RESET pin, ignore pin state for   1ms after startup
// FLASH_OB_RST_12MS        PD7 is RESET pin, ignore pin state for  12ms after startup (default,
//                          except CH32V007 and CH32M007)
// FLASH_OB_RST_OFF         PD7 is a normal I/O pin (default for CH32V007 and CH32M007)
//
// Notes:
// ------
// - The CH32V00x FLASH can only be programmed in 256-byte pages (fast programming
//   mode). FLASH_write(a, d) loads only the addressed 32-bit word into the page buffer
//   (the rest of the buffer stays 0xFF, which leaves the other cells unchanged), so
//   an erased area can be written in 16-bit steps. Each write call programs one page.
// - Programming can only change bits from 1 to 0. FLASH areas must be erased (to 0xFF)
//   before being written. The smallest erasable unit is a page (256 bytes).
// - The functions unlock and re-lock the FLASH themselves, so FLASH_unlock() is only
//   needed if you access the FLASH controller registers directly.
// - By default (FLASH_IN_RAM 1) the programming and erase functions are executed from RAM
//   (section .data, up to about 700 bytes of RAM and FLASH if all are used), because the
//   FLASH is not readable while it is being programmed or erased. Set FLASH_IN_RAM to 0
//   to save RAM.
// - The addresses (a) in FLASH_END_read(a) and FLASH_END_write(a, d) are counted
//   from the end of the CODE FLASH area meaning these functions can be used to store
//   user data without affecting the firmware code (if there's enough space left).
//   Set the CODE FLASH size of your device in FLASH_SIZE_KB below (CH32V002: 16).
// - Every change of an option byte rewrites the whole option bytes block (the other
//   values are preserved). Write protection of the CODE FLASH is not changed.
// - FLASH_OB_protect() makes the CODE FLASH unreadable for debuggers/programmers.
//   FLASH_OB_unprotect() erases the entire CODE FLASH including the running program!
// - The mapping of the flags differs from the CH32V003 library: the bit for the stop
//   mode reset does not exist here and FLASH_OB_RESET2GPIO() uses FLASH_OB_RST_OFF.
//
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// FLASH parameters
#ifndef FLASH_SIZE_KB
  #define FLASH_SIZE_KB         16        // size of CODE FLASH in KB (CH32V002: 16)
#endif
#ifndef FLASH_IN_RAM
  #define FLASH_IN_RAM          1         // 1: run programming/erase functions from RAM
#endif
#ifndef FLASH_V007
  #define FLASH_V007            0         // 1: CH32V007/CH32M007 (different reset defaults)
#endif

#define FLASH_PAGE_SIZE         256       // size of a FLASH page in bytes
#define FLASH_SIZE              ((uint32_t)FLASH_SIZE_KB * 1024)

void FLASH_write(uint32_t addr, uint16_t data);
void FLASH_PAGE_erase(uint8_t page);
void FLASH_PAGE_write(uint8_t page, const void* buf);
void FLASH_OB_protect(void);
void FLASH_OB_unprotect(void);
void FLASH_OB_erase(void);
void FLASH_OB_USER_write(uint8_t flags);
void FLASH_OB_DATA_write(uint16_t data);

#define FLASH_RDPRT             0x00A5
#define FLASH_BOOT_BASE         0x1FFF0000
#define FLASH_CODE_BASE         FLASH_BASE
#define FLASH_PAGE_BASE(p)      (FLASH_BASE + ((uint32_t)(p) << 8))

#define FLASH_read(a)           (*(__IO uint16_t *)(a))
#define FLASH_END_erase()       FLASH_PAGE_erase((FLASH_SIZE / FLASH_PAGE_SIZE) - 1)
#define FLASH_END_read(a)       (*(__IO uint16_t *)(FLASH_BASE + FLASH_SIZE - 2 - (a)))
#define FLASH_END_write(a,d)    FLASH_write(FLASH_BASE + FLASH_SIZE - 2 - (a), d)

#define FLASH_lock()            FLASH->CTLR |= FLASH_CTLR_LOCK
#define FLASH_unlock()          do{FLASH->KEYR = FLASH_KEY1; FLASH->KEYR = FLASH_KEY2;}while(0)
#define FLASH_locked()          (FLASH->CTLR & FLASH_CTLR_LOCK)
#define FLASH_OB_lock()         FLASH->CTLR &= ~FLASH_CTLR_OPTWRE
#define FLASH_OB_unlock()       do{FLASH->OBKEYR = FLASH_KEY1; FLASH->OBKEYR = FLASH_KEY2;}while(0)
#define FLASH_OB_locked()       (!(FLASH->CTLR & FLASH_CTLR_OPTWRE))
#define FLASH_OB_protected()    (FLASH->OBR & FLASH_OBR_RDPRT)
#define FLASH_FAST_lock()       FLASH->CTLR |= FLASH_CTLR_FLOCK
#define FLASH_FAST_unlock()     do{FLASH->MODEKEYR = FLASH_KEY1; FLASH->MODEKEYR = FLASH_KEY2;}while(0)
#define FLASH_FAST_locked()     (FLASH->CTLR & FLASH_CTLR_FLOCK)

#define FLASH_OB_DATA_read()    (uint16_t)((*(__IO uint16_t *)(OB_BASE + 4)) & 0x00FF) | \
                                (uint16_t)(((*(__IO uint16_t *)(OB_BASE + 6)) & 0x00FF) << 8)
#define FLASH_OB_USER_read()    (uint8_t)((*(__IO uint16_t *)(OB_BASE + 2)) & 0x00FF)
#define FLASH_OB_DATA_get()     ((uint16_t)(((uint32_t)FLASH->OBR >> 10) & 0xFFFF))

#define FLASH_OB_IWDGSW         ((uint8_t)0b00000001)
#define FLASH_OB_STBYRST        ((uint8_t)0b00000100)
#define FLASH_OB_RST_128US      ((uint8_t)0b00000000)
#define FLASH_OB_RST_1MS        ((uint8_t)0b00001000)
#define FLASH_OB_RST_12MS       ((uint8_t)0b00010000)
#define FLASH_OB_RST_OFF        ((uint8_t)0b00011000)

#define FLASH_OB_RESET2GPIO()   FLASH_OB_USER_write(FLASH_OB_IWDGSW | FLASH_OB_STBYRST | FLASH_OB_RST_OFF)
#if FLASH_V007
  #define FLASH_OB_DEFAULT()    FLASH_OB_USER_write(FLASH_OB_IWDGSW | FLASH_OB_STBYRST | FLASH_OB_RST_OFF)
#else
  #define FLASH_OB_DEFAULT()    FLASH_OB_USER_write(FLASH_OB_IWDGSW | FLASH_OB_STBYRST | FLASH_OB_RST_12MS)
#endif

#ifdef __cplusplus
};
#endif
