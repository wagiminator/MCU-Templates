// ===================================================================================
// Delay Functions for STC8H Microcontrollers                                 * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "delay.h"

void DLY_us_h(uint16_t n) {
  n;

  __asm
  
    mov r6,dpl
    mov r7,dph
    mov a,r6

    jz 01$
    inc r7

    01$:
    #if F_CPU % 3000000 >= 1000000
      nop
    #endif

    #if F_CPU % 3000000 >= 2000000
      nop
    #endif

    #if F_CPU >= 6000000
      mov r5,#((F_CPU / 3000000) - 1)
      02$:
      djnz r5, 02$
    #endif
    
    djnz r6, 01$
    djnz r7, 01$

  __endasm;
}

void DLY_ms_h(uint16_t n) {
  n;

  __asm
  
    mov r3,dpl
    mov r4,dph
    mov a,r3

    jz 03$
    inc r4

    03$:
    #if   F_CPU <=  187000
      mov	dptr,#0x0039
    #elif F_CPU <=  375000
      mov	dptr,#0x0078
    #elif F_CPU <= 1000000
      mov	dptr,#0x00f6
    #elif F_CPU <  3000000
      mov	dptr,#0x01f0
    #else
      mov	dptr,#0x03e8
    #endif

    lcall	_DLY_us_h
    
    djnz r3, 03$
    djnz r4, 03$

  __endasm;
}
