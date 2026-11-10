// ===================================================================================
// Project:   CH573F Examples - Self Test
// Version:   v1.0
// Year:      2026
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Automatic test of the microcontroller and the template functions without any
// additional hardware. The results are shown in the terminal (USB serial port). The
// tests start when the terminal is opened and can be repeated with Enter or a key:
//
//    1. System clock       measures the CPU clock (SysTick) against the RTC (32kHz
//                          clock), i.e. checks PLL, SysTick and LSI calibration
//    2. Timer interrupt    periodic timer interrupt (TMR0), 100 x 1ms
//    3. RTC trigger        RTC trigger interrupt after 0.5s while the CPU idles
//    4. Timer PWM          TMR1 PWM on PA10 (25%), duty cycle is measured by reading
//                          the pin
//    5. PWMX               PWM4 on PA12 (75%), measured the same way
//    6. UART loopback      UART0 with internal loopback, 64 bytes at 1 Mbaud
//    7. DataFlash          erase, write and read back the last 256-byte page
//    8. Chip ID            MAC address and unique ID (flash ROM commands)
//    9. Temperature        internal temperature sensor (ADC), plausibility check
//   10. Supply voltage     VDD (ADC), plausibility check
//
// Afterwards, the LED is on if all tests have passed and blinks if a test has
// failed. Type "w" in the terminal to test the watchdog: the watchdog is started
// and not fed, so the board restarts after about 0.5 seconds; after the restart,
// the self test shows "watchdog" as the reason of the last reset.
//
// Note: PA10 (also crystal input X32KI) and PA12 must not be connected to anything
// during the PWM tests. If the 32.768kHz crystal is used (SYS_CLK32K = 1), test 4
// is skipped.
//
// Tested template functions: see above, plus USB device (CDC), keys, reset reasons
// and the watchdog.
//
// Settings in config.h: F_CPU, PIN_LED, TEST_FLASH (0: skip the flash write test).

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "board.h"                        // board support (LED, keys, ADC, info)
#include "term.h"                         // terminal via USB serial port
#include "timer.h"                        // timer interrupt and PWM
#include "pwm.h"                          // PWMX
#include "uart.h"                         // UART
#include "flash.h"                        // DataFlash

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
  uint32_t t0, c0, c1, khz;
  testName("System clock ...... ");
  t0 = RTC_getTicks();
  while(RTC_getTicks() == t0);            // synchronize with the 32kHz tick
  t0 = RTC_getTicks();
  c0 = STK->CNTL;
  while(((RTC_getTicks() - t0) & 0xFFFF) < 3277);     // 100ms
  c1 = STK->CNTL;
  khz = (c0 - c1) / 100;                  // SysTick counts down
  TERM_printf("%d.%d MHz (expected %d MHz)", khz / 1000, (khz % 1000) / 100, F_CPU / 1000000);
  testResult((khz > F_CPU / 1000 * 98 / 100) && (khz < F_CPU / 1000 * 102 / 100));
}

// ===================================================================================
// Test 2: Timer Interrupt
// ===================================================================================
volatile uint32_t timerCount;
TMR0_ISR {
  TMR0_clear();
  timerCount++;
}

void testTimer(void) {
  testName("Timer interrupt ... ");
  timerCount = 0;
  TMR0_start(F_CPU / 1000);               // interrupt every 1ms
  TMR0_INT_enable();
  DLY_ms(100);
  TMR0_stop();
  TMR0_INT_disable();
  TERM_printf("%d interrupts in 100 ms", timerCount);
  testResult((timerCount >= 99) && (timerCount <= 101));
}

// ===================================================================================
// Test 3: RTC Trigger Interrupt and IDLE
// ===================================================================================
volatile uint8_t rtcFlag;
RTC_ISR {
  RTC_TRIG_clear();
  rtcFlag = 1;
}

