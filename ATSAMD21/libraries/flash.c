// ===================================================================================
// Basic FLASH Functions (NVMCTRL) for SAMD21                                 * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "flash.h"

// Execute NVM command (address in bytes, the NVMCTRL uses 16-bit word addresses)
static void FLASH_command(uint32_t cmd, uint32_t addr) {
  while(!(NVMCTRL->INTFLAG.reg & NVMCTRL_INTFLAG_READY));   // wait until ready
  NVMCTRL->STATUS.reg = NVMCTRL_STATUS_MASK;                // clear error flags
  NVMCTRL->ADDR.reg   = addr >> 1;                          // set address
  NVMCTRL->CTRLA.reg  = NVMCTRL_CTRLA_CMDEX_KEY | cmd;      // execute command
  while(!(NVMCTRL->INTFLAG.reg & NVMCTRL_INTFLAG_READY));   // wait until finished
}

// Prepare page buffer (manual write mode, clear page buffer)
static void FLASH_prepare(uint32_t addr) {
  NVMCTRL->CTRLB.reg |= NVMCTRL_CTRLB_MANW;                 // manual page write
  FLASH_command(NVMCTRL_CTRLA_CMD_PBC, addr);               // clear page buffer
}

// Write page buffer to flash and invalidate cache
static void FLASH_finish(uint32_t addr) {
  FLASH_command(NVMCTRL_CTRLA_CMD_WP, addr);                // write page
  FLASH_command(NVMCTRL_CTRLA_CMD_INVALL, 0);               // invalidate cache
}

// Write 32-bit word to flash address (word aligned, location must be erased)
void FLASH_write(uint32_t addr, uint32_t data) {
  addr &= ~(uint32_t)3;                                     // word aligned
  FLASH_prepare(addr);
  *(__IO uint32_t*)addr = data;                             // write to page buffer
  FLASH_finish(addr);
}

// Write flash page (64 bytes) at address (page aligned) from buffer
void FLASH_PAGE_write(uint32_t addr, const uint32_t* buf) {
  uint8_t i;
  __IO uint32_t* dst;
  addr &= ~(uint32_t)(FLASH_PAGESIZE - 1);                  // page aligned
  dst = (__IO uint32_t*)addr;
  FLASH_prepare(addr);
  for(i = FLASH_PAGESIZE / 4; i; i--) *dst++ = *buf++;      // fill page buffer
  FLASH_finish(addr);
}

// Erase flash row (256 bytes) containing address
void FLASH_ROW_erase(uint32_t addr) {
  FLASH_command(NVMCTRL_CTRLA_CMD_ER, addr & ~(uint32_t)(FLASH_ROWSIZE - 1));
  FLASH_command(NVMCTRL_CTRLA_CMD_INVALL, 0);               // invalidate cache
}

// Lock region (FLASH/16) containing address (until next reset)
void FLASH_REGION_lock(uint32_t addr) {
  FLASH_command(NVMCTRL_CTRLA_CMD_LR, addr);
}

// Unlock region (FLASH/16) containing address
void FLASH_REGION_unlock(uint32_t addr) {
  FLASH_command(NVMCTRL_CTRLA_CMD_UR, addr);
}

// Set security bit (prevents debugger access, only removable by chip erase)
void FLASH_protect(void) {
  FLASH_command(NVMCTRL_CTRLA_CMD_SSB, 0);
}
