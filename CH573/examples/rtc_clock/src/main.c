// ===================================================================================
// Project:   CH573F Examples - RTC Clock with Alarm
// Version:   v1.0
// Year:      2026
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// A clock with alarm based on the real-time clock (RTC) of the CH573. The time is
// set and shown via the terminal (USB serial port). The LED flashes briefly every
// second (heartbeat). When the alarm time is reached (RTC trigger interrupt), the
// LED blinks and a message is sent every 10 seconds, until a key is pressed or
// "alarm off" is typed. Between the clock updates, the CPU sleeps (IDLE mode, woken
// up by the periodic RTC timer interrupt and USB).
//
// Commands (in addition to the commands of the USB shell, type "help"):
//
//   time [hh:mm:ss]          show or set time
//   date [yyyy-mm-dd]        show or set date (the weekday is calculated)
//   alarm [hh:mm[:ss]|off]   show, set or switch off the daily alarm
//   clock [on|off]           show the time every second in the terminal (in front
//                            of the prompt, paused while a command is typed)
//   calib                    calibrate internal 32kHz oscillator (LSI) again
//
// The RTC of the CH573 counts the time of day and the days. The date is calculated
// from the day counter (day 0 = 2020-01-01, valid until 2064).
//
// Tested template functions: RTC (time, days, trigger interrupt, periodic timer),
// 32kHz clock (LSI calibration or LSE crystal), sleep (IDLE_now), USB device (CDC),
// keys, shell.
//
// Settings in config.h: CLOCK_TIME, CLOCK_DATE (start time after power-on),
// SYS_CLK32K (0: internal LSI, 1: 32.768kHz crystal on PA10/PA11), PIN_LED, USB
// descriptors.

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "board.h"                        // board support (LED, keys, ADC, info)
#include "term.h"                         // terminal via USB serial port
#include "shell.h"                        // command line interpreter

#define YEAR_REF      2020                // year of day 0

const char* WEEKDAYS[] = {"Wed", "Thu", "Fri", "Sat", "Sun", "Mon", "Tue"}; // 2020-01-01 = Wed

uint8_t  alarmSet  = 0;                   // alarm active?
uint8_t  alarmH, alarmM, alarmS;          // alarm time
volatile uint8_t alarmFlag = 0;           // set by alarm interrupt
uint8_t  ringing   = 0;                   // alarm is ringing
uint8_t  showClock = 1;                   // show time every second

// ===================================================================================
// RTC Functions
// ===================================================================================

// RTC interrupt: trigger (alarm) and periodic timer (wake-up from IDLE)
RTC_ISR {
  if(RTC_TRIG_flag()) {
    RTC_TRIG_clear();
    RTC_TRIG_stop();                      // the alarm is set again after ringing
    alarmFlag = 1;
  }
  if(RTC_TMR_flag()) RTC_TMR_clear();
}

// Check leap year
uint8_t isLeap(uint16_t y) {
  return (!(y % 4) && (y % 100)) || !(y % 400);
}

// Days of month m (1..12) in year y
uint8_t monthDays(uint16_t y, uint8_t m) {
  const uint8_t days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  return (m == 2 && isLeap(y)) ? 29 : days[m - 1];
}

// Convert day counter to date
void dayToDate(uint16_t day, uint16_t* y, uint8_t* m, uint8_t* d) {
  *y = YEAR_REF;
  while(day >= (isLeap(*y) ? 366 : 365)) day -= isLeap((*y)++) ? 366 : 365;
  *m = 1;
  while(day >= monthDays(*y, *m)) day -= monthDays(*y, (*m)++);
  *d = day + 1;
}

// Convert date to day counter
uint16_t dateToDay(uint16_t y, uint8_t m, uint8_t d) {
  uint16_t day = d - 1;
  for(uint16_t i = YEAR_REF; i < y; i++) day += isLeap(i) ? 366 : 365;
  for(uint8_t  i = 1; i < m; i++) day += monthDays(y, i);
  return day;
}

