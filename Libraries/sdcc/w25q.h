// ===================================================================================
// Basic W25Qxx SPI NOR Flash Functions                                       * v1.0 *
// ===================================================================================
//
// For W25Q (Winbond) and most compatible 25-series SPI flash memories up to 128Mbit
// (16MB) with 24-bit addresses, 256-byte pages and 4KB sectors.
//
// Functions available:
// --------------------
// W25_init()               init CS pin (and SPI), wake up flash
// W25_readID()             read JEDEC ID (manufacturer, type, capacity),
//                          e.g. 0xEF4017 = Winbond W25Q64
// W25_read(a,p,l)          read l bytes from address a into buffer p
// W25_write(a,p,l)         write l bytes from buffer p to address a (handles page
//                          boundaries), the area must be erased before
// W25_eraseSector(a)       erase 4KB sector containing address a (~50ms)
// W25_eraseChip()          erase whole chip (several seconds up to minutes)
// W25_busy()               check if a write or erase operation is in progress
// W25_sleep(s)             set power-down state (0: wake up, 1: power down)
//
// Notes:
// ------
// - Erasing sets all bits to 1, writing can only change bits from 1 to 0.
// - All write and erase functions wait until the operation is completed.
// - SPI: mode 0 or 3, the flash supports high clock rates (> 50MHz).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "gpio.h"
#include "spi.h"                      // choose your SPI library
#include "system.h"
#ifdef __SDCC
#include "delay.h"
#endif

// W25Q parameters
#define W25_PIN_CS            P14     // pin connected to CS
#define W25_INIT_SPI          1       // 1: init SPI with W25_init()

// W25Q functions
void W25_init(void);                  // init CS pin, wake up flash
uint32_t W25_readID(void);            // read JEDEC ID
uint8_t W25_busy(void);               // check if write/erase in progress
void W25_read(uint32_t addr, uint8_t* buf, uint16_t len);
void W25_write(uint32_t addr, const uint8_t* buf, uint16_t len);
void W25_eraseSector(uint32_t addr);  // erase 4KB sector
void W25_eraseChip(void);             // erase whole chip
void W25_sleep(uint8_t slp);          // set power-down state

#ifdef __cplusplus
}
#endif
