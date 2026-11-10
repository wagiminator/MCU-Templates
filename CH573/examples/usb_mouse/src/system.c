// ===================================================================================
// Basic System Functions for CH573, CH571                                    * v1.0 *
// ===================================================================================
//
// This file must be included!!!!
//
// References:
// -----------
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
// - CH573 datasheet and EVT (StdPeriphDriver, startup file)
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "system.h"

// ===================================================================================
// Setup Microcontroller (this function is called automatically at startup)
// ===================================================================================
void SYS_init(void) {

  // Enable DC/DC converter
  #if SYS_DCDC_EN > 0
    DCDC_enable();
  #endif

  // Init system clock
  #if SYS_CLK_INIT > 0
    CLK_init();
  #endif

  // Init SYSTICK
  #if SYS_TICK_INIT > 0
    STK_init();
  #endif
}

// ===================================================================================
// System Clock Functions
// ===================================================================================

// Short delay loop at startup (clock independent enough for oscillator startup)
RAM_FUNC static void CLK_wait(uint32_t n) {
  while(n--) {__NOP(); __NOP();}
}

// Init system clock according to F_CPU (executed from RAM, as the flash timing changes)
RAM_FUNC void CLK_init(void) {
  SAFE_ACCESS R8_PLL_CONFIG &= ~(1 << 5);
  #if SYS_CLK_PLL
    if(!(R8_HFCK_PWR_CTRL & RB_CLK_PLL_PON)) {
      SAFE_ACCESS R8_HFCK_PWR_CTRL |= RB_CLK_PLL_PON;   // power on PLL
      CLK_wait(2000);                                   // wait for PLL to settle
    }
    SAFE_ACCESS {
      R16_CLK_SYS_CFG = SYS_CLK_CFG;                    // set clock source and divider
      __NOP(); __NOP(); __NOP(); __NOP();
    }
    SAFE_ACCESS R8_FLASH_CFG = 0x53;                    // set flash timing for PLL
  #else
    if(!(R8_HFCK_PWR_CTRL & RB_CLK_XT32M_PON)) {
      SAFE_ACCESS R8_HFCK_PWR_CTRL |= RB_CLK_XT32M_PON; // power on HSE
      CLK_wait(1200);                                   // wait for HSE to settle
    }
    SAFE_ACCESS {
      R16_CLK_SYS_CFG = SYS_CLK_CFG;                    // set clock source and divider
      __NOP(); __NOP(); __NOP(); __NOP();
    }
    SAFE_ACCESS R8_FLASH_CFG = 0x51;                    // set flash timing for HSE
  #endif
}

// Reset system clock to default state (HSE/5 = 6.4MHz)
RAM_FUNC void CLK_reset(void) {
  SAFE_ACCESS R8_PLL_CONFIG &= ~(1 << 5);
  SAFE_ACCESS {
    R16_CLK_SYS_CFG = (0 << 6) | 5;
    __NOP(); __NOP(); __NOP(); __NOP();
  }
  SAFE_ACCESS R8_FLASH_CFG = 0x51;
}

// Get current system clock frequency in Hz
uint32_t CLK_getFreq(void) {
  uint8_t cfg = R16_CLK_SYS_CFG & 0xFF;
  uint8_t div = cfg & RB_CLK_PLL_DIV;
  if(!div) div = 32;
  switch(cfg & RB_CLK_SYS_MOD) {
    case (0 << 6): return 32000000 / div;               // divided from HSE
    case (1 << 6): return 480000000 / div;              // divided from PLL
    case (2 << 6): return 32000000;                     // directly from HSE
    default:       return 32768;                        // 32kHz clock
  }
}

// Enable DC/DC converter (only if the chip supports it)
void DCDC_enable(void) {
  uint32_t hw[2];
  FLASH_ROM_info(0x7F00C, hw);                          // read hardware configuration
  if(hw[0] & ((uint32_t)1 << 13)) return;               // DC/DC not available
  SAFE_ACCESS R16_POWER_PLAN |= RB_PWR_DCDC_EN | RB_PWR_DCDC_PRE;
}

// ===================================================================================
// Low-Speed 32kHz Clock (CLK32K) Functions
// ===================================================================================
uint16_t LSI_tune = 0;                                  // LSI calibration value