// Print date and time
void printDateTime(void (*out)(char)) {
  uint16_t day = RTC_getDay(), y;
  uint32_t t   = RTC_getTime();
  uint8_t  m, d;
  if(RTC_getDay() != day) {               // midnight passed while reading
    day = RTC_getDay();
    t   = RTC_getTime();
  }
  dayToDate(day, &y, &m, &d);
  printF(out, "%s %04d-%02d-%02d %02d:%02d:%02d", WEEKDAYS[day % 7], y, m, d,
         RTC_getHours(t), RTC_getMinutes(t), RTC_getSeconds(t));
}

// Parse "a:b[:c]" or "a-b-c" into three numbers, returns number of values found
uint8_t parse3(const char* s, uint16_t* v) {
  uint8_t n = 0;
  v[0] = v[1] = v[2] = 0;
  while(*s && (n < 3)) {
    if((*s < '0') || (*s > '9')) return 0;
    while((*s >= '0') && (*s <= '9')) v[n] = v[n] * 10 + (*s++ - '0');
    n++;
    if((*s == ':') || (*s == '-')) s++;
    else break;
  }
  return n;
}

// Set alarm: RTC trigger at the alarm time (compares with the 32kHz tick counter)
void setAlarm(void) {
  uint32_t sec = (uint32_t)alarmH * 3600 + alarmM * 60 + alarmS;
  uint32_t t   = ((sec / 2) << 16) | ((sec & 1) ? 0x8000 : 0);
  RTC_TRIG_clear();
  SAFE_ACCESS {
    R32_RTC_TRIG = t;
    R8_RTC_MODE_CTRL = (R8_RTC_MODE_CTRL & ~(RB_RTC_LOAD_LO | RB_RTC_LOAD_HI)) | RB_RTC_TRIG_EN;
  }
}

