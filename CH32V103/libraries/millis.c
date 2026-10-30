// ===================================================================================
// Millis Functions for CH32V103                                              * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "millis.h"

volatile uint32_t MIL_millis = 0;   // millis counter
static uint64_t   MIL_cmp;          // current SysTick compare value

// Write 64-bit SysTick compare value (registers can only be written byte by byte)
static void MIL_setCMP(uint64_t val) {
  __IO uint8_t* cmp = (__IO uint8_t*)&STK->CMPL;
  uint8_t i;
  for(i=8; i; i--, val>>=8) *cmp++ = (uint8_t)val;  // CMPL (byte 0..3), CMPH (4..7)
}

// Read 64-bit SysTick counter value
static uint64_t MIL_getCNT(void) {
  uint32_t hi, lo;
  do {
    hi = STK->CNTH;
    lo = STK->CNTL;
  } while(hi != STK->CNTH);         // repeat if low word overflowed meanwhile
  return ((uint64_t)hi << 32) | lo;
}

// Init millis counter
void MIL_init(void) {
  MIL_millis = 0;
  MIL_cmp = MIL_getCNT() + DLY_MS_TIME;   // first interrupt 1ms later
  MIL_setCMP(MIL_cmp);
  STK->CTLR = STK_CTLR_STE;               // make sure SysTick is running (HCLK/8)
  NVIC_EnableIRQ(SysTicK_IRQn);           // enable the SysTick IRQ
}

// Read millis counter value
uint32_t MIL_read(void) {
  return MIL_millis;
}

// Reset millis counter value
void MIL_reset(void) {
  MIL_millis = 0;
}

// Interrupt service routine (triggered when SysTick counter equals compare value)
void SysTick_Handler(void) __attribute__((interrupt));
void SysTick_Handler(void) {
  uint64_t now = MIL_getCNT();
  do {
    MIL_millis++;                         // increase millis counter
    MIL_cmp += DLY_MS_TIME;               // next interrupt 1ms later
  } while((int64_t)(MIL_cmp - now) <= 16);// catch up if interrupt was delayed (margin
                                          // for the byte-wise register write)
  MIL_setCMP(MIL_cmp);
}