// Init 32kHz clock according to SYS_CLK32K
void CLK32K_init(void) {
  #if SYS_CLK32K > 0
    SAFE_ACCESS R8_CK32K_CONFIG |= RB_CLK_OSC32K_XT | RB_CLK_INT32K_PON | RB_CLK_XT32K_PON;
  #else
    SAFE_ACCESS R8_CK32K_CONFIG &= ~(RB_CLK_OSC32K_XT | RB_CLK_XT32K_PON);
    SAFE_ACCESS R8_CK32K_CONFIG |= RB_CLK_INT32K_PON;
    LSI_calibrate();
  #endif
}

// Measure system clock cycles during 5 cycles of the 32kHz clock (runs from RAM)
RAM_FUNC static uint16_t LSI_measure(uint16_t tune) {
  uint16_t cnt;
  SAFE_ACCESS R16_INT32K_TUNE = tune;
  while(!(R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT));          // discard current value
  cnt = R16_OSC_CAL_CNT;
  while(R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT);
  while(!(R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT));          // wait for new value
  cnt = R16_OSC_CAL_CNT & RB_OSC_CAL_CNT;
  return cnt;
}

// Calibrate internal 32kHz oscillator (LSI) to 32768Hz using the system clock
// (algorithm of the WCH StdPeriphDriver, without floating point)
void LSI_calibrate(void) {
  uint8_t  cfg = R16_CLK_SYS_CFG & 0xFF;
  uint8_t  div = cfg & RB_CLK_PLL_DIV;
  uint32_t freq;
  int16_t  step;                                        // step factor * 10 (negative)
  uint16_t target, cnt, tune, last = 0;
  int16_t  diff, lastdiff = 0, corr = 0;
  int32_t  next;
  uint8_t  k = 0;

  // Get expected count value and step factor depending on the system clock
  if(!div) div = 32;
  switch(cfg & RB_CLK_SYS_MOD) {
    case (1 << 6): freq = 480000000 / div; step = -div;      break;
    case (0 << 6): freq = 32000000 / div;  step = -16 * div; break;
    case (2 << 6): freq = 32000000;        step = -16;       break;
    default:       return;                              // 32kHz system clock: no calibration
  }
  target = (5 * freq + 16384) / 32768;

  // Approximate tuning value
  tune = LSI_tune ? LSI_tune : 2048;
  SAFE_ACCESS R8_OSC_CAL_CTRL = RB_OSC_CNT_EN;
  do {
    cnt  = LSI_measure(tune);
    diff = (int16_t)(cnt - target);
    LSI_tune = tune;
    k++;
    if(diff == 0) return;                               // perfect match
    if((diff ^ lastdiff) < 0 && lastdiff) {             // sign change
      if((corr >= -1) && (corr <= 1)) {                 // smallest step: take better value
        if((diff < 0 ? -diff : diff) > (lastdiff < 0 ? -lastdiff : lastdiff)) {
          LSI_tune = last;
          SAFE_ACCESS R16_INT32K_TUNE = last;
        }
        return;
      }
    }
    if(((uint32_t)(diff < 0 ? -diff : diff) * 10000 / target) < 11) return; // < 0.11%
    lastdiff = diff;
    last     = tune;
    corr     = (int16_t)((int32_t)diff * step / 10);
    next     = (int32_t)tune - corr;
    if(next < 0) next = 0;
    if(next > RB_INT32K_TUNE) next = RB_INT32K_TUNE;
    tune     = next;
    if(tune == last) tune += (diff > 0) ? 1 : -1;
  } while(k < 20);
}

// ===================================================================================
// Delay Functions
// ===================================================================================

// Wait n counts of SysTick (SysTick is counting down)
void DLY_ticks(uint32_t n) {
  uint32_t end = STK->CNTL - n;
  while(((int32_t)(STK->CNTL - end)) > 0);
}

// ===================================================================================
// Reset (RST) Functions
// ===================================================================================

// Perform software reset
RAM_FUNC void RST_now(void) {
  FLASH_ROM_reset();
  SAFE_ACCESS R8_RST_WDOG_CTRL |= RB_SOFTWARE_RESET;
  while(1);
}

// ===================================================================================
// Watchdog Timer (WDT) Functions
// ===================================================================================
uint8_t WDT_value;                                      // watchdog reload value

// ===================================================================================
// Real-Time Clock (RTC) Functions
// ===================================================================================

// Init 32kHz clock and start RTC
void RTC_init(void) {
  CLK32K_init();
}

// Wait until 32kHz clock pin is high and stable (required before loading RTC)
static void RTC_sync(void) {
  volatile uint8_t pin;
  do {
    pin = R8_CK32K_CONFIG & RB_32K_CLK_PIN;
  } while((pin != (R8_CK32K_CONFIG & RB_32K_CLK_PIN)) || (!pin));
}

