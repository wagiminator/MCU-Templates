// ===================================================================================
// Basic Timer Functions (TMR0..TMR3, 26-bit) for CH573/CH571                 * v1.0 *
// ===================================================================================
//
// Functions available (x = 0..3):
// -------------------------------
// TMRx_start(n)            start timer with period of n system clock ticks (2..2^26)
// TMRx_stop()              stop timer
// TMRx_read()              read current counter value
// TMRx_setPeriod(n)        set period (n system clock ticks)
//
// TMRx_INT_enable()        enable timer period end interrupt
// TMRx_INT_disable()       disable timer period end interrupt
// TMRx_flag()              check timer period end flag
// TMRx_clear()             clear timer period end flag
// TMRx_ISR { }             timer interrupt service routine (call TMRx_clear())
//
// TMRx_PWM_start(n, d)     start PWM output on timer pin, period n ticks, d high ticks
// TMRx_PWM_write(d)        set PWM duty (high ticks)
//
// Timer pins (for PWM, set below in timer parameters):
// ----------------------------------------------------
// TMRx_REMAP    TMR0   TMR1   TMR2   TMR3
//         0     PA9    PA10   PA11   PB22
//         1     PB23   PB10   PB11
//
// Notes:
// ------
// - The timers count with F_CPU, e.g. n = F_CPU / 1000 for a period of 1ms.
// - TMR1 and TMR2 additionally support DMA, TMR0..3 also capture/count (not used here).
// - The CH571 has only TMR0..TMR2 (TMR3 is missing on some packages).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"
#include "gpio.h"

// Timer Parameters
#define TMR0_REMAP         0           // TMR0 pin mapping (see above)
#define TMR1_REMAP         0           // TMR1 pin mapping (see above)
#define TMR2_REMAP         0           // TMR2 pin mapping (see above)

// Timer Macros
#define TMR0_start(n)       {R32_TMR0_CNT_END = (n); R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR; \
                               R8_TMR0_CTRL_MOD = RB_TMR_COUNT_EN;}
#define TMR0_stop()         R8_TMR0_CTRL_MOD &= ~RB_TMR_COUNT_EN
#define TMR0_read()         R32_TMR0_COUNT
#define TMR0_setPeriod(n)   R32_TMR0_CNT_END = (n)
#define TMR0_INT_enable()   {R8_TMR0_INTER_EN |= RB_TMR_IE_CYC_END; NVIC_EnableIRQ(TMR0_IRQn);}
#define TMR0_INT_disable()  {R8_TMR0_INTER_EN &= ~RB_TMR_IE_CYC_END; NVIC_DisableIRQ(TMR0_IRQn);}
#define TMR0_flag()         (R8_TMR0_INT_FLAG & RB_TMR_IF_CYC_END)
#define TMR0_clear()        R8_TMR0_INT_FLAG = RB_TMR_IF_CYC_END
#define TMR0_ISR            void TMR0_IRQHandler(void) __attribute__((interrupt)); \
                             void TMR0_IRQHandler(void)
#define TMR0_PWM_write(d)   R32_TMR0_FIFO = (d)

#define TMR1_start(n)       {R32_TMR1_CNT_END = (n); R8_TMR1_CTRL_MOD = RB_TMR_ALL_CLEAR; \
                               R8_TMR1_CTRL_MOD = RB_TMR_COUNT_EN;}
#define TMR1_stop()         R8_TMR1_CTRL_MOD &= ~RB_TMR_COUNT_EN
#define TMR1_read()         R32_TMR1_COUNT
#define TMR1_setPeriod(n)   R32_TMR1_CNT_END = (n)
#define TMR1_INT_enable()   {R8_TMR1_INTER_EN |= RB_TMR_IE_CYC_END; NVIC_EnableIRQ(TMR1_IRQn);}
#define TMR1_INT_disable()  {R8_TMR1_INTER_EN &= ~RB_TMR_IE_CYC_END; NVIC_DisableIRQ(TMR1_IRQn);}
#define TMR1_flag()         (R8_TMR1_INT_FLAG & RB_TMR_IF_CYC_END)
#define TMR1_clear()        R8_TMR1_INT_FLAG = RB_TMR_IF_CYC_END
#define TMR1_ISR            void TMR1_IRQHandler(void) __attribute__((interrupt)); \
                             void TMR1_IRQHandler(void)
#define TMR1_PWM_write(d)   R32_TMR1_FIFO = (d)

#define TMR2_start(n)       {R32_TMR2_CNT_END = (n); R8_TMR2_CTRL_MOD = RB_TMR_ALL_CLEAR; \
                               R8_TMR2_CTRL_MOD = RB_TMR_COUNT_EN;}
#define TMR2_stop()         R8_TMR2_CTRL_MOD &= ~RB_TMR_COUNT_EN
#define TMR2_read()         R32_TMR2_COUNT
#define TMR2_setPeriod(n)   R32_TMR2_CNT_END = (n)
#define TMR2_INT_enable()   {R8_TMR2_INTER_EN |= RB_TMR_IE_CYC_END; NVIC_EnableIRQ(TMR2_IRQn);}
#define TMR2_INT_disable()  {R8_TMR2_INTER_EN &= ~RB_TMR_IE_CYC_END; NVIC_DisableIRQ(TMR2_IRQn);}
#define TMR2_flag()         (R8_TMR2_INT_FLAG & RB_TMR_IF_CYC_END)
#define TMR2_clear()        R8_TMR2_INT_FLAG = RB_TMR_IF_CYC_END
#define TMR2_ISR            void TMR2_IRQHandler(void) __attribute__((interrupt)); \
                             void TMR2_IRQHandler(void)
#define TMR2_PWM_write(d)   R32_TMR2_FIFO = (d)

#define TMR3_start(n)       {R32_TMR3_CNT_END = (n); R8_TMR3_CTRL_MOD = RB_TMR_ALL_CLEAR; \
                               R8_TMR3_CTRL_MOD = RB_TMR_COUNT_EN;}
#define TMR3_stop()         R8_TMR3_CTRL_MOD &= ~RB_TMR_COUNT_EN
#define TMR3_read()         R32_TMR3_COUNT
#define TMR3_setPeriod(n)   R32_TMR3_CNT_END = (n)
#define TMR3_INT_enable()   {R8_TMR3_INTER_EN |= RB_TMR_IE_CYC_END; NVIC_EnableIRQ(TMR3_IRQn);}
#define TMR3_INT_disable()  {R8_TMR3_INTER_EN &= ~RB_TMR_IE_CYC_END; NVIC_DisableIRQ(TMR3_IRQn);}
#define TMR3_flag()         (R8_TMR3_INT_FLAG & RB_TMR_IF_CYC_END)
#define TMR3_clear()        R8_TMR3_INT_FLAG = RB_TMR_IF_CYC_END
#define TMR3_ISR            void TMR3_IRQHandler(void) __attribute__((interrupt)); \
                             void TMR3_IRQHandler(void)
#define TMR3_PWM_write(d)   R32_TMR3_FIFO = (d)

// Timer PWM Functions
void TMR0_PWM_start(uint32_t n, uint32_t d);
void TMR1_PWM_start(uint32_t n, uint32_t d);
void TMR2_PWM_start(uint32_t n, uint32_t d);
void TMR3_PWM_start(uint32_t n, uint32_t d);

#ifdef __cplusplus
};
#endif
