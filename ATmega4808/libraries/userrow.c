// ===================================================================================
// User Row (USERROW) Functions for megaAVR 0-Series                          * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "userrow.h"

// Write NVMCTRL command (SPM-protected register)
static void USERROW_cmd(uint8_t cmd) {
  asm volatile(
    "out %[ccp], %[key] \n"                     // unlock protected register...
    "sts %[ctrla], %[cmd] \n"                   // ...and write command within 4 cycles
    :: [ccp] "I" (0x34), [key] "d" ((uint8_t)CCP_SPM_gc),
       [ctrla] "n" (0x1000), [cmd] "r" (cmd)    // NVMCTRL.CTRLA
    : "memory"
  );
}

// Write block of bytes (only bytes loaded into the page buffer are erased/written)
void USERROW_write_block(const void* ptr, uint8_t addr, uint8_t len) {
  const uint8_t* src = (const uint8_t*)ptr;
  if(addr >= USERROW_SIZE) return;
  if(len > USERROW_SIZE - addr) len = USERROW_SIZE - addr;
  while(USERROW_busy());                        // wait for last operation
  USERROW_cmd(NVMCTRL_CMD_PAGEBUFCLR_gc);       // clear page buffer
  while(USERROW_busy());
  uint8_t changed = 0;
  volatile uint8_t* dst = &USERROW.USERROW0 + addr;
  while(len--) {
    if(*dst != *src) {                          // only write changed bytes
      *dst = *src;                              // load page buffer
      changed = 1;
    }
    dst++; src++;
  }
  if(changed) {
    USERROW_cmd(NVMCTRL_CMD_PAGEERASEWRITE_gc); // erase and write loaded bytes
    while(USERROW_busy());                      // wait until done
  }
}

// Write (update) single byte
void USERROW_write(uint8_t addr, uint8_t data) {
  USERROW_write_block(&data, addr, 1);
}

// Read block of bytes
void USERROW_read_block(void* ptr, uint8_t addr, uint8_t len) {
  uint8_t* dst = (uint8_t*)ptr;
  while(len--) *dst++ = USERROW_read(addr++);
}
