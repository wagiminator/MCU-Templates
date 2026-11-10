// ===================================================================================
// Basic FLASH Functions for RP2350 (external QSPI flash via boot ROM)       * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "flash.h"

// Boot ROM flash functions
typedef void (*rom_void_fn)(void);
typedef void (*rom_erase_fn)(uint32_t offs, uint32_t count, uint32_t block_size, uint8_t block_cmd);
typedef void (*rom_program_fn)(uint32_t offs, const uint8_t* data, uint32_t count);

static struct {
  rom_void_fn    connect;         // connect_internal_flash ('IF')
  rom_void_fn    exit_xip;        // flash_exit_xip ('EX')
  rom_erase_fn   erase;           // flash_range_erase ('RE')
  rom_program_fn program;         // flash_range_program ('RP')
  rom_void_fn    flush;           // flash_flush_cache ('FC')
} FLASH_rom;

// Copy of the XIP setup function (boot RAM) in SRAM to restore fast XIP mode after
// flash operations, saved QSPI pad settings and QMI settings of chip select 1
static uint32_t FLASH_xipSetup[64];
static uint32_t FLASH_pads[6];
static uint32_t FLASH_qmi1[3];

// Prepare flash operation (get ROM functions, copy XIP setup function)
static void FLASH_prepare(void) {
  uint8_t i;
  FLASH_rom.connect  = (rom_void_fn)   ROM_func('I', 'F');
  FLASH_rom.exit_xip = (rom_void_fn)   ROM_func('E', 'X');
  FLASH_rom.erase    = (rom_erase_fn)  ROM_func('R', 'E');
  FLASH_rom.program  = (rom_program_fn)ROM_func('R', 'P');
  FLASH_rom.flush    = (rom_void_fn)   ROM_func('F', 'C');
  for(i = 0; i < 64; i++) FLASH_xipSetup[i] = ((const uint32_t*)BOOTRAM_BASE)[i];
}

// Leave XIP mode (no flash access possible until FLASH_enterXIP)
static RAM_FUNC void FLASH_exitXIP(void) {
  volatile uint32_t* pads = &PADS_QSPI->GPIO_QSPI_SCLK;
  uint8_t i;
  __asm volatile("" ::: "memory");
  for(i = 0; i < 6; i++) FLASH_pads[i] = pads[i];   // save pad settings
  FLASH_qmi1[0] = QMI->M[1].TIMING;                 // save QMI settings of CS1
  FLASH_qmi1[1] = QMI->M[1].RFMT;
  FLASH_qmi1[2] = QMI->M[1].RCMD;
  FLASH_rom.connect();
  FLASH_rom.exit_xip();
}

// Flush cache and restore fast XIP mode by calling the XIP setup function in SRAM
static RAM_FUNC void FLASH_enterXIP(void) {
  volatile uint32_t* pads = &PADS_QSPI->GPIO_QSPI_SCLK;
  uint8_t i;
  FLASH_rom.flush();                                // also removes CS force
  ((rom_void_fn)((uint32_t)FLASH_xipSetup + 1))();  // thumb bit
  for(i = 0; i < 6; i++) pads[i] = FLASH_pads[i];   // restore pad settings
  QMI->M[1].TIMING = FLASH_qmi1[0];                 // restore QMI settings of CS1
  QMI->M[1].RFMT   = FLASH_qmi1[1];
  QMI->M[1].RCMD   = FLASH_qmi1[2];
}

// Erase and program in SRAM (operation: 0 = erase sector, 1 = program page)
static RAM_FUNC void FLASH_operation(uint8_t op, uint32_t offs, const uint8_t* buf) {
  FLASH_exitXIP();
  if(op) FLASH_rom.program(offs, buf, FLASH_PAGE_SIZE);
  else   FLASH_rom.erase(offs, FLASH_SECTOR_SIZE, 1 << 16, 0xD8);
  FLASH_enterXIP();
}

// Send command and receive answer via QMI (direct mode, chip select 0 forced)
static RAM_FUNC void FLASH_command(const uint8_t* tx, uint8_t* rx, uint8_t len) {
  uint8_t i;
  FLASH_exitXIP();
  REG_SET(QMI->DIRECT_CSR, QMI_DIRECT_CSR_ASSERT_CS0N); // CS low
  REG_SET(QMI->DIRECT_CSR, QMI_DIRECT_CSR_EN);          // enable direct mode
  for(i = 0; i < len; i++) {
    while(QMI->DIRECT_CSR & QMI_DIRECT_CSR_TXFULL);
    QMI->DIRECT_TX = tx[i];
    while(QMI->DIRECT_CSR & QMI_DIRECT_CSR_RXEMPTY);
    rx[i] = QMI->DIRECT_RX;
  }
  REG_CLR(QMI->DIRECT_CSR, QMI_DIRECT_CSR_EN);          // disable direct mode
  REG_CLR(QMI->DIRECT_CSR, QMI_DIRECT_CSR_ASSERT_CS0N); // CS high
  FLASH_enterXIP();
}

// Erase sector (4096 bytes) containing address
void FLASH_SECTOR_erase(uint32_t addr) {
  FLASH_prepare();
  INT_ATOMIC_BLOCK {
    FLASH_operation(0, (addr - XIP_BASE) & ~(FLASH_SECTOR_SIZE - 1), 0);
  }
}

// Write page (256 bytes, address must be page aligned) from buffer
void FLASH_PAGE_write(uint32_t addr, const void* buf) {
  static uint8_t page[FLASH_PAGE_SIZE] __attribute__((aligned(4)));
  uint16_t i;
  for(i = 0; i < FLASH_PAGE_SIZE; i++) page[i] = ((const uint8_t*)buf)[i];  // copy to SRAM
  FLASH_prepare();
  INT_ATOMIC_BLOCK {
    FLASH_operation(1, (addr - XIP_BASE) & ~(FLASH_PAGE_SIZE - 1), page);
  }
}

// Write single 32-bit word (address must be word aligned, other bytes stay unchanged)
void FLASH_write(uint32_t addr, uint32_t data) {
  static uint32_t page[FLASH_PAGE_SIZE / 4];
  uint8_t i;
  for(i = 0; i < FLASH_PAGE_SIZE / 4; i++) page[i] = 0xFFFFFFFF;   // 0xFF: no change
  page[(addr & (FLASH_PAGE_SIZE - 1)) >> 2] = data;
  FLASH_PAGE_write(addr & ~(FLASH_PAGE_SIZE - 1), page);
}

// Read 64-bit unique ID of the flash chip (command 4Bh)
void FLASH_getUID(uint8_t* buf) {
  uint8_t tx[13] = {0x4B}, rx[13];
  uint8_t i;
  FLASH_prepare();
  INT_ATOMIC_BLOCK {
    FLASH_command(tx, rx, 13);
  }
  for(i = 0; i < 8; i++) buf[i] = rx[i + 5];
}

// Read JEDEC ID of the flash chip (command 9Fh), e.g. 0xEF4016 = W25Q32
uint32_t FLASH_getJEDEC(void) {
  uint8_t tx[4] = {0x9F}, rx[4];
  FLASH_prepare();
  INT_ATOMIC_BLOCK {
    FLASH_command(tx, rx, 4);
  }
  return ((uint32_t)rx[1] << 16) | ((uint32_t)rx[2] << 8) | rx[3];
}
