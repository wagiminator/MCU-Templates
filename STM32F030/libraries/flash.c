// ===================================================================================
// Basic FLASH Functions for STM32F03x                                        * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "flash.h"

#define FLASH_busy()        (FLASH->SR & FLASH_SR_BSY)

// Write 16-bit data to FLASH address (half-word aligned)
void FLASH_write(uint32_t addr, uint16_t data) {
  while(FLASH_busy());                            // wait for last operation
  FLASH->SR  = FLASH_SR_EOP | FLASH_SR_PGERR | FLASH_SR_WRPRTERR; // clear flags
  FLASH->CR |= FLASH_CR_PG;                           // programming mode
  *(__IO uint16_t*)addr = data;                   // write data
  while(FLASH_busy());                            // wait until finished
  FLASH->CR &= ~FLASH_CR_PG;                          // end programming mode
}

// Erase FLASH page containing address
void FLASH_PAGE_erase(uint32_t addr) {
  while(FLASH_busy());                            // wait for last operation
  FLASH->SR  = FLASH_SR_EOP | FLASH_SR_PGERR | FLASH_SR_WRPRTERR; // clear flags
  FLASH->CR |= FLASH_CR_PER;                          // page erase mode
  FLASH->AR  = addr;                            // page address
  FLASH->CR |= FLASH_CR_STRT;                         // start erase
  while(FLASH_busy());                            // wait until finished
  FLASH->CR &= ~FLASH_CR_PER;                         // end page erase mode
}