// Set time of day (hours, minutes, seconds)
void RTC_setTime(uint8_t h, uint8_t m, uint8_t s) {
  uint32_t t = ((uint32_t)(h % 24) * 1800 + (uint32_t)m * 30 + s / 2) << 16;
  if(s & 1) t |= 0x8000;
  RTC_sync();
  SAFE_ACCESS {
    R32_RTC_TRIG = t;
    R8_RTC_MODE_CTRL |= RB_RTC_LOAD_LO;
  }
}

// Set day counter
void RTC_setDay(uint16_t d) {
  RTC_sync();
  SAFE_ACCESS {
    R32_RTC_TRIG = d & 0x3FFF;
    R8_RTC_MODE_CTRL |= RB_RTC_LOAD_HI;
  }
}

// Get 32768Hz ticks since midnight
uint32_t RTC_getTicks(void) {
  volatile uint32_t t;
  do {
    t = R32_RTC_CNT_32K;
  } while(t != R32_RTC_CNT_32K);
  return t;
}

// Get time of day in seconds
uint32_t RTC_getTime(void) {
  uint32_t t = RTC_getTicks();
  return (t >> 16) * 2 + ((t & 0xFFFF) >= 0x8000);
}

// Set trigger (alarm) n ticks from now
void RTC_TRIG_set(uint32_t n) {
  uint32_t t = RTC_getTicks() + n;
  if(t >= RTC_TICKS_DAY) t -= RTC_TICKS_DAY;
  SAFE_ACCESS {
    R32_RTC_TRIG = t;
    R8_RTC_MODE_CTRL = (R8_RTC_MODE_CTRL & ~(RB_RTC_LOAD_LO | RB_RTC_LOAD_HI)) | RB_RTC_TRIG_EN;
  }
}

// ===================================================================================
// Sleep Functions
// ===================================================================================

// Prepare oscillators for deep sleep (fast restart), close flash access
RAM_FUNC static void SLEEP_prepare(void) {
  uint8_t x32k = R8_XT32K_TUNE;
  uint8_t x32m = (R8_XT32M_TUNE & 0xFC) | 0x03;
  FLASH_ROM_reset();
  R8_FLASH_CTRL = 0x04;
  if(R16_RTC_CNT_32K > 0x3FFF) x32k = (x32k & 0xFC) | 0x01;
  SAFE_ACCESS R8_BAT_DET_CTRL = 0;
  SAFE_ACCESS {
    R8_XT32K_TUNE = x32k;
    R8_XT32M_TUNE = x32m;
  }
}

// Stop CPU, wake up by interrupt
RAM_FUNC void IDLE_now(void) {
  FLASH_ROM_reset();
  R8_FLASH_CTRL = 0x04;
  NVIC->SCTLR &= ~PFIC_SLEEPDEEP;
  __WFI();
  __NOP(); __NOP();
}

// Stop CPU and clocks (except 32kHz), wake up by enabled event
RAM_FUNC void HALT_now(void) {
  SLEEP_prepare();
  SAFE_ACCESS R8_PLL_CONFIG |= (1 << 5);
  NVIC->SCTLR |= PFIC_SLEEPDEEP;
  __WFI();
  __NOP(); __NOP();
  SAFE_ACCESS R8_PLL_CONFIG &= ~(1 << 5);
}

// Power down peripherals (RAM is kept), wake up by enabled event
RAM_FUNC void SLEEP_now(void) {
  SLEEP_prepare();
  NVIC->SCTLR |= PFIC_SLEEPDEEP;
  SAFE_ACCESS R8_SLP_POWER_CTRL |= RB_RAM_RET_LV;
  SAFE_ACCESS R8_PLL_CONFIG |= (1 << 5);
  SAFE_ACCESS R16_POWER_PLAN = RB_PWR_PLAN_EN | RB_PWR_MUST_0010 | RB_PWR_CORE
                             | RB_PWR_RAM2K | RB_PWR_RAM16K | RB_PWR_EXTEND;
  __WFI();
  __NOP(); __NOP();
  SAFE_ACCESS R8_PLL_CONFIG &= ~(1 << 5);
  CLK_wait(70 * (F_CPU / 1000000) / 2);                 // wait >70us for HSE to stabilize
}

