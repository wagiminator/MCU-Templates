// ===================================================================================
// Basic PIO Functions for RP2350 (programmable I/O, PIO0..PIO2)             * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// PIO_init(pio)                  reset and release PIO block (PIO0, PIO1 or PIO2)
// PIO_load(pio,prog,len,offs)    load program (len instructions) into instruction
//                                memory at offset offs (JMP targets are relocated)
// PIO_pin(pio, PIN)              connect PIN to PIO block (GPIO function PIO0..PIO2)
//
// PIO_SM_enable(pio,sm)          start state machine sm (0..3)
// PIO_SM_disable(pio,sm)         stop state machine
// PIO_SM_restart(pio,sm)         restart state machine (clear internal state)
// PIO_SM_exec(pio,sm,instr)      execute instruction immediately on state machine
// PIO_SM_setClock(pio,sm,f)      set state machine clock frequency f in Hz
// PIO_SM_setWrap(pio,sm,b,t)     set wrap bottom (b) and top (t) address
// PIO_SM_put(pio,sm,data)        write 32-bit word into TX FIFO (blocking)
// PIO_SM_get(pio,sm)             read 32-bit word from RX FIFO (blocking)
// PIO_SM_txFull(pio,sm)          check if TX FIFO is full
// PIO_SM_rxEmpty(pio,sm)         check if RX FIFO is empty
//
// Instruction encoding (for hand-assembled programs, see datasheet chapter 11.4):
// -------------------------------------------------------------------------------
// PIO_JMP(cond,addr)  PIO_WAIT(pol,src,idx)  PIO_IN(src,n)   PIO_OUT(dest,n)
// PIO_PUSH(ifF,blk)   PIO_PULL(ifE,blk)      PIO_MOV(d,op,s) PIO_IRQ(clr,wait,idx)
// PIO_SET(dest,data)  PIO_NOP                PIO_SIDE(n,v)   PIO_DELAY(n)
// e.g. PIO_OUT(PIO_X, 1) | PIO_SIDE(1, 0) | PIO_DELAY(2)  ->  out x, 1 side 0 [2]
//
// Notes:
// ------
// - The RP2350 has three PIO blocks (PIO0..PIO2). Each PIO block has 32 instruction
//   words and 4 state machines with 4-word TX and RX FIFOs. Programs written with pioasm (Raspberry Pi Pico SDK) can be used by
//   copying the instruction words from the generated header file.
// - Side-set and delay share bits 12..8 of each instruction: with n side-set bits
//   (PINCTRL SIDESET_COUNT), the remaining 5-n bits are used for the delay.
// - The configuration registers (CLKDIV, EXECCTRL, SHIFTCTRL, PINCTRL) are accessed
//   directly via pio->SM[sm] (see rp2350.h).
// - New features of the RP2350 PIO (version 1): WAIT on the JMP pin (PIO_JMPPIN), MOV
//   to pin directions (PIO_PINDIRS_MOV), random access to the RX FIFO (pio->RXF_BLK),
//   GPIO base for the RP2350B (pio->GPIOBASE), IRQ flags of neighbouring blocks.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"
#include "gpio.h"

// ===================================================================================
// PIO Instruction Encoding
// ===================================================================================
// JMP conditions
#define PIO_ALWAYS        0               // always
#define PIO_NOT_X         1               // !X: scratch X zero
#define PIO_X_DEC         2               // X--: scratch X non-zero, post-decrement
#define PIO_NOT_Y         3               // !Y: scratch Y zero
#define PIO_Y_DEC         4               // Y--: scratch Y non-zero, post-decrement
#define PIO_X_NE_Y        5               // X!=Y
#define PIO_PIN           6               // input pin (EXECCTRL JMP_PIN)
#define PIO_NOT_OSRE      7               // !OSRE: output shift register not empty

// Sources and destinations
#define PIO_PINS          0               // IN/OUT/SET/MOV: pins
#define PIO_GPIO          0               // WAIT source: GPIO
#define PIO_WAIT_PIN      1               // WAIT source: input pin (relative to IN_BASE)
#define PIO_WAIT_IRQ      2               // WAIT source: IRQ flag
#define PIO_JMPPIN        3               // WAIT source: JMP pin (RP2350)
#define PIO_X             1               // scratch register X
#define PIO_Y             2               // scratch register Y
#define PIO_NULL          3               // IN/OUT/MOV: null (zeros)
#define PIO_PINDIRS       4               // OUT/SET: pin directions
#define PIO_PINDIRS_MOV   3               // MOV destination: pin directions (RP2350)
#define PIO_EXEC_MOV      4               // MOV destination: execute
#define PIO_STATUS        5               // MOV source: status
#define PIO_PC            5               // OUT/MOV destination: program counter
#define PIO_ISR           6               // IN/OUT/MOV: input shift register
#define PIO_OSR           7               // OUT/MOV: output shift register
#define PIO_EXEC_OUT      7               // OUT destination: execute

