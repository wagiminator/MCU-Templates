// ===================================================================================
// Basic FLASH Functions for CH32X035/X034/X033                               * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "flash.h"

#define FLASH_busy()        (FLASH->STATR & FLASH_STATR_BSY)

// Unlock FLASH and fast programming mode
void FLASH_unlock(void) {
  FLASH->KEYR     = FLASH_KEY_1;                  // unlock FLASH
  FLASH->KEYR     = FLASH_KEY_2;
  FLASH->MODEKEYR = FLASH_KEY_1;                  // unlock fast mode
  FLASH->MODEKEYR = FLASH_KEY_2;
}

// Write FLASH page (256 bytes) at page address from word-aligned buffer
void FLASH_PAGE_write(uint32_t addr, const uint32_t* buf) {
  uint8_t i;
  volatile uint32_t* dst = (volatile uint32_t*)(addr & ~(FLASH_PAGE_SIZE - 1));
  while(FLASH_busy());                            // wait for last operation
  FLASH->STATR = FLASH_STATR_EOP | FLASH_STATR_WRPRTERR; // clear flags
  FLASH->CTLR |= FLASH_CTLR_FTPG;                 // fast page programming mode
  FLASH->CTLR |= FLASH_CTLR_BUFRST;               // clear page buffer
  while(FLASH_busy());
  FLASH->STATR = FLASH_STATR_EOP;
  for(i = 0; i < FLASH_PAGE_SIZE / 4; i++) {      // load 64 words into buffer
    dst[i] = *buf++;
    FLASH->CTLR |= FLASH_CTLR_BUFLOAD;
    while(FLASH_busy());
  }
  FLASH->ADDR  = (uint32_t)dst;                   // page address
  FLASH->CTLR |= FLASH_CTLR_STRT;                 // start programming
  while(FLASH_busy());                            // wait until finished
  FLASH->STATR = FLASH_STATR_EOP;
  FLASH->CTLR &= ~FLASH_CTLR_FTPG;                // end fast page programming mode
}

// Erase FLASH page (256 bytes) containing address
void FLASH_PAGE_erase(uint32_t addr) {
  while(FLASH_busy());                            // wait for last operation
  FLASH->STATR = FLASH_STATR_EOP | FLASH_STATR_WRPRTERR; // clear flags
  FLASH->CTLR |= FLASH_CTLR_FTER;                 // fast page erase mode
  FLASH->ADDR  = addr & ~(FLASH_PAGE_SIZE - 1);   // page address
  FLASH->CTLR |= FLASH_CTLR_STRT;                 // start erase
  while(FLASH_busy());                            // wait until finished
  FLASH->STATR = FLASH_STATR_EOP;
  FLASH->CTLR &= ~FLASH_CTLR_FTER;                // end fast page erase mode
}
