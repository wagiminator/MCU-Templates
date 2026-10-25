// ===================================================================================
// Basic FLASH Functions for CH32V002, V004, V005, V006, V007, M007           * v1.0 *
// ===================================================================================
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#include "flash.h"

// Time-critical functions run from RAM (section .data is copied to RAM at startup)
#if FLASH_IN_RAM
  #define FLASH_RAMFUNC(n)  __attribute__((section(".data.flashfn." #n), noinline))
#else
  #define FLASH_RAMFUNC(n)
#endif

#define FLASH_busy()        (FLASH->STATR & FLASH_STATR_BSY)
#define FLASH_wait()        do{}while(FLASH_busy())

// Unlock FLASH controller and fast programming/erase mode
#define FLASH_FULL_unlock() do{ FLASH_unlock(); FLASH_FAST_unlock(); }while(0)

// Lock FLASH controller and fast programming/erase mode
#define FLASH_FULL_lock()   do{ FLASH_FAST_lock(); FLASH_lock(); }while(0)

// Erase FLASH page (256 bytes)
FLASH_RAMFUNC(FLASH_PAGE_erase) void FLASH_PAGE_erase(uint8_t page) {
  uint32_t addr = FLASH_PAGE_BASE(page);
  FLASH_FULL_unlock();
  FLASH_wait();
  FLASH->CTLR |=  FLASH_CTLR_PAGE_ER;               // fast page erase mode
  FLASH->ADDR  =  addr;
  FLASH->CTLR |=  FLASH_CTLR_STRT;
  FLASH_wait();
  FLASH->CTLR &= ~FLASH_CTLR_PAGE_ER;
  FLASH_FULL_lock();
}

// Write 256 bytes to an (erased) FLASH page
FLASH_RAMFUNC(FLASH_PAGE_write) void FLASH_PAGE_write(uint8_t page, const void* buf) {
  const uint8_t* src = (const uint8_t*)buf;
  uint32_t addr = FLASH_PAGE_BASE(page);
  uint8_t i;
  FLASH_FULL_unlock();
  FLASH_wait();
  FLASH->CTLR |= FLASH_CTLR_PAGE_PG;                // fast page programming mode
  FLASH->CTLR |= FLASH_CTLR_BUF_RST;                // clear page buffer
  FLASH_wait();
  for(i = 0; i < FLASH_PAGE_SIZE / 4; i++) {
    uint32_t word = (uint32_t)src[0] | ((uint32_t)src[1] << 8)
                  | ((uint32_t)src[2] << 16) | ((uint32_t)src[3] << 24);
    *(__IO uint32_t *)(addr + ((uint32_t)i << 2)) = word;
    FLASH->CTLR |= FLASH_CTLR_BUF_LOAD;             // load word into page buffer
    FLASH_wait();
    src += 4;
  }
  FLASH->ADDR  = addr;
  FLASH->CTLR |= FLASH_CTLR_STRT;                   // program page
  FLASH_wait();
  FLASH->CTLR &= ~FLASH_CTLR_PAGE_PG;
  FLASH_FULL_lock();
}

// Write 16-bit data to (erased) FLASH addr; only the addressed word is loaded into
// the page buffer, all other buffer bytes are 0xFF and leave the cells unchanged
FLASH_RAMFUNC(FLASH_write) void FLASH_write(uint32_t addr, uint16_t data) {
  uint32_t word = (addr & 2) ? (((uint32_t)data << 16) | 0x0000FFFF)
                             : (0xFFFF0000 | (uint32_t)data);
  FLASH_FULL_unlock();
  FLASH_wait();
  FLASH->CTLR |= FLASH_CTLR_PAGE_PG;
  FLASH->CTLR |= FLASH_CTLR_BUF_RST;
  FLASH_wait();
  *(__IO uint32_t *)(addr & ~(uint32_t)3) = word;
  FLASH->CTLR |= FLASH_CTLR_BUF_LOAD;
  FLASH_wait();
  FLASH->ADDR  = addr & ~(uint32_t)(FLASH_PAGE_SIZE - 1);
  FLASH->CTLR |= FLASH_CTLR_STRT;
  FLASH_wait();
  FLASH->CTLR &= ~FLASH_CTLR_PAGE_PG;
  FLASH_FULL_lock();
}

// ===================================================================================
// Option Bytes
// ===================================================================================
// The option bytes block (16 bytes at OB_BASE) consists of four 32-bit words, each
// byte is followed by its complement:
// word0 = nUSER:USER:nRDPR:RDPR   word1 = nData1:Data1:nData0:Data0
// word2 = nWRPR1:WRPR1:nWRPR0:WRPR0   word3 = nWRPR3:WRPR3:nWRPR2:WRPR2
// It can only be changed by erasing and re-programming the whole block.