// Instructions
#define PIO_JMP(cond, addr)     (uint16_t)(0x0000 | ((cond) << 5) | ((addr) & 31))
#define PIO_WAIT(pol, src, idx) (uint16_t)(0x2000 | ((pol) << 7) | ((src) << 5) | ((idx) & 31))
#define PIO_IN(src, n)          (uint16_t)(0x4000 | ((src) << 5) | ((n) & 31))
#define PIO_OUT(dest, n)        (uint16_t)(0x6000 | ((dest) << 5) | ((n) & 31))
#define PIO_PUSH(iffull, block) (uint16_t)(0x8000 | ((iffull) << 6) | ((block) << 5))
#define PIO_PULL(ifempty, block)(uint16_t)(0x8080 | ((ifempty) << 6) | ((block) << 5))
#define PIO_MOV(dest, op, src)  (uint16_t)(0xA000 | ((dest) << 5) | ((op) << 3) | (src))
#define PIO_IRQ(clr, wait, idx) (uint16_t)(0xC000 | ((clr) << 6) | ((wait) << 5) | ((idx) & 31))
#define PIO_SET(dest, data)     (uint16_t)(0xE000 | ((dest) << 5) | ((data) & 31))
#define PIO_NOP                 PIO_MOV(PIO_Y, 0, PIO_Y)
#define PIO_SIDE(n, v)          (uint16_t)((v) << (13 - (n)))   // n side-set bits
#define PIO_DELAY(d)            (uint16_t)(((d) & 31) << 8)

// ===================================================================================
// PIO Block Functions
// ===================================================================================
#define PIO_init(pio)           RESET_cycle((pio) == PIO0 ? RESETS_RESET_PIO0 :            \
                                            (pio) == PIO1 ? RESETS_RESET_PIO1 : RESETS_RESET_PIO2)
#define PIO_pin(pio, PIN)       PIN_alternate(PIN, (pio) == PIO0 ? GPIO_FUNC_PIO0 :         \
                                                   (pio) == PIO1 ? GPIO_FUNC_PIO1 : GPIO_FUNC_PIO2)

// Load program into instruction memory at offset (JMP targets are relocated)
static inline void PIO_load(PIO_TypeDef* pio, const uint16_t* prog, uint8_t len, uint8_t offs) {
  uint8_t i;
  for(i = 0; i < len; i++) {
    uint16_t instr = prog[i];
    if((instr & 0xE000) == 0x0000) instr = (instr & ~31) | ((instr + offs) & 31); // JMP
    pio->INSTR_MEM[(offs + i) & 31] = instr;
  }
}

// ===================================================================================
// State Machine Functions
// ===================================================================================
#define PIO_SM_enable(pio, sm)        REG_SET((pio)->CTRL, (uint32_t)1 << (sm))
#define PIO_SM_disable(pio, sm)       REG_CLR((pio)->CTRL, (uint32_t)1 << (sm))
#define PIO_SM_restart(pio, sm)       REG_SET((pio)->CTRL, (uint32_t)1 << (PIO_CTRL_SM_RESTART_Pos + (sm)))
#define PIO_SM_exec(pio, sm, instr)   ((pio)->SM[sm].INSTR = (instr))
#define PIO_SM_txFull(pio, sm)        ((pio)->FSTAT & ((uint32_t)1 << (PIO_FSTAT_TXFULL_Pos + (sm))))
#define PIO_SM_rxEmpty(pio, sm)       ((pio)->FSTAT & ((uint32_t)1 << (PIO_FSTAT_RXEMPTY_Pos + (sm))))
#define PIO_SM_setWrap(pio, sm, b, t) ((pio)->SM[sm].EXECCTRL = ((pio)->SM[sm].EXECCTRL     \
                                      & ~(PIO_SM_EXECCTRL_WRAP_TOP_Msk | PIO_SM_EXECCTRL_WRAP_BOTTOM_Msk)) \
                                      | ((uint32_t)(t) << PIO_SM_EXECCTRL_WRAP_TOP_Pos)    \
                                      | ((uint32_t)(b) << PIO_SM_EXECCTRL_WRAP_BOTTOM_Pos))

// Set state machine clock frequency (16.8 fractional divider of the system clock)
static inline void PIO_SM_setClock(PIO_TypeDef* pio, uint8_t sm, uint32_t freq) {
  uint32_t div = (uint32_t)(((uint64_t)F_CPU * 256 + freq / 2) / freq);  // 16.8 format
  if(div < 256) div = 256;
  pio->SM[sm].CLKDIV = div << PIO_SM_CLKDIV_FRAC_Pos;
}

// Write 32-bit word into TX FIFO (blocking)
static inline void PIO_SM_put(PIO_TypeDef* pio, uint8_t sm, uint32_t data) {
  while(PIO_SM_txFull(pio, sm));
  pio->TXF[sm] = data;
}

// Read 32-bit word from RX FIFO (blocking)
static inline uint32_t PIO_SM_get(PIO_TypeDef* pio, uint8_t sm) {
  while(PIO_SM_rxEmpty(pio, sm));
  return pio->RXF[sm];
}

#ifdef __cplusplus
};
#endif