// ===================================================================================
// Shell Extension (additional commands)
// ===================================================================================
uint8_t SHELL_command(const char* cmd, const char* arg) {
  uint16_t v[3];
  if(!strcmp(cmd, "help")) {
    TERM_print("  time [hh:mm:ss]      show or set time\n"
               "  date [yyyy-mm-dd]    show or set date\n"
               "  alarm [hh:mm|off]    show, set or switch off the daily alarm\n"
               "  clock [on|off]       show the time every second\n"
               "  calib                calibrate internal 32kHz oscillator\n");
    return 1;
  }
  if(!strcmp(cmd, "time") || !strcmp(cmd, "date")) {
    if(*arg) {
      if(cmd[0] == 't') {
        if((parse3(arg, v) < 2) || (v[0] > 23) || (v[1] > 59) || (v[2] > 59)) {
          TERM_print("Usage: time hh:mm:ss\n");
          return 1;
        }
        RTC_setTime(v[0], v[1], v[2]);
      }
      else {
        if((parse3(arg, v) != 3) || (v[0] < YEAR_REF) || (v[0] > YEAR_REF + 44)
           || (v[1] < 1) || (v[1] > 12) || (v[2] < 1) || (v[2] > monthDays(v[0], v[1]))) {
          TERM_print("Usage: date yyyy-mm-dd\n");
          return 1;
        }
        RTC_setDay(dateToDay(v[0], v[1], v[2]));
      }
      if(alarmSet) setAlarm();            // setting the time overwrites the trigger
      DLY_ms(5);                          // wait until the RTC has taken the values
    }
    printDateTime(TERM_putchar);
    TERM_print("\n");
    return 1;
  }
  if(!strcmp(cmd, "alarm")) {
    if(!strcmp(arg, "off")) {
      alarmSet = 0;
      ringing  = 0;
      RTC_TRIG_stop();
    }
    else if(*arg) {
      if((parse3(arg, v) < 2) || (v[0] > 23) || (v[1] > 59) || (v[2] > 59)) {
        TERM_print("Usage: alarm hh:mm[:ss] or alarm off\n");
        return 1;
      }
      alarmH = v[0]; alarmM = v[1]; alarmS = v[2];
      alarmSet = 1;
      setAlarm();
    }
    if(alarmSet) TERM_printf("Alarm at %02d:%02d:%02d\n", alarmH, alarmM, alarmS);
    else         TERM_print("Alarm is off\n");
    return 1;
  }
  if(!strcmp(cmd, "clock")) {
    if(!strcmp(arg, "on"))  showClock = 1;
    if(!strcmp(arg, "off")) showClock = 0;
    TERM_printf("Clock display is %s\n", showClock ? "on" : "off");
    return 1;
  }
  if(!strcmp(cmd, "calib")) {
    #if SYS_CLK32K == 0
    LSI_calibrate();
    TERM_printf("LSI calibrated, tuning value: %d\n", LSI_tune);
    #else
    TERM_print("The external 32.768kHz crystal (LSE) is used\n");
    #endif
    return 1;
  }
  if(!strcmp(cmd, "info")) {
    TERM_print("Date/time:   ");
    printDateTime(TERM_putchar);
    TERM_printf("\n32kHz clock: %s\n", CLK32K_isLSE() ? "LSE (crystal)" : "LSI (calibrated RC)");
    return 1;
  }
  return 0;
}

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  uint8_t  connected = 0;
  uint32_t lastSec = 0xFFFFFFFF, sec;
  uint32_t ringTime = 0, calibTime = 0;
  uint16_t v[3];

  // Setup
  BOARD_init();                           // init LED, keys, millis, ADC
  TERM_init();                            // init USB serial port
  SHELL_init(TERM_putchar, TERM_flush, 1, "clock> ");
  RTC_init();                             // start 32kHz clock (LSI is calibrated)
  if(RST_wasPower()) {                    // set default time after power-on
    parse3(CLOCK_DATE, v); RTC_setDay(dateToDay(v[0], v[1], v[2]));
    parse3(CLOCK_TIME, v); RTC_setTime(v[0], v[1], v[2]);
  }
  RTC_TMR_start(RTC_TMR_125MS);           // periodic wake-up from IDLE
  RTC_INT_enable();

  // Loop
  while(1) {
    // Terminal opened: show welcome message
    if(TERM_connected() != connected) {
      connected = TERM_connected();
      if(connected) {
        DLY_ms(100);
        TERM_print("\n=== CH573F - RTC Clock ===\n");
        TERM_print("Set the clock with 'time hh:mm:ss' and 'date yyyy-mm-dd',\n");
        TERM_print("the alarm with 'alarm hh:mm'. Type 'help' for all commands.\n");
        SHELL_prompt();
      }
    }

    // Characters received via USB: feed into shell
    while(TERM_available()) SHELL_input(TERM_read());

    // Alarm interrupt: start ringing
    if(alarmFlag) {
      alarmFlag = 0;
      ringing   = 1;
      ringTime  = BOARD_millis() - 10000;
    }

    // Ringing: blink LED, message every 10 seconds, stop with a key
    if(ringing) {
      LED_set((BOARD_millis() / 125) & 1);
      if(BOARD_millis() - ringTime >= 10000) {
        ringTime = BOARD_millis();
        TERM_print("\n*** ALARM *** ");
        printDateTime(TERM_putchar);
        TERM_print(" - press a key or type 'alarm off'\n");
        SHELL_prompt();
      }
      if(BTN_pressed(BTN_BOOT) || BTN_pressed(BTN_KEY) || !alarmSet) {
        ringing = 0;
        LED_off();
        TERM_print("\nAlarm stopped\n");
        SHELL_prompt();
        if(alarmSet) setAlarm();          // set alarm again for the next day
      }
      continue;
    }

    // New second: LED heartbeat and clock display
    sec = RTC_getTime();
    if(sec != lastSec) {
      lastSec = sec;
      LED_flash(20);
      if(showClock && connected && !SHELL_busy()) {
        TERM_print("\r");                 // overwrite the line with the prompt
        printDateTime(TERM_putchar);
        TERM_print("  ");
        SHELL_prompt();
      }
    }
    LED_update();
    if(BTN_pressed(BTN_BOOT) || BTN_pressed(BTN_KEY)) {
      TERM_print("\n");
      printDateTime(TERM_putchar);
      TERM_print("\n");
      SHELL_prompt();
    }

    // Recalibrate LSI every 10 minutes (temperature drift)
    #if SYS_CLK32K == 0
    if(BOARD_millis() - calibTime >= 600000) {
      calibTime = BOARD_millis();
      LSI_calibrate();
    }
    #else
    (void)calibTime;
    #endif

    IDLE_now();                           // sleep until next interrupt
  }
}
