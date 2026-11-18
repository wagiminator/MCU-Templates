// ===================================================================================
// Basic PIOC Functions for CH32X035/X034/X033 (programmable I/O controller)  * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// PIOC_init(io)              init PIOC, connect pins io (PIOC_IO0 and/or PIOC_IO1)
// PIOC_load(prog,len)        stop and reset PIOC, load program (len bytes)
// PIOC_start()               start program (from the beginning after PIOC_load/reset)
// PIOC_stop()                pause program (can be continued with PIOC_start)
// PIOC_reset()               stop and reset PIOC (program counter and registers)
// PIOC_running()             check if PIOC is running
//
// PIOC_ready()               check if PIOC has read the last byte written by host
// PIOC_write(c)              write command/data byte to PIOC (CTRL_WR), waits until
//                            PIOC has read the previous one
// PIOC_available()           check if PIOC has written a byte to be read by host
// PIOC_read()                read byte written by PIOC (CTRL_RD), waits if necessary
// PIOC_command(c)            write command byte and wait for the answer byte
//
// PIOC_setReg(n,v)           write 8-bit value v to data register n (0..31)
// PIOC_getReg(n)             read 8-bit value from data register n (0..31)
// PIOC_setReg16(n,v)         write 16-bit value v to data registers n/n+1 (n even)
// PIOC_getReg16(n)           read 16-bit value from data registers n/n+1 (n even)
// PIOC_setExchange(v)        write 8-bit value to data exchange register
// PIOC_getExchange()         read 8-bit value from data exchange register
//
// PIOC_writeRAM(offs,buf,len) copy buffer to PIOC code RAM at byte offset (pauses PIOC)
// PIOC_readRAM(offs,buf,len)  copy PIOC code RAM at byte offset to buffer (pauses PIOC)
//
// PIOC_INT_enable()          enable PIOC interrupt (PIOC sets INT_REQ)
// PIOC_INT_disable()         disable PIOC interrupt
// PIOC_INT_clear()           clear interrupt request
// PIOC_INT_pending()         check if interrupt request is set
// PIOC_ISR { }               interrupt service routine (optional, see notes)
//
// PIOC pin mapping (set below in PIOC parameters):
// ------------------------------------------------
// PIOC_MAP   IO0   IO1
//    0       PC18  PC19      (both pins are also SWDIO/SWCLK of the debug interface)
//    1       PC7   PC19      (PC7 is not available on all packages)
//
// PIOC programs:
// --------------
// The PIOC (also called eMCU) is a small 8-bit RISC core (RISC8B) running at F_CPU
// with its own program (up to 2048 16-bit instruction words), 2 I/O pins (IO0, IO1),
// an 8-bit timer/PWM, a bit encoder/decoder (PWM duty or Manchester code), 32 data
// registers and 2 control registers for data exchange with the host. The programs
// are written in RISC8B assembler and translated with the WCH assembler WASM53B or
// with pioc_asm.py (see folder 'pioc_asm'), which creates a C header with the code.
//
// Data exchange:
// - DATA_REG0..31 and DATA_EXCH can be read and written by both sides (host wins).
// - CTRL_WR is written by host and read by PIOC. Writing sets the flag DATA_MW_SR,
//   which is cleared when the PIOC reads CTRL_WR (PIOC: WAITB WB_DATA_MW_SR_1).
// - CTRL_RD is written by PIOC and read by host. Writing sets the flag DATA_SW_MR,
//   which is cleared when the host reads CTRL_RD.
// - The PIOC can request an interrupt (BS SFR_SYS_CFG,SB_INT_REQ).
//
// Notes:
// ------
// - The 4KB code RAM of the PIOC is the upper part of the system SRAM (0x20004000).
//   The RAM size in the linker script (ld/ch32x035.ld) must be reduced to 16K!
// - The PIOC is clocked with the system clock (F_CPU), i.e. the timing of the PIOC
//   programs depends on F_CPU.
// - The data registers can only be written while the PIOC is running, they are
//   cleared by a reset (PIOC_load, PIOC_reset). The code RAM can only be accessed
//   while the PIOC is stopped (PIOC_writeRAM/PIOC_readRAM pause it automatically).
// - If PC18 or PC19 is used, the debug interface (SWD) is disabled by PIOC_init().
//   The firmware can then be uploaded via the USB bootloader.
// - The interrupt vector table must be enabled (SYS_USE_VECTORS = 1 in system.h)
//   if PIOC_INT_enable() is used. The interrupt request is cleared automatically
//   before PIOC_ISR is called, PIOC_ISR is an ordinary function.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// ===================================================================================
// PIOC Parameters
// ===================================================================================
#ifndef PIOC_MAP
  #define PIOC_MAP          0     // pin mapping (see above)
