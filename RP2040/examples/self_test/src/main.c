// ===================================================================================
// Project:   Raspberry Pi Pico Examples - Self Test
// Version:   v1.0
// Year:      2026
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Automatic test of the microcontroller and the template functions. The results are
// shown in the terminal (USB serial port). The tests start when the terminal is
// opened and can be repeated with Enter or the BOOTSEL button:
//
//    1. System clock       measures the CPU clock (SysTick) against the 1us timer
//                          (crystal), i.e. checks PLL and tick generator
//    2. Timer interrupt    periodic timer interrupt (timer library), 100 x 1ms
//    3. Real-time clock    RTC counts 2 seconds while the CPU sleeps (SLEEP_ms)
//    4. CRC / DMA          CRC-32 of "123456789" with the DMA sniffer
//    5. Flash              JEDEC ID, erase, write and read back the last sector
//    6. Dual core          core 1 answers via the inter-core FIFO, both cores
//                          increment a shared counter protected by a spinlock
//    7. PIO                a PIO program inverts and bit-reverses data words
//    8. Temperature        internal temperature sensor (ADC), plausibility check
//    9. VSYS               supply voltage via GP29 (ADC), plausibility check
//   10. PWM                LED fades in and out (PWM), the counter must run
//
// Afterwards, the LED is on if all tests have passed and blinks if a test has
// failed. Type "w" in the terminal to test the watchdog: the watchdog is started
// and not fed, so the board restarts after 1 second; after the restart, the self
// test shows "watchdog" as the reason of the last reset.
//
// Tested template functions: see above, plus USB device (CDC), BOOTSEL button,
// reset reasons and the watchdog.
//
// Settings in config.h: F_CPU, PIN_LED, TEST_FLASH (0: skip the flash write test).

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "board.h"                        // board support (LED, button, ADC, info)
#include "term.h"                         // terminal via USB serial port
#include "timer.h"                        // periodic timer interrupt
#include "flash.h"                        // flash functions
#include "multicore.h"                    // core 1, FIFO, spinlocks
#include "pio.h"                          // programmable I/O
#include "pwm.h"                          // PWM

uint8_t passed, total;                    // test counters

// Print test name
void testName(const char* name) {
  total++;
  TERM_printf("%2d. %s", total, name);
}

// Print test result
void testResult(uint8_t ok) {
  if(ok) passed++;
  TERM_print(ok ? "  OK\n" : "  FAILED\n");
}

// ===================================================================================
// Test 1: System Clock
// ===================================================================================
void testClock(void) {
  uint32_t t0, c0, c1, cycles, khz;
  testName("System clock ...... ");
  INT_ATOMIC_BLOCK {
    SysTick->LOAD = 0xFFFFFF;             // SysTick counts down with the CPU clock
    SysTick->VAL  = 0;
    t0 = MIC_read();
    while(MIC_read() == t0);              // synchronize with the 1us timer tick
    t0 = MIC_read();
    c0 = SysTick->VAL;
    while(MIC_read() - t0 < 10000);       // 10ms
    c1 = SysTick->VAL;
  }
  cycles = (c0 - c1) & 0xFFFFFF;
  khz = cycles / 10;
  TERM_printf("%d.%d MHz (expected %d MHz)", khz / 1000, (khz % 1000) / 100, F_CPU / 1000000);
  testResult((khz > F_CPU / 1000 * 99 / 100) && (khz < F_CPU / 1000 * 101 / 100));
}

// ===================================================================================
// Test 2: Timer Interrupt
// ===================================================================================
volatile uint32_t timerCount;
TIM_ISR { timerCount++; }

void testTimer(void) {
  testName("Timer interrupt ... ");
  TIM_init();
  timerCount = 0;
  TIM_start(1000);                        // interrupt every 1ms
  DLY_ms(100);
  TIM_stop();
  TERM_printf("%d interrupts in 100 ms", timerCount);
  testResult((timerCount >= 99) && (timerCount <= 101));
}