#define OB_WORD(hi, lo)     ((uint32_t)(((uint32_t)(~(hi) & 0xFF) << 24) | ((uint32_t)((hi) & 0xFF) << 16) | \
                                        ((uint32_t)(~(lo) & 0xFF) <<  8) |  (uint32_t)((lo) & 0xFF)))

// Erase and re-program the option bytes block
FLASH_RAMFUNC(FLASH_OB_program) static void FLASH_OB_program(uint32_t w0, uint32_t w1, uint32_t w2, uint32_t w3) {
  FLASH_FULL_unlock();
  FLASH_OB_unlock();
  FLASH_wait();

  // Erase option bytes
  FLASH->CTLR |=  FLASH_CTLR_OPTER;
  FLASH->CTLR |=  FLASH_CTLR_STRT;
  FLASH_wait();
  FLASH->CTLR &= ~FLASH_CTLR_OPTER;

  // Program option bytes (fast page programming, 4 words)
  FLASH->CTLR |= FLASH_CTLR_PAGE_PG;
  FLASH->CTLR |= FLASH_CTLR_BUF_RST;
  FLASH_wait();
  *(__IO uint32_t *)(OB_BASE +  0) = w0;
  FLASH->CTLR |= FLASH_CTLR_BUF_LOAD;
  FLASH_wait();
  *(__IO uint32_t *)(OB_BASE +  4) = w1;
  FLASH->CTLR |= FLASH_CTLR_BUF_LOAD;
  FLASH_wait();
  *(__IO uint32_t *)(OB_BASE +  8) = w2;
  FLASH->CTLR |= FLASH_CTLR_BUF_LOAD;
  FLASH_wait();
  *(__IO uint32_t *)(OB_BASE + 12) = w3;
  FLASH->CTLR |= FLASH_CTLR_BUF_LOAD;
  FLASH_wait();
  FLASH->ADDR  = OB_BASE;
  FLASH->CTLR |= FLASH_CTLR_STRT;
  FLASH_wait();
  FLASH->CTLR &= ~FLASH_CTLR_PAGE_PG;

  FLASH_OB_lock();
  FLASH_FULL_lock();
}

// Read option bytes
#define OB_RAW(n)           (*(__IO uint32_t *)(OB_BASE + 4 * (n)))
#define OB_RDPR_cur()       ((uint8_t)(OB_RAW(0) & 0xFF))
#define OB_USER_cur()       ((uint8_t)((OB_RAW(0) >> 16) & 0xFF))
#define OB_DATA_cur()       ((uint16_t)((OB_RAW(1) & 0xFF) | (((OB_RAW(1) >> 16) & 0xFF) << 8)))

// Set option bytes read protection and CODE FLASH write protection
void FLASH_OB_protect(void) {
  FLASH_OB_program(OB_WORD(OB_USER_cur(), 0x00), OB_WORD((OB_DATA_cur() >> 8) & 0xFF, OB_DATA_cur() & 0xFF),
                   OB_RAW(2), OB_RAW(3));
}

// Remove option bytes read protection and CODE FLASH write protection
void FLASH_OB_unprotect(void) {
  FLASH_OB_program(OB_WORD(OB_USER_cur(), FLASH_RDPRT), OB_WORD((OB_DATA_cur() >> 8) & 0xFF, OB_DATA_cur() & 0xFF),
                   OB_RAW(2), OB_RAW(3));
}

// Erase option bytes area and set defaults (no protection, user data 0xFFFF)
void FLASH_OB_erase(void) {
  FLASH_OB_program(OB_WORD(OB_USER_cur(), FLASH_RDPRT), OB_WORD(0xFF, 0xFF), 0x00FF00FF, 0x00FF00FF);
}

// Write option bytes user flags
void FLASH_OB_USER_write(uint8_t flags) {
  uint8_t user = 0xC2 | (OB_USER_cur() & 0x20) | (flags & 0x1D);  // keep startup mode bit
  FLASH_OB_program(OB_WORD(user, OB_RDPR_cur()), OB_WORD((OB_DATA_cur() >> 8) & 0xFF, OB_DATA_cur() & 0xFF),
                   OB_RAW(2), OB_RAW(3));
}

// Write option bytes user data
void FLASH_OB_DATA_write(uint16_t data) {
  FLASH_OB_program(OB_WORD(OB_USER_cur(), OB_RDPR_cur()), OB_WORD(data >> 8, data & 0xFF),
                   OB_RAW(2), OB_RAW(3));
}