void testRTC(void) {
  uint32_t start, ms;
  testName("RTC trigger ....... ");
  rtcFlag = 0;
  RTC_TRIG_clear();
  RTC_INT_enable();
  start = BOARD_millis();
  RTC_TRIG_set(16384);                    // trigger in 0.5s
  while(!rtcFlag && (BOARD_millis() - start < 1000)) IDLE_now();
  ms = BOARD_millis() - start;
  RTC_TRIG_stop();
  RTC_INT_disable();
  TERM_printf("interrupt after %d ms (expected 500 ms)", ms);
  testResult(rtcFlag && (ms >= 490) && (ms <= 510));
}

// ===================================================================================
// Test 4 and 5: PWM (duty cycle measured by reading the pin)
// ===================================================================================
uint8_t measureDuty(uint8_t pin) {
  uint32_t high = 0;
  for(uint16_t i = 0; i < 10000; i++) {
    if(PIN_read(pin)) high++;
    DLY_ticks(37);                        // not synchronous to the PWM period
  }
  return (high + 50) / 100;               // duty cycle in %
}

void testTimerPWM(void) {
  uint8_t duty;
  testName("Timer PWM (PA10) .. ");
  #if SYS_CLK32K > 0
  TERM_print("skipped (crystal on PA10)");
  total--;
  TERM_print("\n");
  #else
  TMR1_PWM_start(1000, 250);              // 25% duty cycle
  DLY_ms(5);
  duty = measureDuty(PA10);
  TMR1_stop();
  PIN_input(PA10);
  TERM_printf("duty cycle %d%% (expected 25%%)", duty);
  testResult((duty >= 23) && (duty <= 27));
  #endif
}

void testPWMX(void) {
  uint8_t duty;
  testName("PWMX (PA12) ....... ");
  PWM_init();
  PWM_write(PA12, 192);                   // 75% duty cycle
  PWM_start(PA12);
  DLY_ms(5);
  duty = measureDuty(PA12);
  PWM_stop(PA12);
  PIN_input(PA12);
  TERM_printf("duty cycle %d%% (expected 75%%)", duty);
  testResult((duty >= 73) && (duty <= 77));
}

// ===================================================================================
// Test 6: UART Loopback
// ===================================================================================
void testUART(void) {
  uint8_t  i, errors = 0;
  uint32_t start;
  testName("UART loopback ..... ");
  UART0_init();
  UART0_setBAUD(1000000);
  R8_UART0_MCR |= RB_MCR_LOOP;            // internal loopback (TX -> RX)
  R8_UART0_FCR |= RB_FCR_RX_FIFO_CLR;
  for(i = 0; i < 64; i++) {
    UART0_write(i * 3 + 1);
    start = BOARD_millis();
    while(!UART0_available() && (BOARD_millis() - start < 10));
    if(!UART0_available() || (R8_UART0_RBR != (uint8_t)(i * 3 + 1))) errors++;
  }
  R8_UART0_MCR &= ~RB_MCR_LOOP;
  TERM_printf("%d of 64 bytes received correctly", 64 - errors);
  testResult(!errors);
}

// ===================================================================================
// Test 7: DataFlash
// ===================================================================================
void testFlash(void) {
  uint8_t  ok = 1;
  #if TEST_FLASH > 0
  uint8_t  buf[16];
  const uint8_t data[16] = {0x12, 0x34, 0x56, 0x78, 0xA5, 0xC3, 0xF0, 0x0F,
                            0x00, 0xFF, 0x55, 0xAA, 0x01, 0x02, 0x04, 0x08};
  uint32_t addr = FLASH_DATA_SIZE - FLASH_DATA_PAGE;   // last page
  #endif
  testName("DataFlash ......... ");
  #if TEST_FLASH > 0
  ok &= !FLASH_DATA_erase(addr, FLASH_DATA_PAGE);
  ok &= !FLASH_DATA_read(addr, buf, 16);
  for(uint8_t i = 0; i < 16; i++) ok &= (buf[i] == 0xFF);
  ok &= !FLASH_DATA_write(addr, data, 16);
  ok &= !FLASH_DATA_read(addr, buf, 16);
  for(uint8_t i = 0; i < 16; i++) ok &= (buf[i] == data[i]);
  ok &= !FLASH_DATA_erase(addr, FLASH_DATA_PAGE);     // clean up
  ok &= (FLASH_DATA_readByte(addr) == 0xFF);
  TERM_print("erase, write, read back last page");
  #else
  TERM_print("skipped (TEST_FLASH = 0)");
  #endif
  testResult(ok);
}