// Power down everything, wake-up causes a reset
RAM_FUNC void SHUTDOWN_now(void) {
  SLEEP_prepare();
  CLK_reset();
  NVIC->SCTLR |= PFIC_SLEEPDEEP;
  SAFE_ACCESS R8_SLP_POWER_CTRL |= RB_RAM_RET_LV;
  SAFE_ACCESS R16_POWER_PLAN = RB_PWR_PLAN_EN | RB_PWR_MUST_0010;
  __WFI();
  __NOP(); __NOP();
  while(1);
}

// Put device into SLEEP or HALT for n milliseconds (uses RTC trigger)
static void SLEEP_RTC(uint32_t n, void (*sleepfunc)(void)) {
  uint32_t irq = NVIC_GetStatusIRQ(RTC_IRQn);
  INT_ATOMIC_BLOCK {
    RTC_TRIG_clear();
    RTC_TRIG_set(RTC_MS_TICKS(n));
    WAKE_RTC_enable();
    NVIC_EnableIRQ(RTC_IRQn);
    sleepfunc();
    RTC_TRIG_stop();
    RTC_TRIG_clear();
    NVIC_ClearPendingIRQ(RTC_IRQn);
    if(!irq) NVIC_DisableIRQ(RTC_IRQn);
  }
}

void SLEEP_ms(uint32_t n) { SLEEP_RTC(n, SLEEP_now); }
void HALT_ms(uint32_t n)  { SLEEP_RTC(n, HALT_now);  }

// ===================================================================================
// Low-Level Flash ROM Access (reconstructed from the WCH ISP library)
// ===================================================================================
static uint32_t FLASH_irq;                              // saved interrupt status

// Start flash command
RAM_FUNC void FLASH_ROM_begin(uint8_t cmd) {
  R8_FLASH_CTRL = 0;
  R8_FLASH_CTRL = 0x05;
  __NOP();
  R8_FLASH_DATA = cmd;
}

// End flash command
RAM_FUNC void FLASH_ROM_end(void) {
  while((int8_t)R8_FLASH_CTRL < 0);
  R8_FLASH_CTRL = 0;
}

// Read byte from flash
RAM_FUNC uint8_t FLASH_ROM_in(void) {
  while((int8_t)R8_FLASH_CTRL < 0);
  return R8_FLASH_DATA;
}

// Write byte to flash
RAM_FUNC void FLASH_ROM_out(uint8_t data) {
  while((int8_t)R8_FLASH_CTRL < 0);
  R8_FLASH_DATA = data;
}

// Send command with address (write enable for erase/write commands)
RAM_FUNC void FLASH_ROM_addr(uint8_t cmd, uint32_t addr) {
  uint8_t n = 5;
  if((cmd & 0xBF) != 0x0B) {                            // not a read command:
    FLASH_ROM_begin(0x06);                              // write enable
    FLASH_ROM_end();
    n = 3;
  }
  FLASH_ROM_begin(cmd);
  while(n--) {
    FLASH_ROM_out((uint8_t)(addr >> 16));
    addr <<= 8;
  }
}

// Wait for erase/write to finish, returns 0 on timeout
RAM_FUNC uint8_t FLASH_ROM_wait(void) {
  uint8_t  status;
  uint32_t timeout = 0x80000;
  FLASH_ROM_end();
  do {
    FLASH_ROM_begin(0x05);                              // read status register
    FLASH_ROM_in();
    status = FLASH_ROM_in();
    FLASH_ROM_end();
    if(!(status & 0x01)) return status | 0x01;          // not busy anymore
  } while(--timeout);
  return 0;
}

// Open flash access (mode: RB_ROM_CTRL_EN for read, additionally RB_ROM_DATA_WE and
// RB_ROM_CODE_WE for erase/write), disables interrupts
RAM_FUNC void FLASH_ROM_open(uint8_t mode) {
  FLASH_irq = __get_MSTATUS() & 0x08;
  __disable_irq();
  SAFE_ACCESS R8_GLOB_ROM_CFG |= mode;
  R8_FLASH_CTRL = 0x04;
  FLASH_ROM_begin(0xFF);
  FLASH_ROM_end();
}

// Close flash access, restores interrupts
RAM_FUNC void FLASH_ROM_close(void) {
  FLASH_ROM_end();
  SAFE_ACCESS R8_GLOB_ROM_CFG &= RB_ROM_CODE_OFS;
  if(FLASH_irq) __enable_irq();
}

// Software reset of flash ROM
RAM_FUNC void FLASH_ROM_reset(void) {
  FLASH_ROM_open(RB_ROM_CTRL_EN);
  FLASH_ROM_begin(0x66);
  FLASH_ROM_end();
  FLASH_ROM_begin(0x99);
  FLASH_ROM_close();
}

