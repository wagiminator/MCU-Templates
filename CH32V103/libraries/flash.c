// ===================================================================================
// Basic FLASH Functions for CH32V103                                         * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "flash.h"

#define FLASH_busy()        (FLASH->STATR & FLASH_STATR_BSY)

// Write 16-bit data to FLASH address (half-word aligned)
void FLASH_write(uint32_t addr, uint16_t data) {
  while(FLASH_busy());                            // wait for last operation
  FLASH->STATR  = FLASH_STATR_EOP | FLASH_STATR_PGERR | FLASH_STATR_WRPRTERR; // clear flags
  FLASH->CTLR |= FLASH_CTLR_PG;                           // programming mode
  *(__IO uint16_t*)addr = data;                   // write data
  while(FLASH_busy());                            // wait until finished
  FLASH->CTLR &= ~FLASH_CTLR_PG;                          // end programming mode
}

// Erase FLASH page containing address
void FLASH_PAGE_erase(uint32_t addr) {
  while(FLASH_busy());                            // wait for last operation
  FLASH->STATR  = FLASH_STATR_EOP | FLASH_STATR_PGERR | FLASH_STATR_WRPRTERR; // clear flags
  FLASH->CTLR |= FLASH_CTLR_PER;                          // page erase mode
  FLASH->ADDR  = addr;                            // page address
  FLASH->CTLR |= FLASH_CTLR_STRT;                         // start erase
  while(FLASH_busy());                            // wait until finished
  FLASH->CTLR &= ~FLASH_CTLR_PER;                         // end page erase mode
}
