// ===================================================================================
// Multicore Functions for RP2040 (core 1, inter-core FIFO, spinlocks)       * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// CORE1_launch(f)          reset core 1 and start function f on core 1
// CORE1_reset()            reset core 1 (core 1 waits in the boot ROM)
//
// FIFO_write(d)            send 32-bit word to the other core (blocking)
// FIFO_read()              receive 32-bit word from the other core (blocking)
// FIFO_available()         check if a word from the other core is waiting
// FIFO_ready()             check if a word can be sent (FIFO not full)
// FIFO_clear()             discard all received words, clear error flags
// FIFO_INT_enable()        enable FIFO interrupt of the calling core (SIO_IRQ_PROCn)
// FIFO_INT_disable()       disable FIFO interrupt of the calling core
//
// SPIN_lock(n)             acquire spinlock n (0..31), wait until available
// SPIN_tryLock(n)          try to acquire spinlock n, returns 1 if successful
// SPIN_unlock(n)           release spinlock n
//
// Notes:
// ------
// - Each core has its own NVIC, SysTick and interrupt enables. Interrupts must be
//   enabled on the core that should handle them (e.g. TIM_start() called by core 1
//   lets core 1 handle the timer interrupt). Both cores use the same vector table.
// - The functions of system.h (clock, RTC, watchdog etc.) should only be used by
//   core 0. Core 1 uses the system clock set up by core 0. DLY functions use the
//   SysTick of the respective core, which is enabled by CORE1_launch().
// - The stack size of core 1 can be set below. The stack of core 0 is at the top of
//   the SRAM (SRAM5/SRAM4).
// - The FIFOs are 8 words deep in each direction. Each core reads its own RX FIFO
//   and writes to the RX FIFO of the other core (SIO->FIFO_RD/FIFO_WR).
// - Spinlocks 16..31 are free for the user, 0..15 may be used by future libraries.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// Multicore parameters
#define CORE1_STACK_SIZE  2048            // stack size of core 1 in bytes

// Core 1 functions
void CORE1_launch(void (*func)(void));    // start function on core 1
void CORE1_reset(void);                   // reset core 1

// Inter-core FIFO functions
#define FIFO_available()  (SIO->FIFO_ST & SIO_FIFO_ST_VLD)
#define FIFO_ready()      (SIO->FIFO_ST & SIO_FIFO_ST_RDY)
#define FIFO_INT_enable()   NVIC_EnableIRQ(CPU_ID ? SIO_IRQ_PROC1_IRQn : SIO_IRQ_PROC0_IRQn)
#define FIFO_INT_disable()  NVIC_DisableIRQ(CPU_ID ? SIO_IRQ_PROC1_IRQn : SIO_IRQ_PROC0_IRQn)

static inline void FIFO_write(uint32_t data) {
  while(!FIFO_ready());
  SIO->FIFO_WR = data;
  __SEV();                                // wake up other core
}

static inline uint32_t FIFO_read(void) {
  while(!FIFO_available()) __WFE();
  return SIO->FIFO_RD;
}

static inline void FIFO_clear(void) {
  while(FIFO_available()) (void)SIO->FIFO_RD;
  SIO->FIFO_ST = 0xFF;                    // clear error flags
}

// Spinlock functions
#define SPIN_tryLock(n)   (SIO->SPINLOCK[n] != 0)
#define SPIN_unlock(n)    (__DMB(), SIO->SPINLOCK[n] = 0)
static inline void SPIN_lock(uint8_t n) {
  while(!SPIN_tryLock(n));
  __DMB();
}

#ifdef __cplusplus
};
#endif