// ===================================================================================
// Device Electronic Signature (ESIG) Functions
// ===================================================================================

// Read 8 bytes of chip information at addr (bit 13 of addr set: only 6 bytes)
RAM_FUNC void FLASH_ROM_info(uint32_t addr, uint32_t* buf) {
  uint32_t v;
  FLASH_ROM_open(RB_ROM_CTRL_EN);
  FLASH_ROM_addr(0x0B, addr);
  for(uint8_t i=0; i<8; i++) {
    FLASH_ROM_in();
    if(i == 3) buf[0] = R32_FLASH_DATA;
  }
  v = R32_FLASH_DATA;
  if(addr & ((uint32_t)1 << 13)) ((uint16_t*)buf)[2] = (uint16_t)v;
  else buf[1] = v;
  FLASH_ROM_close();
}

// Read 8-byte unique ID
RAM_FUNC void ESIG_getUID(uint8_t* buf) {
  uint8_t i;
  for(i=0; i<8; i++) buf[i] = 0;
  FLASH_ROM_open(RB_ROM_CTRL_EN);
  FLASH_ROM_addr(0x4B, 0);
  for(i=16; i; i--) buf[(i - 1) & 7] ^= FLASH_ROM_in();
  FLASH_ROM_close();
}

// Read 6-byte MAC address (LSB first)
void ESIG_getMAC(uint8_t* buf) {
  uint32_t info[2];
  FLASH_ROM_info(0x7F018, info);
  for(uint8_t i=0; i<6; i++) buf[i] = ((uint8_t*)info)[i];
}

// ===================================================================================
// C++ Support
// ===================================================================================
#ifdef __cplusplus
extern void __cxa_pure_virtual() { while (1); }
extern void (*__preinit_array_start[]) (void) __attribute__((weak));
extern void (*__preinit_array_end[]) (void) __attribute__((weak));
extern void (*__init_array_start[]) (void) __attribute__((weak));
extern void (*__init_array_end[]) (void) __attribute__((weak));

void __libc_init_array(void) {
  uint32_t count, i;
  count = __preinit_array_end - __preinit_array_start;
  for (i = 0; i < count; i++) __preinit_array_start[i]();
  count = __init_array_end - __init_array_start;
  for (i = 0; i < count; i++) __init_array_start[i]();
}
#endif

// ===================================================================================
// C version of CH573 Startup .s file from WCH
// ===================================================================================
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _data_lma;
extern uint32_t _data_vma;
extern uint32_t _edata;
extern uint32_t _highcode_lma;
extern uint32_t _highcode_vma_start;
extern uint32_t _highcode_vma_end;

// Prototypes
int main(void)                __attribute__((section(".text.main"), used));
void vectors(void)            __attribute__((section(".vector"), naked, used));
void reset_handler(void)      __attribute__((section(".text.reset_handler"), naked, used));
void SYS_startup(void)        __attribute__((section(".text.reset_handler"), noreturn, used));

#if SYS_USE_VECTORS > 0
// Unless a specific handler is overridden, it just spins forever
void default_handler(void)    __attribute__((section(".text.vector_handler"), naked, used));
void default_handler(void)    { while(1); }

// All interrupt handlers are aliased to default_handler unless overridden individually
#define DUMMY_HANDLER __attribute__((section(".text.vector_handler"), weak, alias("default_handler"), used))
DUMMY_HANDLER void NMI_Handler(void);
DUMMY_HANDLER void HardFault_Handler(void);
DUMMY_HANDLER void SysTick_Handler(void);
DUMMY_HANDLER void SW_Handler(void);
DUMMY_HANDLER void TMR0_IRQHandler(void);
DUMMY_HANDLER void GPIOA_IRQHandler(void);
DUMMY_HANDLER void GPIOB_IRQHandler(void);
DUMMY_HANDLER void SPI0_IRQHandler(void);
DUMMY_HANDLER void BB_IRQHandler(void);
DUMMY_HANDLER void LLE_IRQHandler(void);
DUMMY_HANDLER void USB_IRQHandler(void);
DUMMY_HANDLER void TMR1_IRQHandler(void);
DUMMY_HANDLER void TMR2_IRQHandler(void);
DUMMY_HANDLER void UART0_IRQHandler(void);
DUMMY_HANDLER void UART1_IRQHandler(void);
DUMMY_HANDLER void RTC_IRQHandler(void);
DUMMY_HANDLER void ADC_IRQHandler(void);
DUMMY_HANDLER void PWMX_IRQHandler(void);
DUMMY_HANDLER void TMR3_IRQHandler(void);
DUMMY_HANDLER void UART2_IRQHandler(void);
DUMMY_HANDLER void UART3_IRQHandler(void);
DUMMY_HANDLER void WDOG_BAT_IRQHandler(void);
#endif  // SYS_USE_VECTORS > 0

