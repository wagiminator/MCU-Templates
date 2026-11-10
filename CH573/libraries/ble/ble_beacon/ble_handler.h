// ===================================================================================
// Bluetooth LE Handler for CH573/CH571                                       * v1.0 *
// ===================================================================================
//
// This is the MCU-specific part of the Bluetooth LE libraries. It starts the WCH
// BLE library (controller and host, running on the small task scheduler TMOS) and
// implements the unified BLE host API (see ble_host.h) on top of it, so that the
// MCU-independent application libraries (ble_uart, ble_beacon, ..) can be used
// unchanged on all templates with Bluetooth.
//
// Functions available (in addition to ble_host.h):
// ------------------------------------------------
// BLE_getMAC(mac)          get Bluetooth device address (6 bytes, MSB first)
// BLE_millis()             get milliseconds (time base of the BLE stack)
// BLE_delay(ms)            wait n milliseconds, BLE stack keeps running
// BLE_random()             get 32-bit pseudo random number
//
// Notes:
// ------
// - The WCH BLE library (LIBCH57xBLE.a and CH57xBLE_LIB.h) must be in the "blobs"
//   folder of the project, the makefile links it automatically, if ble_handler.c is
//   in the src folder. When building inside this repository, ../libraries/blobs is
//   found without copying.
// - The BLE library uses the RTC (32kHz clock) as its time base, SysTick (must be
//   running, default) as random seed and the ADC temperature sensor for
//   calibration. Don't stop or reset the RTC, don't use RTC triggers and sleep
//   functions while BLE is active.
// - The 32kHz clock is the calibrated internal RC oscillator (LSI) by default. If
//   the board has a 32.768kHz crystal on PA10/PA11, set SYS_CLK32K to 1 in config.h
//   (more accurate timing, less power).
// - The system clock must be at least 24MHz (default 60MHz).
// - BLE_poll() must be called frequently (e.g. in the main loop), long delays
//   should be replaced by BLE_delay().
// - The library needs about 8KB of RAM (BLE_MEMHEAP_SIZE) for its internal heap.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__has_include)
  #if __has_include("config.h")
    #include "config.h"
  #endif
#endif
#include "system.h"

// ===================================================================================
// BLE Handler Parameters (defaults, can be changed in config.h)
// ===================================================================================
#ifndef BLE_TX_POWER
  #define BLE_TX_POWER        0           // TX power in dBm: -20, -14, -8, -4, 0, 1 .. 5
#endif
#ifndef BLE_MEMHEAP_SIZE
  #define BLE_MEMHEAP_SIZE    (8 * 1024)  // RAM for the BLE library (min. 4KB)
#endif
#ifndef BLE_BUFF_NUM
  #define BLE_BUFF_NUM        5           // number of packets buffered by the controller
#endif
#ifndef BLE_CALIBRATION
  #define BLE_CALIBRATION     120000      // period of RF and LSI calibration in ms (0: off)
#endif
#ifndef BLE_CONN_INT_MIN
  #define BLE_CONN_INT_MIN    8           // preferred min connection interval (n * 1.25ms)
#endif
#ifndef BLE_CONN_INT_MAX
  #define BLE_CONN_INT_MAX    40          // preferred max connection interval (n * 1.25ms)
#endif

#if F_CPU < 24000000
  #error BLE requires a system clock of at least 24MHz!
#endif

// ===================================================================================
// BLE Handler Functions
// ===================================================================================
void     BLE_getMAC(uint8_t* mac);      // get Bluetooth device address (MSB first)
uint32_t BLE_millis(void);              // milliseconds (time base of BLE stack)
void     BLE_delay(uint32_t ms);        // wait n ms, BLE stack keeps running
uint32_t BLE_random(void);              // 32-bit pseudo random number

#ifdef __cplusplus
};
#endif