#endif

// ===================================================================================
// PIOC Definitions
// ===================================================================================
#define PIOC_IO0            PIOC_MST_IO_EN0         // IO0 pin
#define PIOC_IO1            PIOC_MST_IO_EN1         // IO1 pin
#define PIOC_RAM            ((volatile uint8_t*)PIOC_SRAM_BASE) // code RAM
#define PIOC_RAM_SIZE       4096                    // code RAM size in bytes
#define PIOC_REG            ((volatile uint8_t*)&PIOC->DATA_REG0) // data registers
#define PIOC_REG16          ((volatile uint16_t*)&PIOC->DATA_REG0)

// ===================================================================================
// PIOC Functions and Macros
// ===================================================================================
void PIOC_init(uint8_t io);                       // init PIOC and pins
void PIOC_load(const uint8_t* prog, uint16_t len);// stop/reset PIOC and load program
void PIOC_writeRAM(uint16_t offs, const uint8_t* buf, uint16_t len); // write code RAM
void PIOC_readRAM(uint16_t offs, uint8_t* buf, uint16_t len);        // read code RAM
void PIOC_write(uint8_t c);                       // write byte to PIOC (CTRL_WR)
uint8_t PIOC_read(void);                          // read byte from PIOC (CTRL_RD)
uint8_t PIOC_command(uint8_t c);                  // write command and wait for answer

#define PIOC_start()        PIOC->SYS_CFG = (PIOC->SYS_CFG & ~PIOC_MST_RESET) | PIOC_MST_CLK_GATE
#define PIOC_stop()         PIOC->SYS_CFG &= ~PIOC_MST_CLK_GATE
#define PIOC_reset()        PIOC->SYS_CFG = (PIOC->SYS_CFG & (PIOC_IO0 | PIOC_IO1)) | PIOC_MST_RESET
#define PIOC_running()      (PIOC->SYS_CFG & PIOC_MST_CLK_GATE)

#define PIOC_ready()        (!(PIOC->SYS_CFG & PIOC_DATA_MW_SR))
#define PIOC_available()    (PIOC->SYS_CFG & PIOC_DATA_SW_MR)

#define PIOC_setReg(n,v)    PIOC_REG[n] = (v)
#define PIOC_getReg(n)      (PIOC_REG[n])
#define PIOC_setReg16(n,v)  PIOC_REG16[(n)>>1] = (v)
#define PIOC_getReg16(n)    (PIOC_REG16[(n)>>1])
#define PIOC_setExchange(v) PIOC->DATA_EXCH = (v)
#define PIOC_getExchange()  (PIOC->DATA_EXCH)

#define PIOC_INT_enable()   NVIC_EnableIRQ(PIOC_IRQn)
#define PIOC_INT_disable()  NVIC_DisableIRQ(PIOC_IRQn)
#define PIOC_INT_clear()    PIOC->CTRL_RD = 0       // dummy write clears INT_REQ
#define PIOC_INT_pending()  (PIOC->SYS_CFG & PIOC_INT_REQ)

#define PIOC_ISR            void PIOC_callback(void)

#ifdef __cplusplus
};
#endif