// Interrupt vector table at start of flash. The CH573 (QingKe V3A) uses jump
// instructions in the vector table (mtvec mode 1), so every handler must be within
// +/-1MB of the table (in flash).
void vectors(void) {
  asm volatile(
  " .option push              \n\
    .option norvc             \n\
    j reset_handler           \n"   //  0 - Reset
    #if SYS_USE_VECTORS > 0
  " .word 0                   \n\
    j NMI_Handler             \n\
    j HardFault_Handler       \n\
    .word 0                   \n\
    .word 0                   \n\
    .word 0                   \n\
    .word 0                   \n\
    .word 0                   \n\
    .word 0                   \n\
    .word 0                   \n\
    .word 0                   \n\
    j SysTick_Handler         \n\
    .word 0                   \n\
    j SW_Handler              \n\
    .word 0                   \n\
    j TMR0_IRQHandler         \n\
    j GPIOA_IRQHandler        \n\
    j GPIOB_IRQHandler        \n\
    j SPI0_IRQHandler         \n\
    j BB_IRQHandler           \n\
    j LLE_IRQHandler          \n\
    j USB_IRQHandler          \n\
    .word 0                   \n\
    j TMR1_IRQHandler         \n\
    j TMR2_IRQHandler         \n\
    j UART0_IRQHandler        \n\
    j UART1_IRQHandler        \n\
    j RTC_IRQHandler          \n\
    j ADC_IRQHandler          \n\
    .word 0                   \n\
    j PWMX_IRQHandler         \n\
    j TMR3_IRQHandler         \n\
    j UART2_IRQHandler        \n\
    j UART3_IRQHandler        \n\
    j WDOG_BAT_IRQHandler     \n"
    #endif  // SYS_USE_VECTORS > 0
  " .option pop               \n"
  );
}
//  1 - Reserved,  2 - NMI,  3 - HardFault,  4..11 - Reserved, 12 - SysTick,
// 13 - Reserved, 14 - SW, 15 - Reserved, 16 - TMR0, 17 - GPIOA, 18 - GPIOB,
// 19 - SPI0, 20 - BLE BB, 21 - BLE LLE, 22 - USB, 23 - Reserved, 24 - TMR1,
// 25 - TMR2, 26 - UART0, 27 - UART1, 28 - RTC, 29 - ADC/TouchKey, 30 - Reserved,
// 31 - PWMX, 32 - TMR3, 33 - UART2, 34 - UART3, 35 - Watchdog/Battery

// Reset handler: set global pointer and stack pointer, then continue in C
void reset_handler(void) {
  asm volatile(
  " .option push              \n\
    .option norelax           \n\
    la gp, __global_pointer$  \n\
    .option pop               \n\
    la sp, _eusrstack         \n\
    j SYS_startup             \n"
  );
}

// Startup: copy code and data to RAM, init system, set vectors and start main()
void SYS_startup(void) {
  uint32_t *src, *dst;

  // Copy code for RAM (highcode) from FLASH to RAM
  src = &_highcode_lma;
  dst = &_highcode_vma_start;
  while(dst < &_highcode_vma_end) *dst++ = *src++;

  // Copy data from FLASH to RAM
  src = &_data_lma;
  dst = &_data_vma;
  while(dst < &_edata) *dst++ = *src++;

  // Clear uninitialized variables
  #if SYS_CLEAR_BSS > 0
  dst = &_sbss;
  while(dst < &_ebss) *dst++ = 0;
  #endif

  // C++ Support
  #ifdef __cplusplus
  __libc_init_array();
  #endif

  // Init system
  SYS_init();

  // Set vectors (mode 1: jump instructions) and processor status, start main()
  asm volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr     \n"
    #endif
  " la a0, vectors            \n\
    ori a0, a0, 1             \n\
    csrw mtvec, a0            \n\
    li a0, 0x1888             \n\
    csrs mstatus, a0          \n\
    csrw mepc, %[main]        \n\
    mret                      \n"
    : : [main] "r" (main) : "a0", "memory"
  );
  while(1);
}
