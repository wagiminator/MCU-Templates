// ===================================================================================
// Basic FLASH Functions for STM32C0xx                                        * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "flash.h"

#define FLASH_busy()        (FLASH->SR & FLASH_SR_BSY1)

// Write 64-bit double word to FLASH address (double-word aligned)
void FLASH_write(uint32_t addr, uint32_t low, uint32_t high) {
  while(FLASH_busy());                            // wait for last operation
  FLASH->SR  = FLASH_ERRORS | FLASH_SR_EOP;       // clear flags
  FLASH->CR |= FLASH_CR_PG;                       // programming mode
  *(__IO uint32_t*)addr       = low;              // write first word
  __asm volatile ("isb");
  *(__IO uint32_t*)(addr + 4) = high;             // write second word -> start
  while(FLASH_busy());                            // wait until finished
  FLASH->SR  = FLASH_SR_EOP;                      // clear end of operation flag
  FLASH->CR &= ~FLASH_CR_PG;                      // end programming mode
}

// Erase FLASH page containing address
void FLASH_PAGE_erase(uint32_t addr) {
  uint32_t page = (addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  while(FLASH_busy());                            // wait for last operation
  FLASH->SR  = FLASH_ERRORS | FLASH_SR_EOP;       // clear flags
  FLASH->CR  = (FLASH->CR & ~FLASH_CR_PNB_Msk)    // page erase mode, page number
             | FLASH_CR_PER | (page << FLASH_CR_PNB_Pos);
  FLASH->CR |= FLASH_CR_STRT;                     // start erase
  while(FLASH_busy());                            // wait until finished
  FLASH->CR &= ~(FLASH_CR_PER | FLASH_CR_PNB_Msk);// end page erase mode
}
