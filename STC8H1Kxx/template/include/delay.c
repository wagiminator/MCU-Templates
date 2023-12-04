// ===================================================================================
// Delay Functions for STC8H Microcontrollers                                 * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "delay.h"

void DLY_us(uint16_t n) {
  n;

  __asm

    #if F_CPU < 3000000
      mov a,dph
      clr c
      rrc a
      mov r7,a
      mov a,dpl
      rrc a
      mov r6,a

      #if F_CPU <= 1000000
        mov a,r7
        clr c
        rrc a
        mov r7,a
        mov a,r6
        rrc a
        mov r6,a
      #endif

      #if F_CPU <= 375000
        mov a,r7
        clr c
        rrc a
        mov r7,a
        mov a,r6
        rrc a
        mov r6,a
      #endif

      #if F_CPU <= 187000
        mov a,r7
        clr c
        rrc a
        mov r7,a
        mov a,r6
        rrc a
        mov r6,a
      #endif

    #else
      mov	r6,dpl
	    mov	r7,dph
	    mov a,r6
    #endif

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

void DLY_ms(uint16_t n) {
  while(n) {
    DLY_us(1000);
    --n;
  }
}