// ===================================================================================
// Test 8: Chip ID (MAC address and unique ID)
// ===================================================================================
void testChipID(void) {
  uint8_t mac[6], uid[8], i, ok = 0;
  testName("Chip ID ........... ");
  ESIG_getMAC(mac);
  ESIG_getUID(uid);
  for(i = 0; i < 6; i++) if((mac[i] != 0x00) && (mac[i] != 0xFF)) ok |= 1;
  for(i = 0; i < 8; i++) if((uid[i] != 0x00) && (uid[i] != 0xFF)) ok |= 2;
  TERM_printf("chip 0x%02x, MAC %02x:%02x:%02x:%02x:%02x:%02x", ESIG_CHIPID,
              mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
  testResult((ok == 3) && ((ESIG_CHIPID == 0x73) || (ESIG_CHIPID == 0x71)));
}

// ===================================================================================
// Test 9 and 10: Temperature and Supply Voltage
// ===================================================================================
void testTemp(void) {
  int16_t t = BOARD_temp();
  testName("Temperature ....... ");
  TERM_printf("%s%d.%d C", t < 0 ? "-" : "", (t < 0 ? -t : t) / 10, (t < 0 ? -t : t) % 10);
  testResult((t > 0) && (t < 700));
}

void testVDD(void) {
  uint16_t mv = BOARD_vdd();
  testName("Supply voltage .... ");
  TERM_printf("%d.%02d V", mv / 1000, (mv % 1000) / 10);
  testResult((mv > 1700) && (mv < 3700));
}

// ===================================================================================
// Run all Tests
// ===================================================================================
void runTests(void) {
  passed = 0; total = 0;
  TERM_print("\n=== CH573F - Self Test ===\n");
  BOARD_printInfo(TERM_putchar);
  TERM_print("\n");
  testClock();
  testTimer();
  testRTC();
  testTimerPWM();
  testPWMX();
  testUART();
  testFlash();
  testChipID();
  testTemp();
  testVDD();
  TERM_printf("\n%d of %d tests passed%s\n", passed, total, (passed == total) ? " - all OK!" : "");
  TERM_print("Press Enter or a key to repeat, 'w' to test the watchdog.\n");
}

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  uint8_t connected = 0, allOK = 0;

  // Setup
  BOARD_init();                           // init LED, keys, millis, ADC
  TERM_init();                            // init USB serial port
  RTC_init();                             // 32kHz clock for RTC (LSI calibrated)

  // Loop
  while(1) {
    // Terminal opened: run tests
    if(TERM_connected() != connected) {
      connected = TERM_connected();
      if(connected) {
        DLY_ms(100);
        runTests();
        allOK = (passed == total);
      }
    }

    // Enter or key: repeat tests, 'w': watchdog test
    if(TERM_available()) {
      char c = TERM_read();
      if((c == '\r') || (c == '\n')) {
        runTests();
        allOK = (passed == total);
      }
      else if((c == 'w') || (c == 'W')) {
        TERM_printf("Starting watchdog (%d ms) without feeding it ...\n", WDT_MAX_MS);
        WDT_start(WDT_MAX_MS);
        while(1);                         // wait for the watchdog reset
      }
    }
    if(BTN_pressed(BTN_BOOT) || BTN_pressed(BTN_KEY)) {
      runTests();
      allOK = (passed == total);
    }

    // LED: on if all tests passed, blinking if not (or not yet run)
    if(allOK) LED_on();
    else      LED_set((BOARD_millis() / 250) & 1);
  }
}
