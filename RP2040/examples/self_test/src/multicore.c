// ===================================================================================
// Multicore Functions for RP2350 (core 1, inter-core FIFO, spinlocks)       * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "multicore.h"

static uint32_t CORE1_stack[CORE1_STACK_SIZE / 4] __attribute__((aligned(8)));
#if SPIN_LOCK_SW > 0
volatile uint8_t SPIN_locks[32];          // software spinlocks
#endif
static void (*CORE1_func)(void);
extern void (*const vectors[])(void);     // vector table (system.c)

// Entry point of core 1 (called by the boot ROM)
static void CORE1_entry(void) {
  __set_MSPLIM(0);                        // no stack limit
  #if SYS_FPU_EN > 0
  SCB->CPACR |= (uint32_t)0xF << 20;      // enable FPU of core 1
  __DSB();
  __ISB();
  #endif
  SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk; // SysTick for DLY
  CORE1_func();                           // start user function
  while(1) __WFE();                       // user function returned: sleep
}

// Reset core 1 (core 1 starts in the boot ROM and waits for the launch sequence)
void CORE1_reset(void) {
  REG_SET(PSM->FRCE_OFF, PSM_FRCE_OFF_PROC1);       // power off core 1
  while(!(PSM->FRCE_OFF & PSM_FRCE_OFF_PROC1));
  REG_CLR(PSM->FRCE_OFF, PSM_FRCE_OFF_PROC1);       // power on core 1
  (void)FIFO_read();                                // core 1 sends 0 when ready
}

// Reset core 1 and start function on core 1 (launch sequence of the boot ROM)
void CORE1_launch(void (*func)(void)) {
  const uint32_t cmd[] = {0, 0, 1, (uint32_t)vectors,
                          (uint32_t)&CORE1_stack[CORE1_STACK_SIZE / 4],
                          (uint32_t)CORE1_entry};
  uint8_t seq = 0;
  CORE1_func = func;
  CORE1_reset();
  while(seq < 6) {
    if(!cmd[seq]) {                       // drain FIFO before sending 0
      FIFO_clear();
      __SEV();
    }
    FIFO_write(cmd[seq]);
    seq = (FIFO_read() == cmd[seq]) ? seq + 1 : 0;  // core 1 echoes each word
  }
}
