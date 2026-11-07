// ===================================================================================
// Delay Functions for CH558 and CH559                                        * v1.1 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "ch559.h"
#include "delay.h"

// ===================================================================================
// Delay Loop Parameters
// ===================================================================================
// One loop iteration takes DLY_NOPS + 5 cycles. A taken djnz takes 4 cycles plus one
// cycle each if its own address or the target address is odd. With an odd number of
// nops, exactly one of both addresses is odd, regardless of where the linker places
// the function, so djnz always takes 5 cycles.
#if   F_CPU == 56000000
  #define DLY_NOPS    51      // 56 cycles = 1us
  #define DLY_SHIFT   0
#elif F_CPU == 48000000
  #define DLY_NOPS    43      // 48 cycles = 1us
  #define DLY_SHIFT   0
#elif F_CPU == 32000000
  #define DLY_NOPS    27      // 32 cycles = 1us
  #define DLY_SHIFT   0
#elif F_CPU == 24000000
  #define DLY_NOPS    19      // 24 cycles = 1us
  #define DLY_SHIFT   0
#elif F_CPU == 16000000
  #define DLY_NOPS    11      // 16 cycles = 1us
  #define DLY_SHIFT   0
#elif F_CPU == 12000000
  #define DLY_NOPS    7       // 12 cycles = 1us
  #define DLY_SHIFT   0
#elif F_CPU == 8000000
  #define DLY_NOPS    3       //  8 cycles = 1us
  #define DLY_SHIFT   0
#elif F_CPU == 6000000
  #define DLY_NOPS    1       //  6 cycles = 1us
  #define DLY_SHIFT   0
#elif F_CPU == 4000000
  #define DLY_NOPS    3       //  8 cycles = 2us
  #define DLY_SHIFT   1
#elif F_CPU == 3000000
  #define DLY_NOPS    7       // 12 cycles = 4us
  #define DLY_SHIFT   2
#else
  #error Unsupported F_CPU for delay functions
#endif

// ===================================================================================
// Delay in Units of us
// ===================================================================================
void DLY_us(uint16_t n) {
  n;                          // stop unreferenced argument warning
  __asm
    .rept DLY_SHIFT           ; divide n for low system clock
    clr  c
    mov  a, dph
    rrc  a
    mov  dph, a
    mov  a, dpl
    rrc  a
    mov  dpl, a
    .endm
    mov  a, dpl
    orl  a, dph
    jz   99$                  ; n == 0 -> return
    mov  r6, dpl              ; r6 <- low byte
    mov  r7, dph              ; r7 <- high byte
    mov  a, r6
    jz   01$                  ; low byte != 0 -> one more outer loop
    inc  r7
  01$:
    .rept DLY_NOPS
    nop
    .endm
    djnz r6, 01$              ; inner loop
    djnz r7, 01$              ; outer loop
  99$:
  __endasm;
}

// ===================================================================================
// Delay in Units of ms
// ===================================================================================
void DLY_ms(uint16_t n) {
  while(n--) DLY_us(1000);
}