// ===================================================================================
// Test 3: Real-Time Clock and Sleep
// ===================================================================================
void testRTC(void) {
  uint32_t t, s0, s1, start;
  testName("Real-time clock ... ");
  if(!RTC_isInit()) RTC_init();
  t = RTC_readTime(); (void)RTC_readDate();
  s0 = RTC_getSeconds(t);
  start = BOARD_millis();
  do {                                    // wait for the next second
    t = RTC_readTime(); (void)RTC_readDate();
  } while((RTC_getSeconds(t) == s0) && (BOARD_millis() - start < 1500));
  s0 = RTC_getSeconds(t);
  SLEEP_ms(2100);                         // sleep 2.1 seconds
  t = RTC_readTime(); (void)RTC_readDate();
  s1 = RTC_getSeconds(t);
  TERM_printf("%d s in 2.1 s of sleep", (s1 + 60 - s0) % 60);
  testResult(((s1 + 60 - s0) % 60) == 2);
}

// ===================================================================================
// Test 4: CRC with DMA Sniffer
// ===================================================================================
void testCRC(void) {
  uint32_t crc;
  testName("CRC / DMA ......... ");
  crc = CRC_calc32("123456789", 9);
  TERM_printf("CRC-32 = 0x%08x (expected 0xCBF43926)", crc);
  testResult(crc == 0xCBF43926);
}

// ===================================================================================
// Test 5: Flash
// ===================================================================================
void testFlash(void) {
  uint32_t jedec = FLASH_getJEDEC();
  uint8_t  ok;
  testName("Flash ............. ");
  TERM_printf("JEDEC ID 0x%06x, %d KB", jedec, 1 << ((jedec & 0xFF) - 10));
  ok = ((jedec & 0xFF) >= 0x14) && ((jedec & 0xFF) <= 0x19);   // 1..32 MB
  #if TEST_FLASH > 0
  FLASH_END_erase();                      // erase last sector
  ok &= (FLASH_END_read(0) == 0xFFFFFFFF) && (FLASH_END_read(4092) == 0xFFFFFFFF);
  FLASH_END_write(0, 0x12345678);         // write two words
  FLASH_END_write(4092, 0xA5C3F00F);
  ok &= (FLASH_END_read(0) == 0x12345678) && (FLASH_END_read(4092) == 0xA5C3F00F);
  FLASH_END_erase();                      // clean up
  ok &= (FLASH_END_read(0) == 0xFFFFFFFF);
  TERM_print(", write/read");
  #endif
  testResult(ok);
}

// ===================================================================================
// Test 6: Dual Core (FIFO and Spinlock)
// ===================================================================================
#define CORE_COUNT    20000               // increments per core
#define CORE_LOCK     16                  // spinlock number
volatile uint32_t sharedCounter;

// Increment shared counter n times (protected by spinlock)
void countUp(uint32_t n) {
  while(n--) {
    SPIN_lock(CORE_LOCK);
    sharedCounter++;
    SPIN_unlock(CORE_LOCK);
  }
}

// Function running on core 1
void core1Main(void) {
  uint32_t v;
  while((v = FIFO_read()) != 0) FIFO_write(v + 1);  // echo + 1 until 0 is received
  countUp(CORE_COUNT);
  FIFO_write(0xC0DE);                     // done
}

void testCores(void) {
  uint32_t i, ok = 1;
  testName("Dual core ......... ");
  sharedCounter = 0;
  CORE1_launch(core1Main);
  for(i = 1; i <= 100; i++) {
    FIFO_write(i);
    if(FIFO_read() != i + 1) ok = 0;
  }
  FIFO_write(0);                          // start counting on core 1
  countUp(CORE_COUNT);                    // count on core 0 at the same time
  if(FIFO_read() != 0xC0DE) ok = 0;
  CORE1_reset();                          // core 1 back to the boot ROM
  TERM_printf("FIFO %s, counter %d (expected %d)", ok ? "ok" : "error", sharedCounter, 2 * CORE_COUNT);
  testResult(ok && (sharedCounter == 2 * CORE_COUNT));
}

// ===================================================================================
// Test 7: PIO
// ===================================================================================
uint32_t pioRun(uint8_t op, uint32_t value) {
  const uint16_t prog[] = {
    PIO_PULL(0, 1),                       // pull block
    PIO_MOV(PIO_ISR, op, PIO_OSR),        // mov isr, op osr (1: invert, 2: bit-reverse)
    PIO_PUSH(0, 1)                        // push block
  };
  PIO_init(PIO0);
  PIO_load(PIO0, prog, 3, 0);
  PIO_SM_setWrap(PIO0, 0, 0, 2);
  PIO_SM_exec(PIO0, 0, PIO_JMP(PIO_ALWAYS, 0));
  PIO_SM_enable(PIO0, 0);
  PIO_SM_put(PIO0, 0, value);
  value = PIO_SM_get(PIO0, 0);
  PIO_SM_disable(PIO0, 0);
  return value;
}

void testPIO(void) {
  uint32_t inv, rev;
  testName("PIO ............... ");
  inv = pioRun(1, 0x12345678);
  rev = pioRun(2, 0x12345678);
  TERM_printf("invert 0x%08x, reverse 0x%08x", inv, rev);
  testResult((inv == 0xEDCBA987) && (rev == 0x1E6A2C48));
}

// ===================================================================================
// Test 8 and 9: ADC (Temperature, VSYS)
// ===================================================================================
void testTemp(void) {
  int16_t t = BOARD_temp();
  testName("Temperature ....... ");
  if(t < 0) TERM_printf("-%d.%d C", -t / 10, -t % 10);
  else      TERM_printf("%d.%d C", t / 10, t % 10);
  testResult((t > 0) && (t < 800));
}

void testVSYS(void) {
  uint16_t mv = BOARD_vsys();
  testName("VSYS .............. ");
  TERM_printf("%d.%02d V (USB: approx. 4.7..5.1 V)", mv / 1000, (mv % 1000) / 10);
  testResult((mv > 4000) && (mv < 5500));
}

// ===================================================================================
// Test 10: PWM
// ===================================================================================
void testPWM(void) {
  uint16_t i, c0, c1;
  uint8_t  ok;
  testName("PWM ............... ");
  PWM_init();
  PWM_start(PIN_LED);
  c0 = PWM->CH[PWM_SLICE(PIN_LED)].CTR;
  DLY_us(10);
  c1 = PWM->CH[PWM_SLICE(PIN_LED)].CTR;
  ok = (c0 != c1);                        // counter is running
  for(i = 0; i < 512; i++) {              // fade LED in and out
    PWM_write(PIN_LED, (i < 256) ? i : 511 - i);
    DLY_ms(3);
  }
  PWM_stop(PIN_LED);                      // back to normal output
  LED_off();
  TERM_print("LED faded in and out");
  testResult(ok);
}

// ===================================================================================
// Run All Tests
// ===================================================================================
void runTests(void) {
  passed = 0; total = 0;
  LED_off();
  TERM_print("\n=== Raspberry Pi Pico - Self Test ===\n");
  BOARD_printInfo(TERM_putchar);
  TERM_print("\n");
  testClock();
  testTimer();
  testRTC();
  testCRC();
  testFlash();
  testCores();
  testPIO();
  testTemp();
  testVSYS();
  testPWM();
  TERM_printf("\n%d of %d tests passed.\n", passed, total);
  TERM_print("Press Enter or the BOOTSEL button to repeat the tests, type 'w' to test the watchdog.\n");
  LED_set(passed == total);
}

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  uint8_t  connected = 0;
  uint32_t blinkTime = 0;

  // Setup
  BOARD_init();                           // init LED, millis, ADC
  TERM_init();                            // init USB serial port

  // Loop
  while(1) {
    // Terminal opened: run tests
    if(TERM_connected() != connected) {
      connected = TERM_connected();
      if(connected) {
        DLY_ms(200);                      // give the terminal some time
        runTests();
      }
    }

    // Input from terminal: Enter repeats the tests, 'w' starts the watchdog test
    while(TERM_available()) {
      char c = TERM_read();
      if((c == '\r') || (c == '\n')) runTests();
      else if((c == 'w') || (c == 'W')) {
        TERM_print("\nWatchdog started (1 s) and not fed - the board restarts now.\n");
        TERM_print("Reconnect the terminal, the self test shows the reason of the reset.\n");
        IWDG_start(1000);
        while(1);                         // wait for watchdog reset
      }
    }

    // BOOTSEL button: repeat the tests
    if(BTN_pressed()) runTests();

    // Blink LED if a test has failed
    if(total && (passed != total) && (BOARD_millis() - blinkTime >= 250)) {
      blinkTime = BOARD_millis();
      LED_toggle();
    }
  }
}
