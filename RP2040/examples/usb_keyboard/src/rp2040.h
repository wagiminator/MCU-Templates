// ===================================================================================
// Header File for RP2040                                                     * v1.0 *
// ===================================================================================
// Peripheral register definitions for the RP2040. The register structures and bit
// definitions were generated from the SVD file of the Raspberry Pi Pico SDK
// (src/rp2040/hardware_regs/RP2040.svd) and are named after it. Registers that exist
// several times in a row are combined into arrays (e.g. IO_BANK0->GPIO[n].CTRL,
// DMA->CH[n].READ_ADDR, PIO0->SM[n].CLKDIV, PADS_BANK0->GPIO[n]). Bit definitions
// follow the CMSIS style: PERIPHERAL_REGISTER_FIELD_Pos, _Msk and the mask itself;
// enumerated field values are defined as PERIPHERAL_REGISTER_FIELD_VALUE.
// This file also contains a copy of core_cm0plus.h (CMSIS 5).
//
// Copyright of the original register descriptions:
// Copyright (c) 2024 Raspberry Pi Ltd.
// SPDX-License-Identifier: BSD-3-Clause
//
// Copyright of core_cm0plus.h:
// Copyright (c) 2009-2018 Arm Limited. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// ===================================================================================
// MCU Definitions
// ===================================================================================
#define XOSC_VALUE          12000000    // crystal frequency (Hz), 12MHz on Raspberry Pi Pico
#define ROSC_VALUE           6500000    // internal ring oscillator, approx. (Hz)
#define USB_CLK_VALUE       48000000    // USB clock (Hz)

// Memory map
#define ROM_BASE            0x00000000  // boot ROM (16KB)
#define XIP_BASE            0x10000000  // external flash, cached (execute in place)
#define XIP_NOALLOC_BASE    0x11000000  // external flash, cache hit check without allocation
#define XIP_NOCACHE_BASE    0x12000000  // external flash, cache bypassed
#define XIP_NOCACHE_NOALLOC_BASE 0x13000000 // external flash, no cache, no allocation
#define XIP_SRAM_BASE       0x15000000  // XIP cache as SRAM (16KB, if cache disabled)
#define SRAM_BASE           0x20000000  // SRAM (striped SRAM0..3: 256KB)
#define SRAM4_BASE          0x20040000  // SRAM4 (4KB, used for core 1 stack)
#define SRAM5_BASE          0x20041000  // SRAM5 (4KB, used for core 0 stack)
#define SRAM_END            0x20042000  // end of SRAM
#define USB_DPRAM_BASE      0x50100000  // USB dual-port RAM (4KB)
#define SIO_BASE            0xD0000000  // single-cycle IO (SIO)
#define FLASH_PAGE_SIZE     256         // flash page size (programming)
#define FLASH_SECTOR_SIZE   4096        // flash sector size (erasing)

// Atomic register access aliases (not for SIO): write to address + 0x1000 (XOR),
// + 0x2000 (SET) or + 0x3000 (CLR) to modify individual bits without read-modify-write
#define REG_ALIAS_XOR       0x1000
#define REG_ALIAS_SET       0x2000
#define REG_ALIAS_CLR       0x3000
#define REG_XOR(reg, mask)  (*(volatile uint32_t*)((uint32_t)&(reg) | REG_ALIAS_XOR) = (mask))
#define REG_SET(reg, mask)  (*(volatile uint32_t*)((uint32_t)&(reg) | REG_ALIAS_SET) = (mask))
#define REG_CLR(reg, mask)  (*(volatile uint32_t*)((uint32_t)&(reg) | REG_ALIAS_CLR) = (mask))
#define REG_WRITE_MASK(reg, val, mask) REG_XOR(reg, ((reg) ^ (val)) & (mask))

typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;

// ===================================================================================
// Interrupt Numbers
// ===================================================================================
typedef enum IRQn {
  // Cortex-M0+ processor exceptions
  NonMaskableInt_IRQn          = -14,   // non maskable interrupt
  HardFault_IRQn               = -13,   // hard fault
  SVCall_IRQn                  =  -5,   // supervisor call
  PendSV_IRQn                  =  -2,   // pendable request for system service
  SysTick_IRQn                 =  -1,   // system tick timer
  // RP2040 interrupts (each core has its own NVIC)
  TIMER_IRQ_0_IRQn             =  0,
  TIMER_IRQ_1_IRQn             =  1,
  TIMER_IRQ_2_IRQn             =  2,
  TIMER_IRQ_3_IRQn             =  3,
  PWM_IRQ_WRAP_IRQn            =  4,
  USBCTRL_IRQ_IRQn             =  5,
  XIP_IRQ_IRQn                 =  6,
  PIO0_IRQ_0_IRQn              =  7,
  PIO0_IRQ_1_IRQn              =  8,
  PIO1_IRQ_0_IRQn              =  9,
  PIO1_IRQ_1_IRQn              = 10,
  DMA_IRQ_0_IRQn               = 11,
  DMA_IRQ_1_IRQn               = 12,
  IO_IRQ_BANK0_IRQn            = 13,
  IO_IRQ_QSPI_IRQn             = 14,
  SIO_IRQ_PROC0_IRQn           = 15,
  SIO_IRQ_PROC1_IRQn           = 16,
  CLOCKS_IRQ_IRQn              = 17,
  SPI0_IRQ_IRQn                = 18,
  SPI1_IRQ_IRQn                = 19,
  UART0_IRQ_IRQn               = 20,
  UART1_IRQ_IRQn               = 21,
  ADC_IRQ_FIFO_IRQn            = 22,
  I2C0_IRQ_IRQn                = 23,
  I2C1_IRQ_IRQn                = 24,
  RTC_IRQ_IRQn                 = 25,
  SPARE_IRQ_0_IRQn             = 26,
  SPARE_IRQ_1_IRQn             = 27,
  SPARE_IRQ_2_IRQn             = 28,
  SPARE_IRQ_3_IRQn             = 29,
  SPARE_IRQ_4_IRQn             = 30,
  SPARE_IRQ_5_IRQn             = 31,
} IRQn_Type;

// ===================================================================================
// GPIO Function Select Values (IO_BANK0->GPIO[n].CTRL FUNCSEL)
// ===================================================================================
#define GPIO_FUNC_XIP       0           // XIP (QSPI pins only)
#define GPIO_FUNC_SPI       1           // SPI0/SPI1
#define GPIO_FUNC_UART      2           // UART0/UART1
#define GPIO_FUNC_I2C       3           // I2C0/I2C1
#define GPIO_FUNC_PWM       4           // PWM
#define GPIO_FUNC_SIO       5           // software controlled GPIO (SIO)
#define GPIO_FUNC_PIO0      6           // PIO0
#define GPIO_FUNC_PIO1      7           // PIO1
#define GPIO_FUNC_GPCK      8           // clock in/out (GPIN/GPOUT)
#define GPIO_FUNC_USB       9           // USB VBUS/overcurrent detection
#define GPIO_FUNC_NULL      31          // no function (reset state)

// ===================================================================================
// DMA Transfer Request Signals (DREQ, DMA->CH[n].CTRL_TRIG TREQ_SEL)
// ===================================================================================
#define DREQ_PIO0_TX0       0
#define DREQ_PIO0_TX1       1
#define DREQ_PIO0_TX2       2
#define DREQ_PIO0_TX3       3
#define DREQ_PIO0_RX0       4
#define DREQ_PIO0_RX1       5
#define DREQ_PIO0_RX2       6
#define DREQ_PIO0_RX3       7
#define DREQ_PIO1_TX0       8
#define DREQ_PIO1_TX1       9
#define DREQ_PIO1_TX2       10
#define DREQ_PIO1_TX3       11
#define DREQ_PIO1_RX0       12
#define DREQ_PIO1_RX1       13
#define DREQ_PIO1_RX2       14
#define DREQ_PIO1_RX3       15
#define DREQ_SPI0_TX        16
#define DREQ_SPI0_RX        17
#define DREQ_SPI1_TX        18
#define DREQ_SPI1_RX        19
#define DREQ_UART0_TX       20
#define DREQ_UART0_RX       21
#define DREQ_UART1_TX       22
#define DREQ_UART1_RX       23
#define DREQ_PWM_WRAP0      24
#define DREQ_PWM_WRAP1      25
#define DREQ_PWM_WRAP2      26
#define DREQ_PWM_WRAP3      27
#define DREQ_PWM_WRAP4      28
#define DREQ_PWM_WRAP5      29
#define DREQ_PWM_WRAP6      30
#define DREQ_PWM_WRAP7      31
#define DREQ_I2C0_TX        32
#define DREQ_I2C0_RX        33
#define DREQ_I2C1_TX        34
#define DREQ_I2C1_RX        35
#define DREQ_ADC            36
#define DREQ_XIP_STREAM     37
#define DREQ_XIP_SSITX      38
#define DREQ_XIP_SSIRX      39
#define DREQ_TIMER0         0x3B        // DMA pacing timer 0
#define DREQ_TIMER1         0x3C        // DMA pacing timer 1
#define DREQ_TIMER2         0x3D        // DMA pacing timer 2
#define DREQ_TIMER3         0x3E        // DMA pacing timer 3
#define DREQ_FORCE          0x3F        // permanent request (unpaced transfer)

// ===================================================================================
// Cortex-M0+ Processor Configuration and Core Peripherals (core_cm0plus.h)
// ===================================================================================
#define __CM0PLUS_REV          1         // core revision r0p1
#define __MPU_PRESENT          1         // MPU present
#define __NVIC_PRIO_BITS       2         // number of bits used for priority levels
#define __VTOR_PRESENT         1         // VTOR present
#define __Vendor_SysTickConfig 0         // standard SysTick config

/**************************************************************************//**
 * @file     core_cm0plus.h
 * @brief    CMSIS Cortex-M0+ Core Peripheral Access Layer Header File
 * @version  V5.0.7
 * @date     13. March 2019
 ******************************************************************************/
/*
 * Copyright (c) 2009-2018 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*******************************************************************************
 *                 CMSIS definitions
 ******************************************************************************/

/*  CMSIS Version definitions */
#define __CM_CMSIS_VERSION_MAIN  ( 5U)                                      /*!< [31:16] CMSIS Core(M) main version */
#define __CM_CMSIS_VERSION_SUB   ( 1U)                                      /*!< [15:0]  CMSIS Core(M) sub version */
#define __CM_CMSIS_VERSION       ((__CM_CMSIS_VERSION_MAIN << 16U) | \
                                   __CM_CMSIS_VERSION_SUB           )       /*!< CMSIS Core(M) version number */
 
/*  CMSIS CM0+ definitions */
#define __CM0PLUS_CMSIS_VERSION_MAIN (__CM_CMSIS_VERSION_MAIN)                  /*!< \deprecated [31:16] CMSIS HAL main version */
#define __CM0PLUS_CMSIS_VERSION_SUB  (__CM_CMSIS_VERSION_SUB)                   /*!< \deprecated [15:0]  CMSIS HAL sub version */
#define __CM0PLUS_CMSIS_VERSION      ((__CM0PLUS_CMSIS_VERSION_MAIN << 16U) | \
                                       __CM0PLUS_CMSIS_VERSION_SUB           )  /*!< \deprecated CMSIS HAL version number */

#if defined (__VFP_FP__) && !defined(__SOFTFP__)
  #error "Compiler generates FPU instructions for a device without an FPU (check __FPU_PRESENT)"
#endif

/* ignore some GCC warnings */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* Fallback for __has_builtin */
#ifndef __has_builtin
  #define __has_builtin(x) (0)
#endif

/* CMSIS compiler specific defines */
#ifndef   __ASM
  #define __ASM                                  __asm
#endif
#ifndef   __INLINE
  #define __INLINE                               inline
#endif
#ifndef   __STATIC_INLINE
  #define __STATIC_INLINE                        static inline
#endif
#ifndef   __STATIC_FORCEINLINE                 
  #define __STATIC_FORCEINLINE                   __attribute__((always_inline)) static inline
#endif                                           
#ifndef   __NO_RETURN
  #define __NO_RETURN                            __attribute__((__noreturn__))
#endif
#ifndef   __USED
  #define __USED                                 __attribute__((used))
#endif
#ifndef   __WEAK
  #define __WEAK                                 __attribute__((weak))
#endif
#ifndef   __PACKED
  #define __PACKED                               __attribute__((packed, aligned(1)))
#endif
#ifndef   __PACKED_STRUCT
  #define __PACKED_STRUCT                        struct __attribute__((packed, aligned(1)))
#endif
#ifndef   __PACKED_UNION
  #define __PACKED_UNION                         union __attribute__((packed, aligned(1)))
#endif
#ifndef   __UNALIGNED_UINT32        /* deprecated */
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wpacked"
  #pragma GCC diagnostic ignored "-Wattributes"
  struct __attribute__((packed)) T_UINT32 { uint32_t v; };
  #pragma GCC diagnostic pop
  #define __UNALIGNED_UINT32(x)                  (((struct T_UINT32 *)(x))->v)
#endif
#ifndef   __UNALIGNED_UINT16_WRITE
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wpacked"
  #pragma GCC diagnostic ignored "-Wattributes"
  __PACKED_STRUCT T_UINT16_WRITE { uint16_t v; };
  #pragma GCC diagnostic pop
  #define __UNALIGNED_UINT16_WRITE(addr, val)    (void)((((struct T_UINT16_WRITE *)(void *)(addr))->v) = (val))
#endif
#ifndef   __UNALIGNED_UINT16_READ
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wpacked"
  #pragma GCC diagnostic ignored "-Wattributes"
  __PACKED_STRUCT T_UINT16_READ { uint16_t v; };
  #pragma GCC diagnostic pop
  #define __UNALIGNED_UINT16_READ(addr)          (((const struct T_UINT16_READ *)(const void *)(addr))->v)
#endif
#ifndef   __UNALIGNED_UINT32_WRITE
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wpacked"
  #pragma GCC diagnostic ignored "-Wattributes"
  __PACKED_STRUCT T_UINT32_WRITE { uint32_t v; };
  #pragma GCC diagnostic pop
  #define __UNALIGNED_UINT32_WRITE(addr, val)    (void)((((struct T_UINT32_WRITE *)(void *)(addr))->v) = (val))
#endif
#ifndef   __UNALIGNED_UINT32_READ
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wpacked"
  #pragma GCC diagnostic ignored "-Wattributes"
  __PACKED_STRUCT T_UINT32_READ { uint32_t v; };
  #pragma GCC diagnostic pop
  #define __UNALIGNED_UINT32_READ(addr)          (((const struct T_UINT32_READ *)(const void *)(addr))->v)
#endif
#ifndef   __ALIGNED
  #define __ALIGNED(x)                           __attribute__((aligned(x)))
#endif
#ifndef   __RESTRICT
  #define __RESTRICT                             __restrict
#endif

/* check device defines and use defaults */
#if defined __CHECK_DEVICE_DEFINES
  #ifndef __CM0PLUS_REV
    #define __CM0PLUS_REV             0x0000U
    #warning "__CM0PLUS_REV not defined in device header file; using default!"
  #endif

  #ifndef __MPU_PRESENT
    #define __MPU_PRESENT             0U
    #warning "__MPU_PRESENT not defined in device header file; using default!"
  #endif

  #ifndef __VTOR_PRESENT
    #define __VTOR_PRESENT            0U
    #warning "__VTOR_PRESENT not defined in device header file; using default!"
  #endif

  #ifndef __NVIC_PRIO_BITS
    #define __NVIC_PRIO_BITS          2U
    #warning "__NVIC_PRIO_BITS not defined in device header file; using default!"
  #endif

  #ifndef __Vendor_SysTickConfig
    #define __Vendor_SysTickConfig    0U
    #warning "__Vendor_SysTickConfig not defined in device header file; using default!"
  #endif
#endif

#pragma GCC diagnostic pop

/* IO definitions (access restrictions to peripheral registers) */
/**
    \defgroup CMSIS_glob_defs CMSIS Global Defines

    <strong>IO Type Qualifiers</strong> are used
    \li to specify the access to peripheral variables.
    \li for automatic generation of peripheral register debug information.
*/
#ifdef __cplusplus
  #define   __I     volatile             /*!< Defines 'read only' permissions */
#else
  #define   __I     volatile const       /*!< Defines 'read only' permissions */
#endif
#define     __O     volatile             /*!< Defines 'write only' permissions */
#define     __IO    volatile             /*!< Defines 'read / write' permissions */

/* following defines should be used for structure members */
#define     __IM     volatile const      /*! Defines 'read only' structure member permissions */
#define     __OM     volatile            /*! Defines 'write only' structure member permissions */
#define     __IOM    volatile            /*! Defines 'read / write' structure member permissions */

/*@} end of group Cortex-M0+ */

/*******************************************************************************
 *                 Register Abstraction
  Core Register contain:
  - Core Register
  - Core NVIC Register
  - Core SCB Register
  - Core SysTick Register
  - Core MPU Register
 ******************************************************************************/
/**
  \defgroup CMSIS_core_register Defines and Type Definitions
  \brief Type definitions and defines for Cortex-M processor based devices.
*/

/**
  \ingroup    CMSIS_core_register
  \defgroup   CMSIS_CORE  Status and Control Registers
  \brief      Core Register type definitions.
  @{
 */

/**
  \brief  Union type to access the Application Program Status Register (APSR).
 */
typedef union
{
  struct
  {
    uint32_t _reserved0:28;              /*!< bit:  0..27  Reserved */
    uint32_t V:1;                        /*!< bit:     28  Overflow condition code flag */
    uint32_t C:1;                        /*!< bit:     29  Carry condition code flag */
    uint32_t Z:1;                        /*!< bit:     30  Zero condition code flag */
    uint32_t N:1;                        /*!< bit:     31  Negative condition code flag */
  } b;                                   /*!< Structure used for bit  access */
  uint32_t w;                            /*!< Type      used for word access */
} APSR_Type;

/* APSR Register Definitions */
#define APSR_N_Pos                         31U                                            /*!< APSR: N Position */
#define APSR_N_Msk                         (1UL << APSR_N_Pos)                            /*!< APSR: N Mask */

#define APSR_Z_Pos                         30U                                            /*!< APSR: Z Position */
#define APSR_Z_Msk                         (1UL << APSR_Z_Pos)                            /*!< APSR: Z Mask */

#define APSR_C_Pos                         29U                                            /*!< APSR: C Position */
#define APSR_C_Msk                         (1UL << APSR_C_Pos)                            /*!< APSR: C Mask */

#define APSR_V_Pos                         28U                                            /*!< APSR: V Position */
#define APSR_V_Msk                         (1UL << APSR_V_Pos)                            /*!< APSR: V Mask */

/**
  \brief  Union type to access the Interrupt Program Status Register (IPSR).
 */
typedef union
{
  struct
  {
    uint32_t ISR:9;                      /*!< bit:  0.. 8  Exception number */
    uint32_t _reserved0:23;              /*!< bit:  9..31  Reserved */
  } b;                                   /*!< Structure used for bit  access */
  uint32_t w;                            /*!< Type      used for word access */
} IPSR_Type;

/* IPSR Register Definitions */
#define IPSR_ISR_Pos                        0U                                            /*!< IPSR: ISR Position */
#define IPSR_ISR_Msk                       (0x1FFUL /*<< IPSR_ISR_Pos*/)                  /*!< IPSR: ISR Mask */

/**
  \brief  Union type to access the Special-Purpose Program Status Registers (xPSR).
 */
typedef union
{
  struct
  {
    uint32_t ISR:9;                      /*!< bit:  0.. 8  Exception number */
    uint32_t _reserved0:15;              /*!< bit:  9..23  Reserved */
    uint32_t T:1;                        /*!< bit:     24  Thumb bit        (read 0) */
    uint32_t _reserved1:3;               /*!< bit: 25..27  Reserved */
    uint32_t V:1;                        /*!< bit:     28  Overflow condition code flag */
    uint32_t C:1;                        /*!< bit:     29  Carry condition code flag */
    uint32_t Z:1;                        /*!< bit:     30  Zero condition code flag */
    uint32_t N:1;                        /*!< bit:     31  Negative condition code flag */
  } b;                                   /*!< Structure used for bit  access */
  uint32_t w;                            /*!< Type      used for word access */
} xPSR_Type;

/* xPSR Register Definitions */
#define xPSR_N_Pos                         31U                                            /*!< xPSR: N Position */
#define xPSR_N_Msk                         (1UL << xPSR_N_Pos)                            /*!< xPSR: N Mask */

#define xPSR_Z_Pos                         30U                                            /*!< xPSR: Z Position */
#define xPSR_Z_Msk                         (1UL << xPSR_Z_Pos)                            /*!< xPSR: Z Mask */

#define xPSR_C_Pos                         29U                                            /*!< xPSR: C Position */
#define xPSR_C_Msk                         (1UL << xPSR_C_Pos)                            /*!< xPSR: C Mask */

#define xPSR_V_Pos                         28U                                            /*!< xPSR: V Position */
#define xPSR_V_Msk                         (1UL << xPSR_V_Pos)                            /*!< xPSR: V Mask */

#define xPSR_T_Pos                         24U                                            /*!< xPSR: T Position */
#define xPSR_T_Msk                         (1UL << xPSR_T_Pos)                            /*!< xPSR: T Mask */

#define xPSR_ISR_Pos                        0U                                            /*!< xPSR: ISR Position */
#define xPSR_ISR_Msk                       (0x1FFUL /*<< xPSR_ISR_Pos*/)                  /*!< xPSR: ISR Mask */

/**
  \brief  Union type to access the Control Registers (CONTROL).
 */
typedef union
{
  struct
  {
    uint32_t nPRIV:1;                    /*!< bit:      0  Execution privilege in Thread mode */
    uint32_t SPSEL:1;                    /*!< bit:      1  Stack to be used */
    uint32_t _reserved1:30;              /*!< bit:  2..31  Reserved */
  } b;                                   /*!< Structure used for bit  access */
  uint32_t w;                            /*!< Type      used for word access */
} CONTROL_Type;

/* CONTROL Register Definitions */
#define CONTROL_SPSEL_Pos                   1U                                            /*!< CONTROL: SPSEL Position */
#define CONTROL_SPSEL_Msk                  (1UL << CONTROL_SPSEL_Pos)                     /*!< CONTROL: SPSEL Mask */

#define CONTROL_nPRIV_Pos                   0U                                            /*!< CONTROL: nPRIV Position */
#define CONTROL_nPRIV_Msk                  (1UL /*<< CONTROL_nPRIV_Pos*/)                 /*!< CONTROL: nPRIV Mask */

/*@} end of group CMSIS_CORE */

/**
  \ingroup    CMSIS_core_register
  \defgroup   CMSIS_NVIC  Nested Vectored Interrupt Controller (NVIC)
  \brief      Type definitions for the NVIC Registers
  @{
 */

/**
  \brief  Structure type to access the Nested Vectored Interrupt Controller (NVIC).
 */
typedef struct
{
  __IOM uint32_t ISER[1U];               /*!< Offset: 0x000 (R/W)  Interrupt Set Enable Register */
        uint32_t RESERVED0[31U];
  __IOM uint32_t ICER[1U];               /*!< Offset: 0x080 (R/W)  Interrupt Clear Enable Register */
        uint32_t RESERVED1[31U];
  __IOM uint32_t ISPR[1U];               /*!< Offset: 0x100 (R/W)  Interrupt Set Pending Register */
        uint32_t RESERVED2[31U];
  __IOM uint32_t ICPR[1U];               /*!< Offset: 0x180 (R/W)  Interrupt Clear Pending Register */
        uint32_t RESERVED3[31U];
        uint32_t RESERVED4[64U];
  __IOM uint32_t IP[8U];                 /*!< Offset: 0x300 (R/W)  Interrupt Priority Register */
}  NVIC_Type;

/*@} end of group CMSIS_NVIC */

/**
  \ingroup  CMSIS_core_register
  \defgroup CMSIS_SCB     System Control Block (SCB)
  \brief    Type definitions for the System Control Block Registers
  @{
 */

/**
  \brief  Structure type to access the System Control Block (SCB).
 */
typedef struct
{
  __IM  uint32_t CPUID;                  /*!< Offset: 0x000 (R/ )  CPUID Base Register */
  __IOM uint32_t ICSR;                   /*!< Offset: 0x004 (R/W)  Interrupt Control and State Register */
#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
  __IOM uint32_t VTOR;                   /*!< Offset: 0x008 (R/W)  Vector Table Offset Register */
#else
        uint32_t RESERVED0;
#endif
  __IOM uint32_t AIRCR;                  /*!< Offset: 0x00C (R/W)  Application Interrupt and Reset Control Register */
  __IOM uint32_t SCR;                    /*!< Offset: 0x010 (R/W)  System Control Register */
  __IOM uint32_t CCR;                    /*!< Offset: 0x014 (R/W)  Configuration Control Register */
        uint32_t RESERVED1;
  __IOM uint32_t SHP[2U];                /*!< Offset: 0x01C (R/W)  System Handlers Priority Registers. [0] is RESERVED */
  __IOM uint32_t SHCSR;                  /*!< Offset: 0x024 (R/W)  System Handler Control and State Register */
} SCB_Type;

/* SCB CPUID Register Definitions */
#define SCB_CPUID_IMPLEMENTER_Pos          24U                                            /*!< SCB CPUID: IMPLEMENTER Position */
#define SCB_CPUID_IMPLEMENTER_Msk          (0xFFUL << SCB_CPUID_IMPLEMENTER_Pos)          /*!< SCB CPUID: IMPLEMENTER Mask */

#define SCB_CPUID_VARIANT_Pos              20U                                            /*!< SCB CPUID: VARIANT Position */
#define SCB_CPUID_VARIANT_Msk              (0xFUL << SCB_CPUID_VARIANT_Pos)               /*!< SCB CPUID: VARIANT Mask */

#define SCB_CPUID_ARCHITECTURE_Pos         16U                                            /*!< SCB CPUID: ARCHITECTURE Position */
#define SCB_CPUID_ARCHITECTURE_Msk         (0xFUL << SCB_CPUID_ARCHITECTURE_Pos)          /*!< SCB CPUID: ARCHITECTURE Mask */

#define SCB_CPUID_PARTNO_Pos                4U                                            /*!< SCB CPUID: PARTNO Position */
#define SCB_CPUID_PARTNO_Msk               (0xFFFUL << SCB_CPUID_PARTNO_Pos)              /*!< SCB CPUID: PARTNO Mask */

#define SCB_CPUID_REVISION_Pos              0U                                            /*!< SCB CPUID: REVISION Position */
#define SCB_CPUID_REVISION_Msk             (0xFUL /*<< SCB_CPUID_REVISION_Pos*/)          /*!< SCB CPUID: REVISION Mask */

/* SCB Interrupt Control State Register Definitions */
#define SCB_ICSR_NMIPENDSET_Pos            31U                                            /*!< SCB ICSR: NMIPENDSET Position */
#define SCB_ICSR_NMIPENDSET_Msk            (1UL << SCB_ICSR_NMIPENDSET_Pos)               /*!< SCB ICSR: NMIPENDSET Mask */

#define SCB_ICSR_PENDSVSET_Pos             28U                                            /*!< SCB ICSR: PENDSVSET Position */
#define SCB_ICSR_PENDSVSET_Msk             (1UL << SCB_ICSR_PENDSVSET_Pos)                /*!< SCB ICSR: PENDSVSET Mask */

#define SCB_ICSR_PENDSVCLR_Pos             27U                                            /*!< SCB ICSR: PENDSVCLR Position */
#define SCB_ICSR_PENDSVCLR_Msk             (1UL << SCB_ICSR_PENDSVCLR_Pos)                /*!< SCB ICSR: PENDSVCLR Mask */

#define SCB_ICSR_PENDSTSET_Pos             26U                                            /*!< SCB ICSR: PENDSTSET Position */
#define SCB_ICSR_PENDSTSET_Msk             (1UL << SCB_ICSR_PENDSTSET_Pos)                /*!< SCB ICSR: PENDSTSET Mask */

#define SCB_ICSR_PENDSTCLR_Pos             25U                                            /*!< SCB ICSR: PENDSTCLR Position */
#define SCB_ICSR_PENDSTCLR_Msk             (1UL << SCB_ICSR_PENDSTCLR_Pos)                /*!< SCB ICSR: PENDSTCLR Mask */

#define SCB_ICSR_ISRPREEMPT_Pos            23U                                            /*!< SCB ICSR: ISRPREEMPT Position */
#define SCB_ICSR_ISRPREEMPT_Msk            (1UL << SCB_ICSR_ISRPREEMPT_Pos)               /*!< SCB ICSR: ISRPREEMPT Mask */

#define SCB_ICSR_ISRPENDING_Pos            22U                                            /*!< SCB ICSR: ISRPENDING Position */
#define SCB_ICSR_ISRPENDING_Msk            (1UL << SCB_ICSR_ISRPENDING_Pos)               /*!< SCB ICSR: ISRPENDING Mask */

#define SCB_ICSR_VECTPENDING_Pos           12U                                            /*!< SCB ICSR: VECTPENDING Position */
#define SCB_ICSR_VECTPENDING_Msk           (0x1FFUL << SCB_ICSR_VECTPENDING_Pos)          /*!< SCB ICSR: VECTPENDING Mask */

#define SCB_ICSR_VECTACTIVE_Pos             0U                                            /*!< SCB ICSR: VECTACTIVE Position */
#define SCB_ICSR_VECTACTIVE_Msk            (0x1FFUL /*<< SCB_ICSR_VECTACTIVE_Pos*/)       /*!< SCB ICSR: VECTACTIVE Mask */

#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
/* SCB Interrupt Control State Register Definitions */
#define SCB_VTOR_TBLOFF_Pos                 8U                                            /*!< SCB VTOR: TBLOFF Position */
#define SCB_VTOR_TBLOFF_Msk                (0xFFFFFFUL << SCB_VTOR_TBLOFF_Pos)            /*!< SCB VTOR: TBLOFF Mask */
#endif

/* SCB Application Interrupt and Reset Control Register Definitions */
#define SCB_AIRCR_VECTKEY_Pos              16U                                            /*!< SCB AIRCR: VECTKEY Position */
#define SCB_AIRCR_VECTKEY_Msk              (0xFFFFUL << SCB_AIRCR_VECTKEY_Pos)            /*!< SCB AIRCR: VECTKEY Mask */

#define SCB_AIRCR_VECTKEYSTAT_Pos          16U                                            /*!< SCB AIRCR: VECTKEYSTAT Position */
#define SCB_AIRCR_VECTKEYSTAT_Msk          (0xFFFFUL << SCB_AIRCR_VECTKEYSTAT_Pos)        /*!< SCB AIRCR: VECTKEYSTAT Mask */

#define SCB_AIRCR_ENDIANESS_Pos            15U                                            /*!< SCB AIRCR: ENDIANESS Position */
#define SCB_AIRCR_ENDIANESS_Msk            (1UL << SCB_AIRCR_ENDIANESS_Pos)               /*!< SCB AIRCR: ENDIANESS Mask */

#define SCB_AIRCR_SYSRESETREQ_Pos           2U                                            /*!< SCB AIRCR: SYSRESETREQ Position */
#define SCB_AIRCR_SYSRESETREQ_Msk          (1UL << SCB_AIRCR_SYSRESETREQ_Pos)             /*!< SCB AIRCR: SYSRESETREQ Mask */

#define SCB_AIRCR_VECTCLRACTIVE_Pos         1U                                            /*!< SCB AIRCR: VECTCLRACTIVE Position */
#define SCB_AIRCR_VECTCLRACTIVE_Msk        (1UL << SCB_AIRCR_VECTCLRACTIVE_Pos)           /*!< SCB AIRCR: VECTCLRACTIVE Mask */

/* SCB System Control Register Definitions */
#define SCB_SCR_SEVONPEND_Pos               4U                                            /*!< SCB SCR: SEVONPEND Position */
#define SCB_SCR_SEVONPEND_Msk              (1UL << SCB_SCR_SEVONPEND_Pos)                 /*!< SCB SCR: SEVONPEND Mask */

#define SCB_SCR_SLEEPDEEP_Pos               2U                                            /*!< SCB SCR: SLEEPDEEP Position */
#define SCB_SCR_SLEEPDEEP_Msk              (1UL << SCB_SCR_SLEEPDEEP_Pos)                 /*!< SCB SCR: SLEEPDEEP Mask */

#define SCB_SCR_SLEEPONEXIT_Pos             1U                                            /*!< SCB SCR: SLEEPONEXIT Position */
#define SCB_SCR_SLEEPONEXIT_Msk            (1UL << SCB_SCR_SLEEPONEXIT_Pos)               /*!< SCB SCR: SLEEPONEXIT Mask */

/* SCB Configuration Control Register Definitions */
#define SCB_CCR_STKALIGN_Pos                9U                                            /*!< SCB CCR: STKALIGN Position */
#define SCB_CCR_STKALIGN_Msk               (1UL << SCB_CCR_STKALIGN_Pos)                  /*!< SCB CCR: STKALIGN Mask */

#define SCB_CCR_UNALIGN_TRP_Pos             3U                                            /*!< SCB CCR: UNALIGN_TRP Position */
#define SCB_CCR_UNALIGN_TRP_Msk            (1UL << SCB_CCR_UNALIGN_TRP_Pos)               /*!< SCB CCR: UNALIGN_TRP Mask */

/* SCB System Handler Control and State Register Definitions */
#define SCB_SHCSR_SVCALLPENDED_Pos         15U                                            /*!< SCB SHCSR: SVCALLPENDED Position */
#define SCB_SHCSR_SVCALLPENDED_Msk         (1UL << SCB_SHCSR_SVCALLPENDED_Pos)            /*!< SCB SHCSR: SVCALLPENDED Mask */

/*@} end of group CMSIS_SCB */

/**
  \ingroup  CMSIS_core_register
  \defgroup CMSIS_SysTick     System Tick Timer (SysTick)
  \brief    Type definitions for the System Timer Registers.
  @{
 */

/**
  \brief  Structure type to access the System Timer (SysTick).
 */
typedef struct
{
  __IOM uint32_t CTRL;                   /*!< Offset: 0x000 (R/W)  SysTick Control and Status Register */
  __IOM uint32_t LOAD;                   /*!< Offset: 0x004 (R/W)  SysTick Reload Value Register */
  __IOM uint32_t VAL;                    /*!< Offset: 0x008 (R/W)  SysTick Current Value Register */
  __IM  uint32_t CALIB;                  /*!< Offset: 0x00C (R/ )  SysTick Calibration Register */
} SysTick_Type;

/* SysTick Control / Status Register Definitions */
#define SysTick_CTRL_COUNTFLAG_Pos         16U                                            /*!< SysTick CTRL: COUNTFLAG Position */
#define SysTick_CTRL_COUNTFLAG_Msk         (1UL << SysTick_CTRL_COUNTFLAG_Pos)            /*!< SysTick CTRL: COUNTFLAG Mask */
#define SysTick_CTRL_COUNTFLAG             SysTick_CTRL_COUNTFLAG_Msk

#define SysTick_CTRL_CLKSOURCE_Pos          2U                                            /*!< SysTick CTRL: CLKSOURCE Position */
#define SysTick_CTRL_CLKSOURCE_Msk         (1UL << SysTick_CTRL_CLKSOURCE_Pos)            /*!< SysTick CTRL: CLKSOURCE Mask */
#define SysTick_CTRL_CLKSOURCE             SysTick_CTRL_CLKSOURCE_Msk

#define SysTick_CTRL_TICKINT_Pos            1U                                            /*!< SysTick CTRL: TICKINT Position */
#define SysTick_CTRL_TICKINT_Msk           (1UL << SysTick_CTRL_TICKINT_Pos)              /*!< SysTick CTRL: TICKINT Mask */
#define SysTick_CTRL_TICKINT               SysTick_CTRL_TICKINT_Msk

#define SysTick_CTRL_ENABLE_Pos             0U                                            /*!< SysTick CTRL: ENABLE Position */
#define SysTick_CTRL_ENABLE_Msk            (1UL /*<< SysTick_CTRL_ENABLE_Pos*/)           /*!< SysTick CTRL: ENABLE Mask */
#define SysTick_CTRL_ENABLE                SysTick_CTRL_ENABLE_Msk

/* SysTick Reload Register Definitions */
#define SysTick_LOAD_RELOAD_Pos             0U                                            /*!< SysTick LOAD: RELOAD Position */
#define SysTick_LOAD_RELOAD_Msk            (0xFFFFFFUL /*<< SysTick_LOAD_RELOAD_Pos*/)    /*!< SysTick LOAD: RELOAD Mask */
#define SysTick_LOAD_RELOAD                SysTick_LOAD_RELOAD_Msk

/* SysTick Current Register Definitions */
#define SysTick_VAL_CURRENT_Pos             0U                                            /*!< SysTick VAL: CURRENT Position */
#define SysTick_VAL_CURRENT_Msk            (0xFFFFFFUL /*<< SysTick_VAL_CURRENT_Pos*/)    /*!< SysTick VAL: CURRENT Mask */

/* SysTick Calibration Register Definitions */
#define SysTick_CALIB_NOREF_Pos            31U                                            /*!< SysTick CALIB: NOREF Position */
#define SysTick_CALIB_NOREF_Msk            (1UL << SysTick_CALIB_NOREF_Pos)               /*!< SysTick CALIB: NOREF Mask */

#define SysTick_CALIB_SKEW_Pos             30U                                            /*!< SysTick CALIB: SKEW Position */
#define SysTick_CALIB_SKEW_Msk             (1UL << SysTick_CALIB_SKEW_Pos)                /*!< SysTick CALIB: SKEW Mask */

#define SysTick_CALIB_TENMS_Pos             0U                                            /*!< SysTick CALIB: TENMS Position */
#define SysTick_CALIB_TENMS_Msk            (0xFFFFFFUL /*<< SysTick_CALIB_TENMS_Pos*/)    /*!< SysTick CALIB: TENMS Mask */

/*@} end of group CMSIS_SysTick */

#if defined (__MPU_PRESENT) && (__MPU_PRESENT == 1U)
/**
  \ingroup  CMSIS_core_register
  \defgroup CMSIS_MPU     Memory Protection Unit (MPU)
  \brief    Type definitions for the Memory Protection Unit (MPU)
  @{
 */

/**
  \brief  Structure type to access the Memory Protection Unit (MPU).
 */
typedef struct
{
  __IM  uint32_t TYPE;                   /*!< Offset: 0x000 (R/ )  MPU Type Register */
  __IOM uint32_t CTRL;                   /*!< Offset: 0x004 (R/W)  MPU Control Register */
  __IOM uint32_t RNR;                    /*!< Offset: 0x008 (R/W)  MPU Region RNRber Register */
  __IOM uint32_t RBAR;                   /*!< Offset: 0x00C (R/W)  MPU Region Base Address Register */
  __IOM uint32_t RASR;                   /*!< Offset: 0x010 (R/W)  MPU Region Attribute and Size Register */
} MPU_Type;

#define MPU_TYPE_RALIASES                  1U

/* MPU Type Register Definitions */
#define MPU_TYPE_IREGION_Pos               16U                                            /*!< MPU TYPE: IREGION Position */
#define MPU_TYPE_IREGION_Msk               (0xFFUL << MPU_TYPE_IREGION_Pos)               /*!< MPU TYPE: IREGION Mask */

#define MPU_TYPE_DREGION_Pos                8U                                            /*!< MPU TYPE: DREGION Position */
#define MPU_TYPE_DREGION_Msk               (0xFFUL << MPU_TYPE_DREGION_Pos)               /*!< MPU TYPE: DREGION Mask */

#define MPU_TYPE_SEPARATE_Pos               0U                                            /*!< MPU TYPE: SEPARATE Position */
#define MPU_TYPE_SEPARATE_Msk              (1UL /*<< MPU_TYPE_SEPARATE_Pos*/)             /*!< MPU TYPE: SEPARATE Mask */

/* MPU Control Register Definitions */
#define MPU_CTRL_PRIVDEFENA_Pos             2U                                            /*!< MPU CTRL: PRIVDEFENA Position */
#define MPU_CTRL_PRIVDEFENA_Msk            (1UL << MPU_CTRL_PRIVDEFENA_Pos)               /*!< MPU CTRL: PRIVDEFENA Mask */

#define MPU_CTRL_HFNMIENA_Pos               1U                                            /*!< MPU CTRL: HFNMIENA Position */
#define MPU_CTRL_HFNMIENA_Msk              (1UL << MPU_CTRL_HFNMIENA_Pos)                 /*!< MPU CTRL: HFNMIENA Mask */

#define MPU_CTRL_ENABLE_Pos                 0U                                            /*!< MPU CTRL: ENABLE Position */
#define MPU_CTRL_ENABLE_Msk                (1UL /*<< MPU_CTRL_ENABLE_Pos*/)               /*!< MPU CTRL: ENABLE Mask */

/* MPU Region Number Register Definitions */
#define MPU_RNR_REGION_Pos                  0U                                            /*!< MPU RNR: REGION Position */
#define MPU_RNR_REGION_Msk                 (0xFFUL /*<< MPU_RNR_REGION_Pos*/)             /*!< MPU RNR: REGION Mask */

/* MPU Region Base Address Register Definitions */
#define MPU_RBAR_ADDR_Pos                   8U                                            /*!< MPU RBAR: ADDR Position */
#define MPU_RBAR_ADDR_Msk                  (0xFFFFFFUL << MPU_RBAR_ADDR_Pos)              /*!< MPU RBAR: ADDR Mask */

#define MPU_RBAR_VALID_Pos                  4U                                            /*!< MPU RBAR: VALID Position */
#define MPU_RBAR_VALID_Msk                 (1UL << MPU_RBAR_VALID_Pos)                    /*!< MPU RBAR: VALID Mask */

#define MPU_RBAR_REGION_Pos                 0U                                            /*!< MPU RBAR: REGION Position */
#define MPU_RBAR_REGION_Msk                (0xFUL /*<< MPU_RBAR_REGION_Pos*/)             /*!< MPU RBAR: REGION Mask */

/* MPU Region Attribute and Size Register Definitions */
#define MPU_RASR_ATTRS_Pos                 16U                                            /*!< MPU RASR: MPU Region Attribute field Position */
#define MPU_RASR_ATTRS_Msk                 (0xFFFFUL << MPU_RASR_ATTRS_Pos)               /*!< MPU RASR: MPU Region Attribute field Mask */

#define MPU_RASR_XN_Pos                    28U                                            /*!< MPU RASR: ATTRS.XN Position */
#define MPU_RASR_XN_Msk                    (1UL << MPU_RASR_XN_Pos)                       /*!< MPU RASR: ATTRS.XN Mask */

#define MPU_RASR_AP_Pos                    24U                                            /*!< MPU RASR: ATTRS.AP Position */
#define MPU_RASR_AP_Msk                    (0x7UL << MPU_RASR_AP_Pos)                     /*!< MPU RASR: ATTRS.AP Mask */

#define MPU_RASR_TEX_Pos                   19U                                            /*!< MPU RASR: ATTRS.TEX Position */
#define MPU_RASR_TEX_Msk                   (0x7UL << MPU_RASR_TEX_Pos)                    /*!< MPU RASR: ATTRS.TEX Mask */

#define MPU_RASR_S_Pos                     18U                                            /*!< MPU RASR: ATTRS.S Position */
#define MPU_RASR_S_Msk                     (1UL << MPU_RASR_S_Pos)                        /*!< MPU RASR: ATTRS.S Mask */

#define MPU_RASR_C_Pos                     17U                                            /*!< MPU RASR: ATTRS.C Position */
#define MPU_RASR_C_Msk                     (1UL << MPU_RASR_C_Pos)                        /*!< MPU RASR: ATTRS.C Mask */

#define MPU_RASR_B_Pos                     16U                                            /*!< MPU RASR: ATTRS.B Position */
#define MPU_RASR_B_Msk                     (1UL << MPU_RASR_B_Pos)                        /*!< MPU RASR: ATTRS.B Mask */

#define MPU_RASR_SRD_Pos                    8U                                            /*!< MPU RASR: Sub-Region Disable Position */
#define MPU_RASR_SRD_Msk                   (0xFFUL << MPU_RASR_SRD_Pos)                   /*!< MPU RASR: Sub-Region Disable Mask */

#define MPU_RASR_SIZE_Pos                   1U                                            /*!< MPU RASR: Region Size Field Position */
#define MPU_RASR_SIZE_Msk                  (0x1FUL << MPU_RASR_SIZE_Pos)                  /*!< MPU RASR: Region Size Field Mask */

#define MPU_RASR_ENABLE_Pos                 0U                                            /*!< MPU RASR: Region enable bit Position */
#define MPU_RASR_ENABLE_Msk                (1UL /*<< MPU_RASR_ENABLE_Pos*/)               /*!< MPU RASR: Region enable bit Disable Mask */

/*@} end of group CMSIS_MPU */
#endif

/**
  \ingroup  CMSIS_core_register
  \defgroup CMSIS_CoreDebug       Core Debug Registers (CoreDebug)
  \brief    Cortex-M0+ Core Debug Registers (DCB registers, SHCSR, and DFSR) are only accessible over DAP and not via processor.
            Therefore they are not covered by the Cortex-M0+ header file.
  @{
 */
/*@} end of group CMSIS_CoreDebug */

/**
  \ingroup    CMSIS_core_register
  \defgroup   CMSIS_core_bitfield     Core register bit field macros
  \brief      Macros for use with bit field definitions (xxx_Pos, xxx_Msk).
  @{
 */

/**
  \brief   Mask and shift a bit field value for use in a register bit range.
  \param[in] field  Name of the register bit field.
  \param[in] value  Value of the bit field. This parameter is interpreted as an uint32_t type.
  \return           Masked and shifted value.
*/
#define _VAL2FLD(field, value)    (((uint32_t)(value) << field ## _Pos) & field ## _Msk)

/**
  \brief     Mask and shift a register value to extract a bit filed value.
  \param[in] field  Name of the register bit field.
  \param[in] value  Value of register. This parameter is interpreted as an uint32_t type.
  \return           Masked and shifted bit field value.
*/
#define _FLD2VAL(field, value)    (((uint32_t)(value) & field ## _Msk) >> field ## _Pos)

/*@} end of group CMSIS_core_bitfield */

/**
  \ingroup    CMSIS_core_register
  \defgroup   CMSIS_core_base     Core Definitions
  \brief      Definitions for base addresses, unions, and structures.
  @{
 */

/* Memory mapping of Core Hardware */
#define SCS_BASE            (0xE000E000UL)                            /*!< System Control Space Base Address */
#define SysTick_BASE        (SCS_BASE +  0x0010UL)                    /*!< SysTick Base Address */
#define NVIC_BASE           (SCS_BASE +  0x0100UL)                    /*!< NVIC Base Address */
#define SCB_BASE            (SCS_BASE +  0x0D00UL)                    /*!< System Control Block Base Address */

#define SCB                 ((SCB_Type       *)     SCB_BASE      )   /*!< SCB configuration struct */
#define SysTick             ((SysTick_Type   *)     SysTick_BASE  )   /*!< SysTick configuration struct */
#define NVIC                ((NVIC_Type      *)     NVIC_BASE     )   /*!< NVIC configuration struct */

#if defined (__MPU_PRESENT) && (__MPU_PRESENT == 1U)
  #define MPU_BASE          (SCS_BASE +  0x0D90UL)                    /*!< Memory Protection Unit */
  #define MPU               ((MPU_Type       *)     MPU_BASE      )   /*!< Memory Protection Unit */
#endif

/*@} */

/*@}*/


// ===================================================================================
// Peripheral Register Structures
// ===================================================================================
// ADC
typedef struct {
  __IO uint32_t CS;                          // 0x000 ADC Control and Status
  __IO uint32_t RESULT;                      // 0x004 Result of most recent ADC conversion
  __IO uint32_t FCS;                         // 0x008 FIFO control and status
  __IO uint32_t FIFO;                        // 0x00C Conversion result FIFO
  __IO uint32_t DIV;                         // 0x010 Clock divider. If non-zero, CS_START_MANY will sta
  __IO uint32_t INTR;                        // 0x014 Raw Interrupts
  __IO uint32_t INTE;                        // 0x018 Interrupt Enable
  __IO uint32_t INTF;                        // 0x01C Interrupt Force
  __IO uint32_t INTS;                        // 0x020 Interrupt status after masking & forcing
} ADC_TypeDef;

// BUSCTRL
typedef struct {
  __IO uint32_t BUS_PRIORITY;                // 0x000 Set the priority of each master for bus arbitratio
  __IO uint32_t BUS_PRIORITY_ACK;            // 0x004 Bus priority acknowledge
  __IO uint32_t PERFCTR0;                    // 0x008 Bus fabric performance counter 0
  __IO uint32_t PERFSEL0;                    // 0x00C Bus fabric performance event select for PERFCTR0
  __IO uint32_t PERFCTR1;                    // 0x010 Bus fabric performance counter 1
  __IO uint32_t PERFSEL1;                    // 0x014 Bus fabric performance event select for PERFCTR1
  __IO uint32_t PERFCTR2;                    // 0x018 Bus fabric performance counter 2
  __IO uint32_t PERFSEL2;                    // 0x01C Bus fabric performance event select for PERFCTR2
  __IO uint32_t PERFCTR3;                    // 0x020 Bus fabric performance counter 3
  __IO uint32_t PERFSEL3;                    // 0x024 Bus fabric performance event select for PERFCTR3
} BUSCTRL_TypeDef;

// CLOCKS
typedef struct {
  __IO uint32_t CTRL;                        // 0x00 Clock control, can be changed on-the-fly (except for auxsrc)
  __IO uint32_t DIV;                         // 0x04 Clock divisor, can be changed on-the-fly
  __IO uint32_t SELECTED;                    // 0x08 Indicates which SRC is currently selected by the glitchless 
} CLOCKS_CLK_GPOUT_TypeDef;

typedef struct {
  CLOCKS_CLK_GPOUT_TypeDef CLK_GPOUT[4];     // 0x000
  __IO uint32_t CLK_REF_CTRL;                // 0x030 Clock control, can be changed on-the-fly (except f
  __IO uint32_t CLK_REF_DIV;                 // 0x034 Clock divisor, can be changed on-the-fly
  __IO uint32_t CLK_REF_SELECTED;            // 0x038 Indicates which SRC is currently selected by the g
  __IO uint32_t CLK_SYS_CTRL;                // 0x03C Clock control, can be changed on-the-fly (except f
  __IO uint32_t CLK_SYS_DIV;                 // 0x040 Clock divisor, can be changed on-the-fly
  __IO uint32_t CLK_SYS_SELECTED;            // 0x044 Indicates which SRC is currently selected by the g
  __IO uint32_t CLK_PERI_CTRL;               // 0x048 Clock control, can be changed on-the-fly (except f
  __I  uint32_t RESERVED0;
  __IO uint32_t CLK_PERI_SELECTED;           // 0x050 Indicates which SRC is currently selected by the g
  __IO uint32_t CLK_USB_CTRL;                // 0x054 Clock control, can be changed on-the-fly (except f
  __IO uint32_t CLK_USB_DIV;                 // 0x058 Clock divisor, can be changed on-the-fly
  __IO uint32_t CLK_USB_SELECTED;            // 0x05C Indicates which SRC is currently selected by the g
  __IO uint32_t CLK_ADC_CTRL;                // 0x060 Clock control, can be changed on-the-fly (except f
  __IO uint32_t CLK_ADC_DIV;                 // 0x064 Clock divisor, can be changed on-the-fly
  __IO uint32_t CLK_ADC_SELECTED;            // 0x068 Indicates which SRC is currently selected by the g
  __IO uint32_t CLK_RTC_CTRL;                // 0x06C Clock control, can be changed on-the-fly (except f
  __IO uint32_t CLK_RTC_DIV;                 // 0x070 Clock divisor, can be changed on-the-fly
  __IO uint32_t CLK_RTC_SELECTED;            // 0x074 Indicates which SRC is currently selected by the g
  __IO uint32_t CLK_SYS_RESUS_CTRL;          // 0x078 
  __IO uint32_t CLK_SYS_RESUS_STATUS;        // 0x07C 
  __IO uint32_t FC0_REF_KHZ;                 // 0x080 Reference clock frequency in kHz
  __IO uint32_t FC0_MIN_KHZ;                 // 0x084 Minimum pass frequency in kHz. This is optional. S
  __IO uint32_t FC0_MAX_KHZ;                 // 0x088 Maximum pass frequency in kHz. This is optional. S
  __IO uint32_t FC0_DELAY;                   // 0x08C Delays the start of frequency counting to allow th
  __IO uint32_t FC0_INTERVAL;                // 0x090 The test interval is 0.98us * 2**interval, but let
  __IO uint32_t FC0_SRC;                     // 0x094 Clock sent to frequency counter, set to 0 when not
  __IO uint32_t FC0_STATUS;                  // 0x098 Frequency counter status
  __IO uint32_t FC0_RESULT;                  // 0x09C Result of frequency measurement, only valid when s
  __IO uint32_t WAKE_EN0;                    // 0x0A0 enable clock in wake mode
  __IO uint32_t WAKE_EN1;                    // 0x0A4 enable clock in wake mode
  __IO uint32_t SLEEP_EN0;                   // 0x0A8 enable clock in sleep mode
  __IO uint32_t SLEEP_EN1;                   // 0x0AC enable clock in sleep mode
  __IO uint32_t ENABLED0;                    // 0x0B0 indicates the state of the clock enable
  __IO uint32_t ENABLED1;                    // 0x0B4 indicates the state of the clock enable
  __IO uint32_t INTR;                        // 0x0B8 Raw Interrupts
  __IO uint32_t INTE;                        // 0x0BC Interrupt Enable
  __IO uint32_t INTF;                        // 0x0C0 Interrupt Force
  __IO uint32_t INTS;                        // 0x0C4 Interrupt status after masking & forcing
} CLOCKS_TypeDef;

// DMA
typedef struct {
  __IO uint32_t READ_ADDR;                   // 0x00 DMA Channel 0 Read Address pointer
  __IO uint32_t WRITE_ADDR;                  // 0x04 DMA Channel 0 Write Address pointer
  __IO uint32_t TRANS_COUNT;                 // 0x08 DMA Channel 0 Transfer Count
  __IO uint32_t CTRL_TRIG;                   // 0x0C DMA Channel 0 Control and Status
  __IO uint32_t AL1_CTRL;                    // 0x10 Alias for channel 0 CTRL register
  __IO uint32_t AL1_READ_ADDR;               // 0x14 Alias for channel 0 READ_ADDR register
  __IO uint32_t AL1_WRITE_ADDR;              // 0x18 Alias for channel 0 WRITE_ADDR register
  __IO uint32_t AL1_TRANS_COUNT_TRIG;        // 0x1C Alias for channel 0 TRANS_COUNT register This is a trigger r
  __IO uint32_t AL2_CTRL;                    // 0x20 Alias for channel 0 CTRL register
  __IO uint32_t AL2_TRANS_COUNT;             // 0x24 Alias for channel 0 TRANS_COUNT register
  __IO uint32_t AL2_READ_ADDR;               // 0x28 Alias for channel 0 READ_ADDR register
  __IO uint32_t AL2_WRITE_ADDR_TRIG;         // 0x2C Alias for channel 0 WRITE_ADDR register This is a trigger re
  __IO uint32_t AL3_CTRL;                    // 0x30 Alias for channel 0 CTRL register
  __IO uint32_t AL3_WRITE_ADDR;              // 0x34 Alias for channel 0 WRITE_ADDR register
  __IO uint32_t AL3_TRANS_COUNT;             // 0x38 Alias for channel 0 TRANS_COUNT register
  __IO uint32_t AL3_READ_ADDR_TRIG;          // 0x3C Alias for channel 0 READ_ADDR register This is a trigger reg
} DMA_CH_TypeDef;

typedef struct {
  __IO uint32_t CTDREQ;                      // 0x00 Read: get channel DREQ counter (i.e. how many accesses the D
  __IO uint32_t TCR;                         // 0x04 Read to get channel TRANS_COUNT reload value, i.e. the lengt
  __I  uint32_t RESERVED0[14];
} DMA_CH_DBG_TypeDef;

typedef struct {
  DMA_CH_TypeDef CH[12];                     // 0x000
  __I  uint32_t RESERVED0[64];
  __IO uint32_t INTR;                        // 0x400 Interrupt Status (raw)
  __IO uint32_t INTE0;                       // 0x404 Interrupt Enables for IRQ 0
  __IO uint32_t INTF0;                       // 0x408 Force Interrupts
  __IO uint32_t INTS0;                       // 0x40C Interrupt Status for IRQ 0
  __I  uint32_t RESERVED1;
  __IO uint32_t INTE1;                       // 0x414 Interrupt Enables for IRQ 1
  __IO uint32_t INTF1;                       // 0x418 Force Interrupts for IRQ 1
  __IO uint32_t INTS1;                       // 0x41C Interrupt Status (masked) for IRQ 1
  __IO uint32_t PACE_TIMER[4];               // 0x420 Pacing (X/Y) Fractional Timer The pacing timer pro
  __IO uint32_t MULTI_CHAN_TRIGGER;          // 0x430 Trigger one or more channels simultaneously
  __IO uint32_t SNIFF_CTRL;                  // 0x434 Sniffer Control
  __IO uint32_t SNIFF_DATA;                  // 0x438 Data accumulator for sniff hardware
  __I  uint32_t RESERVED2;
  __IO uint32_t FIFO_LEVELS;                 // 0x440 Debug RAF, WAF, TDF levels
  __IO uint32_t CHAN_ABORT;                  // 0x444 Abort an in-progress transfer sequence on one or m
  __IO uint32_t N_CHANNELS;                  // 0x448 The number of channels this DMA instance is equipp
  __I  uint32_t RESERVED3[237];
  DMA_CH_DBG_TypeDef CH_DBG[12];             // 0x800
} DMA_TypeDef;

// I2C
typedef struct {
  __IO uint32_t IC_CON;                      // 0x000 I2C Control Register. This register can be written
  __IO uint32_t IC_TAR;                      // 0x004 I2C Target Address Register This register is 12 bi
  __IO uint32_t IC_SAR;                      // 0x008 I2C Slave Address Register
  __I  uint32_t RESERVED0;
  __IO uint32_t IC_DATA_CMD;                 // 0x010 I2C Rx/Tx Data Buffer and Command Register; this i
  __IO uint32_t IC_SS_SCL_HCNT;              // 0x014 Standard Speed I2C Clock SCL High Count Register
  __IO uint32_t IC_SS_SCL_LCNT;              // 0x018 Standard Speed I2C Clock SCL Low Count Register
  __IO uint32_t IC_FS_SCL_HCNT;              // 0x01C Fast Mode or Fast Mode Plus I2C Clock SCL High Cou
  __IO uint32_t IC_FS_SCL_LCNT;              // 0x020 Fast Mode or Fast Mode Plus I2C Clock SCL Low Coun
  __I  uint32_t RESERVED1[2];
  __IO uint32_t IC_INTR_STAT;                // 0x02C I2C Interrupt Status Register Each bit in this reg
  __IO uint32_t IC_INTR_MASK;                // 0x030 I2C Interrupt Mask Register. These bits mask their
  __IO uint32_t IC_RAW_INTR_STAT;            // 0x034 I2C Raw Interrupt Status Register Unlike the IC_IN
  __IO uint32_t IC_RX_TL;                    // 0x038 I2C Receive FIFO Threshold Register
  __IO uint32_t IC_TX_TL;                    // 0x03C I2C Transmit FIFO Threshold Register
  __IO uint32_t IC_CLR_INTR;                 // 0x040 Clear Combined and Individual Interrupt Register
  __IO uint32_t IC_CLR_RX_UNDER;             // 0x044 Clear RX_UNDER Interrupt Register
  __IO uint32_t IC_CLR_RX_OVER;              // 0x048 Clear RX_OVER Interrupt Register
  __IO uint32_t IC_CLR_TX_OVER;              // 0x04C Clear TX_OVER Interrupt Register
  __IO uint32_t IC_CLR_RD_REQ;               // 0x050 Clear RD_REQ Interrupt Register
  __IO uint32_t IC_CLR_TX_ABRT;              // 0x054 Clear TX_ABRT Interrupt Register
  __IO uint32_t IC_CLR_RX_DONE;              // 0x058 Clear RX_DONE Interrupt Register
  __IO uint32_t IC_CLR_ACTIVITY;             // 0x05C Clear ACTIVITY Interrupt Register
  __IO uint32_t IC_CLR_STOP_DET;             // 0x060 Clear STOP_DET Interrupt Register
  __IO uint32_t IC_CLR_START_DET;            // 0x064 Clear START_DET Interrupt Register
  __IO uint32_t IC_CLR_GEN_CALL;             // 0x068 Clear GEN_CALL Interrupt Register
  __IO uint32_t IC_ENABLE;                   // 0x06C I2C Enable Register
  __IO uint32_t IC_STATUS;                   // 0x070 I2C Status Register This is a read-only register u
  __IO uint32_t IC_TXFLR;                    // 0x074 I2C Transmit FIFO Level Register This register con
  __IO uint32_t IC_RXFLR;                    // 0x078 I2C Receive FIFO Level Register This register cont
  __IO uint32_t IC_SDA_HOLD;                 // 0x07C I2C SDA Hold Time Length Register The bits [15:0] 
  __IO uint32_t IC_TX_ABRT_SOURCE;           // 0x080 I2C Transmit Abort Source Register This register h
  __IO uint32_t IC_SLV_DATA_NACK_ONLY;       // 0x084 Generate Slave Data NACK Register The register is 
  __IO uint32_t IC_DMA_CR;                   // 0x088 DMA Control Register The register is used to enabl
  __IO uint32_t IC_DMA_TDLR;                 // 0x08C DMA Transmit Data Level Register
  __IO uint32_t IC_DMA_RDLR;                 // 0x090 I2C Receive Data Level Register
  __IO uint32_t IC_SDA_SETUP;                // 0x094 I2C SDA Setup Register This register controls the 
  __IO uint32_t IC_ACK_GENERAL_CALL;         // 0x098 I2C ACK General Call Register The register control
  __IO uint32_t IC_ENABLE_STATUS;            // 0x09C I2C Enable Status Register The register is used to
  __IO uint32_t IC_FS_SPKLEN;                // 0x0A0 I2C SS, FS or FM+ spike suppression limit This reg
  __I  uint32_t RESERVED2;
  __IO uint32_t IC_CLR_RESTART_DET;          // 0x0A8 Clear RESTART_DET Interrupt Register
  __I  uint32_t RESERVED3[18];
  __IO uint32_t IC_COMP_PARAM_1;             // 0x0F4 Component Parameter Register 1 Note This register 
  __IO uint32_t IC_COMP_VERSION;             // 0x0F8 I2C Component Version Register
  __IO uint32_t IC_COMP_TYPE;                // 0x0FC I2C Component Type Register
} I2C_TypeDef;

// IO_BANK0
typedef struct {
  __IO uint32_t STATUS;                      // 0x00 GPIO status
  __IO uint32_t CTRL;                        // 0x04 GPIO control including function select and overrides.
} IO_BANK0_GPIO_TypeDef;

typedef struct {
  __IO uint32_t INTE[4];                     // 0x00 Interrupt Enable for proc0
  __IO uint32_t INTF[4];                     // 0x10 Interrupt Force for proc0
  __IO uint32_t INTS[4];                     // 0x20 Interrupt status after masking & forcing for proc0
} IO_BANK0_PROC_TypeDef;

typedef struct {
  IO_BANK0_GPIO_TypeDef GPIO[30];            // 0x000
  __IO uint32_t INTR[4];                     // 0x0F0 Raw Interrupts
  IO_BANK0_PROC_TypeDef PROC[2];             // 0x100
  __IO uint32_t DORMANT_WAKE_INTE[4];        // 0x160 Interrupt Enable for dormant_wake
  __IO uint32_t DORMANT_WAKE_INTF[4];        // 0x170 Interrupt Force for dormant_wake
  __IO uint32_t DORMANT_WAKE_INTS[4];        // 0x180 Interrupt status after masking & forcing for dorma
} IO_BANK0_TypeDef;

// IO_QSPI
typedef struct {
  __IO uint32_t STATUS;                      // 0x00 GPIO status
  __IO uint32_t CTRL;                        // 0x04 GPIO control including function select and overrides.
} IO_QSPI_GPIO_QSPI_SD_TypeDef;

typedef struct {
  __IO uint32_t INTE;                        // 0x00 Interrupt Enable for proc0
  __IO uint32_t INTF;                        // 0x04 Interrupt Force for proc0
  __IO uint32_t INTS;                        // 0x08 Interrupt status after masking & forcing for proc0
} IO_QSPI_PROC_TypeDef;

typedef struct {
  __IO uint32_t GPIO_QSPI_SCLK_STATUS;       // 0x000 GPIO status
  __IO uint32_t GPIO_QSPI_SCLK_CTRL;         // 0x004 GPIO control including function select and overrid
  __IO uint32_t GPIO_QSPI_SS_STATUS;         // 0x008 GPIO status
  __IO uint32_t GPIO_QSPI_SS_CTRL;           // 0x00C GPIO control including function select and overrid
  IO_QSPI_GPIO_QSPI_SD_TypeDef GPIO_QSPI_SD[4]; // 0x010
  __IO uint32_t INTR;                        // 0x030 Raw Interrupts
  IO_QSPI_PROC_TypeDef PROC[2];              // 0x034
  __IO uint32_t DORMANT_WAKE_INTE;           // 0x04C Interrupt Enable for dormant_wake
  __IO uint32_t DORMANT_WAKE_INTF;           // 0x050 Interrupt Force for dormant_wake
  __IO uint32_t DORMANT_WAKE_INTS;           // 0x054 Interrupt status after masking & forcing for dorma
} IO_QSPI_TypeDef;

// PADS_BANK0
typedef struct {
  __IO uint32_t VOLTAGE_SELECT;              // 0x000 Voltage select. Per bank control
  __IO uint32_t GPIO[30];                    // 0x004 Pad control register
  __IO uint32_t SWCLK;                       // 0x07C Pad control register
  __IO uint32_t SWD;                         // 0x080 Pad control register
} PADS_BANK0_TypeDef;

// PADS_QSPI
typedef struct {
  __IO uint32_t VOLTAGE_SELECT;              // 0x000 Voltage select. Per bank control
  __IO uint32_t GPIO_QSPI_SCLK;              // 0x004 Pad control register
  __IO uint32_t GPIO_QSPI_SD[4];             // 0x008 Pad control register
  __IO uint32_t GPIO_QSPI_SS;                // 0x018 Pad control register
} PADS_QSPI_TypeDef;

// PIO
typedef struct {
  __IO uint32_t CLKDIV;                      // 0x00 Clock divisor register for state machine 0 Frequency = clock
  __IO uint32_t EXECCTRL;                    // 0x04 Execution/behavioural settings for state machine 0
  __IO uint32_t SHIFTCTRL;                   // 0x08 Control behaviour of the input/output shift registers for st
  __IO uint32_t ADDR;                        // 0x0C Current instruction address of state machine 0
  __IO uint32_t INSTR;                       // 0x10 Read to see the instruction currently addressed by state mac
  __IO uint32_t PINCTRL;                     // 0x14 State machine pin control
} PIO_SM_TypeDef;

typedef struct {
  __IO uint32_t INTE;                        // 0x00 Interrupt Enable for irq0
  __IO uint32_t INTF;                        // 0x04 Interrupt Force for irq0
  __IO uint32_t INTS;                        // 0x08 Interrupt status after masking & forcing for irq0
} PIO_IRQ_CTRL_TypeDef;

typedef struct {
  __IO uint32_t CTRL;                        // 0x000 PIO control register
  __IO uint32_t FSTAT;                       // 0x004 FIFO status register
  __IO uint32_t FDEBUG;                      // 0x008 FIFO debug register
  __IO uint32_t FLEVEL;                      // 0x00C FIFO levels
  __IO uint32_t TXF[4];                      // 0x010 Direct write access to the TX FIFO for this state 
  __IO uint32_t RXF[4];                      // 0x020 Direct read access to the RX FIFO for this state m
  __IO uint32_t IRQ;                         // 0x030 State machine IRQ flags register. Write 1 to clear
  __IO uint32_t IRQ_FORCE;                   // 0x034 Writing a 1 to each of these bits will forcibly as
  __IO uint32_t INPUT_SYNC_BYPASS;           // 0x038 There is a 2-flipflop synchronizer on each GPIO in
  __IO uint32_t DBG_PADOUT;                  // 0x03C Read to sample the pad output values PIO is curren
  __IO uint32_t DBG_PADOE;                   // 0x040 Read to sample the pad output enables (direction) 
  __IO uint32_t DBG_CFGINFO;                 // 0x044 The PIO hardware has some free parameters that may
  __IO uint32_t INSTR_MEM[32];               // 0x048 Write-only access to instruction memory location 0
  PIO_SM_TypeDef SM[4];                      // 0x0C8
  __IO uint32_t INTR;                        // 0x128 Raw Interrupts
  PIO_IRQ_CTRL_TypeDef IRQ_CTRL[2];          // 0x12C
} PIO_TypeDef;

// PLL
typedef struct {
  __IO uint32_t CS;                          // 0x000 Control and Status GENERAL CONSTRAINTS: Reference 
  __IO uint32_t PWR;                         // 0x004 Controls the PLL power modes.
  __IO uint32_t FBDIV_INT;                   // 0x008 Feedback divisor (note: this PLL does not support 
  __IO uint32_t PRIM;                        // 0x00C Controls the PLL post dividers for the primary out
} PLL_TypeDef;

// PSM
typedef struct {
  __IO uint32_t FRCE_ON;                     // 0x000 Force block out of reset (i.e. power it on)
  __IO uint32_t FRCE_OFF;                    // 0x004 Force into reset (i.e. power it off)
  __IO uint32_t WDSEL;                       // 0x008 Set to 1 if this peripheral should be reset when t
  __IO uint32_t DONE;                        // 0x00C Indicates the peripheral's registers are ready to 
} PSM_TypeDef;

// PWM
typedef struct {
  __IO uint32_t CSR;                         // 0x00 Control and status register
  __IO uint32_t DIV;                         // 0x04 INT and FRAC form a fixed-point fractional number. Counting 
  __IO uint32_t CTR;                         // 0x08 Direct access to the PWM counter
  __IO uint32_t CC;                          // 0x0C Counter compare values
  __IO uint32_t TOP;                         // 0x10 Counter wrap value
} PWM_CH_TypeDef;

typedef struct {
  PWM_CH_TypeDef CH[8];                      // 0x000
  __IO uint32_t EN;                          // 0x0A0 This register aliases the CSR_EN bits for all chan
  __IO uint32_t INTR;                        // 0x0A4 Raw Interrupts
  __IO uint32_t INTE;                        // 0x0A8 Interrupt Enable
  __IO uint32_t INTF;                        // 0x0AC Interrupt Force
  __IO uint32_t INTS;                        // 0x0B0 Interrupt status after masking & forcing
} PWM_TypeDef;

// RESETS
typedef struct {
  __IO uint32_t RESET;                       // 0x000 Reset control. If a bit is set it means the periph
  __IO uint32_t WDSEL;                       // 0x004 Watchdog select. If a bit is set then the watchdog
  __IO uint32_t RESET_DONE;                  // 0x008 Reset done. If a bit is set then a reset done sign
} RESETS_TypeDef;

// ROSC
typedef struct {
  __IO uint32_t CTRL;                        // 0x000 Ring Oscillator control
  __IO uint32_t FREQA;                       // 0x004 The FREQA & FREQB registers control the frequency 
  __IO uint32_t FREQB;                       // 0x008 For a detailed description see freqa register
  __IO uint32_t DORMANT;                     // 0x00C Ring Oscillator pause control
  __IO uint32_t DIV;                         // 0x010 Controls the output divider
  __IO uint32_t PHASE;                       // 0x014 Controls the phase shifted output
  __IO uint32_t STATUS;                      // 0x018 Ring Oscillator Status
  __IO uint32_t RANDOMBIT;                   // 0x01C This just reads the state of the oscillator output
  __IO uint32_t COUNT;                       // 0x020 A down counter running at the ROSC frequency which
} ROSC_TypeDef;

// RTC
typedef struct {
  __IO uint32_t CLKDIV_M1;                   // 0x000 Divider minus 1 for the 1 second counter. Safe to 
  __IO uint32_t SETUP_0;                     // 0x004 RTC setup register 0
  __IO uint32_t SETUP_1;                     // 0x008 RTC setup register 1
  __IO uint32_t CTRL;                        // 0x00C RTC Control and status
  __IO uint32_t IRQ_SETUP_0;                 // 0x010 Interrupt setup register 0
  __IO uint32_t IRQ_SETUP_1;                 // 0x014 Interrupt setup register 1
  __IO uint32_t RTC_1;                       // 0x018 RTC register 1.
  __IO uint32_t RTC_0;                       // 0x01C RTC register 0 Read this before RTC 1!
  __IO uint32_t INTR;                        // 0x020 Raw Interrupts
  __IO uint32_t INTE;                        // 0x024 Interrupt Enable
  __IO uint32_t INTF;                        // 0x028 Interrupt Force
  __IO uint32_t INTS;                        // 0x02C Interrupt status after masking & forcing
} RTC_TypeDef;

// SIO
typedef struct {
  __IO uint32_t ACCUM[2];                    // 0x00 Read/write access to accumulator 0
  __IO uint32_t BASE[3];                     // 0x08 Read/write access to BASE0 register.
  __IO uint32_t POP_LANE[2];                 // 0x14 Read LANE0 result, and simultaneously write lane results to 
  __IO uint32_t POP_FULL;                    // 0x1C Read FULL result, and simultaneously write lane results to b
  __IO uint32_t PEEK_LANE[2];                // 0x20 Read LANE0 result, without altering any internal state (PEEK
  __IO uint32_t PEEK_FULL;                   // 0x28 Read FULL result, without altering any internal state (PEEK)
  __IO uint32_t CTRL_LANE[2];                // 0x2C Control register for lane 0
  __IO uint32_t ACCUM0_ADD;                  // 0x34 Values written here are atomically added to ACCUM0 Reading y
  __IO uint32_t ACCUM1_ADD;                  // 0x38 Values written here are atomically added to ACCUM1 Reading y
  __IO uint32_t BASE_1AND0;                  // 0x3C On write, the lower 16 bits go to BASE0, upper bits to BASE1
} SIO_INTERP_TypeDef;

typedef struct {
  __IO uint32_t CPUID;                       // 0x000 Processor core identifier
  __IO uint32_t GPIO_IN;                     // 0x004 Input value for GPIO pins
  __IO uint32_t GPIO_HI_IN;                  // 0x008 Input value for QSPI pins
  __I  uint32_t RESERVED0;
  __IO uint32_t GPIO_OUT;                    // 0x010 GPIO output value
  __IO uint32_t GPIO_OUT_SET;                // 0x014 GPIO output value set
  __IO uint32_t GPIO_OUT_CLR;                // 0x018 GPIO output value clear
  __IO uint32_t GPIO_OUT_XOR;                // 0x01C GPIO output value XOR
  __IO uint32_t GPIO_OE;                     // 0x020 GPIO output enable
  __IO uint32_t GPIO_OE_SET;                 // 0x024 GPIO output enable set
  __IO uint32_t GPIO_OE_CLR;                 // 0x028 GPIO output enable clear
  __IO uint32_t GPIO_OE_XOR;                 // 0x02C GPIO output enable XOR
  __IO uint32_t GPIO_HI_OUT;                 // 0x030 QSPI output value
  __IO uint32_t GPIO_HI_OUT_SET;             // 0x034 QSPI output value set
  __IO uint32_t GPIO_HI_OUT_CLR;             // 0x038 QSPI output value clear
  __IO uint32_t GPIO_HI_OUT_XOR;             // 0x03C QSPI output value XOR
  __IO uint32_t GPIO_HI_OE;                  // 0x040 QSPI output enable
  __IO uint32_t GPIO_HI_OE_SET;              // 0x044 QSPI output enable set
  __IO uint32_t GPIO_HI_OE_CLR;              // 0x048 QSPI output enable clear
  __IO uint32_t GPIO_HI_OE_XOR;              // 0x04C QSPI output enable XOR
  __IO uint32_t FIFO_ST;                     // 0x050 Status register for inter-core FIFOs (mailboxes). 
  __IO uint32_t FIFO_WR;                     // 0x054 Write access to this core's TX FIFO
  __IO uint32_t FIFO_RD;                     // 0x058 Read access to this core's RX FIFO
  __IO uint32_t SPINLOCK_ST;                 // 0x05C Spinlock state A bitmap containing the state of al
  __IO uint32_t DIV_UDIVIDEND;               // 0x060 Divider unsigned dividend Write to the DIVIDEND op
  __IO uint32_t DIV_UDIVISOR;                // 0x064 Divider unsigned divisor Write to the DIVISOR oper
  __IO uint32_t DIV_SDIVIDEND;               // 0x068 Divider signed dividend The same as UDIVIDEND, but
  __IO uint32_t DIV_SDIVISOR;                // 0x06C Divider signed divisor The same as UDIVISOR, but s
  __IO uint32_t DIV_QUOTIENT;                // 0x070 Divider result quotient The result of `DIVIDEND / 
  __IO uint32_t DIV_REMAINDER;               // 0x074 Divider result remainder The result of `DIVIDEND %
  __IO uint32_t DIV_CSR;                     // 0x078 Control and status register for divider.
  __I  uint32_t RESERVED1;
  SIO_INTERP_TypeDef INTERP[2];              // 0x080
  __IO uint32_t SPINLOCK[32];                // 0x100 Reading from a spinlock address will: - Return 0 i
} SIO_TypeDef;

// SPI
typedef struct {
  __IO uint32_t SSPCR0;                      // 0x000 Control register 0, SSPCR0 on page 3-4
  __IO uint32_t SSPCR1;                      // 0x004 Control register 1, SSPCR1 on page 3-5
  __IO uint32_t SSPDR;                       // 0x008 Data register, SSPDR on page 3-6
  __IO uint32_t SSPSR;                       // 0x00C Status register, SSPSR on page 3-7
  __IO uint32_t SSPCPSR;                     // 0x010 Clock prescale register, SSPCPSR on page 3-8
  __IO uint32_t SSPIMSC;                     // 0x014 Interrupt mask set or clear register, SSPIMSC on p
  __IO uint32_t SSPRIS;                      // 0x018 Raw interrupt status register, SSPRIS on page 3-10
  __IO uint32_t SSPMIS;                      // 0x01C Masked interrupt status register, SSPMIS on page 3
  __IO uint32_t SSPICR;                      // 0x020 Interrupt clear register, SSPICR on page 3-11
  __IO uint32_t SSPDMACR;                    // 0x024 DMA control register, SSPDMACR on page 3-12
  __I  uint32_t RESERVED0[1006];
  __IO uint32_t SSPPERIPHID0;                // 0xFE0 Peripheral identification registers, SSPPeriphID0-
  __IO uint32_t SSPPERIPHID1;                // 0xFE4 Peripheral identification registers, SSPPeriphID0-
  __IO uint32_t SSPPERIPHID2;                // 0xFE8 Peripheral identification registers, SSPPeriphID0-
  __IO uint32_t SSPPERIPHID3;                // 0xFEC Peripheral identification registers, SSPPeriphID0-
  __IO uint32_t SSPPCELLID[4];               // 0xFF0 PrimeCell identification registers, SSPPCellID0-3 
} SPI_TypeDef;

// SSI
typedef struct {
  __IO uint32_t CTRLR0;                      // 0x000 Control register 0
  __IO uint32_t CTRLR1;                      // 0x004 Master Control register 1
  __IO uint32_t SSIENR;                      // 0x008 SSI Enable
  __IO uint32_t MWCR;                        // 0x00C Microwire Control
  __IO uint32_t SER;                         // 0x010 Slave enable
  __IO uint32_t BAUDR;                       // 0x014 Baud rate
  __IO uint32_t TXFTLR;                      // 0x018 TX FIFO threshold level
  __IO uint32_t RXFTLR;                      // 0x01C RX FIFO threshold level
  __IO uint32_t TXFLR;                       // 0x020 TX FIFO level
  __IO uint32_t RXFLR;                       // 0x024 RX FIFO level
  __IO uint32_t SR;                          // 0x028 Status register
  __IO uint32_t IMR;                         // 0x02C Interrupt mask
  __IO uint32_t ISR;                         // 0x030 Interrupt status
  __IO uint32_t RISR;                        // 0x034 Raw interrupt status
  __IO uint32_t TXOICR;                      // 0x038 TX FIFO overflow interrupt clear
  __IO uint32_t RXOICR;                      // 0x03C RX FIFO overflow interrupt clear
  __IO uint32_t RXUICR;                      // 0x040 RX FIFO underflow interrupt clear
  __IO uint32_t MSTICR;                      // 0x044 Multi-master interrupt clear
  __IO uint32_t ICR;                         // 0x048 Interrupt clear
  __IO uint32_t DMACR;                       // 0x04C DMA control
  __IO uint32_t DMATDLR;                     // 0x050 DMA TX data level
  __IO uint32_t DMARDLR;                     // 0x054 DMA RX data level
  __IO uint32_t IDR;                         // 0x058 Identification register
  __IO uint32_t SSI_VERSION_ID;              // 0x05C Version ID
  __IO uint32_t DR0;                         // 0x060 Data Register 0 (of 36)
  __I  uint32_t RESERVED0[35];
  __IO uint32_t RX_SAMPLE_DLY;               // 0x0F0 RX sample delay
  __IO uint32_t SPI_CTRLR0;                  // 0x0F4 SPI control
  __IO uint32_t TXD_DRIVE_EDGE;              // 0x0F8 TX drive edge
} SSI_TypeDef;

// SYSCFG
typedef struct {
  __IO uint32_t PROC_NMI_MASK[2];            // 0x000 Processor core 0 NMI source mask
  __IO uint32_t PROC_CONFIG;                 // 0x008 Configuration for processors
  __IO uint32_t PROC_IN_SYNC_BYPASS;         // 0x00C For each bit, if 1, bypass the input synchronizer 
  __IO uint32_t PROC_IN_SYNC_BYPASS_HI;      // 0x010 For each bit, if 1, bypass the input synchronizer 
  __IO uint32_t DBGFORCE;                    // 0x014 Directly control the SWD debug port of either proc
  __IO uint32_t MEMPOWERDOWN;                // 0x018 Control power downs to memories. Set high to power
} SYSCFG_TypeDef;

// SYSINFO
typedef struct {
  __IO uint32_t CHIP_ID;                     // 0x000 JEDEC JEP-106 compliant chip identifier.
  __IO uint32_t PLATFORM;                    // 0x004 Platform register. Allows software to know what en
  __I  uint32_t RESERVED0[14];
  __IO uint32_t GITREF_RP2040;               // 0x040 Git hash of the chip source. Used to identify chip
} SYSINFO_TypeDef;

// TBMAN
typedef struct {
  __IO uint32_t PLATFORM;                    // 0x000 Indicates the type of platform in use
} TBMAN_TypeDef;

// TIMER
typedef struct {
  __IO uint32_t TIMEHW;                      // 0x000 Write to bits 63:32 of time always write timelw be
  __IO uint32_t TIMELW;                      // 0x004 Write to bits 31:0 of time writes do not get copie
  __IO uint32_t TIMEHR;                      // 0x008 Read from bits 63:32 of time always read timelr be
  __IO uint32_t TIMELR;                      // 0x00C Read from bits 31:0 of time
  __IO uint32_t ALARM[4];                    // 0x010 Arm alarm 0, and configure the time it will fire. 
  __IO uint32_t ARMED;                       // 0x020 Indicates the armed/disarmed status of each alarm.
  __IO uint32_t TIMERAWH;                    // 0x024 Raw read from bits 63:32 of time (no side effects)
  __IO uint32_t TIMERAWL;                    // 0x028 Raw read from bits 31:0 of time (no side effects)
  __IO uint32_t DBGPAUSE;                    // 0x02C Set bits high to enable pause when the correspondi
  __IO uint32_t PAUSE;                       // 0x030 Set high to pause the timer
  __IO uint32_t INTR;                        // 0x034 Raw Interrupts
  __IO uint32_t INTE;                        // 0x038 Interrupt Enable
  __IO uint32_t INTF;                        // 0x03C Interrupt Force
  __IO uint32_t INTS;                        // 0x040 Interrupt status after masking & forcing
} TIMER_TypeDef;

// UART
typedef struct {
  __IO uint32_t UARTDR;                      // 0x000 Data Register, UARTDR
  __IO uint32_t UARTRSR;                     // 0x004 Receive Status Register/Error Clear Register, UART
  __I  uint32_t RESERVED0[4];
  __IO uint32_t UARTFR;                      // 0x018 Flag Register, UARTFR
  __I  uint32_t RESERVED1;
  __IO uint32_t UARTILPR;                    // 0x020 IrDA Low-Power Counter Register, UARTILPR
  __IO uint32_t UARTIBRD;                    // 0x024 Integer Baud Rate Register, UARTIBRD
  __IO uint32_t UARTFBRD;                    // 0x028 Fractional Baud Rate Register, UARTFBRD
  __IO uint32_t UARTLCR_H;                   // 0x02C Line Control Register, UARTLCR_H
  __IO uint32_t UARTCR;                      // 0x030 Control Register, UARTCR
  __IO uint32_t UARTIFLS;                    // 0x034 Interrupt FIFO Level Select Register, UARTIFLS
  __IO uint32_t UARTIMSC;                    // 0x038 Interrupt Mask Set/Clear Register, UARTIMSC
  __IO uint32_t UARTRIS;                     // 0x03C Raw Interrupt Status Register, UARTRIS
  __IO uint32_t UARTMIS;                     // 0x040 Masked Interrupt Status Register, UARTMIS
  __IO uint32_t UARTICR;                     // 0x044 Interrupt Clear Register, UARTICR
  __IO uint32_t UARTDMACR;                   // 0x048 DMA Control Register, UARTDMACR
  __I  uint32_t RESERVED2[997];
  __IO uint32_t UARTPERIPHID0;               // 0xFE0 UARTPeriphID0 Register
  __IO uint32_t UARTPERIPHID1;               // 0xFE4 UARTPeriphID1 Register
  __IO uint32_t UARTPERIPHID2;               // 0xFE8 UARTPeriphID2 Register
  __IO uint32_t UARTPERIPHID3;               // 0xFEC UARTPeriphID3 Register
  __IO uint32_t UARTPCELLID[4];              // 0xFF0 UARTPCellID0 Register
} UART_TypeDef;

// USB
typedef struct {
  __IO uint32_t ADDR_ENDP;                   // 0x000 Device address and endpoint control
  __IO uint32_t ADDR_ENDP1;                  // 0x004 Interrupt endpoint 1. Only valid for HOST mode.
  __IO uint32_t ADDR_ENDP2;                  // 0x008 Interrupt endpoint 2. Only valid for HOST mode.
  __IO uint32_t ADDR_ENDP3;                  // 0x00C Interrupt endpoint 3. Only valid for HOST mode.
  __IO uint32_t ADDR_ENDP4;                  // 0x010 Interrupt endpoint 4. Only valid for HOST mode.
  __IO uint32_t ADDR_ENDP5;                  // 0x014 Interrupt endpoint 5. Only valid for HOST mode.
  __IO uint32_t ADDR_ENDP6;                  // 0x018 Interrupt endpoint 6. Only valid for HOST mode.
  __IO uint32_t ADDR_ENDP7;                  // 0x01C Interrupt endpoint 7. Only valid for HOST mode.
  __IO uint32_t ADDR_ENDP8;                  // 0x020 Interrupt endpoint 8. Only valid for HOST mode.
  __IO uint32_t ADDR_ENDP9;                  // 0x024 Interrupt endpoint 9. Only valid for HOST mode.
  __IO uint32_t ADDR_ENDP10;                 // 0x028 Interrupt endpoint 10. Only valid for HOST mode.
  __IO uint32_t ADDR_ENDP11;                 // 0x02C Interrupt endpoint 11. Only valid for HOST mode.
  __IO uint32_t ADDR_ENDP12;                 // 0x030 Interrupt endpoint 12. Only valid for HOST mode.
  __IO uint32_t ADDR_ENDP13;                 // 0x034 Interrupt endpoint 13. Only valid for HOST mode.
  __IO uint32_t ADDR_ENDP14;                 // 0x038 Interrupt endpoint 14. Only valid for HOST mode.
  __IO uint32_t ADDR_ENDP15;                 // 0x03C Interrupt endpoint 15. Only valid for HOST mode.
  __IO uint32_t MAIN_CTRL;                   // 0x040 Main control register
  __IO uint32_t SOF_WR;                      // 0x044 Set the SOF (Start of Frame) frame number in the h
  __IO uint32_t SOF_RD;                      // 0x048 Read the last SOF (Start of Frame) frame number se
  __IO uint32_t SIE_CTRL;                    // 0x04C SIE control register
  __IO uint32_t SIE_STATUS;                  // 0x050 SIE status register
  __IO uint32_t INT_EP_CTRL;                 // 0x054 interrupt endpoint control register
  __IO uint32_t BUFF_STATUS;                 // 0x058 Buffer status register. A bit set here indicates t
  __IO uint32_t BUFF_CPU_SHOULD_HANDLE;      // 0x05C Which of the double buffers should be handled. Onl
  __IO uint32_t EP_ABORT;                    // 0x060 Device only: Can be set to ignore the buffer contr
  __IO uint32_t EP_ABORT_DONE;               // 0x064 Device only: Used in conjunction with `EP_ABORT`. 
  __IO uint32_t EP_STALL_ARM;                // 0x068 Device: this bit must be set in conjunction with t
  __IO uint32_t NAK_POLL;                    // 0x06C Used by the host controller. Sets the wait time in
  __IO uint32_t EP_STATUS_STALL_NAK;         // 0x070 Device: bits are set when the `IRQ_ON_NAK` or `IRQ
  __IO uint32_t USB_MUXING;                  // 0x074 Where to connect the USB controller. Should be to_
  __IO uint32_t USB_PWR;                     // 0x078 Overrides for the power signals in the event that 
  __IO uint32_t USBPHY_DIRECT;               // 0x07C This register allows for direct control of the USB
  __IO uint32_t USBPHY_DIRECT_OVERRIDE;      // 0x080 Override enable for each control in usbphy_direct
  __IO uint32_t USBPHY_TRIM;                 // 0x084 Used to adjust trim values of USB phy pull down re
  __I  uint32_t RESERVED0;
  __IO uint32_t INTR;                        // 0x08C Raw Interrupts
  __IO uint32_t INTE;                        // 0x090 Interrupt Enable
  __IO uint32_t INTF;                        // 0x094 Interrupt Force
  __IO uint32_t INTS;                        // 0x098 Interrupt status after masking & forcing
} USB_TypeDef;

// VREG_AND_CHIP_RESET
typedef struct {
  __IO uint32_t VREG;                        // 0x000 Voltage regulator control and status
  __IO uint32_t BOD;                         // 0x004 brown-out detection control
  __IO uint32_t CHIP_RESET;                  // 0x008 Chip reset control and status
} VREG_AND_CHIP_RESET_TypeDef;

// WATCHDOG
typedef struct {
  __IO uint32_t CTRL;                        // 0x000 Watchdog control The rst_wdsel register determines
  __IO uint32_t LOAD;                        // 0x004 Load the watchdog timer. The maximum setting is 0x
  __IO uint32_t REASON;                      // 0x008 Logs the reason for the last reset. Both bits are 
  __IO uint32_t SCRATCH[8];                  // 0x00C Scratch register. Information persists through sof
  __IO uint32_t TICK;                        // 0x02C Controls the tick generator
} WATCHDOG_TypeDef;

// XIP_CTRL
typedef struct {
  __IO uint32_t CTRL;                        // 0x000 Cache control
  __IO uint32_t FLUSH;                       // 0x004 Cache Flush control
  __IO uint32_t STAT;                        // 0x008 Cache Status
  __IO uint32_t CTR_HIT;                     // 0x00C Cache Hit counter
  __IO uint32_t CTR_ACC;                     // 0x010 Cache Access counter
  __IO uint32_t STREAM_ADDR;                 // 0x014 FIFO stream address
  __IO uint32_t STREAM_CTR;                  // 0x018 FIFO stream control
  __IO uint32_t STREAM_FIFO;                 // 0x01C FIFO stream data
} XIP_CTRL_TypeDef;

// XOSC
typedef struct {
  __IO uint32_t CTRL;                        // 0x000 Crystal Oscillator Control
  __IO uint32_t STATUS;                      // 0x004 Crystal Oscillator Status
  __IO uint32_t DORMANT;                     // 0x008 Crystal Oscillator pause control
  __IO uint32_t STARTUP;                     // 0x00C Controls the startup delay
  __I  uint32_t RESERVED0[3];
  __IO uint32_t COUNT;                       // 0x01C A down counter running at the xosc frequency which
} XOSC_TypeDef;

// ===================================================================================
// Peripheral Instances
// ===================================================================================
#define ADC                  ((ADC_TypeDef *) 0x4004C000)
#define BUSCTRL              ((BUSCTRL_TypeDef *) 0x40030000)
#define CLOCKS               ((CLOCKS_TypeDef *) 0x40008000)
#define DMA                  ((DMA_TypeDef *) 0x50000000)
#define I2C0                 ((I2C_TypeDef *) 0x40044000)
#define I2C1                 ((I2C_TypeDef *) 0x40048000)
#define IO_BANK0             ((IO_BANK0_TypeDef *) 0x40014000)
#define IO_QSPI              ((IO_QSPI_TypeDef *) 0x40018000)
#define PADS_BANK0           ((PADS_BANK0_TypeDef *) 0x4001C000)
#define PADS_QSPI            ((PADS_QSPI_TypeDef *) 0x40020000)
#define PIO0                 ((PIO_TypeDef *) 0x50200000)
#define PIO1                 ((PIO_TypeDef *) 0x50300000)
#define PLL_SYS              ((PLL_TypeDef *) 0x40028000)
#define PLL_USB              ((PLL_TypeDef *) 0x4002C000)
#define PSM                  ((PSM_TypeDef *) 0x40010000)
#define PWM                  ((PWM_TypeDef *) 0x40050000)
#define RESETS               ((RESETS_TypeDef *) 0x4000C000)
#define ROSC                 ((ROSC_TypeDef *) 0x40060000)
#define RTC                  ((RTC_TypeDef *) 0x4005C000)
#define SIO                  ((SIO_TypeDef *) 0xD0000000)
#define SPI0                 ((SPI_TypeDef *) 0x4003C000)
#define SPI1                 ((SPI_TypeDef *) 0x40040000)
#define SSI                  ((SSI_TypeDef *) 0x18000000)
#define SYSCFG               ((SYSCFG_TypeDef *) 0x40004000)
#define SYSINFO              ((SYSINFO_TypeDef *) 0x40000000)
#define TBMAN                ((TBMAN_TypeDef *) 0x4006C000)
#define TIMER                ((TIMER_TypeDef *) 0x40054000)
#define UART0                ((UART_TypeDef *) 0x40034000)
#define UART1                ((UART_TypeDef *) 0x40038000)
#define USB                  ((USB_TypeDef *) 0x50110000)
#define VREG_AND_CHIP_RESET  ((VREG_AND_CHIP_RESET_TypeDef *) 0x40064000)
#define WATCHDOG             ((WATCHDOG_TypeDef *) 0x40058000)
#define XIP_CTRL             ((XIP_CTRL_TypeDef *) 0x14000000)
#define XOSC                 ((XOSC_TypeDef *) 0x40024000)

// ===================================================================================
// USB Device Dual-Port RAM (DPRAM) - endpoint control and buffer control registers
// ===================================================================================
typedef struct {
  __IO uint32_t IN;                         // IN endpoint control (host <- device)
  __IO uint32_t OUT;                        // OUT endpoint control (host -> device)
} USB_DPRAM_EP_TypeDef;

typedef struct {
  union {
    struct {
      __IO uint32_t SETUP_PACKET_LOW;       // 0x000 bytes 0..3 of the last SETUP packet
      __IO uint32_t SETUP_PACKET_HIGH;      // 0x004 bytes 4..7 of the last SETUP packet
    };
    USB_DPRAM_EP_TypeDef EP_CTRL[16];       // 0x000 endpoint control EP1..EP15 (EP_CTRL[n])
  };
  USB_DPRAM_EP_TypeDef EP_BUF_CTRL[16];     // 0x080 buffer control EP0..EP15
  __IO uint8_t  EP0_BUF[64];                // 0x100 EP0 buffer (shared by IN and OUT)
  __IO uint8_t  OPT_BUF[64];                // 0x140 optional second EP0 buffer
  __IO uint8_t  BUF[3712];                  // 0x180 data buffers for EP1..EP15
} USB_DPRAM_TypeDef;
// (EP_CTRL[0] occupies the same address as the SETUP packet and must not be used)
_Static_assert(sizeof(USB_DPRAM_TypeDef) == 4096, "USB_DPRAM_TypeDef size");

#define USB_DPRAM           ((USB_DPRAM_TypeDef *) USB_DPRAM_BASE)

// Endpoint control register (USB_DPRAM->EP_CTRL[n].IN / .OUT)
#define USB_EP_CTRL_ENABLE                  ((uint32_t)0x80000000)  // enable endpoint
#define USB_EP_CTRL_DOUBLE_BUFFERED         ((uint32_t)0x40000000)  // double buffered
#define USB_EP_CTRL_INT_PER_BUFFER          ((uint32_t)0x20000000)  // interrupt per buffer
#define USB_EP_CTRL_INT_PER_DOUBLE_BUFFER   ((uint32_t)0x10000000)  // interrupt per double buffer
#define USB_EP_CTRL_TYPE_Pos                26                      // endpoint type
#define USB_EP_CTRL_TYPE_Msk                ((uint32_t)0x0C000000)
#define USB_EP_CTRL_TYPE_CONTROL            ((uint32_t)0x00000000)
#define USB_EP_CTRL_TYPE_ISOCHRONOUS        ((uint32_t)0x04000000)
#define USB_EP_CTRL_TYPE_BULK               ((uint32_t)0x08000000)
#define USB_EP_CTRL_TYPE_INTERRUPT          ((uint32_t)0x0C000000)
#define USB_EP_CTRL_INT_ON_STALL            ((uint32_t)0x00020000)  // interrupt on stall
#define USB_EP_CTRL_INT_ON_NAK              ((uint32_t)0x00010000)  // interrupt on NAK
#define USB_EP_CTRL_BUFFER_ADDR_Msk         ((uint32_t)0x0000FFC0)  // buffer offset in DPRAM

// Buffer control register (USB_DPRAM->EP_BUF_CTRL[n].IN / .OUT), buffer 0 bits
#define USB_BUF_CTRL_FULL                   ((uint32_t)0x00008000)  // buffer full
#define USB_BUF_CTRL_LAST                   ((uint32_t)0x00004000)  // last buffer of transfer
#define USB_BUF_CTRL_DATA1_PID              ((uint32_t)0x00002000)  // DATA1 PID
#define USB_BUF_CTRL_DATA0_PID              ((uint32_t)0x00000000)  // DATA0 PID
#define USB_BUF_CTRL_SEL                    ((uint32_t)0x00001000)  // reset buffer select
#define USB_BUF_CTRL_STALL                  ((uint32_t)0x00000800)  // stall
#define USB_BUF_CTRL_AVAIL                  ((uint32_t)0x00000400)  // buffer available
#define USB_BUF_CTRL_LEN_Msk                ((uint32_t)0x000003FF)  // buffer length

// ===================================================================================
// Peripheral Register Bit Definitions
// ===================================================================================
// ADC->CS
#define ADC_CS_RROBIN_Pos                                    16
#define ADC_CS_RROBIN_Msk                                    ((uint32_t)0x001F0000)
#define ADC_CS_RROBIN                                        ADC_CS_RROBIN_Msk
#define ADC_CS_AINSEL_Pos                                    12
#define ADC_CS_AINSEL_Msk                                    ((uint32_t)0x00007000)
#define ADC_CS_AINSEL                                        ADC_CS_AINSEL_Msk
#define ADC_CS_ERR_STICKY_Pos                                10
#define ADC_CS_ERR_STICKY_Msk                                ((uint32_t)0x00000400)
#define ADC_CS_ERR_STICKY                                    ADC_CS_ERR_STICKY_Msk
#define ADC_CS_ERR_Pos                                       9
#define ADC_CS_ERR_Msk                                       ((uint32_t)0x00000200)
#define ADC_CS_ERR                                           ADC_CS_ERR_Msk
#define ADC_CS_READY_Pos                                     8
#define ADC_CS_READY_Msk                                     ((uint32_t)0x00000100)
#define ADC_CS_READY                                         ADC_CS_READY_Msk
#define ADC_CS_START_MANY_Pos                                3
#define ADC_CS_START_MANY_Msk                                ((uint32_t)0x00000008)
#define ADC_CS_START_MANY                                    ADC_CS_START_MANY_Msk
#define ADC_CS_START_ONCE_Pos                                2
#define ADC_CS_START_ONCE_Msk                                ((uint32_t)0x00000004)
#define ADC_CS_START_ONCE                                    ADC_CS_START_ONCE_Msk
#define ADC_CS_TS_EN_Pos                                     1
#define ADC_CS_TS_EN_Msk                                     ((uint32_t)0x00000002)
#define ADC_CS_TS_EN                                         ADC_CS_TS_EN_Msk
#define ADC_CS_EN_Pos                                        0
#define ADC_CS_EN_Msk                                        ((uint32_t)0x00000001)
#define ADC_CS_EN                                            ADC_CS_EN_Msk
// ADC->RESULT
#define ADC_RESULT_RESULT_Pos                                0
#define ADC_RESULT_RESULT_Msk                                ((uint32_t)0x00000FFF)
#define ADC_RESULT_RESULT                                    ADC_RESULT_RESULT_Msk
// ADC->FCS
#define ADC_FCS_THRESH_Pos                                   24
#define ADC_FCS_THRESH_Msk                                   ((uint32_t)0x0F000000)
#define ADC_FCS_THRESH                                       ADC_FCS_THRESH_Msk
#define ADC_FCS_LEVEL_Pos                                    16
#define ADC_FCS_LEVEL_Msk                                    ((uint32_t)0x000F0000)
#define ADC_FCS_LEVEL                                        ADC_FCS_LEVEL_Msk
#define ADC_FCS_OVER_Pos                                     11
#define ADC_FCS_OVER_Msk                                     ((uint32_t)0x00000800)
#define ADC_FCS_OVER                                         ADC_FCS_OVER_Msk
#define ADC_FCS_UNDER_Pos                                    10
#define ADC_FCS_UNDER_Msk                                    ((uint32_t)0x00000400)
#define ADC_FCS_UNDER                                        ADC_FCS_UNDER_Msk
#define ADC_FCS_FULL_Pos                                     9
#define ADC_FCS_FULL_Msk                                     ((uint32_t)0x00000200)
#define ADC_FCS_FULL                                         ADC_FCS_FULL_Msk
#define ADC_FCS_EMPTY_Pos                                    8
#define ADC_FCS_EMPTY_Msk                                    ((uint32_t)0x00000100)
#define ADC_FCS_EMPTY                                        ADC_FCS_EMPTY_Msk
#define ADC_FCS_DREQ_EN_Pos                                  3
#define ADC_FCS_DREQ_EN_Msk                                  ((uint32_t)0x00000008)
#define ADC_FCS_DREQ_EN                                      ADC_FCS_DREQ_EN_Msk
#define ADC_FCS_ERR_Pos                                      2
#define ADC_FCS_ERR_Msk                                      ((uint32_t)0x00000004)
#define ADC_FCS_ERR                                          ADC_FCS_ERR_Msk
#define ADC_FCS_SHIFT_Pos                                    1
#define ADC_FCS_SHIFT_Msk                                    ((uint32_t)0x00000002)
#define ADC_FCS_SHIFT                                        ADC_FCS_SHIFT_Msk
#define ADC_FCS_EN_Pos                                       0
#define ADC_FCS_EN_Msk                                       ((uint32_t)0x00000001)
#define ADC_FCS_EN                                           ADC_FCS_EN_Msk
// ADC->FIFO
#define ADC_FIFO_ERR_Pos                                     15
#define ADC_FIFO_ERR_Msk                                     ((uint32_t)0x00008000)
#define ADC_FIFO_ERR                                         ADC_FIFO_ERR_Msk
#define ADC_FIFO_VAL_Pos                                     0
#define ADC_FIFO_VAL_Msk                                     ((uint32_t)0x00000FFF)
#define ADC_FIFO_VAL                                         ADC_FIFO_VAL_Msk
// ADC->DIV
#define ADC_DIV_INT_Pos                                      8
#define ADC_DIV_INT_Msk                                      ((uint32_t)0x00FFFF00)
#define ADC_DIV_INT                                          ADC_DIV_INT_Msk
#define ADC_DIV_FRAC_Pos                                     0
#define ADC_DIV_FRAC_Msk                                     ((uint32_t)0x000000FF)
#define ADC_DIV_FRAC                                         ADC_DIV_FRAC_Msk
// ADC->INTR
#define ADC_INTR_FIFO_Pos                                    0
#define ADC_INTR_FIFO_Msk                                    ((uint32_t)0x00000001)
#define ADC_INTR_FIFO                                        ADC_INTR_FIFO_Msk
// ADC->INTE
#define ADC_INTE_FIFO_Pos                                    0
#define ADC_INTE_FIFO_Msk                                    ((uint32_t)0x00000001)
#define ADC_INTE_FIFO                                        ADC_INTE_FIFO_Msk
// ADC->INTF
#define ADC_INTF_FIFO_Pos                                    0
#define ADC_INTF_FIFO_Msk                                    ((uint32_t)0x00000001)
#define ADC_INTF_FIFO                                        ADC_INTF_FIFO_Msk
// ADC->INTS
#define ADC_INTS_FIFO_Pos                                    0
#define ADC_INTS_FIFO_Msk                                    ((uint32_t)0x00000001)
#define ADC_INTS_FIFO                                        ADC_INTS_FIFO_Msk

// BUSCTRL->BUS_PRIORITY
#define BUSCTRL_BUS_PRIORITY_DMA_W_Pos                       12
#define BUSCTRL_BUS_PRIORITY_DMA_W_Msk                       ((uint32_t)0x00001000)
#define BUSCTRL_BUS_PRIORITY_DMA_W                           BUSCTRL_BUS_PRIORITY_DMA_W_Msk
#define BUSCTRL_BUS_PRIORITY_DMA_R_Pos                       8
#define BUSCTRL_BUS_PRIORITY_DMA_R_Msk                       ((uint32_t)0x00000100)
#define BUSCTRL_BUS_PRIORITY_DMA_R                           BUSCTRL_BUS_PRIORITY_DMA_R_Msk
#define BUSCTRL_BUS_PRIORITY_PROC1_Pos                       4
#define BUSCTRL_BUS_PRIORITY_PROC1_Msk                       ((uint32_t)0x00000010)
#define BUSCTRL_BUS_PRIORITY_PROC1                           BUSCTRL_BUS_PRIORITY_PROC1_Msk
#define BUSCTRL_BUS_PRIORITY_PROC0_Pos                       0
#define BUSCTRL_BUS_PRIORITY_PROC0_Msk                       ((uint32_t)0x00000001)
#define BUSCTRL_BUS_PRIORITY_PROC0                           BUSCTRL_BUS_PRIORITY_PROC0_Msk
// BUSCTRL->BUS_PRIORITY_ACK
#define BUSCTRL_BUS_PRIORITY_ACK_BUS_PRIORITY_ACK_Pos        0
#define BUSCTRL_BUS_PRIORITY_ACK_BUS_PRIORITY_ACK_Msk        ((uint32_t)0x00000001)
#define BUSCTRL_BUS_PRIORITY_ACK_BUS_PRIORITY_ACK            BUSCTRL_BUS_PRIORITY_ACK_BUS_PRIORITY_ACK_Msk
// BUSCTRL->PERFCTR0
#define BUSCTRL_PERFCTR0_PERFCTR0_Pos                        0
#define BUSCTRL_PERFCTR0_PERFCTR0_Msk                        ((uint32_t)0x00FFFFFF)
#define BUSCTRL_PERFCTR0_PERFCTR0                            BUSCTRL_PERFCTR0_PERFCTR0_Msk
// BUSCTRL->PERFSEL0
#define BUSCTRL_PERFSEL0_PERFSEL0_Pos                        0
#define BUSCTRL_PERFSEL0_PERFSEL0_Msk                        ((uint32_t)0x0000001F)
#define BUSCTRL_PERFSEL0_PERFSEL0                            BUSCTRL_PERFSEL0_PERFSEL0_Msk
#define BUSCTRL_PERFSEL0_PERFSEL0_APB_CONTESTED              ((uint32_t)0x00000000)
#define BUSCTRL_PERFSEL0_PERFSEL0_APB                        ((uint32_t)0x00000001)
#define BUSCTRL_PERFSEL0_PERFSEL0_FASTPERI_CONTESTED         ((uint32_t)0x00000002)
#define BUSCTRL_PERFSEL0_PERFSEL0_FASTPERI                   ((uint32_t)0x00000003)
#define BUSCTRL_PERFSEL0_PERFSEL0_SRAM5_CONTESTED            ((uint32_t)0x00000004)
#define BUSCTRL_PERFSEL0_PERFSEL0_SRAM5                      ((uint32_t)0x00000005)
#define BUSCTRL_PERFSEL0_PERFSEL0_SRAM4_CONTESTED            ((uint32_t)0x00000006)
#define BUSCTRL_PERFSEL0_PERFSEL0_SRAM4                      ((uint32_t)0x00000007)
#define BUSCTRL_PERFSEL0_PERFSEL0_SRAM3_CONTESTED            ((uint32_t)0x00000008)
#define BUSCTRL_PERFSEL0_PERFSEL0_SRAM3                      ((uint32_t)0x00000009)
#define BUSCTRL_PERFSEL0_PERFSEL0_SRAM2_CONTESTED            ((uint32_t)0x0000000A)
#define BUSCTRL_PERFSEL0_PERFSEL0_SRAM2                      ((uint32_t)0x0000000B)
#define BUSCTRL_PERFSEL0_PERFSEL0_SRAM1_CONTESTED            ((uint32_t)0x0000000C)
#define BUSCTRL_PERFSEL0_PERFSEL0_SRAM1                      ((uint32_t)0x0000000D)
#define BUSCTRL_PERFSEL0_PERFSEL0_SRAM0_CONTESTED            ((uint32_t)0x0000000E)
#define BUSCTRL_PERFSEL0_PERFSEL0_SRAM0                      ((uint32_t)0x0000000F)
#define BUSCTRL_PERFSEL0_PERFSEL0_XIP_MAIN_CONTESTED         ((uint32_t)0x00000010)
#define BUSCTRL_PERFSEL0_PERFSEL0_XIP_MAIN                   ((uint32_t)0x00000011)
#define BUSCTRL_PERFSEL0_PERFSEL0_ROM_CONTESTED              ((uint32_t)0x00000012)
#define BUSCTRL_PERFSEL0_PERFSEL0_ROM                        ((uint32_t)0x00000013)
// BUSCTRL->PERFCTR1
#define BUSCTRL_PERFCTR1_PERFCTR1_Pos                        0
#define BUSCTRL_PERFCTR1_PERFCTR1_Msk                        ((uint32_t)0x00FFFFFF)
#define BUSCTRL_PERFCTR1_PERFCTR1                            BUSCTRL_PERFCTR1_PERFCTR1_Msk
// BUSCTRL->PERFSEL1
#define BUSCTRL_PERFSEL1_PERFSEL1_Pos                        0
#define BUSCTRL_PERFSEL1_PERFSEL1_Msk                        ((uint32_t)0x0000001F)
#define BUSCTRL_PERFSEL1_PERFSEL1                            BUSCTRL_PERFSEL1_PERFSEL1_Msk
#define BUSCTRL_PERFSEL1_PERFSEL1_APB_CONTESTED              ((uint32_t)0x00000000)
#define BUSCTRL_PERFSEL1_PERFSEL1_APB                        ((uint32_t)0x00000001)
#define BUSCTRL_PERFSEL1_PERFSEL1_FASTPERI_CONTESTED         ((uint32_t)0x00000002)
#define BUSCTRL_PERFSEL1_PERFSEL1_FASTPERI                   ((uint32_t)0x00000003)
#define BUSCTRL_PERFSEL1_PERFSEL1_SRAM5_CONTESTED            ((uint32_t)0x00000004)
#define BUSCTRL_PERFSEL1_PERFSEL1_SRAM5                      ((uint32_t)0x00000005)
#define BUSCTRL_PERFSEL1_PERFSEL1_SRAM4_CONTESTED            ((uint32_t)0x00000006)
#define BUSCTRL_PERFSEL1_PERFSEL1_SRAM4                      ((uint32_t)0x00000007)
#define BUSCTRL_PERFSEL1_PERFSEL1_SRAM3_CONTESTED            ((uint32_t)0x00000008)
#define BUSCTRL_PERFSEL1_PERFSEL1_SRAM3                      ((uint32_t)0x00000009)
#define BUSCTRL_PERFSEL1_PERFSEL1_SRAM2_CONTESTED            ((uint32_t)0x0000000A)
#define BUSCTRL_PERFSEL1_PERFSEL1_SRAM2                      ((uint32_t)0x0000000B)
#define BUSCTRL_PERFSEL1_PERFSEL1_SRAM1_CONTESTED            ((uint32_t)0x0000000C)
#define BUSCTRL_PERFSEL1_PERFSEL1_SRAM1                      ((uint32_t)0x0000000D)
#define BUSCTRL_PERFSEL1_PERFSEL1_SRAM0_CONTESTED            ((uint32_t)0x0000000E)
#define BUSCTRL_PERFSEL1_PERFSEL1_SRAM0                      ((uint32_t)0x0000000F)
#define BUSCTRL_PERFSEL1_PERFSEL1_XIP_MAIN_CONTESTED         ((uint32_t)0x00000010)
#define BUSCTRL_PERFSEL1_PERFSEL1_XIP_MAIN                   ((uint32_t)0x00000011)
#define BUSCTRL_PERFSEL1_PERFSEL1_ROM_CONTESTED              ((uint32_t)0x00000012)
#define BUSCTRL_PERFSEL1_PERFSEL1_ROM                        ((uint32_t)0x00000013)
// BUSCTRL->PERFCTR2
#define BUSCTRL_PERFCTR2_PERFCTR2_Pos                        0
#define BUSCTRL_PERFCTR2_PERFCTR2_Msk                        ((uint32_t)0x00FFFFFF)
#define BUSCTRL_PERFCTR2_PERFCTR2                            BUSCTRL_PERFCTR2_PERFCTR2_Msk
// BUSCTRL->PERFSEL2
#define BUSCTRL_PERFSEL2_PERFSEL2_Pos                        0
#define BUSCTRL_PERFSEL2_PERFSEL2_Msk                        ((uint32_t)0x0000001F)
#define BUSCTRL_PERFSEL2_PERFSEL2                            BUSCTRL_PERFSEL2_PERFSEL2_Msk
#define BUSCTRL_PERFSEL2_PERFSEL2_APB_CONTESTED              ((uint32_t)0x00000000)
#define BUSCTRL_PERFSEL2_PERFSEL2_APB                        ((uint32_t)0x00000001)
#define BUSCTRL_PERFSEL2_PERFSEL2_FASTPERI_CONTESTED         ((uint32_t)0x00000002)
#define BUSCTRL_PERFSEL2_PERFSEL2_FASTPERI                   ((uint32_t)0x00000003)
#define BUSCTRL_PERFSEL2_PERFSEL2_SRAM5_CONTESTED            ((uint32_t)0x00000004)
#define BUSCTRL_PERFSEL2_PERFSEL2_SRAM5                      ((uint32_t)0x00000005)
#define BUSCTRL_PERFSEL2_PERFSEL2_SRAM4_CONTESTED            ((uint32_t)0x00000006)
#define BUSCTRL_PERFSEL2_PERFSEL2_SRAM4                      ((uint32_t)0x00000007)
#define BUSCTRL_PERFSEL2_PERFSEL2_SRAM3_CONTESTED            ((uint32_t)0x00000008)
#define BUSCTRL_PERFSEL2_PERFSEL2_SRAM3                      ((uint32_t)0x00000009)
#define BUSCTRL_PERFSEL2_PERFSEL2_SRAM2_CONTESTED            ((uint32_t)0x0000000A)
#define BUSCTRL_PERFSEL2_PERFSEL2_SRAM2                      ((uint32_t)0x0000000B)
#define BUSCTRL_PERFSEL2_PERFSEL2_SRAM1_CONTESTED            ((uint32_t)0x0000000C)
#define BUSCTRL_PERFSEL2_PERFSEL2_SRAM1                      ((uint32_t)0x0000000D)
#define BUSCTRL_PERFSEL2_PERFSEL2_SRAM0_CONTESTED            ((uint32_t)0x0000000E)
#define BUSCTRL_PERFSEL2_PERFSEL2_SRAM0                      ((uint32_t)0x0000000F)
#define BUSCTRL_PERFSEL2_PERFSEL2_XIP_MAIN_CONTESTED         ((uint32_t)0x00000010)
#define BUSCTRL_PERFSEL2_PERFSEL2_XIP_MAIN                   ((uint32_t)0x00000011)
#define BUSCTRL_PERFSEL2_PERFSEL2_ROM_CONTESTED              ((uint32_t)0x00000012)
#define BUSCTRL_PERFSEL2_PERFSEL2_ROM                        ((uint32_t)0x00000013)
// BUSCTRL->PERFCTR3
#define BUSCTRL_PERFCTR3_PERFCTR3_Pos                        0
#define BUSCTRL_PERFCTR3_PERFCTR3_Msk                        ((uint32_t)0x00FFFFFF)
#define BUSCTRL_PERFCTR3_PERFCTR3                            BUSCTRL_PERFCTR3_PERFCTR3_Msk
// BUSCTRL->PERFSEL3
#define BUSCTRL_PERFSEL3_PERFSEL3_Pos                        0
#define BUSCTRL_PERFSEL3_PERFSEL3_Msk                        ((uint32_t)0x0000001F)
#define BUSCTRL_PERFSEL3_PERFSEL3                            BUSCTRL_PERFSEL3_PERFSEL3_Msk
#define BUSCTRL_PERFSEL3_PERFSEL3_APB_CONTESTED              ((uint32_t)0x00000000)
#define BUSCTRL_PERFSEL3_PERFSEL3_APB                        ((uint32_t)0x00000001)
#define BUSCTRL_PERFSEL3_PERFSEL3_FASTPERI_CONTESTED         ((uint32_t)0x00000002)
#define BUSCTRL_PERFSEL3_PERFSEL3_FASTPERI                   ((uint32_t)0x00000003)
#define BUSCTRL_PERFSEL3_PERFSEL3_SRAM5_CONTESTED            ((uint32_t)0x00000004)
#define BUSCTRL_PERFSEL3_PERFSEL3_SRAM5                      ((uint32_t)0x00000005)
#define BUSCTRL_PERFSEL3_PERFSEL3_SRAM4_CONTESTED            ((uint32_t)0x00000006)
#define BUSCTRL_PERFSEL3_PERFSEL3_SRAM4                      ((uint32_t)0x00000007)
#define BUSCTRL_PERFSEL3_PERFSEL3_SRAM3_CONTESTED            ((uint32_t)0x00000008)
#define BUSCTRL_PERFSEL3_PERFSEL3_SRAM3                      ((uint32_t)0x00000009)
#define BUSCTRL_PERFSEL3_PERFSEL3_SRAM2_CONTESTED            ((uint32_t)0x0000000A)
#define BUSCTRL_PERFSEL3_PERFSEL3_SRAM2                      ((uint32_t)0x0000000B)
#define BUSCTRL_PERFSEL3_PERFSEL3_SRAM1_CONTESTED            ((uint32_t)0x0000000C)
#define BUSCTRL_PERFSEL3_PERFSEL3_SRAM1                      ((uint32_t)0x0000000D)
#define BUSCTRL_PERFSEL3_PERFSEL3_SRAM0_CONTESTED            ((uint32_t)0x0000000E)
#define BUSCTRL_PERFSEL3_PERFSEL3_SRAM0                      ((uint32_t)0x0000000F)
#define BUSCTRL_PERFSEL3_PERFSEL3_XIP_MAIN_CONTESTED         ((uint32_t)0x00000010)
#define BUSCTRL_PERFSEL3_PERFSEL3_XIP_MAIN                   ((uint32_t)0x00000011)
#define BUSCTRL_PERFSEL3_PERFSEL3_ROM_CONTESTED              ((uint32_t)0x00000012)
#define BUSCTRL_PERFSEL3_PERFSEL3_ROM                        ((uint32_t)0x00000013)

// CLOCKS->CLK_GPOUT[n].CTRL
#define CLOCKS_CLK_GPOUT_CTRL_NUDGE_Pos                      20
#define CLOCKS_CLK_GPOUT_CTRL_NUDGE_Msk                      ((uint32_t)0x00100000)
#define CLOCKS_CLK_GPOUT_CTRL_NUDGE                          CLOCKS_CLK_GPOUT_CTRL_NUDGE_Msk
#define CLOCKS_CLK_GPOUT_CTRL_PHASE_Pos                      16
#define CLOCKS_CLK_GPOUT_CTRL_PHASE_Msk                      ((uint32_t)0x00030000)
#define CLOCKS_CLK_GPOUT_CTRL_PHASE                          CLOCKS_CLK_GPOUT_CTRL_PHASE_Msk
#define CLOCKS_CLK_GPOUT_CTRL_DC50_Pos                       12
#define CLOCKS_CLK_GPOUT_CTRL_DC50_Msk                       ((uint32_t)0x00001000)
#define CLOCKS_CLK_GPOUT_CTRL_DC50                           CLOCKS_CLK_GPOUT_CTRL_DC50_Msk
#define CLOCKS_CLK_GPOUT_CTRL_ENABLE_Pos                     11
#define CLOCKS_CLK_GPOUT_CTRL_ENABLE_Msk                     ((uint32_t)0x00000800)
#define CLOCKS_CLK_GPOUT_CTRL_ENABLE                         CLOCKS_CLK_GPOUT_CTRL_ENABLE_Msk
#define CLOCKS_CLK_GPOUT_CTRL_KILL_Pos                       10
#define CLOCKS_CLK_GPOUT_CTRL_KILL_Msk                       ((uint32_t)0x00000400)
#define CLOCKS_CLK_GPOUT_CTRL_KILL                           CLOCKS_CLK_GPOUT_CTRL_KILL_Msk
#define CLOCKS_CLK_GPOUT_CTRL_AUXSRC_Pos                     5
#define CLOCKS_CLK_GPOUT_CTRL_AUXSRC_Msk                     ((uint32_t)0x000001E0)
#define CLOCKS_CLK_GPOUT_CTRL_AUXSRC                         CLOCKS_CLK_GPOUT_CTRL_AUXSRC_Msk
// CLOCKS->CLK_GPOUT[n].DIV
#define CLOCKS_CLK_GPOUT_DIV_INT_Pos                         8
#define CLOCKS_CLK_GPOUT_DIV_INT_Msk                         ((uint32_t)0xFFFFFF00)
#define CLOCKS_CLK_GPOUT_DIV_INT                             CLOCKS_CLK_GPOUT_DIV_INT_Msk
#define CLOCKS_CLK_GPOUT_DIV_FRAC_Pos                        0
#define CLOCKS_CLK_GPOUT_DIV_FRAC_Msk                        ((uint32_t)0x000000FF)
#define CLOCKS_CLK_GPOUT_DIV_FRAC                            CLOCKS_CLK_GPOUT_DIV_FRAC_Msk
// CLOCKS->CLK_GPOUT[n].SELECTED
#define CLOCKS_CLK_GPOUT_SELECTED_CLK_GPOUT0_SELECTED_Pos    0
#define CLOCKS_CLK_GPOUT_SELECTED_CLK_GPOUT0_SELECTED_Msk    ((uint32_t)0xFFFFFFFF)
#define CLOCKS_CLK_GPOUT_SELECTED_CLK_GPOUT0_SELECTED        CLOCKS_CLK_GPOUT_SELECTED_CLK_GPOUT0_SELECTED_Msk
// CLOCKS->CLK_REF_CTRL
#define CLOCKS_CLK_REF_CTRL_AUXSRC_Pos                       5
#define CLOCKS_CLK_REF_CTRL_AUXSRC_Msk                       ((uint32_t)0x00000060)
#define CLOCKS_CLK_REF_CTRL_AUXSRC                           CLOCKS_CLK_REF_CTRL_AUXSRC_Msk
#define CLOCKS_CLK_REF_CTRL_AUXSRC_CLKSRC_PLL_USB            ((uint32_t)0x00000000)
#define CLOCKS_CLK_REF_CTRL_AUXSRC_CLKSRC_GPIN0              ((uint32_t)0x00000020)
#define CLOCKS_CLK_REF_CTRL_AUXSRC_CLKSRC_GPIN1              ((uint32_t)0x00000040)
#define CLOCKS_CLK_REF_CTRL_SRC_Pos                          0
#define CLOCKS_CLK_REF_CTRL_SRC_Msk                          ((uint32_t)0x00000003)
#define CLOCKS_CLK_REF_CTRL_SRC                              CLOCKS_CLK_REF_CTRL_SRC_Msk
#define CLOCKS_CLK_REF_CTRL_SRC_ROSC_CLKSRC_PH               ((uint32_t)0x00000000)
#define CLOCKS_CLK_REF_CTRL_SRC_CLKSRC_CLK_REF_AUX           ((uint32_t)0x00000001)
#define CLOCKS_CLK_REF_CTRL_SRC_XOSC_CLKSRC                  ((uint32_t)0x00000002)
// CLOCKS->CLK_REF_DIV
#define CLOCKS_CLK_REF_DIV_INT_Pos                           8
#define CLOCKS_CLK_REF_DIV_INT_Msk                           ((uint32_t)0x00000300)
#define CLOCKS_CLK_REF_DIV_INT                               CLOCKS_CLK_REF_DIV_INT_Msk
// CLOCKS->CLK_REF_SELECTED
#define CLOCKS_CLK_REF_SELECTED_CLK_REF_SELECTED_Pos         0
#define CLOCKS_CLK_REF_SELECTED_CLK_REF_SELECTED_Msk         ((uint32_t)0xFFFFFFFF)
#define CLOCKS_CLK_REF_SELECTED_CLK_REF_SELECTED             CLOCKS_CLK_REF_SELECTED_CLK_REF_SELECTED_Msk
// CLOCKS->CLK_SYS_CTRL
#define CLOCKS_CLK_SYS_CTRL_AUXSRC_Pos                       5
#define CLOCKS_CLK_SYS_CTRL_AUXSRC_Msk                       ((uint32_t)0x000000E0)
#define CLOCKS_CLK_SYS_CTRL_AUXSRC                           CLOCKS_CLK_SYS_CTRL_AUXSRC_Msk
#define CLOCKS_CLK_SYS_CTRL_AUXSRC_CLKSRC_PLL_SYS            ((uint32_t)0x00000000)
#define CLOCKS_CLK_SYS_CTRL_AUXSRC_CLKSRC_PLL_USB            ((uint32_t)0x00000020)
#define CLOCKS_CLK_SYS_CTRL_AUXSRC_ROSC_CLKSRC               ((uint32_t)0x00000040)
#define CLOCKS_CLK_SYS_CTRL_AUXSRC_XOSC_CLKSRC               ((uint32_t)0x00000060)
#define CLOCKS_CLK_SYS_CTRL_AUXSRC_CLKSRC_GPIN0              ((uint32_t)0x00000080)
#define CLOCKS_CLK_SYS_CTRL_AUXSRC_CLKSRC_GPIN1              ((uint32_t)0x000000A0)
#define CLOCKS_CLK_SYS_CTRL_SRC_Pos                          0
#define CLOCKS_CLK_SYS_CTRL_SRC_Msk                          ((uint32_t)0x00000001)
#define CLOCKS_CLK_SYS_CTRL_SRC                              CLOCKS_CLK_SYS_CTRL_SRC_Msk
#define CLOCKS_CLK_SYS_CTRL_SRC_CLK_REF                      ((uint32_t)0x00000000)
#define CLOCKS_CLK_SYS_CTRL_SRC_CLKSRC_CLK_SYS_AUX           ((uint32_t)0x00000001)
// CLOCKS->CLK_SYS_DIV
#define CLOCKS_CLK_SYS_DIV_INT_Pos                           8
#define CLOCKS_CLK_SYS_DIV_INT_Msk                           ((uint32_t)0xFFFFFF00)
#define CLOCKS_CLK_SYS_DIV_INT                               CLOCKS_CLK_SYS_DIV_INT_Msk
#define CLOCKS_CLK_SYS_DIV_FRAC_Pos                          0
#define CLOCKS_CLK_SYS_DIV_FRAC_Msk                          ((uint32_t)0x000000FF)
#define CLOCKS_CLK_SYS_DIV_FRAC                              CLOCKS_CLK_SYS_DIV_FRAC_Msk
// CLOCKS->CLK_SYS_SELECTED
#define CLOCKS_CLK_SYS_SELECTED_CLK_SYS_SELECTED_Pos         0
#define CLOCKS_CLK_SYS_SELECTED_CLK_SYS_SELECTED_Msk         ((uint32_t)0xFFFFFFFF)
#define CLOCKS_CLK_SYS_SELECTED_CLK_SYS_SELECTED             CLOCKS_CLK_SYS_SELECTED_CLK_SYS_SELECTED_Msk
// CLOCKS->CLK_PERI_CTRL
#define CLOCKS_CLK_PERI_CTRL_ENABLE_Pos                      11
#define CLOCKS_CLK_PERI_CTRL_ENABLE_Msk                      ((uint32_t)0x00000800)
#define CLOCKS_CLK_PERI_CTRL_ENABLE                          CLOCKS_CLK_PERI_CTRL_ENABLE_Msk
#define CLOCKS_CLK_PERI_CTRL_KILL_Pos                        10
#define CLOCKS_CLK_PERI_CTRL_KILL_Msk                        ((uint32_t)0x00000400)
#define CLOCKS_CLK_PERI_CTRL_KILL                            CLOCKS_CLK_PERI_CTRL_KILL_Msk
#define CLOCKS_CLK_PERI_CTRL_AUXSRC_Pos                      5
#define CLOCKS_CLK_PERI_CTRL_AUXSRC_Msk                      ((uint32_t)0x000000E0)
#define CLOCKS_CLK_PERI_CTRL_AUXSRC                          CLOCKS_CLK_PERI_CTRL_AUXSRC_Msk
#define CLOCKS_CLK_PERI_CTRL_AUXSRC_CLK_SYS                  ((uint32_t)0x00000000)
#define CLOCKS_CLK_PERI_CTRL_AUXSRC_CLKSRC_PLL_SYS           ((uint32_t)0x00000020)
#define CLOCKS_CLK_PERI_CTRL_AUXSRC_CLKSRC_PLL_USB           ((uint32_t)0x00000040)
#define CLOCKS_CLK_PERI_CTRL_AUXSRC_ROSC_CLKSRC_PH           ((uint32_t)0x00000060)
#define CLOCKS_CLK_PERI_CTRL_AUXSRC_XOSC_CLKSRC              ((uint32_t)0x00000080)
#define CLOCKS_CLK_PERI_CTRL_AUXSRC_CLKSRC_GPIN0             ((uint32_t)0x000000A0)
#define CLOCKS_CLK_PERI_CTRL_AUXSRC_CLKSRC_GPIN1             ((uint32_t)0x000000C0)
// CLOCKS->CLK_PERI_SELECTED
#define CLOCKS_CLK_PERI_SELECTED_CLK_PERI_SELECTED_Pos       0
#define CLOCKS_CLK_PERI_SELECTED_CLK_PERI_SELECTED_Msk       ((uint32_t)0xFFFFFFFF)
#define CLOCKS_CLK_PERI_SELECTED_CLK_PERI_SELECTED           CLOCKS_CLK_PERI_SELECTED_CLK_PERI_SELECTED_Msk
// CLOCKS->CLK_USB_CTRL
#define CLOCKS_CLK_USB_CTRL_NUDGE_Pos                        20
#define CLOCKS_CLK_USB_CTRL_NUDGE_Msk                        ((uint32_t)0x00100000)
#define CLOCKS_CLK_USB_CTRL_NUDGE                            CLOCKS_CLK_USB_CTRL_NUDGE_Msk
#define CLOCKS_CLK_USB_CTRL_PHASE_Pos                        16
#define CLOCKS_CLK_USB_CTRL_PHASE_Msk                        ((uint32_t)0x00030000)
#define CLOCKS_CLK_USB_CTRL_PHASE                            CLOCKS_CLK_USB_CTRL_PHASE_Msk
#define CLOCKS_CLK_USB_CTRL_ENABLE_Pos                       11
#define CLOCKS_CLK_USB_CTRL_ENABLE_Msk                       ((uint32_t)0x00000800)
#define CLOCKS_CLK_USB_CTRL_ENABLE                           CLOCKS_CLK_USB_CTRL_ENABLE_Msk
#define CLOCKS_CLK_USB_CTRL_KILL_Pos                         10
#define CLOCKS_CLK_USB_CTRL_KILL_Msk                         ((uint32_t)0x00000400)
#define CLOCKS_CLK_USB_CTRL_KILL                             CLOCKS_CLK_USB_CTRL_KILL_Msk
#define CLOCKS_CLK_USB_CTRL_AUXSRC_Pos                       5
#define CLOCKS_CLK_USB_CTRL_AUXSRC_Msk                       ((uint32_t)0x000000E0)
#define CLOCKS_CLK_USB_CTRL_AUXSRC                           CLOCKS_CLK_USB_CTRL_AUXSRC_Msk
#define CLOCKS_CLK_USB_CTRL_AUXSRC_CLKSRC_PLL_USB            ((uint32_t)0x00000000)
#define CLOCKS_CLK_USB_CTRL_AUXSRC_CLKSRC_PLL_SYS            ((uint32_t)0x00000020)
#define CLOCKS_CLK_USB_CTRL_AUXSRC_ROSC_CLKSRC_PH            ((uint32_t)0x00000040)
#define CLOCKS_CLK_USB_CTRL_AUXSRC_XOSC_CLKSRC               ((uint32_t)0x00000060)
#define CLOCKS_CLK_USB_CTRL_AUXSRC_CLKSRC_GPIN0              ((uint32_t)0x00000080)
#define CLOCKS_CLK_USB_CTRL_AUXSRC_CLKSRC_GPIN1              ((uint32_t)0x000000A0)
// CLOCKS->CLK_USB_DIV
#define CLOCKS_CLK_USB_DIV_INT_Pos                           8
#define CLOCKS_CLK_USB_DIV_INT_Msk                           ((uint32_t)0x00000300)
#define CLOCKS_CLK_USB_DIV_INT                               CLOCKS_CLK_USB_DIV_INT_Msk
// CLOCKS->CLK_USB_SELECTED
#define CLOCKS_CLK_USB_SELECTED_CLK_USB_SELECTED_Pos         0
#define CLOCKS_CLK_USB_SELECTED_CLK_USB_SELECTED_Msk         ((uint32_t)0xFFFFFFFF)
#define CLOCKS_CLK_USB_SELECTED_CLK_USB_SELECTED             CLOCKS_CLK_USB_SELECTED_CLK_USB_SELECTED_Msk
// CLOCKS->CLK_ADC_CTRL
#define CLOCKS_CLK_ADC_CTRL_NUDGE_Pos                        20
#define CLOCKS_CLK_ADC_CTRL_NUDGE_Msk                        ((uint32_t)0x00100000)
#define CLOCKS_CLK_ADC_CTRL_NUDGE                            CLOCKS_CLK_ADC_CTRL_NUDGE_Msk
#define CLOCKS_CLK_ADC_CTRL_PHASE_Pos                        16
#define CLOCKS_CLK_ADC_CTRL_PHASE_Msk                        ((uint32_t)0x00030000)
#define CLOCKS_CLK_ADC_CTRL_PHASE                            CLOCKS_CLK_ADC_CTRL_PHASE_Msk
#define CLOCKS_CLK_ADC_CTRL_ENABLE_Pos                       11
#define CLOCKS_CLK_ADC_CTRL_ENABLE_Msk                       ((uint32_t)0x00000800)
#define CLOCKS_CLK_ADC_CTRL_ENABLE                           CLOCKS_CLK_ADC_CTRL_ENABLE_Msk
#define CLOCKS_CLK_ADC_CTRL_KILL_Pos                         10
#define CLOCKS_CLK_ADC_CTRL_KILL_Msk                         ((uint32_t)0x00000400)
#define CLOCKS_CLK_ADC_CTRL_KILL                             CLOCKS_CLK_ADC_CTRL_KILL_Msk
#define CLOCKS_CLK_ADC_CTRL_AUXSRC_Pos                       5
#define CLOCKS_CLK_ADC_CTRL_AUXSRC_Msk                       ((uint32_t)0x000000E0)
#define CLOCKS_CLK_ADC_CTRL_AUXSRC                           CLOCKS_CLK_ADC_CTRL_AUXSRC_Msk
#define CLOCKS_CLK_ADC_CTRL_AUXSRC_CLKSRC_PLL_USB            ((uint32_t)0x00000000)
#define CLOCKS_CLK_ADC_CTRL_AUXSRC_CLKSRC_PLL_SYS            ((uint32_t)0x00000020)
#define CLOCKS_CLK_ADC_CTRL_AUXSRC_ROSC_CLKSRC_PH            ((uint32_t)0x00000040)
#define CLOCKS_CLK_ADC_CTRL_AUXSRC_XOSC_CLKSRC               ((uint32_t)0x00000060)
#define CLOCKS_CLK_ADC_CTRL_AUXSRC_CLKSRC_GPIN0              ((uint32_t)0x00000080)
#define CLOCKS_CLK_ADC_CTRL_AUXSRC_CLKSRC_GPIN1              ((uint32_t)0x000000A0)
// CLOCKS->CLK_ADC_DIV
#define CLOCKS_CLK_ADC_DIV_INT_Pos                           8
#define CLOCKS_CLK_ADC_DIV_INT_Msk                           ((uint32_t)0x00000300)
#define CLOCKS_CLK_ADC_DIV_INT                               CLOCKS_CLK_ADC_DIV_INT_Msk
// CLOCKS->CLK_ADC_SELECTED
#define CLOCKS_CLK_ADC_SELECTED_CLK_ADC_SELECTED_Pos         0
#define CLOCKS_CLK_ADC_SELECTED_CLK_ADC_SELECTED_Msk         ((uint32_t)0xFFFFFFFF)
#define CLOCKS_CLK_ADC_SELECTED_CLK_ADC_SELECTED             CLOCKS_CLK_ADC_SELECTED_CLK_ADC_SELECTED_Msk
// CLOCKS->CLK_RTC_CTRL
#define CLOCKS_CLK_RTC_CTRL_NUDGE_Pos                        20
#define CLOCKS_CLK_RTC_CTRL_NUDGE_Msk                        ((uint32_t)0x00100000)
#define CLOCKS_CLK_RTC_CTRL_NUDGE                            CLOCKS_CLK_RTC_CTRL_NUDGE_Msk
#define CLOCKS_CLK_RTC_CTRL_PHASE_Pos                        16
#define CLOCKS_CLK_RTC_CTRL_PHASE_Msk                        ((uint32_t)0x00030000)
#define CLOCKS_CLK_RTC_CTRL_PHASE                            CLOCKS_CLK_RTC_CTRL_PHASE_Msk
#define CLOCKS_CLK_RTC_CTRL_ENABLE_Pos                       11
#define CLOCKS_CLK_RTC_CTRL_ENABLE_Msk                       ((uint32_t)0x00000800)
#define CLOCKS_CLK_RTC_CTRL_ENABLE                           CLOCKS_CLK_RTC_CTRL_ENABLE_Msk
#define CLOCKS_CLK_RTC_CTRL_KILL_Pos                         10
#define CLOCKS_CLK_RTC_CTRL_KILL_Msk                         ((uint32_t)0x00000400)
#define CLOCKS_CLK_RTC_CTRL_KILL                             CLOCKS_CLK_RTC_CTRL_KILL_Msk
#define CLOCKS_CLK_RTC_CTRL_AUXSRC_Pos                       5
#define CLOCKS_CLK_RTC_CTRL_AUXSRC_Msk                       ((uint32_t)0x000000E0)
#define CLOCKS_CLK_RTC_CTRL_AUXSRC                           CLOCKS_CLK_RTC_CTRL_AUXSRC_Msk
#define CLOCKS_CLK_RTC_CTRL_AUXSRC_CLKSRC_PLL_USB            ((uint32_t)0x00000000)
#define CLOCKS_CLK_RTC_CTRL_AUXSRC_CLKSRC_PLL_SYS            ((uint32_t)0x00000020)
#define CLOCKS_CLK_RTC_CTRL_AUXSRC_ROSC_CLKSRC_PH            ((uint32_t)0x00000040)
#define CLOCKS_CLK_RTC_CTRL_AUXSRC_XOSC_CLKSRC               ((uint32_t)0x00000060)
#define CLOCKS_CLK_RTC_CTRL_AUXSRC_CLKSRC_GPIN0              ((uint32_t)0x00000080)
#define CLOCKS_CLK_RTC_CTRL_AUXSRC_CLKSRC_GPIN1              ((uint32_t)0x000000A0)
// CLOCKS->CLK_RTC_DIV
#define CLOCKS_CLK_RTC_DIV_INT_Pos                           8
#define CLOCKS_CLK_RTC_DIV_INT_Msk                           ((uint32_t)0xFFFFFF00)
#define CLOCKS_CLK_RTC_DIV_INT                               CLOCKS_CLK_RTC_DIV_INT_Msk
#define CLOCKS_CLK_RTC_DIV_FRAC_Pos                          0
#define CLOCKS_CLK_RTC_DIV_FRAC_Msk                          ((uint32_t)0x000000FF)
#define CLOCKS_CLK_RTC_DIV_FRAC                              CLOCKS_CLK_RTC_DIV_FRAC_Msk
// CLOCKS->CLK_RTC_SELECTED
#define CLOCKS_CLK_RTC_SELECTED_CLK_RTC_SELECTED_Pos         0
#define CLOCKS_CLK_RTC_SELECTED_CLK_RTC_SELECTED_Msk         ((uint32_t)0xFFFFFFFF)
#define CLOCKS_CLK_RTC_SELECTED_CLK_RTC_SELECTED             CLOCKS_CLK_RTC_SELECTED_CLK_RTC_SELECTED_Msk
// CLOCKS->CLK_SYS_RESUS_CTRL
#define CLOCKS_CLK_SYS_RESUS_CTRL_CLEAR_Pos                  16
#define CLOCKS_CLK_SYS_RESUS_CTRL_CLEAR_Msk                  ((uint32_t)0x00010000)
#define CLOCKS_CLK_SYS_RESUS_CTRL_CLEAR                      CLOCKS_CLK_SYS_RESUS_CTRL_CLEAR_Msk
#define CLOCKS_CLK_SYS_RESUS_CTRL_FRCE_Pos                   12
#define CLOCKS_CLK_SYS_RESUS_CTRL_FRCE_Msk                   ((uint32_t)0x00001000)
#define CLOCKS_CLK_SYS_RESUS_CTRL_FRCE                       CLOCKS_CLK_SYS_RESUS_CTRL_FRCE_Msk
#define CLOCKS_CLK_SYS_RESUS_CTRL_ENABLE_Pos                 8
#define CLOCKS_CLK_SYS_RESUS_CTRL_ENABLE_Msk                 ((uint32_t)0x00000100)
#define CLOCKS_CLK_SYS_RESUS_CTRL_ENABLE                     CLOCKS_CLK_SYS_RESUS_CTRL_ENABLE_Msk
#define CLOCKS_CLK_SYS_RESUS_CTRL_TIMEOUT_Pos                0
#define CLOCKS_CLK_SYS_RESUS_CTRL_TIMEOUT_Msk                ((uint32_t)0x000000FF)
#define CLOCKS_CLK_SYS_RESUS_CTRL_TIMEOUT                    CLOCKS_CLK_SYS_RESUS_CTRL_TIMEOUT_Msk
// CLOCKS->CLK_SYS_RESUS_STATUS
#define CLOCKS_CLK_SYS_RESUS_STATUS_RESUSSED_Pos             0
#define CLOCKS_CLK_SYS_RESUS_STATUS_RESUSSED_Msk             ((uint32_t)0x00000001)
#define CLOCKS_CLK_SYS_RESUS_STATUS_RESUSSED                 CLOCKS_CLK_SYS_RESUS_STATUS_RESUSSED_Msk
// CLOCKS->FC0_REF_KHZ
#define CLOCKS_FC0_REF_KHZ_FC0_REF_KHZ_Pos                   0
#define CLOCKS_FC0_REF_KHZ_FC0_REF_KHZ_Msk                   ((uint32_t)0x000FFFFF)
#define CLOCKS_FC0_REF_KHZ_FC0_REF_KHZ                       CLOCKS_FC0_REF_KHZ_FC0_REF_KHZ_Msk
// CLOCKS->FC0_MIN_KHZ
#define CLOCKS_FC0_MIN_KHZ_FC0_MIN_KHZ_Pos                   0
#define CLOCKS_FC0_MIN_KHZ_FC0_MIN_KHZ_Msk                   ((uint32_t)0x01FFFFFF)
#define CLOCKS_FC0_MIN_KHZ_FC0_MIN_KHZ                       CLOCKS_FC0_MIN_KHZ_FC0_MIN_KHZ_Msk
// CLOCKS->FC0_MAX_KHZ
#define CLOCKS_FC0_MAX_KHZ_FC0_MAX_KHZ_Pos                   0
#define CLOCKS_FC0_MAX_KHZ_FC0_MAX_KHZ_Msk                   ((uint32_t)0x01FFFFFF)
#define CLOCKS_FC0_MAX_KHZ_FC0_MAX_KHZ                       CLOCKS_FC0_MAX_KHZ_FC0_MAX_KHZ_Msk
// CLOCKS->FC0_DELAY
#define CLOCKS_FC0_DELAY_FC0_DELAY_Pos                       0
#define CLOCKS_FC0_DELAY_FC0_DELAY_Msk                       ((uint32_t)0x00000007)
#define CLOCKS_FC0_DELAY_FC0_DELAY                           CLOCKS_FC0_DELAY_FC0_DELAY_Msk
// CLOCKS->FC0_INTERVAL
#define CLOCKS_FC0_INTERVAL_FC0_INTERVAL_Pos                 0
#define CLOCKS_FC0_INTERVAL_FC0_INTERVAL_Msk                 ((uint32_t)0x0000000F)
#define CLOCKS_FC0_INTERVAL_FC0_INTERVAL                     CLOCKS_FC0_INTERVAL_FC0_INTERVAL_Msk
// CLOCKS->FC0_SRC
#define CLOCKS_FC0_SRC_FC0_SRC_Pos                           0
#define CLOCKS_FC0_SRC_FC0_SRC_Msk                           ((uint32_t)0x000000FF)
#define CLOCKS_FC0_SRC_FC0_SRC                               CLOCKS_FC0_SRC_FC0_SRC_Msk
#define CLOCKS_FC0_SRC_FC0_SRC_NULL                          ((uint32_t)0x00000000)
#define CLOCKS_FC0_SRC_FC0_SRC_PLL_SYS_CLKSRC_PRIMARY        ((uint32_t)0x00000001)
#define CLOCKS_FC0_SRC_FC0_SRC_PLL_USB_CLKSRC_PRIMARY        ((uint32_t)0x00000002)
#define CLOCKS_FC0_SRC_FC0_SRC_ROSC_CLKSRC                   ((uint32_t)0x00000003)
#define CLOCKS_FC0_SRC_FC0_SRC_ROSC_CLKSRC_PH                ((uint32_t)0x00000004)
#define CLOCKS_FC0_SRC_FC0_SRC_XOSC_CLKSRC                   ((uint32_t)0x00000005)
#define CLOCKS_FC0_SRC_FC0_SRC_CLKSRC_GPIN0                  ((uint32_t)0x00000006)
#define CLOCKS_FC0_SRC_FC0_SRC_CLKSRC_GPIN1                  ((uint32_t)0x00000007)
#define CLOCKS_FC0_SRC_FC0_SRC_CLK_REF                       ((uint32_t)0x00000008)
#define CLOCKS_FC0_SRC_FC0_SRC_CLK_SYS                       ((uint32_t)0x00000009)
#define CLOCKS_FC0_SRC_FC0_SRC_CLK_PERI                      ((uint32_t)0x0000000A)
#define CLOCKS_FC0_SRC_FC0_SRC_CLK_USB                       ((uint32_t)0x0000000B)
#define CLOCKS_FC0_SRC_FC0_SRC_CLK_ADC                       ((uint32_t)0x0000000C)
#define CLOCKS_FC0_SRC_FC0_SRC_CLK_RTC                       ((uint32_t)0x0000000D)
// CLOCKS->FC0_STATUS
#define CLOCKS_FC0_STATUS_DIED_Pos                           28
#define CLOCKS_FC0_STATUS_DIED_Msk                           ((uint32_t)0x10000000)
#define CLOCKS_FC0_STATUS_DIED                               CLOCKS_FC0_STATUS_DIED_Msk
#define CLOCKS_FC0_STATUS_FAST_Pos                           24
#define CLOCKS_FC0_STATUS_FAST_Msk                           ((uint32_t)0x01000000)
#define CLOCKS_FC0_STATUS_FAST                               CLOCKS_FC0_STATUS_FAST_Msk
#define CLOCKS_FC0_STATUS_SLOW_Pos                           20
#define CLOCKS_FC0_STATUS_SLOW_Msk                           ((uint32_t)0x00100000)
#define CLOCKS_FC0_STATUS_SLOW                               CLOCKS_FC0_STATUS_SLOW_Msk
#define CLOCKS_FC0_STATUS_FAIL_Pos                           16
#define CLOCKS_FC0_STATUS_FAIL_Msk                           ((uint32_t)0x00010000)
#define CLOCKS_FC0_STATUS_FAIL                               CLOCKS_FC0_STATUS_FAIL_Msk
#define CLOCKS_FC0_STATUS_WAITING_Pos                        12
#define CLOCKS_FC0_STATUS_WAITING_Msk                        ((uint32_t)0x00001000)
#define CLOCKS_FC0_STATUS_WAITING                            CLOCKS_FC0_STATUS_WAITING_Msk
#define CLOCKS_FC0_STATUS_RUNNING_Pos                        8
#define CLOCKS_FC0_STATUS_RUNNING_Msk                        ((uint32_t)0x00000100)
#define CLOCKS_FC0_STATUS_RUNNING                            CLOCKS_FC0_STATUS_RUNNING_Msk
#define CLOCKS_FC0_STATUS_DONE_Pos                           4
#define CLOCKS_FC0_STATUS_DONE_Msk                           ((uint32_t)0x00000010)
#define CLOCKS_FC0_STATUS_DONE                               CLOCKS_FC0_STATUS_DONE_Msk
#define CLOCKS_FC0_STATUS_PASS_Pos                           0
#define CLOCKS_FC0_STATUS_PASS_Msk                           ((uint32_t)0x00000001)
#define CLOCKS_FC0_STATUS_PASS                               CLOCKS_FC0_STATUS_PASS_Msk
// CLOCKS->FC0_RESULT
#define CLOCKS_FC0_RESULT_KHZ_Pos                            5
#define CLOCKS_FC0_RESULT_KHZ_Msk                            ((uint32_t)0x3FFFFFE0)
#define CLOCKS_FC0_RESULT_KHZ                                CLOCKS_FC0_RESULT_KHZ_Msk
#define CLOCKS_FC0_RESULT_FRAC_Pos                           0
#define CLOCKS_FC0_RESULT_FRAC_Msk                           ((uint32_t)0x0000001F)
#define CLOCKS_FC0_RESULT_FRAC                               CLOCKS_FC0_RESULT_FRAC_Msk
// CLOCKS->WAKE_EN0
#define CLOCKS_WAKE_EN0_CLK_SYS_SRAM3_Pos                    31
#define CLOCKS_WAKE_EN0_CLK_SYS_SRAM3_Msk                    ((uint32_t)0x80000000)
#define CLOCKS_WAKE_EN0_CLK_SYS_SRAM3                        CLOCKS_WAKE_EN0_CLK_SYS_SRAM3_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_SRAM2_Pos                    30
#define CLOCKS_WAKE_EN0_CLK_SYS_SRAM2_Msk                    ((uint32_t)0x40000000)
#define CLOCKS_WAKE_EN0_CLK_SYS_SRAM2                        CLOCKS_WAKE_EN0_CLK_SYS_SRAM2_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_SRAM1_Pos                    29
#define CLOCKS_WAKE_EN0_CLK_SYS_SRAM1_Msk                    ((uint32_t)0x20000000)
#define CLOCKS_WAKE_EN0_CLK_SYS_SRAM1                        CLOCKS_WAKE_EN0_CLK_SYS_SRAM1_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_SRAM0_Pos                    28
#define CLOCKS_WAKE_EN0_CLK_SYS_SRAM0_Msk                    ((uint32_t)0x10000000)
#define CLOCKS_WAKE_EN0_CLK_SYS_SRAM0                        CLOCKS_WAKE_EN0_CLK_SYS_SRAM0_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_SPI1_Pos                     27
#define CLOCKS_WAKE_EN0_CLK_SYS_SPI1_Msk                     ((uint32_t)0x08000000)
#define CLOCKS_WAKE_EN0_CLK_SYS_SPI1                         CLOCKS_WAKE_EN0_CLK_SYS_SPI1_Msk
#define CLOCKS_WAKE_EN0_CLK_PERI_SPI1_Pos                    26
#define CLOCKS_WAKE_EN0_CLK_PERI_SPI1_Msk                    ((uint32_t)0x04000000)
#define CLOCKS_WAKE_EN0_CLK_PERI_SPI1                        CLOCKS_WAKE_EN0_CLK_PERI_SPI1_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_SPI0_Pos                     25
#define CLOCKS_WAKE_EN0_CLK_SYS_SPI0_Msk                     ((uint32_t)0x02000000)
#define CLOCKS_WAKE_EN0_CLK_SYS_SPI0                         CLOCKS_WAKE_EN0_CLK_SYS_SPI0_Msk
#define CLOCKS_WAKE_EN0_CLK_PERI_SPI0_Pos                    24
#define CLOCKS_WAKE_EN0_CLK_PERI_SPI0_Msk                    ((uint32_t)0x01000000)
#define CLOCKS_WAKE_EN0_CLK_PERI_SPI0                        CLOCKS_WAKE_EN0_CLK_PERI_SPI0_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_SIO_Pos                      23
#define CLOCKS_WAKE_EN0_CLK_SYS_SIO_Msk                      ((uint32_t)0x00800000)
#define CLOCKS_WAKE_EN0_CLK_SYS_SIO                          CLOCKS_WAKE_EN0_CLK_SYS_SIO_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_RTC_Pos                      22
#define CLOCKS_WAKE_EN0_CLK_SYS_RTC_Msk                      ((uint32_t)0x00400000)
#define CLOCKS_WAKE_EN0_CLK_SYS_RTC                          CLOCKS_WAKE_EN0_CLK_SYS_RTC_Msk
#define CLOCKS_WAKE_EN0_CLK_RTC_RTC_Pos                      21
#define CLOCKS_WAKE_EN0_CLK_RTC_RTC_Msk                      ((uint32_t)0x00200000)
#define CLOCKS_WAKE_EN0_CLK_RTC_RTC                          CLOCKS_WAKE_EN0_CLK_RTC_RTC_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_ROSC_Pos                     20
#define CLOCKS_WAKE_EN0_CLK_SYS_ROSC_Msk                     ((uint32_t)0x00100000)
#define CLOCKS_WAKE_EN0_CLK_SYS_ROSC                         CLOCKS_WAKE_EN0_CLK_SYS_ROSC_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_ROM_Pos                      19
#define CLOCKS_WAKE_EN0_CLK_SYS_ROM_Msk                      ((uint32_t)0x00080000)
#define CLOCKS_WAKE_EN0_CLK_SYS_ROM                          CLOCKS_WAKE_EN0_CLK_SYS_ROM_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_RESETS_Pos                   18
#define CLOCKS_WAKE_EN0_CLK_SYS_RESETS_Msk                   ((uint32_t)0x00040000)
#define CLOCKS_WAKE_EN0_CLK_SYS_RESETS                       CLOCKS_WAKE_EN0_CLK_SYS_RESETS_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_PWM_Pos                      17
#define CLOCKS_WAKE_EN0_CLK_SYS_PWM_Msk                      ((uint32_t)0x00020000)
#define CLOCKS_WAKE_EN0_CLK_SYS_PWM                          CLOCKS_WAKE_EN0_CLK_SYS_PWM_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_PSM_Pos                      16
#define CLOCKS_WAKE_EN0_CLK_SYS_PSM_Msk                      ((uint32_t)0x00010000)
#define CLOCKS_WAKE_EN0_CLK_SYS_PSM                          CLOCKS_WAKE_EN0_CLK_SYS_PSM_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_PLL_USB_Pos                  15
#define CLOCKS_WAKE_EN0_CLK_SYS_PLL_USB_Msk                  ((uint32_t)0x00008000)
#define CLOCKS_WAKE_EN0_CLK_SYS_PLL_USB                      CLOCKS_WAKE_EN0_CLK_SYS_PLL_USB_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_PLL_SYS_Pos                  14
#define CLOCKS_WAKE_EN0_CLK_SYS_PLL_SYS_Msk                  ((uint32_t)0x00004000)
#define CLOCKS_WAKE_EN0_CLK_SYS_PLL_SYS                      CLOCKS_WAKE_EN0_CLK_SYS_PLL_SYS_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_PIO1_Pos                     13
#define CLOCKS_WAKE_EN0_CLK_SYS_PIO1_Msk                     ((uint32_t)0x00002000)
#define CLOCKS_WAKE_EN0_CLK_SYS_PIO1                         CLOCKS_WAKE_EN0_CLK_SYS_PIO1_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_PIO0_Pos                     12
#define CLOCKS_WAKE_EN0_CLK_SYS_PIO0_Msk                     ((uint32_t)0x00001000)
#define CLOCKS_WAKE_EN0_CLK_SYS_PIO0                         CLOCKS_WAKE_EN0_CLK_SYS_PIO0_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_PADS_Pos                     11
#define CLOCKS_WAKE_EN0_CLK_SYS_PADS_Msk                     ((uint32_t)0x00000800)
#define CLOCKS_WAKE_EN0_CLK_SYS_PADS                         CLOCKS_WAKE_EN0_CLK_SYS_PADS_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_VREG_AND_CHIP_RESET_Pos      10
#define CLOCKS_WAKE_EN0_CLK_SYS_VREG_AND_CHIP_RESET_Msk      ((uint32_t)0x00000400)
#define CLOCKS_WAKE_EN0_CLK_SYS_VREG_AND_CHIP_RESET          CLOCKS_WAKE_EN0_CLK_SYS_VREG_AND_CHIP_RESET_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_JTAG_Pos                     9
#define CLOCKS_WAKE_EN0_CLK_SYS_JTAG_Msk                     ((uint32_t)0x00000200)
#define CLOCKS_WAKE_EN0_CLK_SYS_JTAG                         CLOCKS_WAKE_EN0_CLK_SYS_JTAG_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_IO_Pos                       8
#define CLOCKS_WAKE_EN0_CLK_SYS_IO_Msk                       ((uint32_t)0x00000100)
#define CLOCKS_WAKE_EN0_CLK_SYS_IO                           CLOCKS_WAKE_EN0_CLK_SYS_IO_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_I2C1_Pos                     7
#define CLOCKS_WAKE_EN0_CLK_SYS_I2C1_Msk                     ((uint32_t)0x00000080)
#define CLOCKS_WAKE_EN0_CLK_SYS_I2C1                         CLOCKS_WAKE_EN0_CLK_SYS_I2C1_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_I2C0_Pos                     6
#define CLOCKS_WAKE_EN0_CLK_SYS_I2C0_Msk                     ((uint32_t)0x00000040)
#define CLOCKS_WAKE_EN0_CLK_SYS_I2C0                         CLOCKS_WAKE_EN0_CLK_SYS_I2C0_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_DMA_Pos                      5
#define CLOCKS_WAKE_EN0_CLK_SYS_DMA_Msk                      ((uint32_t)0x00000020)
#define CLOCKS_WAKE_EN0_CLK_SYS_DMA                          CLOCKS_WAKE_EN0_CLK_SYS_DMA_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_BUSFABRIC_Pos                4
#define CLOCKS_WAKE_EN0_CLK_SYS_BUSFABRIC_Msk                ((uint32_t)0x00000010)
#define CLOCKS_WAKE_EN0_CLK_SYS_BUSFABRIC                    CLOCKS_WAKE_EN0_CLK_SYS_BUSFABRIC_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_BUSCTRL_Pos                  3
#define CLOCKS_WAKE_EN0_CLK_SYS_BUSCTRL_Msk                  ((uint32_t)0x00000008)
#define CLOCKS_WAKE_EN0_CLK_SYS_BUSCTRL                      CLOCKS_WAKE_EN0_CLK_SYS_BUSCTRL_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_ADC_Pos                      2
#define CLOCKS_WAKE_EN0_CLK_SYS_ADC_Msk                      ((uint32_t)0x00000004)
#define CLOCKS_WAKE_EN0_CLK_SYS_ADC                          CLOCKS_WAKE_EN0_CLK_SYS_ADC_Msk
#define CLOCKS_WAKE_EN0_CLK_ADC_ADC_Pos                      1
#define CLOCKS_WAKE_EN0_CLK_ADC_ADC_Msk                      ((uint32_t)0x00000002)
#define CLOCKS_WAKE_EN0_CLK_ADC_ADC                          CLOCKS_WAKE_EN0_CLK_ADC_ADC_Msk
#define CLOCKS_WAKE_EN0_CLK_SYS_CLOCKS_Pos                   0
#define CLOCKS_WAKE_EN0_CLK_SYS_CLOCKS_Msk                   ((uint32_t)0x00000001)
#define CLOCKS_WAKE_EN0_CLK_SYS_CLOCKS                       CLOCKS_WAKE_EN0_CLK_SYS_CLOCKS_Msk
// CLOCKS->WAKE_EN1
#define CLOCKS_WAKE_EN1_CLK_SYS_XOSC_Pos                     14
#define CLOCKS_WAKE_EN1_CLK_SYS_XOSC_Msk                     ((uint32_t)0x00004000)
#define CLOCKS_WAKE_EN1_CLK_SYS_XOSC                         CLOCKS_WAKE_EN1_CLK_SYS_XOSC_Msk
#define CLOCKS_WAKE_EN1_CLK_SYS_XIP_Pos                      13
#define CLOCKS_WAKE_EN1_CLK_SYS_XIP_Msk                      ((uint32_t)0x00002000)
#define CLOCKS_WAKE_EN1_CLK_SYS_XIP                          CLOCKS_WAKE_EN1_CLK_SYS_XIP_Msk
#define CLOCKS_WAKE_EN1_CLK_SYS_WATCHDOG_Pos                 12
#define CLOCKS_WAKE_EN1_CLK_SYS_WATCHDOG_Msk                 ((uint32_t)0x00001000)
#define CLOCKS_WAKE_EN1_CLK_SYS_WATCHDOG                     CLOCKS_WAKE_EN1_CLK_SYS_WATCHDOG_Msk
#define CLOCKS_WAKE_EN1_CLK_USB_USBCTRL_Pos                  11
#define CLOCKS_WAKE_EN1_CLK_USB_USBCTRL_Msk                  ((uint32_t)0x00000800)
#define CLOCKS_WAKE_EN1_CLK_USB_USBCTRL                      CLOCKS_WAKE_EN1_CLK_USB_USBCTRL_Msk
#define CLOCKS_WAKE_EN1_CLK_SYS_USBCTRL_Pos                  10
#define CLOCKS_WAKE_EN1_CLK_SYS_USBCTRL_Msk                  ((uint32_t)0x00000400)
#define CLOCKS_WAKE_EN1_CLK_SYS_USBCTRL                      CLOCKS_WAKE_EN1_CLK_SYS_USBCTRL_Msk
#define CLOCKS_WAKE_EN1_CLK_SYS_UART1_Pos                    9
#define CLOCKS_WAKE_EN1_CLK_SYS_UART1_Msk                    ((uint32_t)0x00000200)
#define CLOCKS_WAKE_EN1_CLK_SYS_UART1                        CLOCKS_WAKE_EN1_CLK_SYS_UART1_Msk
#define CLOCKS_WAKE_EN1_CLK_PERI_UART1_Pos                   8
#define CLOCKS_WAKE_EN1_CLK_PERI_UART1_Msk                   ((uint32_t)0x00000100)
#define CLOCKS_WAKE_EN1_CLK_PERI_UART1                       CLOCKS_WAKE_EN1_CLK_PERI_UART1_Msk
#define CLOCKS_WAKE_EN1_CLK_SYS_UART0_Pos                    7
#define CLOCKS_WAKE_EN1_CLK_SYS_UART0_Msk                    ((uint32_t)0x00000080)
#define CLOCKS_WAKE_EN1_CLK_SYS_UART0                        CLOCKS_WAKE_EN1_CLK_SYS_UART0_Msk
#define CLOCKS_WAKE_EN1_CLK_PERI_UART0_Pos                   6
#define CLOCKS_WAKE_EN1_CLK_PERI_UART0_Msk                   ((uint32_t)0x00000040)
#define CLOCKS_WAKE_EN1_CLK_PERI_UART0                       CLOCKS_WAKE_EN1_CLK_PERI_UART0_Msk
#define CLOCKS_WAKE_EN1_CLK_SYS_TIMER_Pos                    5
#define CLOCKS_WAKE_EN1_CLK_SYS_TIMER_Msk                    ((uint32_t)0x00000020)
#define CLOCKS_WAKE_EN1_CLK_SYS_TIMER                        CLOCKS_WAKE_EN1_CLK_SYS_TIMER_Msk
#define CLOCKS_WAKE_EN1_CLK_SYS_TBMAN_Pos                    4
#define CLOCKS_WAKE_EN1_CLK_SYS_TBMAN_Msk                    ((uint32_t)0x00000010)
#define CLOCKS_WAKE_EN1_CLK_SYS_TBMAN                        CLOCKS_WAKE_EN1_CLK_SYS_TBMAN_Msk
#define CLOCKS_WAKE_EN1_CLK_SYS_SYSINFO_Pos                  3
#define CLOCKS_WAKE_EN1_CLK_SYS_SYSINFO_Msk                  ((uint32_t)0x00000008)
#define CLOCKS_WAKE_EN1_CLK_SYS_SYSINFO                      CLOCKS_WAKE_EN1_CLK_SYS_SYSINFO_Msk
#define CLOCKS_WAKE_EN1_CLK_SYS_SYSCFG_Pos                   2
#define CLOCKS_WAKE_EN1_CLK_SYS_SYSCFG_Msk                   ((uint32_t)0x00000004)
#define CLOCKS_WAKE_EN1_CLK_SYS_SYSCFG                       CLOCKS_WAKE_EN1_CLK_SYS_SYSCFG_Msk
#define CLOCKS_WAKE_EN1_CLK_SYS_SRAM5_Pos                    1
#define CLOCKS_WAKE_EN1_CLK_SYS_SRAM5_Msk                    ((uint32_t)0x00000002)
#define CLOCKS_WAKE_EN1_CLK_SYS_SRAM5                        CLOCKS_WAKE_EN1_CLK_SYS_SRAM5_Msk
#define CLOCKS_WAKE_EN1_CLK_SYS_SRAM4_Pos                    0
#define CLOCKS_WAKE_EN1_CLK_SYS_SRAM4_Msk                    ((uint32_t)0x00000001)
#define CLOCKS_WAKE_EN1_CLK_SYS_SRAM4                        CLOCKS_WAKE_EN1_CLK_SYS_SRAM4_Msk
// CLOCKS->SLEEP_EN0
#define CLOCKS_SLEEP_EN0_CLK_SYS_SRAM3_Pos                   31
#define CLOCKS_SLEEP_EN0_CLK_SYS_SRAM3_Msk                   ((uint32_t)0x80000000)
#define CLOCKS_SLEEP_EN0_CLK_SYS_SRAM3                       CLOCKS_SLEEP_EN0_CLK_SYS_SRAM3_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_SRAM2_Pos                   30
#define CLOCKS_SLEEP_EN0_CLK_SYS_SRAM2_Msk                   ((uint32_t)0x40000000)
#define CLOCKS_SLEEP_EN0_CLK_SYS_SRAM2                       CLOCKS_SLEEP_EN0_CLK_SYS_SRAM2_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_SRAM1_Pos                   29
#define CLOCKS_SLEEP_EN0_CLK_SYS_SRAM1_Msk                   ((uint32_t)0x20000000)
#define CLOCKS_SLEEP_EN0_CLK_SYS_SRAM1                       CLOCKS_SLEEP_EN0_CLK_SYS_SRAM1_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_SRAM0_Pos                   28
#define CLOCKS_SLEEP_EN0_CLK_SYS_SRAM0_Msk                   ((uint32_t)0x10000000)
#define CLOCKS_SLEEP_EN0_CLK_SYS_SRAM0                       CLOCKS_SLEEP_EN0_CLK_SYS_SRAM0_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_SPI1_Pos                    27
#define CLOCKS_SLEEP_EN0_CLK_SYS_SPI1_Msk                    ((uint32_t)0x08000000)
#define CLOCKS_SLEEP_EN0_CLK_SYS_SPI1                        CLOCKS_SLEEP_EN0_CLK_SYS_SPI1_Msk
#define CLOCKS_SLEEP_EN0_CLK_PERI_SPI1_Pos                   26
#define CLOCKS_SLEEP_EN0_CLK_PERI_SPI1_Msk                   ((uint32_t)0x04000000)
#define CLOCKS_SLEEP_EN0_CLK_PERI_SPI1                       CLOCKS_SLEEP_EN0_CLK_PERI_SPI1_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_SPI0_Pos                    25
#define CLOCKS_SLEEP_EN0_CLK_SYS_SPI0_Msk                    ((uint32_t)0x02000000)
#define CLOCKS_SLEEP_EN0_CLK_SYS_SPI0                        CLOCKS_SLEEP_EN0_CLK_SYS_SPI0_Msk
#define CLOCKS_SLEEP_EN0_CLK_PERI_SPI0_Pos                   24
#define CLOCKS_SLEEP_EN0_CLK_PERI_SPI0_Msk                   ((uint32_t)0x01000000)
#define CLOCKS_SLEEP_EN0_CLK_PERI_SPI0                       CLOCKS_SLEEP_EN0_CLK_PERI_SPI0_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_SIO_Pos                     23
#define CLOCKS_SLEEP_EN0_CLK_SYS_SIO_Msk                     ((uint32_t)0x00800000)
#define CLOCKS_SLEEP_EN0_CLK_SYS_SIO                         CLOCKS_SLEEP_EN0_CLK_SYS_SIO_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_RTC_Pos                     22
#define CLOCKS_SLEEP_EN0_CLK_SYS_RTC_Msk                     ((uint32_t)0x00400000)
#define CLOCKS_SLEEP_EN0_CLK_SYS_RTC                         CLOCKS_SLEEP_EN0_CLK_SYS_RTC_Msk
#define CLOCKS_SLEEP_EN0_CLK_RTC_RTC_Pos                     21
#define CLOCKS_SLEEP_EN0_CLK_RTC_RTC_Msk                     ((uint32_t)0x00200000)
#define CLOCKS_SLEEP_EN0_CLK_RTC_RTC                         CLOCKS_SLEEP_EN0_CLK_RTC_RTC_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_ROSC_Pos                    20
#define CLOCKS_SLEEP_EN0_CLK_SYS_ROSC_Msk                    ((uint32_t)0x00100000)
#define CLOCKS_SLEEP_EN0_CLK_SYS_ROSC                        CLOCKS_SLEEP_EN0_CLK_SYS_ROSC_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_ROM_Pos                     19
#define CLOCKS_SLEEP_EN0_CLK_SYS_ROM_Msk                     ((uint32_t)0x00080000)
#define CLOCKS_SLEEP_EN0_CLK_SYS_ROM                         CLOCKS_SLEEP_EN0_CLK_SYS_ROM_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_RESETS_Pos                  18
#define CLOCKS_SLEEP_EN0_CLK_SYS_RESETS_Msk                  ((uint32_t)0x00040000)
#define CLOCKS_SLEEP_EN0_CLK_SYS_RESETS                      CLOCKS_SLEEP_EN0_CLK_SYS_RESETS_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_PWM_Pos                     17
#define CLOCKS_SLEEP_EN0_CLK_SYS_PWM_Msk                     ((uint32_t)0x00020000)
#define CLOCKS_SLEEP_EN0_CLK_SYS_PWM                         CLOCKS_SLEEP_EN0_CLK_SYS_PWM_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_PSM_Pos                     16
#define CLOCKS_SLEEP_EN0_CLK_SYS_PSM_Msk                     ((uint32_t)0x00010000)
#define CLOCKS_SLEEP_EN0_CLK_SYS_PSM                         CLOCKS_SLEEP_EN0_CLK_SYS_PSM_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_PLL_USB_Pos                 15
#define CLOCKS_SLEEP_EN0_CLK_SYS_PLL_USB_Msk                 ((uint32_t)0x00008000)
#define CLOCKS_SLEEP_EN0_CLK_SYS_PLL_USB                     CLOCKS_SLEEP_EN0_CLK_SYS_PLL_USB_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_PLL_SYS_Pos                 14
#define CLOCKS_SLEEP_EN0_CLK_SYS_PLL_SYS_Msk                 ((uint32_t)0x00004000)
#define CLOCKS_SLEEP_EN0_CLK_SYS_PLL_SYS                     CLOCKS_SLEEP_EN0_CLK_SYS_PLL_SYS_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_PIO1_Pos                    13
#define CLOCKS_SLEEP_EN0_CLK_SYS_PIO1_Msk                    ((uint32_t)0x00002000)
#define CLOCKS_SLEEP_EN0_CLK_SYS_PIO1                        CLOCKS_SLEEP_EN0_CLK_SYS_PIO1_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_PIO0_Pos                    12
#define CLOCKS_SLEEP_EN0_CLK_SYS_PIO0_Msk                    ((uint32_t)0x00001000)
#define CLOCKS_SLEEP_EN0_CLK_SYS_PIO0                        CLOCKS_SLEEP_EN0_CLK_SYS_PIO0_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_PADS_Pos                    11
#define CLOCKS_SLEEP_EN0_CLK_SYS_PADS_Msk                    ((uint32_t)0x00000800)
#define CLOCKS_SLEEP_EN0_CLK_SYS_PADS                        CLOCKS_SLEEP_EN0_CLK_SYS_PADS_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_VREG_AND_CHIP_RESET_Pos     10
#define CLOCKS_SLEEP_EN0_CLK_SYS_VREG_AND_CHIP_RESET_Msk     ((uint32_t)0x00000400)
#define CLOCKS_SLEEP_EN0_CLK_SYS_VREG_AND_CHIP_RESET         CLOCKS_SLEEP_EN0_CLK_SYS_VREG_AND_CHIP_RESET_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_JTAG_Pos                    9
#define CLOCKS_SLEEP_EN0_CLK_SYS_JTAG_Msk                    ((uint32_t)0x00000200)
#define CLOCKS_SLEEP_EN0_CLK_SYS_JTAG                        CLOCKS_SLEEP_EN0_CLK_SYS_JTAG_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_IO_Pos                      8
#define CLOCKS_SLEEP_EN0_CLK_SYS_IO_Msk                      ((uint32_t)0x00000100)
#define CLOCKS_SLEEP_EN0_CLK_SYS_IO                          CLOCKS_SLEEP_EN0_CLK_SYS_IO_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_I2C1_Pos                    7
#define CLOCKS_SLEEP_EN0_CLK_SYS_I2C1_Msk                    ((uint32_t)0x00000080)
#define CLOCKS_SLEEP_EN0_CLK_SYS_I2C1                        CLOCKS_SLEEP_EN0_CLK_SYS_I2C1_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_I2C0_Pos                    6
#define CLOCKS_SLEEP_EN0_CLK_SYS_I2C0_Msk                    ((uint32_t)0x00000040)
#define CLOCKS_SLEEP_EN0_CLK_SYS_I2C0                        CLOCKS_SLEEP_EN0_CLK_SYS_I2C0_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_DMA_Pos                     5
#define CLOCKS_SLEEP_EN0_CLK_SYS_DMA_Msk                     ((uint32_t)0x00000020)
#define CLOCKS_SLEEP_EN0_CLK_SYS_DMA                         CLOCKS_SLEEP_EN0_CLK_SYS_DMA_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_BUSFABRIC_Pos               4
#define CLOCKS_SLEEP_EN0_CLK_SYS_BUSFABRIC_Msk               ((uint32_t)0x00000010)
#define CLOCKS_SLEEP_EN0_CLK_SYS_BUSFABRIC                   CLOCKS_SLEEP_EN0_CLK_SYS_BUSFABRIC_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_BUSCTRL_Pos                 3
#define CLOCKS_SLEEP_EN0_CLK_SYS_BUSCTRL_Msk                 ((uint32_t)0x00000008)
#define CLOCKS_SLEEP_EN0_CLK_SYS_BUSCTRL                     CLOCKS_SLEEP_EN0_CLK_SYS_BUSCTRL_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_ADC_Pos                     2
#define CLOCKS_SLEEP_EN0_CLK_SYS_ADC_Msk                     ((uint32_t)0x00000004)
#define CLOCKS_SLEEP_EN0_CLK_SYS_ADC                         CLOCKS_SLEEP_EN0_CLK_SYS_ADC_Msk
#define CLOCKS_SLEEP_EN0_CLK_ADC_ADC_Pos                     1
#define CLOCKS_SLEEP_EN0_CLK_ADC_ADC_Msk                     ((uint32_t)0x00000002)
#define CLOCKS_SLEEP_EN0_CLK_ADC_ADC                         CLOCKS_SLEEP_EN0_CLK_ADC_ADC_Msk
#define CLOCKS_SLEEP_EN0_CLK_SYS_CLOCKS_Pos                  0
#define CLOCKS_SLEEP_EN0_CLK_SYS_CLOCKS_Msk                  ((uint32_t)0x00000001)
#define CLOCKS_SLEEP_EN0_CLK_SYS_CLOCKS                      CLOCKS_SLEEP_EN0_CLK_SYS_CLOCKS_Msk
// CLOCKS->SLEEP_EN1
#define CLOCKS_SLEEP_EN1_CLK_SYS_XOSC_Pos                    14
#define CLOCKS_SLEEP_EN1_CLK_SYS_XOSC_Msk                    ((uint32_t)0x00004000)
#define CLOCKS_SLEEP_EN1_CLK_SYS_XOSC                        CLOCKS_SLEEP_EN1_CLK_SYS_XOSC_Msk
#define CLOCKS_SLEEP_EN1_CLK_SYS_XIP_Pos                     13
#define CLOCKS_SLEEP_EN1_CLK_SYS_XIP_Msk                     ((uint32_t)0x00002000)
#define CLOCKS_SLEEP_EN1_CLK_SYS_XIP                         CLOCKS_SLEEP_EN1_CLK_SYS_XIP_Msk
#define CLOCKS_SLEEP_EN1_CLK_SYS_WATCHDOG_Pos                12
#define CLOCKS_SLEEP_EN1_CLK_SYS_WATCHDOG_Msk                ((uint32_t)0x00001000)
#define CLOCKS_SLEEP_EN1_CLK_SYS_WATCHDOG                    CLOCKS_SLEEP_EN1_CLK_SYS_WATCHDOG_Msk
#define CLOCKS_SLEEP_EN1_CLK_USB_USBCTRL_Pos                 11
#define CLOCKS_SLEEP_EN1_CLK_USB_USBCTRL_Msk                 ((uint32_t)0x00000800)
#define CLOCKS_SLEEP_EN1_CLK_USB_USBCTRL                     CLOCKS_SLEEP_EN1_CLK_USB_USBCTRL_Msk
#define CLOCKS_SLEEP_EN1_CLK_SYS_USBCTRL_Pos                 10
#define CLOCKS_SLEEP_EN1_CLK_SYS_USBCTRL_Msk                 ((uint32_t)0x00000400)
#define CLOCKS_SLEEP_EN1_CLK_SYS_USBCTRL                     CLOCKS_SLEEP_EN1_CLK_SYS_USBCTRL_Msk
#define CLOCKS_SLEEP_EN1_CLK_SYS_UART1_Pos                   9
#define CLOCKS_SLEEP_EN1_CLK_SYS_UART1_Msk                   ((uint32_t)0x00000200)
#define CLOCKS_SLEEP_EN1_CLK_SYS_UART1                       CLOCKS_SLEEP_EN1_CLK_SYS_UART1_Msk
#define CLOCKS_SLEEP_EN1_CLK_PERI_UART1_Pos                  8
#define CLOCKS_SLEEP_EN1_CLK_PERI_UART1_Msk                  ((uint32_t)0x00000100)
#define CLOCKS_SLEEP_EN1_CLK_PERI_UART1                      CLOCKS_SLEEP_EN1_CLK_PERI_UART1_Msk
#define CLOCKS_SLEEP_EN1_CLK_SYS_UART0_Pos                   7
#define CLOCKS_SLEEP_EN1_CLK_SYS_UART0_Msk                   ((uint32_t)0x00000080)
#define CLOCKS_SLEEP_EN1_CLK_SYS_UART0                       CLOCKS_SLEEP_EN1_CLK_SYS_UART0_Msk
#define CLOCKS_SLEEP_EN1_CLK_PERI_UART0_Pos                  6
#define CLOCKS_SLEEP_EN1_CLK_PERI_UART0_Msk                  ((uint32_t)0x00000040)
#define CLOCKS_SLEEP_EN1_CLK_PERI_UART0                      CLOCKS_SLEEP_EN1_CLK_PERI_UART0_Msk
#define CLOCKS_SLEEP_EN1_CLK_SYS_TIMER_Pos                   5
#define CLOCKS_SLEEP_EN1_CLK_SYS_TIMER_Msk                   ((uint32_t)0x00000020)
#define CLOCKS_SLEEP_EN1_CLK_SYS_TIMER                       CLOCKS_SLEEP_EN1_CLK_SYS_TIMER_Msk
#define CLOCKS_SLEEP_EN1_CLK_SYS_TBMAN_Pos                   4
#define CLOCKS_SLEEP_EN1_CLK_SYS_TBMAN_Msk                   ((uint32_t)0x00000010)
#define CLOCKS_SLEEP_EN1_CLK_SYS_TBMAN                       CLOCKS_SLEEP_EN1_CLK_SYS_TBMAN_Msk
#define CLOCKS_SLEEP_EN1_CLK_SYS_SYSINFO_Pos                 3
#define CLOCKS_SLEEP_EN1_CLK_SYS_SYSINFO_Msk                 ((uint32_t)0x00000008)
#define CLOCKS_SLEEP_EN1_CLK_SYS_SYSINFO                     CLOCKS_SLEEP_EN1_CLK_SYS_SYSINFO_Msk
#define CLOCKS_SLEEP_EN1_CLK_SYS_SYSCFG_Pos                  2
#define CLOCKS_SLEEP_EN1_CLK_SYS_SYSCFG_Msk                  ((uint32_t)0x00000004)
#define CLOCKS_SLEEP_EN1_CLK_SYS_SYSCFG                      CLOCKS_SLEEP_EN1_CLK_SYS_SYSCFG_Msk
#define CLOCKS_SLEEP_EN1_CLK_SYS_SRAM5_Pos                   1
#define CLOCKS_SLEEP_EN1_CLK_SYS_SRAM5_Msk                   ((uint32_t)0x00000002)
#define CLOCKS_SLEEP_EN1_CLK_SYS_SRAM5                       CLOCKS_SLEEP_EN1_CLK_SYS_SRAM5_Msk
#define CLOCKS_SLEEP_EN1_CLK_SYS_SRAM4_Pos                   0
#define CLOCKS_SLEEP_EN1_CLK_SYS_SRAM4_Msk                   ((uint32_t)0x00000001)
#define CLOCKS_SLEEP_EN1_CLK_SYS_SRAM4                       CLOCKS_SLEEP_EN1_CLK_SYS_SRAM4_Msk
// CLOCKS->ENABLED0
#define CLOCKS_ENABLED0_CLK_SYS_SRAM3_Pos                    31
#define CLOCKS_ENABLED0_CLK_SYS_SRAM3_Msk                    ((uint32_t)0x80000000)
#define CLOCKS_ENABLED0_CLK_SYS_SRAM3                        CLOCKS_ENABLED0_CLK_SYS_SRAM3_Msk
#define CLOCKS_ENABLED0_CLK_SYS_SRAM2_Pos                    30
#define CLOCKS_ENABLED0_CLK_SYS_SRAM2_Msk                    ((uint32_t)0x40000000)
#define CLOCKS_ENABLED0_CLK_SYS_SRAM2                        CLOCKS_ENABLED0_CLK_SYS_SRAM2_Msk
#define CLOCKS_ENABLED0_CLK_SYS_SRAM1_Pos                    29
#define CLOCKS_ENABLED0_CLK_SYS_SRAM1_Msk                    ((uint32_t)0x20000000)
#define CLOCKS_ENABLED0_CLK_SYS_SRAM1                        CLOCKS_ENABLED0_CLK_SYS_SRAM1_Msk
#define CLOCKS_ENABLED0_CLK_SYS_SRAM0_Pos                    28
#define CLOCKS_ENABLED0_CLK_SYS_SRAM0_Msk                    ((uint32_t)0x10000000)
#define CLOCKS_ENABLED0_CLK_SYS_SRAM0                        CLOCKS_ENABLED0_CLK_SYS_SRAM0_Msk
#define CLOCKS_ENABLED0_CLK_SYS_SPI1_Pos                     27
#define CLOCKS_ENABLED0_CLK_SYS_SPI1_Msk                     ((uint32_t)0x08000000)
#define CLOCKS_ENABLED0_CLK_SYS_SPI1                         CLOCKS_ENABLED0_CLK_SYS_SPI1_Msk
#define CLOCKS_ENABLED0_CLK_PERI_SPI1_Pos                    26
#define CLOCKS_ENABLED0_CLK_PERI_SPI1_Msk                    ((uint32_t)0x04000000)
#define CLOCKS_ENABLED0_CLK_PERI_SPI1                        CLOCKS_ENABLED0_CLK_PERI_SPI1_Msk
#define CLOCKS_ENABLED0_CLK_SYS_SPI0_Pos                     25
#define CLOCKS_ENABLED0_CLK_SYS_SPI0_Msk                     ((uint32_t)0x02000000)
#define CLOCKS_ENABLED0_CLK_SYS_SPI0                         CLOCKS_ENABLED0_CLK_SYS_SPI0_Msk
#define CLOCKS_ENABLED0_CLK_PERI_SPI0_Pos                    24
#define CLOCKS_ENABLED0_CLK_PERI_SPI0_Msk                    ((uint32_t)0x01000000)
#define CLOCKS_ENABLED0_CLK_PERI_SPI0                        CLOCKS_ENABLED0_CLK_PERI_SPI0_Msk
#define CLOCKS_ENABLED0_CLK_SYS_SIO_Pos                      23
#define CLOCKS_ENABLED0_CLK_SYS_SIO_Msk                      ((uint32_t)0x00800000)
#define CLOCKS_ENABLED0_CLK_SYS_SIO                          CLOCKS_ENABLED0_CLK_SYS_SIO_Msk
#define CLOCKS_ENABLED0_CLK_SYS_RTC_Pos                      22
#define CLOCKS_ENABLED0_CLK_SYS_RTC_Msk                      ((uint32_t)0x00400000)
#define CLOCKS_ENABLED0_CLK_SYS_RTC                          CLOCKS_ENABLED0_CLK_SYS_RTC_Msk
#define CLOCKS_ENABLED0_CLK_RTC_RTC_Pos                      21
#define CLOCKS_ENABLED0_CLK_RTC_RTC_Msk                      ((uint32_t)0x00200000)
#define CLOCKS_ENABLED0_CLK_RTC_RTC                          CLOCKS_ENABLED0_CLK_RTC_RTC_Msk
#define CLOCKS_ENABLED0_CLK_SYS_ROSC_Pos                     20
#define CLOCKS_ENABLED0_CLK_SYS_ROSC_Msk                     ((uint32_t)0x00100000)
#define CLOCKS_ENABLED0_CLK_SYS_ROSC                         CLOCKS_ENABLED0_CLK_SYS_ROSC_Msk
#define CLOCKS_ENABLED0_CLK_SYS_ROM_Pos                      19
#define CLOCKS_ENABLED0_CLK_SYS_ROM_Msk                      ((uint32_t)0x00080000)
#define CLOCKS_ENABLED0_CLK_SYS_ROM                          CLOCKS_ENABLED0_CLK_SYS_ROM_Msk
#define CLOCKS_ENABLED0_CLK_SYS_RESETS_Pos                   18
#define CLOCKS_ENABLED0_CLK_SYS_RESETS_Msk                   ((uint32_t)0x00040000)
#define CLOCKS_ENABLED0_CLK_SYS_RESETS                       CLOCKS_ENABLED0_CLK_SYS_RESETS_Msk
#define CLOCKS_ENABLED0_CLK_SYS_PWM_Pos                      17
#define CLOCKS_ENABLED0_CLK_SYS_PWM_Msk                      ((uint32_t)0x00020000)
#define CLOCKS_ENABLED0_CLK_SYS_PWM                          CLOCKS_ENABLED0_CLK_SYS_PWM_Msk
#define CLOCKS_ENABLED0_CLK_SYS_PSM_Pos                      16
#define CLOCKS_ENABLED0_CLK_SYS_PSM_Msk                      ((uint32_t)0x00010000)
#define CLOCKS_ENABLED0_CLK_SYS_PSM                          CLOCKS_ENABLED0_CLK_SYS_PSM_Msk
#define CLOCKS_ENABLED0_CLK_SYS_PLL_USB_Pos                  15
#define CLOCKS_ENABLED0_CLK_SYS_PLL_USB_Msk                  ((uint32_t)0x00008000)
#define CLOCKS_ENABLED0_CLK_SYS_PLL_USB                      CLOCKS_ENABLED0_CLK_SYS_PLL_USB_Msk
#define CLOCKS_ENABLED0_CLK_SYS_PLL_SYS_Pos                  14
#define CLOCKS_ENABLED0_CLK_SYS_PLL_SYS_Msk                  ((uint32_t)0x00004000)
#define CLOCKS_ENABLED0_CLK_SYS_PLL_SYS                      CLOCKS_ENABLED0_CLK_SYS_PLL_SYS_Msk
#define CLOCKS_ENABLED0_CLK_SYS_PIO1_Pos                     13
#define CLOCKS_ENABLED0_CLK_SYS_PIO1_Msk                     ((uint32_t)0x00002000)
#define CLOCKS_ENABLED0_CLK_SYS_PIO1                         CLOCKS_ENABLED0_CLK_SYS_PIO1_Msk
#define CLOCKS_ENABLED0_CLK_SYS_PIO0_Pos                     12
#define CLOCKS_ENABLED0_CLK_SYS_PIO0_Msk                     ((uint32_t)0x00001000)
#define CLOCKS_ENABLED0_CLK_SYS_PIO0                         CLOCKS_ENABLED0_CLK_SYS_PIO0_Msk
#define CLOCKS_ENABLED0_CLK_SYS_PADS_Pos                     11
#define CLOCKS_ENABLED0_CLK_SYS_PADS_Msk                     ((uint32_t)0x00000800)
#define CLOCKS_ENABLED0_CLK_SYS_PADS                         CLOCKS_ENABLED0_CLK_SYS_PADS_Msk
#define CLOCKS_ENABLED0_CLK_SYS_VREG_AND_CHIP_RESET_Pos      10
#define CLOCKS_ENABLED0_CLK_SYS_VREG_AND_CHIP_RESET_Msk      ((uint32_t)0x00000400)
#define CLOCKS_ENABLED0_CLK_SYS_VREG_AND_CHIP_RESET          CLOCKS_ENABLED0_CLK_SYS_VREG_AND_CHIP_RESET_Msk
#define CLOCKS_ENABLED0_CLK_SYS_JTAG_Pos                     9
#define CLOCKS_ENABLED0_CLK_SYS_JTAG_Msk                     ((uint32_t)0x00000200)
#define CLOCKS_ENABLED0_CLK_SYS_JTAG                         CLOCKS_ENABLED0_CLK_SYS_JTAG_Msk
#define CLOCKS_ENABLED0_CLK_SYS_IO_Pos                       8
#define CLOCKS_ENABLED0_CLK_SYS_IO_Msk                       ((uint32_t)0x00000100)
#define CLOCKS_ENABLED0_CLK_SYS_IO                           CLOCKS_ENABLED0_CLK_SYS_IO_Msk
#define CLOCKS_ENABLED0_CLK_SYS_I2C1_Pos                     7
#define CLOCKS_ENABLED0_CLK_SYS_I2C1_Msk                     ((uint32_t)0x00000080)
#define CLOCKS_ENABLED0_CLK_SYS_I2C1                         CLOCKS_ENABLED0_CLK_SYS_I2C1_Msk
#define CLOCKS_ENABLED0_CLK_SYS_I2C0_Pos                     6
#define CLOCKS_ENABLED0_CLK_SYS_I2C0_Msk                     ((uint32_t)0x00000040)
#define CLOCKS_ENABLED0_CLK_SYS_I2C0                         CLOCKS_ENABLED0_CLK_SYS_I2C0_Msk
#define CLOCKS_ENABLED0_CLK_SYS_DMA_Pos                      5
#define CLOCKS_ENABLED0_CLK_SYS_DMA_Msk                      ((uint32_t)0x00000020)
#define CLOCKS_ENABLED0_CLK_SYS_DMA                          CLOCKS_ENABLED0_CLK_SYS_DMA_Msk
#define CLOCKS_ENABLED0_CLK_SYS_BUSFABRIC_Pos                4
#define CLOCKS_ENABLED0_CLK_SYS_BUSFABRIC_Msk                ((uint32_t)0x00000010)
#define CLOCKS_ENABLED0_CLK_SYS_BUSFABRIC                    CLOCKS_ENABLED0_CLK_SYS_BUSFABRIC_Msk
#define CLOCKS_ENABLED0_CLK_SYS_BUSCTRL_Pos                  3
#define CLOCKS_ENABLED0_CLK_SYS_BUSCTRL_Msk                  ((uint32_t)0x00000008)
#define CLOCKS_ENABLED0_CLK_SYS_BUSCTRL                      CLOCKS_ENABLED0_CLK_SYS_BUSCTRL_Msk
#define CLOCKS_ENABLED0_CLK_SYS_ADC_Pos                      2
#define CLOCKS_ENABLED0_CLK_SYS_ADC_Msk                      ((uint32_t)0x00000004)
#define CLOCKS_ENABLED0_CLK_SYS_ADC                          CLOCKS_ENABLED0_CLK_SYS_ADC_Msk
#define CLOCKS_ENABLED0_CLK_ADC_ADC_Pos                      1
#define CLOCKS_ENABLED0_CLK_ADC_ADC_Msk                      ((uint32_t)0x00000002)
#define CLOCKS_ENABLED0_CLK_ADC_ADC                          CLOCKS_ENABLED0_CLK_ADC_ADC_Msk
#define CLOCKS_ENABLED0_CLK_SYS_CLOCKS_Pos                   0
#define CLOCKS_ENABLED0_CLK_SYS_CLOCKS_Msk                   ((uint32_t)0x00000001)
#define CLOCKS_ENABLED0_CLK_SYS_CLOCKS                       CLOCKS_ENABLED0_CLK_SYS_CLOCKS_Msk
// CLOCKS->ENABLED1
#define CLOCKS_ENABLED1_CLK_SYS_XOSC_Pos                     14
#define CLOCKS_ENABLED1_CLK_SYS_XOSC_Msk                     ((uint32_t)0x00004000)
#define CLOCKS_ENABLED1_CLK_SYS_XOSC                         CLOCKS_ENABLED1_CLK_SYS_XOSC_Msk
#define CLOCKS_ENABLED1_CLK_SYS_XIP_Pos                      13
#define CLOCKS_ENABLED1_CLK_SYS_XIP_Msk                      ((uint32_t)0x00002000)
#define CLOCKS_ENABLED1_CLK_SYS_XIP                          CLOCKS_ENABLED1_CLK_SYS_XIP_Msk
#define CLOCKS_ENABLED1_CLK_SYS_WATCHDOG_Pos                 12
#define CLOCKS_ENABLED1_CLK_SYS_WATCHDOG_Msk                 ((uint32_t)0x00001000)
#define CLOCKS_ENABLED1_CLK_SYS_WATCHDOG                     CLOCKS_ENABLED1_CLK_SYS_WATCHDOG_Msk
#define CLOCKS_ENABLED1_CLK_USB_USBCTRL_Pos                  11
#define CLOCKS_ENABLED1_CLK_USB_USBCTRL_Msk                  ((uint32_t)0x00000800)
#define CLOCKS_ENABLED1_CLK_USB_USBCTRL                      CLOCKS_ENABLED1_CLK_USB_USBCTRL_Msk
#define CLOCKS_ENABLED1_CLK_SYS_USBCTRL_Pos                  10
#define CLOCKS_ENABLED1_CLK_SYS_USBCTRL_Msk                  ((uint32_t)0x00000400)
#define CLOCKS_ENABLED1_CLK_SYS_USBCTRL                      CLOCKS_ENABLED1_CLK_SYS_USBCTRL_Msk
#define CLOCKS_ENABLED1_CLK_SYS_UART1_Pos                    9
#define CLOCKS_ENABLED1_CLK_SYS_UART1_Msk                    ((uint32_t)0x00000200)
#define CLOCKS_ENABLED1_CLK_SYS_UART1                        CLOCKS_ENABLED1_CLK_SYS_UART1_Msk
#define CLOCKS_ENABLED1_CLK_PERI_UART1_Pos                   8
#define CLOCKS_ENABLED1_CLK_PERI_UART1_Msk                   ((uint32_t)0x00000100)
#define CLOCKS_ENABLED1_CLK_PERI_UART1                       CLOCKS_ENABLED1_CLK_PERI_UART1_Msk
#define CLOCKS_ENABLED1_CLK_SYS_UART0_Pos                    7
#define CLOCKS_ENABLED1_CLK_SYS_UART0_Msk                    ((uint32_t)0x00000080)
#define CLOCKS_ENABLED1_CLK_SYS_UART0                        CLOCKS_ENABLED1_CLK_SYS_UART0_Msk
#define CLOCKS_ENABLED1_CLK_PERI_UART0_Pos                   6
#define CLOCKS_ENABLED1_CLK_PERI_UART0_Msk                   ((uint32_t)0x00000040)
#define CLOCKS_ENABLED1_CLK_PERI_UART0                       CLOCKS_ENABLED1_CLK_PERI_UART0_Msk
#define CLOCKS_ENABLED1_CLK_SYS_TIMER_Pos                    5
#define CLOCKS_ENABLED1_CLK_SYS_TIMER_Msk                    ((uint32_t)0x00000020)
#define CLOCKS_ENABLED1_CLK_SYS_TIMER                        CLOCKS_ENABLED1_CLK_SYS_TIMER_Msk
#define CLOCKS_ENABLED1_CLK_SYS_TBMAN_Pos                    4
#define CLOCKS_ENABLED1_CLK_SYS_TBMAN_Msk                    ((uint32_t)0x00000010)
#define CLOCKS_ENABLED1_CLK_SYS_TBMAN                        CLOCKS_ENABLED1_CLK_SYS_TBMAN_Msk
#define CLOCKS_ENABLED1_CLK_SYS_SYSINFO_Pos                  3
#define CLOCKS_ENABLED1_CLK_SYS_SYSINFO_Msk                  ((uint32_t)0x00000008)
#define CLOCKS_ENABLED1_CLK_SYS_SYSINFO                      CLOCKS_ENABLED1_CLK_SYS_SYSINFO_Msk
#define CLOCKS_ENABLED1_CLK_SYS_SYSCFG_Pos                   2
#define CLOCKS_ENABLED1_CLK_SYS_SYSCFG_Msk                   ((uint32_t)0x00000004)
#define CLOCKS_ENABLED1_CLK_SYS_SYSCFG                       CLOCKS_ENABLED1_CLK_SYS_SYSCFG_Msk
#define CLOCKS_ENABLED1_CLK_SYS_SRAM5_Pos                    1
#define CLOCKS_ENABLED1_CLK_SYS_SRAM5_Msk                    ((uint32_t)0x00000002)
#define CLOCKS_ENABLED1_CLK_SYS_SRAM5                        CLOCKS_ENABLED1_CLK_SYS_SRAM5_Msk
#define CLOCKS_ENABLED1_CLK_SYS_SRAM4_Pos                    0
#define CLOCKS_ENABLED1_CLK_SYS_SRAM4_Msk                    ((uint32_t)0x00000001)
#define CLOCKS_ENABLED1_CLK_SYS_SRAM4                        CLOCKS_ENABLED1_CLK_SYS_SRAM4_Msk
// CLOCKS->INTR
#define CLOCKS_INTR_CLK_SYS_RESUS_Pos                        0
#define CLOCKS_INTR_CLK_SYS_RESUS_Msk                        ((uint32_t)0x00000001)
#define CLOCKS_INTR_CLK_SYS_RESUS                            CLOCKS_INTR_CLK_SYS_RESUS_Msk
// CLOCKS->INTE
#define CLOCKS_INTE_CLK_SYS_RESUS_Pos                        0
#define CLOCKS_INTE_CLK_SYS_RESUS_Msk                        ((uint32_t)0x00000001)
#define CLOCKS_INTE_CLK_SYS_RESUS                            CLOCKS_INTE_CLK_SYS_RESUS_Msk
// CLOCKS->INTF
#define CLOCKS_INTF_CLK_SYS_RESUS_Pos                        0
#define CLOCKS_INTF_CLK_SYS_RESUS_Msk                        ((uint32_t)0x00000001)
#define CLOCKS_INTF_CLK_SYS_RESUS                            CLOCKS_INTF_CLK_SYS_RESUS_Msk
// CLOCKS->INTS
#define CLOCKS_INTS_CLK_SYS_RESUS_Pos                        0
#define CLOCKS_INTS_CLK_SYS_RESUS_Msk                        ((uint32_t)0x00000001)
#define CLOCKS_INTS_CLK_SYS_RESUS                            CLOCKS_INTS_CLK_SYS_RESUS_Msk

// DMA->CH[n].READ_ADDR
#define DMA_CH_READ_ADDR_CH0_READ_ADDR_Pos                   0
#define DMA_CH_READ_ADDR_CH0_READ_ADDR_Msk                   ((uint32_t)0xFFFFFFFF)
#define DMA_CH_READ_ADDR_CH0_READ_ADDR                       DMA_CH_READ_ADDR_CH0_READ_ADDR_Msk
// DMA->CH[n].WRITE_ADDR
#define DMA_CH_WRITE_ADDR_CH0_WRITE_ADDR_Pos                 0
#define DMA_CH_WRITE_ADDR_CH0_WRITE_ADDR_Msk                 ((uint32_t)0xFFFFFFFF)
#define DMA_CH_WRITE_ADDR_CH0_WRITE_ADDR                     DMA_CH_WRITE_ADDR_CH0_WRITE_ADDR_Msk
// DMA->CH[n].TRANS_COUNT
#define DMA_CH_TRANS_COUNT_CH0_TRANS_COUNT_Pos               0
#define DMA_CH_TRANS_COUNT_CH0_TRANS_COUNT_Msk               ((uint32_t)0xFFFFFFFF)
#define DMA_CH_TRANS_COUNT_CH0_TRANS_COUNT                   DMA_CH_TRANS_COUNT_CH0_TRANS_COUNT_Msk
// DMA->CH[n].CTRL_TRIG
#define DMA_CH_CTRL_TRIG_AHB_ERROR_Pos                       31
#define DMA_CH_CTRL_TRIG_AHB_ERROR_Msk                       ((uint32_t)0x80000000)
#define DMA_CH_CTRL_TRIG_AHB_ERROR                           DMA_CH_CTRL_TRIG_AHB_ERROR_Msk
#define DMA_CH_CTRL_TRIG_READ_ERROR_Pos                      30
#define DMA_CH_CTRL_TRIG_READ_ERROR_Msk                      ((uint32_t)0x40000000)
#define DMA_CH_CTRL_TRIG_READ_ERROR                          DMA_CH_CTRL_TRIG_READ_ERROR_Msk
#define DMA_CH_CTRL_TRIG_WRITE_ERROR_Pos                     29
#define DMA_CH_CTRL_TRIG_WRITE_ERROR_Msk                     ((uint32_t)0x20000000)
#define DMA_CH_CTRL_TRIG_WRITE_ERROR                         DMA_CH_CTRL_TRIG_WRITE_ERROR_Msk
#define DMA_CH_CTRL_TRIG_BUSY_Pos                            24
#define DMA_CH_CTRL_TRIG_BUSY_Msk                            ((uint32_t)0x01000000)
#define DMA_CH_CTRL_TRIG_BUSY                                DMA_CH_CTRL_TRIG_BUSY_Msk
#define DMA_CH_CTRL_TRIG_SNIFF_EN_Pos                        23
#define DMA_CH_CTRL_TRIG_SNIFF_EN_Msk                        ((uint32_t)0x00800000)
#define DMA_CH_CTRL_TRIG_SNIFF_EN                            DMA_CH_CTRL_TRIG_SNIFF_EN_Msk
#define DMA_CH_CTRL_TRIG_BSWAP_Pos                           22
#define DMA_CH_CTRL_TRIG_BSWAP_Msk                           ((uint32_t)0x00400000)
#define DMA_CH_CTRL_TRIG_BSWAP                               DMA_CH_CTRL_TRIG_BSWAP_Msk
#define DMA_CH_CTRL_TRIG_IRQ_QUIET_Pos                       21
#define DMA_CH_CTRL_TRIG_IRQ_QUIET_Msk                       ((uint32_t)0x00200000)
#define DMA_CH_CTRL_TRIG_IRQ_QUIET                           DMA_CH_CTRL_TRIG_IRQ_QUIET_Msk
#define DMA_CH_CTRL_TRIG_TREQ_SEL_Pos                        15
#define DMA_CH_CTRL_TRIG_TREQ_SEL_Msk                        ((uint32_t)0x001F8000)
#define DMA_CH_CTRL_TRIG_TREQ_SEL                            DMA_CH_CTRL_TRIG_TREQ_SEL_Msk
#define DMA_CH_CTRL_TRIG_CHAIN_TO_Pos                        11
#define DMA_CH_CTRL_TRIG_CHAIN_TO_Msk                        ((uint32_t)0x00007800)
#define DMA_CH_CTRL_TRIG_CHAIN_TO                            DMA_CH_CTRL_TRIG_CHAIN_TO_Msk
#define DMA_CH_CTRL_TRIG_RING_SEL_Pos                        10
#define DMA_CH_CTRL_TRIG_RING_SEL_Msk                        ((uint32_t)0x00000400)
#define DMA_CH_CTRL_TRIG_RING_SEL                            DMA_CH_CTRL_TRIG_RING_SEL_Msk
#define DMA_CH_CTRL_TRIG_RING_SIZE_Pos                       6
#define DMA_CH_CTRL_TRIG_RING_SIZE_Msk                       ((uint32_t)0x000003C0)
#define DMA_CH_CTRL_TRIG_RING_SIZE                           DMA_CH_CTRL_TRIG_RING_SIZE_Msk
#define DMA_CH_CTRL_TRIG_INCR_WRITE_Pos                      5
#define DMA_CH_CTRL_TRIG_INCR_WRITE_Msk                      ((uint32_t)0x00000020)
#define DMA_CH_CTRL_TRIG_INCR_WRITE                          DMA_CH_CTRL_TRIG_INCR_WRITE_Msk
#define DMA_CH_CTRL_TRIG_INCR_READ_Pos                       4
#define DMA_CH_CTRL_TRIG_INCR_READ_Msk                       ((uint32_t)0x00000010)
#define DMA_CH_CTRL_TRIG_INCR_READ                           DMA_CH_CTRL_TRIG_INCR_READ_Msk
#define DMA_CH_CTRL_TRIG_DATA_SIZE_Pos                       2
#define DMA_CH_CTRL_TRIG_DATA_SIZE_Msk                       ((uint32_t)0x0000000C)
#define DMA_CH_CTRL_TRIG_DATA_SIZE                           DMA_CH_CTRL_TRIG_DATA_SIZE_Msk
#define DMA_CH_CTRL_TRIG_HIGH_PRIORITY_Pos                   1
#define DMA_CH_CTRL_TRIG_HIGH_PRIORITY_Msk                   ((uint32_t)0x00000002)
#define DMA_CH_CTRL_TRIG_HIGH_PRIORITY                       DMA_CH_CTRL_TRIG_HIGH_PRIORITY_Msk
#define DMA_CH_CTRL_TRIG_EN_Pos                              0
#define DMA_CH_CTRL_TRIG_EN_Msk                              ((uint32_t)0x00000001)
#define DMA_CH_CTRL_TRIG_EN                                  DMA_CH_CTRL_TRIG_EN_Msk
// DMA->CH[n].AL1_CTRL
#define DMA_CH_AL1_CTRL_CH0_AL1_CTRL_Pos                     0
#define DMA_CH_AL1_CTRL_CH0_AL1_CTRL_Msk                     ((uint32_t)0xFFFFFFFF)
#define DMA_CH_AL1_CTRL_CH0_AL1_CTRL                         DMA_CH_AL1_CTRL_CH0_AL1_CTRL_Msk
// DMA->CH[n].AL1_READ_ADDR
#define DMA_CH_AL1_READ_ADDR_CH0_AL1_READ_ADDR_Pos           0
#define DMA_CH_AL1_READ_ADDR_CH0_AL1_READ_ADDR_Msk           ((uint32_t)0xFFFFFFFF)
#define DMA_CH_AL1_READ_ADDR_CH0_AL1_READ_ADDR               DMA_CH_AL1_READ_ADDR_CH0_AL1_READ_ADDR_Msk
// DMA->CH[n].AL1_WRITE_ADDR
#define DMA_CH_AL1_WRITE_ADDR_CH0_AL1_WRITE_ADDR_Pos         0
#define DMA_CH_AL1_WRITE_ADDR_CH0_AL1_WRITE_ADDR_Msk         ((uint32_t)0xFFFFFFFF)
#define DMA_CH_AL1_WRITE_ADDR_CH0_AL1_WRITE_ADDR             DMA_CH_AL1_WRITE_ADDR_CH0_AL1_WRITE_ADDR_Msk
// DMA->CH[n].AL1_TRANS_COUNT_TRIG
#define DMA_CH_AL1_TRANS_COUNT_TRIG_CH0_AL1_TRANS_COUNT_TRIG_Pos 0
#define DMA_CH_AL1_TRANS_COUNT_TRIG_CH0_AL1_TRANS_COUNT_TRIG_Msk ((uint32_t)0xFFFFFFFF)
#define DMA_CH_AL1_TRANS_COUNT_TRIG_CH0_AL1_TRANS_COUNT_TRIG DMA_CH_AL1_TRANS_COUNT_TRIG_CH0_AL1_TRANS_COUNT_TRIG_Msk
// DMA->CH[n].AL2_CTRL
#define DMA_CH_AL2_CTRL_CH0_AL2_CTRL_Pos                     0
#define DMA_CH_AL2_CTRL_CH0_AL2_CTRL_Msk                     ((uint32_t)0xFFFFFFFF)
#define DMA_CH_AL2_CTRL_CH0_AL2_CTRL                         DMA_CH_AL2_CTRL_CH0_AL2_CTRL_Msk
// DMA->CH[n].AL2_TRANS_COUNT
#define DMA_CH_AL2_TRANS_COUNT_CH0_AL2_TRANS_COUNT_Pos       0
#define DMA_CH_AL2_TRANS_COUNT_CH0_AL2_TRANS_COUNT_Msk       ((uint32_t)0xFFFFFFFF)
#define DMA_CH_AL2_TRANS_COUNT_CH0_AL2_TRANS_COUNT           DMA_CH_AL2_TRANS_COUNT_CH0_AL2_TRANS_COUNT_Msk
// DMA->CH[n].AL2_READ_ADDR
#define DMA_CH_AL2_READ_ADDR_CH0_AL2_READ_ADDR_Pos           0
#define DMA_CH_AL2_READ_ADDR_CH0_AL2_READ_ADDR_Msk           ((uint32_t)0xFFFFFFFF)
#define DMA_CH_AL2_READ_ADDR_CH0_AL2_READ_ADDR               DMA_CH_AL2_READ_ADDR_CH0_AL2_READ_ADDR_Msk
// DMA->CH[n].AL2_WRITE_ADDR_TRIG
#define DMA_CH_AL2_WRITE_ADDR_TRIG_CH0_AL2_WRITE_ADDR_TRIG_Pos 0
#define DMA_CH_AL2_WRITE_ADDR_TRIG_CH0_AL2_WRITE_ADDR_TRIG_Msk ((uint32_t)0xFFFFFFFF)
#define DMA_CH_AL2_WRITE_ADDR_TRIG_CH0_AL2_WRITE_ADDR_TRIG   DMA_CH_AL2_WRITE_ADDR_TRIG_CH0_AL2_WRITE_ADDR_TRIG_Msk
// DMA->CH[n].AL3_CTRL
#define DMA_CH_AL3_CTRL_CH0_AL3_CTRL_Pos                     0
#define DMA_CH_AL3_CTRL_CH0_AL3_CTRL_Msk                     ((uint32_t)0xFFFFFFFF)
#define DMA_CH_AL3_CTRL_CH0_AL3_CTRL                         DMA_CH_AL3_CTRL_CH0_AL3_CTRL_Msk
// DMA->CH[n].AL3_WRITE_ADDR
#define DMA_CH_AL3_WRITE_ADDR_CH0_AL3_WRITE_ADDR_Pos         0
#define DMA_CH_AL3_WRITE_ADDR_CH0_AL3_WRITE_ADDR_Msk         ((uint32_t)0xFFFFFFFF)
#define DMA_CH_AL3_WRITE_ADDR_CH0_AL3_WRITE_ADDR             DMA_CH_AL3_WRITE_ADDR_CH0_AL3_WRITE_ADDR_Msk
// DMA->CH[n].AL3_TRANS_COUNT
#define DMA_CH_AL3_TRANS_COUNT_CH0_AL3_TRANS_COUNT_Pos       0
#define DMA_CH_AL3_TRANS_COUNT_CH0_AL3_TRANS_COUNT_Msk       ((uint32_t)0xFFFFFFFF)
#define DMA_CH_AL3_TRANS_COUNT_CH0_AL3_TRANS_COUNT           DMA_CH_AL3_TRANS_COUNT_CH0_AL3_TRANS_COUNT_Msk
// DMA->CH[n].AL3_READ_ADDR_TRIG
#define DMA_CH_AL3_READ_ADDR_TRIG_CH0_AL3_READ_ADDR_TRIG_Pos 0
#define DMA_CH_AL3_READ_ADDR_TRIG_CH0_AL3_READ_ADDR_TRIG_Msk ((uint32_t)0xFFFFFFFF)
#define DMA_CH_AL3_READ_ADDR_TRIG_CH0_AL3_READ_ADDR_TRIG     DMA_CH_AL3_READ_ADDR_TRIG_CH0_AL3_READ_ADDR_TRIG_Msk
// DMA->INTR
#define DMA_INTR_INTR_Pos                                    0
#define DMA_INTR_INTR_Msk                                    ((uint32_t)0x0000FFFF)
#define DMA_INTR_INTR                                        DMA_INTR_INTR_Msk
// DMA->INTE0
#define DMA_INTE0_INTE0_Pos                                  0
#define DMA_INTE0_INTE0_Msk                                  ((uint32_t)0x0000FFFF)
#define DMA_INTE0_INTE0                                      DMA_INTE0_INTE0_Msk
// DMA->INTF0
#define DMA_INTF0_INTF0_Pos                                  0
#define DMA_INTF0_INTF0_Msk                                  ((uint32_t)0x0000FFFF)
#define DMA_INTF0_INTF0                                      DMA_INTF0_INTF0_Msk
// DMA->INTS0
#define DMA_INTS0_INTS0_Pos                                  0
#define DMA_INTS0_INTS0_Msk                                  ((uint32_t)0x0000FFFF)
#define DMA_INTS0_INTS0                                      DMA_INTS0_INTS0_Msk
// DMA->INTE1
#define DMA_INTE1_INTE1_Pos                                  0
#define DMA_INTE1_INTE1_Msk                                  ((uint32_t)0x0000FFFF)
#define DMA_INTE1_INTE1                                      DMA_INTE1_INTE1_Msk
// DMA->INTF1
#define DMA_INTF1_INTF1_Pos                                  0
#define DMA_INTF1_INTF1_Msk                                  ((uint32_t)0x0000FFFF)
#define DMA_INTF1_INTF1                                      DMA_INTF1_INTF1_Msk
// DMA->INTS1
#define DMA_INTS1_INTS1_Pos                                  0
#define DMA_INTS1_INTS1_Msk                                  ((uint32_t)0x0000FFFF)
#define DMA_INTS1_INTS1                                      DMA_INTS1_INTS1_Msk
// DMA->PACE_TIMER
#define DMA_PACE_TIMER_X_Pos                                 16
#define DMA_PACE_TIMER_X_Msk                                 ((uint32_t)0xFFFF0000)
#define DMA_PACE_TIMER_X                                     DMA_PACE_TIMER_X_Msk
#define DMA_PACE_TIMER_Y_Pos                                 0
#define DMA_PACE_TIMER_Y_Msk                                 ((uint32_t)0x0000FFFF)
#define DMA_PACE_TIMER_Y                                     DMA_PACE_TIMER_Y_Msk
// DMA->MULTI_CHAN_TRIGGER
#define DMA_MULTI_CHAN_TRIGGER_MULTI_CHAN_TRIGGER_Pos        0
#define DMA_MULTI_CHAN_TRIGGER_MULTI_CHAN_TRIGGER_Msk        ((uint32_t)0x0000FFFF)
#define DMA_MULTI_CHAN_TRIGGER_MULTI_CHAN_TRIGGER            DMA_MULTI_CHAN_TRIGGER_MULTI_CHAN_TRIGGER_Msk
// DMA->SNIFF_CTRL
#define DMA_SNIFF_CTRL_OUT_INV_Pos                           11
#define DMA_SNIFF_CTRL_OUT_INV_Msk                           ((uint32_t)0x00000800)
#define DMA_SNIFF_CTRL_OUT_INV                               DMA_SNIFF_CTRL_OUT_INV_Msk
#define DMA_SNIFF_CTRL_OUT_REV_Pos                           10
#define DMA_SNIFF_CTRL_OUT_REV_Msk                           ((uint32_t)0x00000400)
#define DMA_SNIFF_CTRL_OUT_REV                               DMA_SNIFF_CTRL_OUT_REV_Msk
#define DMA_SNIFF_CTRL_BSWAP_Pos                             9
#define DMA_SNIFF_CTRL_BSWAP_Msk                             ((uint32_t)0x00000200)
#define DMA_SNIFF_CTRL_BSWAP                                 DMA_SNIFF_CTRL_BSWAP_Msk
#define DMA_SNIFF_CTRL_CALC_Pos                              5
#define DMA_SNIFF_CTRL_CALC_Msk                              ((uint32_t)0x000001E0)
#define DMA_SNIFF_CTRL_CALC                                  DMA_SNIFF_CTRL_CALC_Msk
#define DMA_SNIFF_CTRL_CALC_CRC32                            ((uint32_t)0x00000000)
#define DMA_SNIFF_CTRL_CALC_CRC32R                           ((uint32_t)0x00000020)
#define DMA_SNIFF_CTRL_CALC_CRC16                            ((uint32_t)0x00000040)
#define DMA_SNIFF_CTRL_CALC_CRC16R                           ((uint32_t)0x00000060)
#define DMA_SNIFF_CTRL_CALC_EVEN                             ((uint32_t)0x000001C0)
#define DMA_SNIFF_CTRL_CALC_SUM                              ((uint32_t)0x000001E0)
#define DMA_SNIFF_CTRL_DMACH_Pos                             1
#define DMA_SNIFF_CTRL_DMACH_Msk                             ((uint32_t)0x0000001E)
#define DMA_SNIFF_CTRL_DMACH                                 DMA_SNIFF_CTRL_DMACH_Msk
#define DMA_SNIFF_CTRL_EN_Pos                                0
#define DMA_SNIFF_CTRL_EN_Msk                                ((uint32_t)0x00000001)
#define DMA_SNIFF_CTRL_EN                                    DMA_SNIFF_CTRL_EN_Msk
// DMA->SNIFF_DATA
#define DMA_SNIFF_DATA_SNIFF_DATA_Pos                        0
#define DMA_SNIFF_DATA_SNIFF_DATA_Msk                        ((uint32_t)0xFFFFFFFF)
#define DMA_SNIFF_DATA_SNIFF_DATA                            DMA_SNIFF_DATA_SNIFF_DATA_Msk
// DMA->FIFO_LEVELS
#define DMA_FIFO_LEVELS_RAF_LVL_Pos                          16
#define DMA_FIFO_LEVELS_RAF_LVL_Msk                          ((uint32_t)0x00FF0000)
#define DMA_FIFO_LEVELS_RAF_LVL                              DMA_FIFO_LEVELS_RAF_LVL_Msk
#define DMA_FIFO_LEVELS_WAF_LVL_Pos                          8
#define DMA_FIFO_LEVELS_WAF_LVL_Msk                          ((uint32_t)0x0000FF00)
#define DMA_FIFO_LEVELS_WAF_LVL                              DMA_FIFO_LEVELS_WAF_LVL_Msk
#define DMA_FIFO_LEVELS_TDF_LVL_Pos                          0
#define DMA_FIFO_LEVELS_TDF_LVL_Msk                          ((uint32_t)0x000000FF)
#define DMA_FIFO_LEVELS_TDF_LVL                              DMA_FIFO_LEVELS_TDF_LVL_Msk
// DMA->CHAN_ABORT
#define DMA_CHAN_ABORT_CHAN_ABORT_Pos                        0
#define DMA_CHAN_ABORT_CHAN_ABORT_Msk                        ((uint32_t)0x0000FFFF)
#define DMA_CHAN_ABORT_CHAN_ABORT                            DMA_CHAN_ABORT_CHAN_ABORT_Msk
// DMA->N_CHANNELS
#define DMA_N_CHANNELS_N_CHANNELS_Pos                        0
#define DMA_N_CHANNELS_N_CHANNELS_Msk                        ((uint32_t)0x0000001F)
#define DMA_N_CHANNELS_N_CHANNELS                            DMA_N_CHANNELS_N_CHANNELS_Msk
// DMA->CH_DBG[n].CTDREQ
#define DMA_CH_DBG_CTDREQ_CH0_DBG_CTDREQ_Pos                 0
#define DMA_CH_DBG_CTDREQ_CH0_DBG_CTDREQ_Msk                 ((uint32_t)0x0000003F)
#define DMA_CH_DBG_CTDREQ_CH0_DBG_CTDREQ                     DMA_CH_DBG_CTDREQ_CH0_DBG_CTDREQ_Msk
// DMA->CH_DBG[n].TCR
#define DMA_CH_DBG_TCR_CH0_DBG_TCR_Pos                       0
#define DMA_CH_DBG_TCR_CH0_DBG_TCR_Msk                       ((uint32_t)0xFFFFFFFF)
#define DMA_CH_DBG_TCR_CH0_DBG_TCR                           DMA_CH_DBG_TCR_CH0_DBG_TCR_Msk

// I2C->IC_CON
#define I2C_IC_CON_STOP_DET_IF_MASTER_ACTIVE_Pos             10
#define I2C_IC_CON_STOP_DET_IF_MASTER_ACTIVE_Msk             ((uint32_t)0x00000400)
#define I2C_IC_CON_STOP_DET_IF_MASTER_ACTIVE                 I2C_IC_CON_STOP_DET_IF_MASTER_ACTIVE_Msk
#define I2C_IC_CON_RX_FIFO_FULL_HLD_CTRL_Pos                 9
#define I2C_IC_CON_RX_FIFO_FULL_HLD_CTRL_Msk                 ((uint32_t)0x00000200)
#define I2C_IC_CON_RX_FIFO_FULL_HLD_CTRL                     I2C_IC_CON_RX_FIFO_FULL_HLD_CTRL_Msk
#define I2C_IC_CON_RX_FIFO_FULL_HLD_CTRL_DISABLED            ((uint32_t)0x00000000)
#define I2C_IC_CON_RX_FIFO_FULL_HLD_CTRL_ENABLED             ((uint32_t)0x00000200)
#define I2C_IC_CON_TX_EMPTY_CTRL_Pos                         8
#define I2C_IC_CON_TX_EMPTY_CTRL_Msk                         ((uint32_t)0x00000100)
#define I2C_IC_CON_TX_EMPTY_CTRL                             I2C_IC_CON_TX_EMPTY_CTRL_Msk
#define I2C_IC_CON_TX_EMPTY_CTRL_DISABLED                    ((uint32_t)0x00000000)
#define I2C_IC_CON_TX_EMPTY_CTRL_ENABLED                     ((uint32_t)0x00000100)
#define I2C_IC_CON_STOP_DET_IFADDRESSED_Pos                  7
#define I2C_IC_CON_STOP_DET_IFADDRESSED_Msk                  ((uint32_t)0x00000080)
#define I2C_IC_CON_STOP_DET_IFADDRESSED                      I2C_IC_CON_STOP_DET_IFADDRESSED_Msk
#define I2C_IC_CON_STOP_DET_IFADDRESSED_DISABLED             ((uint32_t)0x00000000)
#define I2C_IC_CON_STOP_DET_IFADDRESSED_ENABLED              ((uint32_t)0x00000080)
#define I2C_IC_CON_IC_SLAVE_DISABLE_Pos                      6
#define I2C_IC_CON_IC_SLAVE_DISABLE_Msk                      ((uint32_t)0x00000040)
#define I2C_IC_CON_IC_SLAVE_DISABLE                          I2C_IC_CON_IC_SLAVE_DISABLE_Msk
#define I2C_IC_CON_IC_SLAVE_DISABLE_SLAVE_ENABLED            ((uint32_t)0x00000000)
#define I2C_IC_CON_IC_SLAVE_DISABLE_SLAVE_DISABLED           ((uint32_t)0x00000040)
#define I2C_IC_CON_IC_RESTART_EN_Pos                         5
#define I2C_IC_CON_IC_RESTART_EN_Msk                         ((uint32_t)0x00000020)
#define I2C_IC_CON_IC_RESTART_EN                             I2C_IC_CON_IC_RESTART_EN_Msk
#define I2C_IC_CON_IC_RESTART_EN_DISABLED                    ((uint32_t)0x00000000)
#define I2C_IC_CON_IC_RESTART_EN_ENABLED                     ((uint32_t)0x00000020)
#define I2C_IC_CON_IC_10BITADDR_MASTER_Pos                   4
#define I2C_IC_CON_IC_10BITADDR_MASTER_Msk                   ((uint32_t)0x00000010)
#define I2C_IC_CON_IC_10BITADDR_MASTER                       I2C_IC_CON_IC_10BITADDR_MASTER_Msk
#define I2C_IC_CON_IC_10BITADDR_MASTER_ADDR_7BITS            ((uint32_t)0x00000000)
#define I2C_IC_CON_IC_10BITADDR_MASTER_ADDR_10BITS           ((uint32_t)0x00000010)
#define I2C_IC_CON_IC_10BITADDR_SLAVE_Pos                    3
#define I2C_IC_CON_IC_10BITADDR_SLAVE_Msk                    ((uint32_t)0x00000008)
#define I2C_IC_CON_IC_10BITADDR_SLAVE                        I2C_IC_CON_IC_10BITADDR_SLAVE_Msk
#define I2C_IC_CON_IC_10BITADDR_SLAVE_ADDR_7BITS             ((uint32_t)0x00000000)
#define I2C_IC_CON_IC_10BITADDR_SLAVE_ADDR_10BITS            ((uint32_t)0x00000008)
#define I2C_IC_CON_SPEED_Pos                                 1
#define I2C_IC_CON_SPEED_Msk                                 ((uint32_t)0x00000006)
#define I2C_IC_CON_SPEED                                     I2C_IC_CON_SPEED_Msk
#define I2C_IC_CON_SPEED_STANDARD                            ((uint32_t)0x00000002)
#define I2C_IC_CON_SPEED_FAST                                ((uint32_t)0x00000004)
#define I2C_IC_CON_SPEED_HIGH                                ((uint32_t)0x00000006)
#define I2C_IC_CON_MASTER_MODE_Pos                           0
#define I2C_IC_CON_MASTER_MODE_Msk                           ((uint32_t)0x00000001)
#define I2C_IC_CON_MASTER_MODE                               I2C_IC_CON_MASTER_MODE_Msk
#define I2C_IC_CON_MASTER_MODE_DISABLED                      ((uint32_t)0x00000000)
#define I2C_IC_CON_MASTER_MODE_ENABLED                       ((uint32_t)0x00000001)
// I2C->IC_TAR
#define I2C_IC_TAR_SPECIAL_Pos                               11
#define I2C_IC_TAR_SPECIAL_Msk                               ((uint32_t)0x00000800)
#define I2C_IC_TAR_SPECIAL                                   I2C_IC_TAR_SPECIAL_Msk
#define I2C_IC_TAR_SPECIAL_DISABLED                          ((uint32_t)0x00000000)
#define I2C_IC_TAR_SPECIAL_ENABLED                           ((uint32_t)0x00000800)
#define I2C_IC_TAR_GC_OR_START_Pos                           10
#define I2C_IC_TAR_GC_OR_START_Msk                           ((uint32_t)0x00000400)
#define I2C_IC_TAR_GC_OR_START                               I2C_IC_TAR_GC_OR_START_Msk
#define I2C_IC_TAR_GC_OR_START_GENERAL_CALL                  ((uint32_t)0x00000000)
#define I2C_IC_TAR_GC_OR_START_START_BYTE                    ((uint32_t)0x00000400)
#define I2C_IC_TAR_IC_TAR_Pos                                0
#define I2C_IC_TAR_IC_TAR_Msk                                ((uint32_t)0x000003FF)
#define I2C_IC_TAR_IC_TAR                                    I2C_IC_TAR_IC_TAR_Msk
// I2C->IC_SAR
#define I2C_IC_SAR_IC_SAR_Pos                                0
#define I2C_IC_SAR_IC_SAR_Msk                                ((uint32_t)0x000003FF)
#define I2C_IC_SAR_IC_SAR                                    I2C_IC_SAR_IC_SAR_Msk
// I2C->IC_DATA_CMD
#define I2C_IC_DATA_CMD_FIRST_DATA_BYTE_Pos                  11
#define I2C_IC_DATA_CMD_FIRST_DATA_BYTE_Msk                  ((uint32_t)0x00000800)
#define I2C_IC_DATA_CMD_FIRST_DATA_BYTE                      I2C_IC_DATA_CMD_FIRST_DATA_BYTE_Msk
#define I2C_IC_DATA_CMD_FIRST_DATA_BYTE_INACTIVE             ((uint32_t)0x00000000)
#define I2C_IC_DATA_CMD_FIRST_DATA_BYTE_ACTIVE               ((uint32_t)0x00000800)
#define I2C_IC_DATA_CMD_RESTART_Pos                          10
#define I2C_IC_DATA_CMD_RESTART_Msk                          ((uint32_t)0x00000400)
#define I2C_IC_DATA_CMD_RESTART                              I2C_IC_DATA_CMD_RESTART_Msk
#define I2C_IC_DATA_CMD_RESTART_DISABLE                      ((uint32_t)0x00000000)
#define I2C_IC_DATA_CMD_RESTART_ENABLE                       ((uint32_t)0x00000400)
#define I2C_IC_DATA_CMD_STOP_Pos                             9
#define I2C_IC_DATA_CMD_STOP_Msk                             ((uint32_t)0x00000200)
#define I2C_IC_DATA_CMD_STOP                                 I2C_IC_DATA_CMD_STOP_Msk
#define I2C_IC_DATA_CMD_STOP_DISABLE                         ((uint32_t)0x00000000)
#define I2C_IC_DATA_CMD_STOP_ENABLE                          ((uint32_t)0x00000200)
#define I2C_IC_DATA_CMD_CMD_Pos                              8
#define I2C_IC_DATA_CMD_CMD_Msk                              ((uint32_t)0x00000100)
#define I2C_IC_DATA_CMD_CMD                                  I2C_IC_DATA_CMD_CMD_Msk
#define I2C_IC_DATA_CMD_CMD_WRITE                            ((uint32_t)0x00000000)
#define I2C_IC_DATA_CMD_CMD_READ                             ((uint32_t)0x00000100)
#define I2C_IC_DATA_CMD_DAT_Pos                              0
#define I2C_IC_DATA_CMD_DAT_Msk                              ((uint32_t)0x000000FF)
#define I2C_IC_DATA_CMD_DAT                                  I2C_IC_DATA_CMD_DAT_Msk
// I2C->IC_SS_SCL_HCNT
#define I2C_IC_SS_SCL_HCNT_IC_SS_SCL_HCNT_Pos                0
#define I2C_IC_SS_SCL_HCNT_IC_SS_SCL_HCNT_Msk                ((uint32_t)0x0000FFFF)
#define I2C_IC_SS_SCL_HCNT_IC_SS_SCL_HCNT                    I2C_IC_SS_SCL_HCNT_IC_SS_SCL_HCNT_Msk
// I2C->IC_SS_SCL_LCNT
#define I2C_IC_SS_SCL_LCNT_IC_SS_SCL_LCNT_Pos                0
#define I2C_IC_SS_SCL_LCNT_IC_SS_SCL_LCNT_Msk                ((uint32_t)0x0000FFFF)
#define I2C_IC_SS_SCL_LCNT_IC_SS_SCL_LCNT                    I2C_IC_SS_SCL_LCNT_IC_SS_SCL_LCNT_Msk
// I2C->IC_FS_SCL_HCNT
#define I2C_IC_FS_SCL_HCNT_IC_FS_SCL_HCNT_Pos                0
#define I2C_IC_FS_SCL_HCNT_IC_FS_SCL_HCNT_Msk                ((uint32_t)0x0000FFFF)
#define I2C_IC_FS_SCL_HCNT_IC_FS_SCL_HCNT                    I2C_IC_FS_SCL_HCNT_IC_FS_SCL_HCNT_Msk
// I2C->IC_FS_SCL_LCNT
#define I2C_IC_FS_SCL_LCNT_IC_FS_SCL_LCNT_Pos                0
#define I2C_IC_FS_SCL_LCNT_IC_FS_SCL_LCNT_Msk                ((uint32_t)0x0000FFFF)
#define I2C_IC_FS_SCL_LCNT_IC_FS_SCL_LCNT                    I2C_IC_FS_SCL_LCNT_IC_FS_SCL_LCNT_Msk
// I2C->IC_INTR_STAT
#define I2C_IC_INTR_STAT_R_RESTART_DET_Pos                   12
#define I2C_IC_INTR_STAT_R_RESTART_DET_Msk                   ((uint32_t)0x00001000)
#define I2C_IC_INTR_STAT_R_RESTART_DET                       I2C_IC_INTR_STAT_R_RESTART_DET_Msk
#define I2C_IC_INTR_STAT_R_RESTART_DET_INACTIVE              ((uint32_t)0x00000000)
#define I2C_IC_INTR_STAT_R_RESTART_DET_ACTIVE                ((uint32_t)0x00001000)
#define I2C_IC_INTR_STAT_R_GEN_CALL_Pos                      11
#define I2C_IC_INTR_STAT_R_GEN_CALL_Msk                      ((uint32_t)0x00000800)
#define I2C_IC_INTR_STAT_R_GEN_CALL                          I2C_IC_INTR_STAT_R_GEN_CALL_Msk
#define I2C_IC_INTR_STAT_R_GEN_CALL_INACTIVE                 ((uint32_t)0x00000000)
#define I2C_IC_INTR_STAT_R_GEN_CALL_ACTIVE                   ((uint32_t)0x00000800)
#define I2C_IC_INTR_STAT_R_START_DET_Pos                     10
#define I2C_IC_INTR_STAT_R_START_DET_Msk                     ((uint32_t)0x00000400)
#define I2C_IC_INTR_STAT_R_START_DET                         I2C_IC_INTR_STAT_R_START_DET_Msk
#define I2C_IC_INTR_STAT_R_START_DET_INACTIVE                ((uint32_t)0x00000000)
#define I2C_IC_INTR_STAT_R_START_DET_ACTIVE                  ((uint32_t)0x00000400)
#define I2C_IC_INTR_STAT_R_STOP_DET_Pos                      9
#define I2C_IC_INTR_STAT_R_STOP_DET_Msk                      ((uint32_t)0x00000200)
#define I2C_IC_INTR_STAT_R_STOP_DET                          I2C_IC_INTR_STAT_R_STOP_DET_Msk
#define I2C_IC_INTR_STAT_R_STOP_DET_INACTIVE                 ((uint32_t)0x00000000)
#define I2C_IC_INTR_STAT_R_STOP_DET_ACTIVE                   ((uint32_t)0x00000200)
#define I2C_IC_INTR_STAT_R_ACTIVITY_Pos                      8
#define I2C_IC_INTR_STAT_R_ACTIVITY_Msk                      ((uint32_t)0x00000100)
#define I2C_IC_INTR_STAT_R_ACTIVITY                          I2C_IC_INTR_STAT_R_ACTIVITY_Msk
#define I2C_IC_INTR_STAT_R_ACTIVITY_INACTIVE                 ((uint32_t)0x00000000)
#define I2C_IC_INTR_STAT_R_ACTIVITY_ACTIVE                   ((uint32_t)0x00000100)
#define I2C_IC_INTR_STAT_R_RX_DONE_Pos                       7
#define I2C_IC_INTR_STAT_R_RX_DONE_Msk                       ((uint32_t)0x00000080)
#define I2C_IC_INTR_STAT_R_RX_DONE                           I2C_IC_INTR_STAT_R_RX_DONE_Msk
#define I2C_IC_INTR_STAT_R_RX_DONE_INACTIVE                  ((uint32_t)0x00000000)
#define I2C_IC_INTR_STAT_R_RX_DONE_ACTIVE                    ((uint32_t)0x00000080)
#define I2C_IC_INTR_STAT_R_TX_ABRT_Pos                       6
#define I2C_IC_INTR_STAT_R_TX_ABRT_Msk                       ((uint32_t)0x00000040)
#define I2C_IC_INTR_STAT_R_TX_ABRT                           I2C_IC_INTR_STAT_R_TX_ABRT_Msk
#define I2C_IC_INTR_STAT_R_TX_ABRT_INACTIVE                  ((uint32_t)0x00000000)
#define I2C_IC_INTR_STAT_R_TX_ABRT_ACTIVE                    ((uint32_t)0x00000040)
#define I2C_IC_INTR_STAT_R_RD_REQ_Pos                        5
#define I2C_IC_INTR_STAT_R_RD_REQ_Msk                        ((uint32_t)0x00000020)
#define I2C_IC_INTR_STAT_R_RD_REQ                            I2C_IC_INTR_STAT_R_RD_REQ_Msk
#define I2C_IC_INTR_STAT_R_RD_REQ_INACTIVE                   ((uint32_t)0x00000000)
#define I2C_IC_INTR_STAT_R_RD_REQ_ACTIVE                     ((uint32_t)0x00000020)
#define I2C_IC_INTR_STAT_R_TX_EMPTY_Pos                      4
#define I2C_IC_INTR_STAT_R_TX_EMPTY_Msk                      ((uint32_t)0x00000010)
#define I2C_IC_INTR_STAT_R_TX_EMPTY                          I2C_IC_INTR_STAT_R_TX_EMPTY_Msk
#define I2C_IC_INTR_STAT_R_TX_EMPTY_INACTIVE                 ((uint32_t)0x00000000)
#define I2C_IC_INTR_STAT_R_TX_EMPTY_ACTIVE                   ((uint32_t)0x00000010)
#define I2C_IC_INTR_STAT_R_TX_OVER_Pos                       3
#define I2C_IC_INTR_STAT_R_TX_OVER_Msk                       ((uint32_t)0x00000008)
#define I2C_IC_INTR_STAT_R_TX_OVER                           I2C_IC_INTR_STAT_R_TX_OVER_Msk
#define I2C_IC_INTR_STAT_R_TX_OVER_INACTIVE                  ((uint32_t)0x00000000)
#define I2C_IC_INTR_STAT_R_TX_OVER_ACTIVE                    ((uint32_t)0x00000008)
#define I2C_IC_INTR_STAT_R_RX_FULL_Pos                       2
#define I2C_IC_INTR_STAT_R_RX_FULL_Msk                       ((uint32_t)0x00000004)
#define I2C_IC_INTR_STAT_R_RX_FULL                           I2C_IC_INTR_STAT_R_RX_FULL_Msk
#define I2C_IC_INTR_STAT_R_RX_FULL_INACTIVE                  ((uint32_t)0x00000000)
#define I2C_IC_INTR_STAT_R_RX_FULL_ACTIVE                    ((uint32_t)0x00000004)
#define I2C_IC_INTR_STAT_R_RX_OVER_Pos                       1
#define I2C_IC_INTR_STAT_R_RX_OVER_Msk                       ((uint32_t)0x00000002)
#define I2C_IC_INTR_STAT_R_RX_OVER                           I2C_IC_INTR_STAT_R_RX_OVER_Msk
#define I2C_IC_INTR_STAT_R_RX_OVER_INACTIVE                  ((uint32_t)0x00000000)
#define I2C_IC_INTR_STAT_R_RX_OVER_ACTIVE                    ((uint32_t)0x00000002)
#define I2C_IC_INTR_STAT_R_RX_UNDER_Pos                      0
#define I2C_IC_INTR_STAT_R_RX_UNDER_Msk                      ((uint32_t)0x00000001)
#define I2C_IC_INTR_STAT_R_RX_UNDER                          I2C_IC_INTR_STAT_R_RX_UNDER_Msk
#define I2C_IC_INTR_STAT_R_RX_UNDER_INACTIVE                 ((uint32_t)0x00000000)
#define I2C_IC_INTR_STAT_R_RX_UNDER_ACTIVE                   ((uint32_t)0x00000001)
// I2C->IC_INTR_MASK
#define I2C_IC_INTR_MASK_M_RESTART_DET_Pos                   12
#define I2C_IC_INTR_MASK_M_RESTART_DET_Msk                   ((uint32_t)0x00001000)
#define I2C_IC_INTR_MASK_M_RESTART_DET                       I2C_IC_INTR_MASK_M_RESTART_DET_Msk
#define I2C_IC_INTR_MASK_M_RESTART_DET_ENABLED               ((uint32_t)0x00000000)
#define I2C_IC_INTR_MASK_M_RESTART_DET_DISABLED              ((uint32_t)0x00001000)
#define I2C_IC_INTR_MASK_M_GEN_CALL_Pos                      11
#define I2C_IC_INTR_MASK_M_GEN_CALL_Msk                      ((uint32_t)0x00000800)
#define I2C_IC_INTR_MASK_M_GEN_CALL                          I2C_IC_INTR_MASK_M_GEN_CALL_Msk
#define I2C_IC_INTR_MASK_M_GEN_CALL_ENABLED                  ((uint32_t)0x00000000)
#define I2C_IC_INTR_MASK_M_GEN_CALL_DISABLED                 ((uint32_t)0x00000800)
#define I2C_IC_INTR_MASK_M_START_DET_Pos                     10
#define I2C_IC_INTR_MASK_M_START_DET_Msk                     ((uint32_t)0x00000400)
#define I2C_IC_INTR_MASK_M_START_DET                         I2C_IC_INTR_MASK_M_START_DET_Msk
#define I2C_IC_INTR_MASK_M_START_DET_ENABLED                 ((uint32_t)0x00000000)
#define I2C_IC_INTR_MASK_M_START_DET_DISABLED                ((uint32_t)0x00000400)
#define I2C_IC_INTR_MASK_M_STOP_DET_Pos                      9
#define I2C_IC_INTR_MASK_M_STOP_DET_Msk                      ((uint32_t)0x00000200)
#define I2C_IC_INTR_MASK_M_STOP_DET                          I2C_IC_INTR_MASK_M_STOP_DET_Msk
#define I2C_IC_INTR_MASK_M_STOP_DET_ENABLED                  ((uint32_t)0x00000000)
#define I2C_IC_INTR_MASK_M_STOP_DET_DISABLED                 ((uint32_t)0x00000200)
#define I2C_IC_INTR_MASK_M_ACTIVITY_Pos                      8
#define I2C_IC_INTR_MASK_M_ACTIVITY_Msk                      ((uint32_t)0x00000100)
#define I2C_IC_INTR_MASK_M_ACTIVITY                          I2C_IC_INTR_MASK_M_ACTIVITY_Msk
#define I2C_IC_INTR_MASK_M_ACTIVITY_ENABLED                  ((uint32_t)0x00000000)
#define I2C_IC_INTR_MASK_M_ACTIVITY_DISABLED                 ((uint32_t)0x00000100)
#define I2C_IC_INTR_MASK_M_RX_DONE_Pos                       7
#define I2C_IC_INTR_MASK_M_RX_DONE_Msk                       ((uint32_t)0x00000080)
#define I2C_IC_INTR_MASK_M_RX_DONE                           I2C_IC_INTR_MASK_M_RX_DONE_Msk
#define I2C_IC_INTR_MASK_M_RX_DONE_ENABLED                   ((uint32_t)0x00000000)
#define I2C_IC_INTR_MASK_M_RX_DONE_DISABLED                  ((uint32_t)0x00000080)
#define I2C_IC_INTR_MASK_M_TX_ABRT_Pos                       6
#define I2C_IC_INTR_MASK_M_TX_ABRT_Msk                       ((uint32_t)0x00000040)
#define I2C_IC_INTR_MASK_M_TX_ABRT                           I2C_IC_INTR_MASK_M_TX_ABRT_Msk
#define I2C_IC_INTR_MASK_M_TX_ABRT_ENABLED                   ((uint32_t)0x00000000)
#define I2C_IC_INTR_MASK_M_TX_ABRT_DISABLED                  ((uint32_t)0x00000040)
#define I2C_IC_INTR_MASK_M_RD_REQ_Pos                        5
#define I2C_IC_INTR_MASK_M_RD_REQ_Msk                        ((uint32_t)0x00000020)
#define I2C_IC_INTR_MASK_M_RD_REQ                            I2C_IC_INTR_MASK_M_RD_REQ_Msk
#define I2C_IC_INTR_MASK_M_RD_REQ_ENABLED                    ((uint32_t)0x00000000)
#define I2C_IC_INTR_MASK_M_RD_REQ_DISABLED                   ((uint32_t)0x00000020)
#define I2C_IC_INTR_MASK_M_TX_EMPTY_Pos                      4
#define I2C_IC_INTR_MASK_M_TX_EMPTY_Msk                      ((uint32_t)0x00000010)
#define I2C_IC_INTR_MASK_M_TX_EMPTY                          I2C_IC_INTR_MASK_M_TX_EMPTY_Msk
#define I2C_IC_INTR_MASK_M_TX_EMPTY_ENABLED                  ((uint32_t)0x00000000)
#define I2C_IC_INTR_MASK_M_TX_EMPTY_DISABLED                 ((uint32_t)0x00000010)
#define I2C_IC_INTR_MASK_M_TX_OVER_Pos                       3
#define I2C_IC_INTR_MASK_M_TX_OVER_Msk                       ((uint32_t)0x00000008)
#define I2C_IC_INTR_MASK_M_TX_OVER                           I2C_IC_INTR_MASK_M_TX_OVER_Msk
#define I2C_IC_INTR_MASK_M_TX_OVER_ENABLED                   ((uint32_t)0x00000000)
#define I2C_IC_INTR_MASK_M_TX_OVER_DISABLED                  ((uint32_t)0x00000008)
#define I2C_IC_INTR_MASK_M_RX_FULL_Pos                       2
#define I2C_IC_INTR_MASK_M_RX_FULL_Msk                       ((uint32_t)0x00000004)
#define I2C_IC_INTR_MASK_M_RX_FULL                           I2C_IC_INTR_MASK_M_RX_FULL_Msk
#define I2C_IC_INTR_MASK_M_RX_FULL_ENABLED                   ((uint32_t)0x00000000)
#define I2C_IC_INTR_MASK_M_RX_FULL_DISABLED                  ((uint32_t)0x00000004)
#define I2C_IC_INTR_MASK_M_RX_OVER_Pos                       1
#define I2C_IC_INTR_MASK_M_RX_OVER_Msk                       ((uint32_t)0x00000002)
#define I2C_IC_INTR_MASK_M_RX_OVER                           I2C_IC_INTR_MASK_M_RX_OVER_Msk
#define I2C_IC_INTR_MASK_M_RX_OVER_ENABLED                   ((uint32_t)0x00000000)
#define I2C_IC_INTR_MASK_M_RX_OVER_DISABLED                  ((uint32_t)0x00000002)
#define I2C_IC_INTR_MASK_M_RX_UNDER_Pos                      0
#define I2C_IC_INTR_MASK_M_RX_UNDER_Msk                      ((uint32_t)0x00000001)
#define I2C_IC_INTR_MASK_M_RX_UNDER                          I2C_IC_INTR_MASK_M_RX_UNDER_Msk
#define I2C_IC_INTR_MASK_M_RX_UNDER_ENABLED                  ((uint32_t)0x00000000)
#define I2C_IC_INTR_MASK_M_RX_UNDER_DISABLED                 ((uint32_t)0x00000001)
// I2C->IC_RAW_INTR_STAT
#define I2C_IC_RAW_INTR_STAT_RESTART_DET_Pos                 12
#define I2C_IC_RAW_INTR_STAT_RESTART_DET_Msk                 ((uint32_t)0x00001000)
#define I2C_IC_RAW_INTR_STAT_RESTART_DET                     I2C_IC_RAW_INTR_STAT_RESTART_DET_Msk
#define I2C_IC_RAW_INTR_STAT_RESTART_DET_INACTIVE            ((uint32_t)0x00000000)
#define I2C_IC_RAW_INTR_STAT_RESTART_DET_ACTIVE              ((uint32_t)0x00001000)
#define I2C_IC_RAW_INTR_STAT_GEN_CALL_Pos                    11
#define I2C_IC_RAW_INTR_STAT_GEN_CALL_Msk                    ((uint32_t)0x00000800)
#define I2C_IC_RAW_INTR_STAT_GEN_CALL                        I2C_IC_RAW_INTR_STAT_GEN_CALL_Msk
#define I2C_IC_RAW_INTR_STAT_GEN_CALL_INACTIVE               ((uint32_t)0x00000000)
#define I2C_IC_RAW_INTR_STAT_GEN_CALL_ACTIVE                 ((uint32_t)0x00000800)
#define I2C_IC_RAW_INTR_STAT_START_DET_Pos                   10
#define I2C_IC_RAW_INTR_STAT_START_DET_Msk                   ((uint32_t)0x00000400)
#define I2C_IC_RAW_INTR_STAT_START_DET                       I2C_IC_RAW_INTR_STAT_START_DET_Msk
#define I2C_IC_RAW_INTR_STAT_START_DET_INACTIVE              ((uint32_t)0x00000000)
#define I2C_IC_RAW_INTR_STAT_START_DET_ACTIVE                ((uint32_t)0x00000400)
#define I2C_IC_RAW_INTR_STAT_STOP_DET_Pos                    9
#define I2C_IC_RAW_INTR_STAT_STOP_DET_Msk                    ((uint32_t)0x00000200)
#define I2C_IC_RAW_INTR_STAT_STOP_DET                        I2C_IC_RAW_INTR_STAT_STOP_DET_Msk
#define I2C_IC_RAW_INTR_STAT_STOP_DET_INACTIVE               ((uint32_t)0x00000000)
#define I2C_IC_RAW_INTR_STAT_STOP_DET_ACTIVE                 ((uint32_t)0x00000200)
#define I2C_IC_RAW_INTR_STAT_ACTIVITY_Pos                    8
#define I2C_IC_RAW_INTR_STAT_ACTIVITY_Msk                    ((uint32_t)0x00000100)
#define I2C_IC_RAW_INTR_STAT_ACTIVITY                        I2C_IC_RAW_INTR_STAT_ACTIVITY_Msk
#define I2C_IC_RAW_INTR_STAT_ACTIVITY_INACTIVE               ((uint32_t)0x00000000)
#define I2C_IC_RAW_INTR_STAT_ACTIVITY_ACTIVE                 ((uint32_t)0x00000100)
#define I2C_IC_RAW_INTR_STAT_RX_DONE_Pos                     7
#define I2C_IC_RAW_INTR_STAT_RX_DONE_Msk                     ((uint32_t)0x00000080)
#define I2C_IC_RAW_INTR_STAT_RX_DONE                         I2C_IC_RAW_INTR_STAT_RX_DONE_Msk
#define I2C_IC_RAW_INTR_STAT_RX_DONE_INACTIVE                ((uint32_t)0x00000000)
#define I2C_IC_RAW_INTR_STAT_RX_DONE_ACTIVE                  ((uint32_t)0x00000080)
#define I2C_IC_RAW_INTR_STAT_TX_ABRT_Pos                     6
#define I2C_IC_RAW_INTR_STAT_TX_ABRT_Msk                     ((uint32_t)0x00000040)
#define I2C_IC_RAW_INTR_STAT_TX_ABRT                         I2C_IC_RAW_INTR_STAT_TX_ABRT_Msk
#define I2C_IC_RAW_INTR_STAT_TX_ABRT_INACTIVE                ((uint32_t)0x00000000)
#define I2C_IC_RAW_INTR_STAT_TX_ABRT_ACTIVE                  ((uint32_t)0x00000040)
#define I2C_IC_RAW_INTR_STAT_RD_REQ_Pos                      5
#define I2C_IC_RAW_INTR_STAT_RD_REQ_Msk                      ((uint32_t)0x00000020)
#define I2C_IC_RAW_INTR_STAT_RD_REQ                          I2C_IC_RAW_INTR_STAT_RD_REQ_Msk
#define I2C_IC_RAW_INTR_STAT_RD_REQ_INACTIVE                 ((uint32_t)0x00000000)
#define I2C_IC_RAW_INTR_STAT_RD_REQ_ACTIVE                   ((uint32_t)0x00000020)
#define I2C_IC_RAW_INTR_STAT_TX_EMPTY_Pos                    4
#define I2C_IC_RAW_INTR_STAT_TX_EMPTY_Msk                    ((uint32_t)0x00000010)
#define I2C_IC_RAW_INTR_STAT_TX_EMPTY                        I2C_IC_RAW_INTR_STAT_TX_EMPTY_Msk
#define I2C_IC_RAW_INTR_STAT_TX_EMPTY_INACTIVE               ((uint32_t)0x00000000)
#define I2C_IC_RAW_INTR_STAT_TX_EMPTY_ACTIVE                 ((uint32_t)0x00000010)
#define I2C_IC_RAW_INTR_STAT_TX_OVER_Pos                     3
#define I2C_IC_RAW_INTR_STAT_TX_OVER_Msk                     ((uint32_t)0x00000008)
#define I2C_IC_RAW_INTR_STAT_TX_OVER                         I2C_IC_RAW_INTR_STAT_TX_OVER_Msk
#define I2C_IC_RAW_INTR_STAT_TX_OVER_INACTIVE                ((uint32_t)0x00000000)
#define I2C_IC_RAW_INTR_STAT_TX_OVER_ACTIVE                  ((uint32_t)0x00000008)
#define I2C_IC_RAW_INTR_STAT_RX_FULL_Pos                     2
#define I2C_IC_RAW_INTR_STAT_RX_FULL_Msk                     ((uint32_t)0x00000004)
#define I2C_IC_RAW_INTR_STAT_RX_FULL                         I2C_IC_RAW_INTR_STAT_RX_FULL_Msk
#define I2C_IC_RAW_INTR_STAT_RX_FULL_INACTIVE                ((uint32_t)0x00000000)
#define I2C_IC_RAW_INTR_STAT_RX_FULL_ACTIVE                  ((uint32_t)0x00000004)
#define I2C_IC_RAW_INTR_STAT_RX_OVER_Pos                     1
#define I2C_IC_RAW_INTR_STAT_RX_OVER_Msk                     ((uint32_t)0x00000002)
#define I2C_IC_RAW_INTR_STAT_RX_OVER                         I2C_IC_RAW_INTR_STAT_RX_OVER_Msk
#define I2C_IC_RAW_INTR_STAT_RX_OVER_INACTIVE                ((uint32_t)0x00000000)
#define I2C_IC_RAW_INTR_STAT_RX_OVER_ACTIVE                  ((uint32_t)0x00000002)
#define I2C_IC_RAW_INTR_STAT_RX_UNDER_Pos                    0
#define I2C_IC_RAW_INTR_STAT_RX_UNDER_Msk                    ((uint32_t)0x00000001)
#define I2C_IC_RAW_INTR_STAT_RX_UNDER                        I2C_IC_RAW_INTR_STAT_RX_UNDER_Msk
#define I2C_IC_RAW_INTR_STAT_RX_UNDER_INACTIVE               ((uint32_t)0x00000000)
#define I2C_IC_RAW_INTR_STAT_RX_UNDER_ACTIVE                 ((uint32_t)0x00000001)
// I2C->IC_RX_TL
#define I2C_IC_RX_TL_RX_TL_Pos                               0
#define I2C_IC_RX_TL_RX_TL_Msk                               ((uint32_t)0x000000FF)
#define I2C_IC_RX_TL_RX_TL                                   I2C_IC_RX_TL_RX_TL_Msk
// I2C->IC_TX_TL
#define I2C_IC_TX_TL_TX_TL_Pos                               0
#define I2C_IC_TX_TL_TX_TL_Msk                               ((uint32_t)0x000000FF)
#define I2C_IC_TX_TL_TX_TL                                   I2C_IC_TX_TL_TX_TL_Msk
// I2C->IC_CLR_INTR
#define I2C_IC_CLR_INTR_CLR_INTR_Pos                         0
#define I2C_IC_CLR_INTR_CLR_INTR_Msk                         ((uint32_t)0x00000001)
#define I2C_IC_CLR_INTR_CLR_INTR                             I2C_IC_CLR_INTR_CLR_INTR_Msk
// I2C->IC_CLR_RX_UNDER
#define I2C_IC_CLR_RX_UNDER_CLR_RX_UNDER_Pos                 0
#define I2C_IC_CLR_RX_UNDER_CLR_RX_UNDER_Msk                 ((uint32_t)0x00000001)
#define I2C_IC_CLR_RX_UNDER_CLR_RX_UNDER                     I2C_IC_CLR_RX_UNDER_CLR_RX_UNDER_Msk
// I2C->IC_CLR_RX_OVER
#define I2C_IC_CLR_RX_OVER_CLR_RX_OVER_Pos                   0
#define I2C_IC_CLR_RX_OVER_CLR_RX_OVER_Msk                   ((uint32_t)0x00000001)
#define I2C_IC_CLR_RX_OVER_CLR_RX_OVER                       I2C_IC_CLR_RX_OVER_CLR_RX_OVER_Msk
// I2C->IC_CLR_TX_OVER
#define I2C_IC_CLR_TX_OVER_CLR_TX_OVER_Pos                   0
#define I2C_IC_CLR_TX_OVER_CLR_TX_OVER_Msk                   ((uint32_t)0x00000001)
#define I2C_IC_CLR_TX_OVER_CLR_TX_OVER                       I2C_IC_CLR_TX_OVER_CLR_TX_OVER_Msk
// I2C->IC_CLR_RD_REQ
#define I2C_IC_CLR_RD_REQ_CLR_RD_REQ_Pos                     0
#define I2C_IC_CLR_RD_REQ_CLR_RD_REQ_Msk                     ((uint32_t)0x00000001)
#define I2C_IC_CLR_RD_REQ_CLR_RD_REQ                         I2C_IC_CLR_RD_REQ_CLR_RD_REQ_Msk
// I2C->IC_CLR_TX_ABRT
#define I2C_IC_CLR_TX_ABRT_CLR_TX_ABRT_Pos                   0
#define I2C_IC_CLR_TX_ABRT_CLR_TX_ABRT_Msk                   ((uint32_t)0x00000001)
#define I2C_IC_CLR_TX_ABRT_CLR_TX_ABRT                       I2C_IC_CLR_TX_ABRT_CLR_TX_ABRT_Msk
// I2C->IC_CLR_RX_DONE
#define I2C_IC_CLR_RX_DONE_CLR_RX_DONE_Pos                   0
#define I2C_IC_CLR_RX_DONE_CLR_RX_DONE_Msk                   ((uint32_t)0x00000001)
#define I2C_IC_CLR_RX_DONE_CLR_RX_DONE                       I2C_IC_CLR_RX_DONE_CLR_RX_DONE_Msk
// I2C->IC_CLR_ACTIVITY
#define I2C_IC_CLR_ACTIVITY_CLR_ACTIVITY_Pos                 0
#define I2C_IC_CLR_ACTIVITY_CLR_ACTIVITY_Msk                 ((uint32_t)0x00000001)
#define I2C_IC_CLR_ACTIVITY_CLR_ACTIVITY                     I2C_IC_CLR_ACTIVITY_CLR_ACTIVITY_Msk
// I2C->IC_CLR_STOP_DET
#define I2C_IC_CLR_STOP_DET_CLR_STOP_DET_Pos                 0
#define I2C_IC_CLR_STOP_DET_CLR_STOP_DET_Msk                 ((uint32_t)0x00000001)
#define I2C_IC_CLR_STOP_DET_CLR_STOP_DET                     I2C_IC_CLR_STOP_DET_CLR_STOP_DET_Msk
// I2C->IC_CLR_START_DET
#define I2C_IC_CLR_START_DET_CLR_START_DET_Pos               0
#define I2C_IC_CLR_START_DET_CLR_START_DET_Msk               ((uint32_t)0x00000001)
#define I2C_IC_CLR_START_DET_CLR_START_DET                   I2C_IC_CLR_START_DET_CLR_START_DET_Msk
// I2C->IC_CLR_GEN_CALL
#define I2C_IC_CLR_GEN_CALL_CLR_GEN_CALL_Pos                 0
#define I2C_IC_CLR_GEN_CALL_CLR_GEN_CALL_Msk                 ((uint32_t)0x00000001)
#define I2C_IC_CLR_GEN_CALL_CLR_GEN_CALL                     I2C_IC_CLR_GEN_CALL_CLR_GEN_CALL_Msk
// I2C->IC_ENABLE
#define I2C_IC_ENABLE_TX_CMD_BLOCK_Pos                       2
#define I2C_IC_ENABLE_TX_CMD_BLOCK_Msk                       ((uint32_t)0x00000004)
#define I2C_IC_ENABLE_TX_CMD_BLOCK                           I2C_IC_ENABLE_TX_CMD_BLOCK_Msk
#define I2C_IC_ENABLE_TX_CMD_BLOCK_NOT_BLOCKED               ((uint32_t)0x00000000)
#define I2C_IC_ENABLE_TX_CMD_BLOCK_BLOCKED                   ((uint32_t)0x00000004)
#define I2C_IC_ENABLE_ABORT_Pos                              1
#define I2C_IC_ENABLE_ABORT_Msk                              ((uint32_t)0x00000002)
#define I2C_IC_ENABLE_ABORT                                  I2C_IC_ENABLE_ABORT_Msk
#define I2C_IC_ENABLE_ABORT_DISABLE                          ((uint32_t)0x00000000)
#define I2C_IC_ENABLE_ABORT_ENABLED                          ((uint32_t)0x00000002)
#define I2C_IC_ENABLE_ENABLE_Pos                             0
#define I2C_IC_ENABLE_ENABLE_Msk                             ((uint32_t)0x00000001)
#define I2C_IC_ENABLE_ENABLE                                 I2C_IC_ENABLE_ENABLE_Msk
#define I2C_IC_ENABLE_ENABLE_DISABLED                        ((uint32_t)0x00000000)
#define I2C_IC_ENABLE_ENABLE_ENABLED                         ((uint32_t)0x00000001)
// I2C->IC_STATUS
#define I2C_IC_STATUS_SLV_ACTIVITY_Pos                       6
#define I2C_IC_STATUS_SLV_ACTIVITY_Msk                       ((uint32_t)0x00000040)
#define I2C_IC_STATUS_SLV_ACTIVITY                           I2C_IC_STATUS_SLV_ACTIVITY_Msk
#define I2C_IC_STATUS_SLV_ACTIVITY_IDLE                      ((uint32_t)0x00000000)
#define I2C_IC_STATUS_SLV_ACTIVITY_ACTIVE                    ((uint32_t)0x00000040)
#define I2C_IC_STATUS_MST_ACTIVITY_Pos                       5
#define I2C_IC_STATUS_MST_ACTIVITY_Msk                       ((uint32_t)0x00000020)
#define I2C_IC_STATUS_MST_ACTIVITY                           I2C_IC_STATUS_MST_ACTIVITY_Msk
#define I2C_IC_STATUS_MST_ACTIVITY_IDLE                      ((uint32_t)0x00000000)
#define I2C_IC_STATUS_MST_ACTIVITY_ACTIVE                    ((uint32_t)0x00000020)
#define I2C_IC_STATUS_RFF_Pos                                4
#define I2C_IC_STATUS_RFF_Msk                                ((uint32_t)0x00000010)
#define I2C_IC_STATUS_RFF                                    I2C_IC_STATUS_RFF_Msk
#define I2C_IC_STATUS_RFF_NOT_FULL                           ((uint32_t)0x00000000)
#define I2C_IC_STATUS_RFF_FULL                               ((uint32_t)0x00000010)
#define I2C_IC_STATUS_RFNE_Pos                               3
#define I2C_IC_STATUS_RFNE_Msk                               ((uint32_t)0x00000008)
#define I2C_IC_STATUS_RFNE                                   I2C_IC_STATUS_RFNE_Msk
#define I2C_IC_STATUS_RFNE_EMPTY                             ((uint32_t)0x00000000)
#define I2C_IC_STATUS_RFNE_NOT_EMPTY                         ((uint32_t)0x00000008)
#define I2C_IC_STATUS_TFE_Pos                                2
#define I2C_IC_STATUS_TFE_Msk                                ((uint32_t)0x00000004)
#define I2C_IC_STATUS_TFE                                    I2C_IC_STATUS_TFE_Msk
#define I2C_IC_STATUS_TFE_NON_EMPTY                          ((uint32_t)0x00000000)
#define I2C_IC_STATUS_TFE_EMPTY                              ((uint32_t)0x00000004)
#define I2C_IC_STATUS_TFNF_Pos                               1
#define I2C_IC_STATUS_TFNF_Msk                               ((uint32_t)0x00000002)
#define I2C_IC_STATUS_TFNF                                   I2C_IC_STATUS_TFNF_Msk
#define I2C_IC_STATUS_TFNF_FULL                              ((uint32_t)0x00000000)
#define I2C_IC_STATUS_TFNF_NOT_FULL                          ((uint32_t)0x00000002)
#define I2C_IC_STATUS_ACTIVITY_Pos                           0
#define I2C_IC_STATUS_ACTIVITY_Msk                           ((uint32_t)0x00000001)
#define I2C_IC_STATUS_ACTIVITY                               I2C_IC_STATUS_ACTIVITY_Msk
#define I2C_IC_STATUS_ACTIVITY_INACTIVE                      ((uint32_t)0x00000000)
#define I2C_IC_STATUS_ACTIVITY_ACTIVE                        ((uint32_t)0x00000001)
// I2C->IC_TXFLR
#define I2C_IC_TXFLR_TXFLR_Pos                               0
#define I2C_IC_TXFLR_TXFLR_Msk                               ((uint32_t)0x0000001F)
#define I2C_IC_TXFLR_TXFLR                                   I2C_IC_TXFLR_TXFLR_Msk
// I2C->IC_RXFLR
#define I2C_IC_RXFLR_RXFLR_Pos                               0
#define I2C_IC_RXFLR_RXFLR_Msk                               ((uint32_t)0x0000001F)
#define I2C_IC_RXFLR_RXFLR                                   I2C_IC_RXFLR_RXFLR_Msk
// I2C->IC_SDA_HOLD
#define I2C_IC_SDA_HOLD_IC_SDA_RX_HOLD_Pos                   16
#define I2C_IC_SDA_HOLD_IC_SDA_RX_HOLD_Msk                   ((uint32_t)0x00FF0000)
#define I2C_IC_SDA_HOLD_IC_SDA_RX_HOLD                       I2C_IC_SDA_HOLD_IC_SDA_RX_HOLD_Msk
#define I2C_IC_SDA_HOLD_IC_SDA_TX_HOLD_Pos                   0
#define I2C_IC_SDA_HOLD_IC_SDA_TX_HOLD_Msk                   ((uint32_t)0x0000FFFF)
#define I2C_IC_SDA_HOLD_IC_SDA_TX_HOLD                       I2C_IC_SDA_HOLD_IC_SDA_TX_HOLD_Msk
// I2C->IC_TX_ABRT_SOURCE
#define I2C_IC_TX_ABRT_SOURCE_TX_FLUSH_CNT_Pos               23
#define I2C_IC_TX_ABRT_SOURCE_TX_FLUSH_CNT_Msk               ((uint32_t)0xFF800000)
#define I2C_IC_TX_ABRT_SOURCE_TX_FLUSH_CNT                   I2C_IC_TX_ABRT_SOURCE_TX_FLUSH_CNT_Msk
#define I2C_IC_TX_ABRT_SOURCE_ABRT_USER_ABRT_Pos             16
#define I2C_IC_TX_ABRT_SOURCE_ABRT_USER_ABRT_Msk             ((uint32_t)0x00010000)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_USER_ABRT                 I2C_IC_TX_ABRT_SOURCE_ABRT_USER_ABRT_Msk
#define I2C_IC_TX_ABRT_SOURCE_ABRT_USER_ABRT_ABRT_USER_ABRT_VOID ((uint32_t)0x00000000)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_USER_ABRT_ABRT_USER_ABRT_GENERATED ((uint32_t)0x00010000)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_SLVRD_INTX_Pos            15
#define I2C_IC_TX_ABRT_SOURCE_ABRT_SLVRD_INTX_Msk            ((uint32_t)0x00008000)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_SLVRD_INTX                I2C_IC_TX_ABRT_SOURCE_ABRT_SLVRD_INTX_Msk
#define I2C_IC_TX_ABRT_SOURCE_ABRT_SLVRD_INTX_ABRT_SLVRD_INTX_VOID ((uint32_t)0x00000000)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_SLVRD_INTX_ABRT_SLVRD_INTX_GENERATED ((uint32_t)0x00008000)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_SLV_ARBLOST_Pos           14
#define I2C_IC_TX_ABRT_SOURCE_ABRT_SLV_ARBLOST_Msk           ((uint32_t)0x00004000)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_SLV_ARBLOST               I2C_IC_TX_ABRT_SOURCE_ABRT_SLV_ARBLOST_Msk
#define I2C_IC_TX_ABRT_SOURCE_ABRT_SLV_ARBLOST_ABRT_SLV_ARBLOST_VOID ((uint32_t)0x00000000)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_SLV_ARBLOST_ABRT_SLV_ARBLOST_GENERATED ((uint32_t)0x00004000)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_SLVFLUSH_TXFIFO_Pos       13
#define I2C_IC_TX_ABRT_SOURCE_ABRT_SLVFLUSH_TXFIFO_Msk       ((uint32_t)0x00002000)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_SLVFLUSH_TXFIFO           I2C_IC_TX_ABRT_SOURCE_ABRT_SLVFLUSH_TXFIFO_Msk
#define I2C_IC_TX_ABRT_SOURCE_ABRT_SLVFLUSH_TXFIFO_ABRT_SLVFLUSH_TXFIFO_VOID ((uint32_t)0x00000000)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_SLVFLUSH_TXFIFO_ABRT_SLVFLUSH_TXFIFO_GENERATED ((uint32_t)0x00002000)
#define I2C_IC_TX_ABRT_SOURCE_ARB_LOST_Pos                   12
#define I2C_IC_TX_ABRT_SOURCE_ARB_LOST_Msk                   ((uint32_t)0x00001000)
#define I2C_IC_TX_ABRT_SOURCE_ARB_LOST                       I2C_IC_TX_ABRT_SOURCE_ARB_LOST_Msk
#define I2C_IC_TX_ABRT_SOURCE_ARB_LOST_ABRT_LOST_VOID        ((uint32_t)0x00000000)
#define I2C_IC_TX_ABRT_SOURCE_ARB_LOST_ABRT_LOST_GENERATED   ((uint32_t)0x00001000)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_MASTER_DIS_Pos            11
#define I2C_IC_TX_ABRT_SOURCE_ABRT_MASTER_DIS_Msk            ((uint32_t)0x00000800)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_MASTER_DIS                I2C_IC_TX_ABRT_SOURCE_ABRT_MASTER_DIS_Msk
#define I2C_IC_TX_ABRT_SOURCE_ABRT_MASTER_DIS_ABRT_MASTER_DIS_VOID ((uint32_t)0x00000000)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_MASTER_DIS_ABRT_MASTER_DIS_GENERATED ((uint32_t)0x00000800)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_10B_RD_NORSTRT_Pos        10
#define I2C_IC_TX_ABRT_SOURCE_ABRT_10B_RD_NORSTRT_Msk        ((uint32_t)0x00000400)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_10B_RD_NORSTRT            I2C_IC_TX_ABRT_SOURCE_ABRT_10B_RD_NORSTRT_Msk
#define I2C_IC_TX_ABRT_SOURCE_ABRT_10B_RD_NORSTRT_ABRT_10B_RD_VOID ((uint32_t)0x00000000)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_10B_RD_NORSTRT_ABRT_10B_RD_GENERATED ((uint32_t)0x00000400)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_SBYTE_NORSTRT_Pos         9
#define I2C_IC_TX_ABRT_SOURCE_ABRT_SBYTE_NORSTRT_Msk         ((uint32_t)0x00000200)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_SBYTE_NORSTRT             I2C_IC_TX_ABRT_SOURCE_ABRT_SBYTE_NORSTRT_Msk
#define I2C_IC_TX_ABRT_SOURCE_ABRT_SBYTE_NORSTRT_ABRT_SBYTE_NORSTRT_VOID ((uint32_t)0x00000000)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_SBYTE_NORSTRT_ABRT_SBYTE_NORSTRT_GENERATED ((uint32_t)0x00000200)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_HS_NORSTRT_Pos            8
#define I2C_IC_TX_ABRT_SOURCE_ABRT_HS_NORSTRT_Msk            ((uint32_t)0x00000100)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_HS_NORSTRT                I2C_IC_TX_ABRT_SOURCE_ABRT_HS_NORSTRT_Msk
#define I2C_IC_TX_ABRT_SOURCE_ABRT_HS_NORSTRT_ABRT_HS_NORSTRT_VOID ((uint32_t)0x00000000)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_HS_NORSTRT_ABRT_HS_NORSTRT_GENERATED ((uint32_t)0x00000100)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_SBYTE_ACKDET_Pos          7
#define I2C_IC_TX_ABRT_SOURCE_ABRT_SBYTE_ACKDET_Msk          ((uint32_t)0x00000080)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_SBYTE_ACKDET              I2C_IC_TX_ABRT_SOURCE_ABRT_SBYTE_ACKDET_Msk
#define I2C_IC_TX_ABRT_SOURCE_ABRT_SBYTE_ACKDET_ABRT_SBYTE_ACKDET_VOID ((uint32_t)0x00000000)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_SBYTE_ACKDET_ABRT_SBYTE_ACKDET_GENERATED ((uint32_t)0x00000080)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_HS_ACKDET_Pos             6
#define I2C_IC_TX_ABRT_SOURCE_ABRT_HS_ACKDET_Msk             ((uint32_t)0x00000040)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_HS_ACKDET                 I2C_IC_TX_ABRT_SOURCE_ABRT_HS_ACKDET_Msk
#define I2C_IC_TX_ABRT_SOURCE_ABRT_HS_ACKDET_ABRT_HS_ACK_VOID ((uint32_t)0x00000000)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_HS_ACKDET_ABRT_HS_ACK_GENERATED ((uint32_t)0x00000040)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_GCALL_READ_Pos            5
#define I2C_IC_TX_ABRT_SOURCE_ABRT_GCALL_READ_Msk            ((uint32_t)0x00000020)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_GCALL_READ                I2C_IC_TX_ABRT_SOURCE_ABRT_GCALL_READ_Msk
#define I2C_IC_TX_ABRT_SOURCE_ABRT_GCALL_READ_ABRT_GCALL_READ_VOID ((uint32_t)0x00000000)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_GCALL_READ_ABRT_GCALL_READ_GENERATED ((uint32_t)0x00000020)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_GCALL_NOACK_Pos           4
#define I2C_IC_TX_ABRT_SOURCE_ABRT_GCALL_NOACK_Msk           ((uint32_t)0x00000010)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_GCALL_NOACK               I2C_IC_TX_ABRT_SOURCE_ABRT_GCALL_NOACK_Msk
#define I2C_IC_TX_ABRT_SOURCE_ABRT_GCALL_NOACK_ABRT_GCALL_NOACK_VOID ((uint32_t)0x00000000)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_GCALL_NOACK_ABRT_GCALL_NOACK_GENERATED ((uint32_t)0x00000010)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_TXDATA_NOACK_Pos          3
#define I2C_IC_TX_ABRT_SOURCE_ABRT_TXDATA_NOACK_Msk          ((uint32_t)0x00000008)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_TXDATA_NOACK              I2C_IC_TX_ABRT_SOURCE_ABRT_TXDATA_NOACK_Msk
#define I2C_IC_TX_ABRT_SOURCE_ABRT_TXDATA_NOACK_ABRT_TXDATA_NOACK_VOID ((uint32_t)0x00000000)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_TXDATA_NOACK_ABRT_TXDATA_NOACK_GENERATED ((uint32_t)0x00000008)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_10ADDR2_NOACK_Pos         2
#define I2C_IC_TX_ABRT_SOURCE_ABRT_10ADDR2_NOACK_Msk         ((uint32_t)0x00000004)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_10ADDR2_NOACK             I2C_IC_TX_ABRT_SOURCE_ABRT_10ADDR2_NOACK_Msk
#define I2C_IC_TX_ABRT_SOURCE_ABRT_10ADDR2_NOACK_INACTIVE    ((uint32_t)0x00000000)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_10ADDR2_NOACK_ACTIVE      ((uint32_t)0x00000004)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_10ADDR1_NOACK_Pos         1
#define I2C_IC_TX_ABRT_SOURCE_ABRT_10ADDR1_NOACK_Msk         ((uint32_t)0x00000002)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_10ADDR1_NOACK             I2C_IC_TX_ABRT_SOURCE_ABRT_10ADDR1_NOACK_Msk
#define I2C_IC_TX_ABRT_SOURCE_ABRT_10ADDR1_NOACK_INACTIVE    ((uint32_t)0x00000000)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_10ADDR1_NOACK_ACTIVE      ((uint32_t)0x00000002)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_7B_ADDR_NOACK_Pos         0
#define I2C_IC_TX_ABRT_SOURCE_ABRT_7B_ADDR_NOACK_Msk         ((uint32_t)0x00000001)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_7B_ADDR_NOACK             I2C_IC_TX_ABRT_SOURCE_ABRT_7B_ADDR_NOACK_Msk
#define I2C_IC_TX_ABRT_SOURCE_ABRT_7B_ADDR_NOACK_INACTIVE    ((uint32_t)0x00000000)
#define I2C_IC_TX_ABRT_SOURCE_ABRT_7B_ADDR_NOACK_ACTIVE      ((uint32_t)0x00000001)
// I2C->IC_SLV_DATA_NACK_ONLY
#define I2C_IC_SLV_DATA_NACK_ONLY_NACK_Pos                   0
#define I2C_IC_SLV_DATA_NACK_ONLY_NACK_Msk                   ((uint32_t)0x00000001)
#define I2C_IC_SLV_DATA_NACK_ONLY_NACK                       I2C_IC_SLV_DATA_NACK_ONLY_NACK_Msk
#define I2C_IC_SLV_DATA_NACK_ONLY_NACK_DISABLED              ((uint32_t)0x00000000)
#define I2C_IC_SLV_DATA_NACK_ONLY_NACK_ENABLED               ((uint32_t)0x00000001)
// I2C->IC_DMA_CR
#define I2C_IC_DMA_CR_TDMAE_Pos                              1
#define I2C_IC_DMA_CR_TDMAE_Msk                              ((uint32_t)0x00000002)
#define I2C_IC_DMA_CR_TDMAE                                  I2C_IC_DMA_CR_TDMAE_Msk
#define I2C_IC_DMA_CR_TDMAE_DISABLED                         ((uint32_t)0x00000000)
#define I2C_IC_DMA_CR_TDMAE_ENABLED                          ((uint32_t)0x00000002)
#define I2C_IC_DMA_CR_RDMAE_Pos                              0
#define I2C_IC_DMA_CR_RDMAE_Msk                              ((uint32_t)0x00000001)
#define I2C_IC_DMA_CR_RDMAE                                  I2C_IC_DMA_CR_RDMAE_Msk
#define I2C_IC_DMA_CR_RDMAE_DISABLED                         ((uint32_t)0x00000000)
#define I2C_IC_DMA_CR_RDMAE_ENABLED                          ((uint32_t)0x00000001)
// I2C->IC_DMA_TDLR
#define I2C_IC_DMA_TDLR_DMATDL_Pos                           0
#define I2C_IC_DMA_TDLR_DMATDL_Msk                           ((uint32_t)0x0000000F)
#define I2C_IC_DMA_TDLR_DMATDL                               I2C_IC_DMA_TDLR_DMATDL_Msk
// I2C->IC_DMA_RDLR
#define I2C_IC_DMA_RDLR_DMARDL_Pos                           0
#define I2C_IC_DMA_RDLR_DMARDL_Msk                           ((uint32_t)0x0000000F)
#define I2C_IC_DMA_RDLR_DMARDL                               I2C_IC_DMA_RDLR_DMARDL_Msk
// I2C->IC_SDA_SETUP
#define I2C_IC_SDA_SETUP_SDA_SETUP_Pos                       0
#define I2C_IC_SDA_SETUP_SDA_SETUP_Msk                       ((uint32_t)0x000000FF)
#define I2C_IC_SDA_SETUP_SDA_SETUP                           I2C_IC_SDA_SETUP_SDA_SETUP_Msk
// I2C->IC_ACK_GENERAL_CALL
#define I2C_IC_ACK_GENERAL_CALL_ACK_GEN_CALL_Pos             0
#define I2C_IC_ACK_GENERAL_CALL_ACK_GEN_CALL_Msk             ((uint32_t)0x00000001)
#define I2C_IC_ACK_GENERAL_CALL_ACK_GEN_CALL                 I2C_IC_ACK_GENERAL_CALL_ACK_GEN_CALL_Msk
#define I2C_IC_ACK_GENERAL_CALL_ACK_GEN_CALL_DISABLED        ((uint32_t)0x00000000)
#define I2C_IC_ACK_GENERAL_CALL_ACK_GEN_CALL_ENABLED         ((uint32_t)0x00000001)
// I2C->IC_ENABLE_STATUS
#define I2C_IC_ENABLE_STATUS_SLV_RX_DATA_LOST_Pos            2
#define I2C_IC_ENABLE_STATUS_SLV_RX_DATA_LOST_Msk            ((uint32_t)0x00000004)
#define I2C_IC_ENABLE_STATUS_SLV_RX_DATA_LOST                I2C_IC_ENABLE_STATUS_SLV_RX_DATA_LOST_Msk
#define I2C_IC_ENABLE_STATUS_SLV_RX_DATA_LOST_INACTIVE       ((uint32_t)0x00000000)
#define I2C_IC_ENABLE_STATUS_SLV_RX_DATA_LOST_ACTIVE         ((uint32_t)0x00000004)
#define I2C_IC_ENABLE_STATUS_SLV_DISABLED_WHILE_BUSY_Pos     1
#define I2C_IC_ENABLE_STATUS_SLV_DISABLED_WHILE_BUSY_Msk     ((uint32_t)0x00000002)
#define I2C_IC_ENABLE_STATUS_SLV_DISABLED_WHILE_BUSY         I2C_IC_ENABLE_STATUS_SLV_DISABLED_WHILE_BUSY_Msk
#define I2C_IC_ENABLE_STATUS_SLV_DISABLED_WHILE_BUSY_INACTIVE ((uint32_t)0x00000000)
#define I2C_IC_ENABLE_STATUS_SLV_DISABLED_WHILE_BUSY_ACTIVE  ((uint32_t)0x00000002)
#define I2C_IC_ENABLE_STATUS_IC_EN_Pos                       0
#define I2C_IC_ENABLE_STATUS_IC_EN_Msk                       ((uint32_t)0x00000001)
#define I2C_IC_ENABLE_STATUS_IC_EN                           I2C_IC_ENABLE_STATUS_IC_EN_Msk
#define I2C_IC_ENABLE_STATUS_IC_EN_DISABLED                  ((uint32_t)0x00000000)
#define I2C_IC_ENABLE_STATUS_IC_EN_ENABLED                   ((uint32_t)0x00000001)
// I2C->IC_FS_SPKLEN
#define I2C_IC_FS_SPKLEN_IC_FS_SPKLEN_Pos                    0
#define I2C_IC_FS_SPKLEN_IC_FS_SPKLEN_Msk                    ((uint32_t)0x000000FF)
#define I2C_IC_FS_SPKLEN_IC_FS_SPKLEN                        I2C_IC_FS_SPKLEN_IC_FS_SPKLEN_Msk
// I2C->IC_CLR_RESTART_DET
#define I2C_IC_CLR_RESTART_DET_CLR_RESTART_DET_Pos           0
#define I2C_IC_CLR_RESTART_DET_CLR_RESTART_DET_Msk           ((uint32_t)0x00000001)
#define I2C_IC_CLR_RESTART_DET_CLR_RESTART_DET               I2C_IC_CLR_RESTART_DET_CLR_RESTART_DET_Msk
// I2C->IC_COMP_PARAM_1
#define I2C_IC_COMP_PARAM_1_TX_BUFFER_DEPTH_Pos              16
#define I2C_IC_COMP_PARAM_1_TX_BUFFER_DEPTH_Msk              ((uint32_t)0x00FF0000)
#define I2C_IC_COMP_PARAM_1_TX_BUFFER_DEPTH                  I2C_IC_COMP_PARAM_1_TX_BUFFER_DEPTH_Msk
#define I2C_IC_COMP_PARAM_1_RX_BUFFER_DEPTH_Pos              8
#define I2C_IC_COMP_PARAM_1_RX_BUFFER_DEPTH_Msk              ((uint32_t)0x0000FF00)
#define I2C_IC_COMP_PARAM_1_RX_BUFFER_DEPTH                  I2C_IC_COMP_PARAM_1_RX_BUFFER_DEPTH_Msk
#define I2C_IC_COMP_PARAM_1_ADD_ENCODED_PARAMS_Pos           7
#define I2C_IC_COMP_PARAM_1_ADD_ENCODED_PARAMS_Msk           ((uint32_t)0x00000080)
#define I2C_IC_COMP_PARAM_1_ADD_ENCODED_PARAMS               I2C_IC_COMP_PARAM_1_ADD_ENCODED_PARAMS_Msk
#define I2C_IC_COMP_PARAM_1_HAS_DMA_Pos                      6
#define I2C_IC_COMP_PARAM_1_HAS_DMA_Msk                      ((uint32_t)0x00000040)
#define I2C_IC_COMP_PARAM_1_HAS_DMA                          I2C_IC_COMP_PARAM_1_HAS_DMA_Msk
#define I2C_IC_COMP_PARAM_1_INTR_IO_Pos                      5
#define I2C_IC_COMP_PARAM_1_INTR_IO_Msk                      ((uint32_t)0x00000020)
#define I2C_IC_COMP_PARAM_1_INTR_IO                          I2C_IC_COMP_PARAM_1_INTR_IO_Msk
#define I2C_IC_COMP_PARAM_1_HC_COUNT_VALUES_Pos              4
#define I2C_IC_COMP_PARAM_1_HC_COUNT_VALUES_Msk              ((uint32_t)0x00000010)
#define I2C_IC_COMP_PARAM_1_HC_COUNT_VALUES                  I2C_IC_COMP_PARAM_1_HC_COUNT_VALUES_Msk
#define I2C_IC_COMP_PARAM_1_MAX_SPEED_MODE_Pos               2
#define I2C_IC_COMP_PARAM_1_MAX_SPEED_MODE_Msk               ((uint32_t)0x0000000C)
#define I2C_IC_COMP_PARAM_1_MAX_SPEED_MODE                   I2C_IC_COMP_PARAM_1_MAX_SPEED_MODE_Msk
#define I2C_IC_COMP_PARAM_1_APB_DATA_WIDTH_Pos               0
#define I2C_IC_COMP_PARAM_1_APB_DATA_WIDTH_Msk               ((uint32_t)0x00000003)
#define I2C_IC_COMP_PARAM_1_APB_DATA_WIDTH                   I2C_IC_COMP_PARAM_1_APB_DATA_WIDTH_Msk
// I2C->IC_COMP_VERSION
#define I2C_IC_COMP_VERSION_IC_COMP_VERSION_Pos              0
#define I2C_IC_COMP_VERSION_IC_COMP_VERSION_Msk              ((uint32_t)0xFFFFFFFF)
#define I2C_IC_COMP_VERSION_IC_COMP_VERSION                  I2C_IC_COMP_VERSION_IC_COMP_VERSION_Msk
// I2C->IC_COMP_TYPE
#define I2C_IC_COMP_TYPE_IC_COMP_TYPE_Pos                    0
#define I2C_IC_COMP_TYPE_IC_COMP_TYPE_Msk                    ((uint32_t)0xFFFFFFFF)
#define I2C_IC_COMP_TYPE_IC_COMP_TYPE                        I2C_IC_COMP_TYPE_IC_COMP_TYPE_Msk

// IO_BANK0->GPIO[n].STATUS
#define IO_BANK0_GPIO_STATUS_IRQTOPROC_Pos                   26
#define IO_BANK0_GPIO_STATUS_IRQTOPROC_Msk                   ((uint32_t)0x04000000)
#define IO_BANK0_GPIO_STATUS_IRQTOPROC                       IO_BANK0_GPIO_STATUS_IRQTOPROC_Msk
#define IO_BANK0_GPIO_STATUS_IRQFROMPAD_Pos                  24
#define IO_BANK0_GPIO_STATUS_IRQFROMPAD_Msk                  ((uint32_t)0x01000000)
#define IO_BANK0_GPIO_STATUS_IRQFROMPAD                      IO_BANK0_GPIO_STATUS_IRQFROMPAD_Msk
#define IO_BANK0_GPIO_STATUS_INTOPERI_Pos                    19
#define IO_BANK0_GPIO_STATUS_INTOPERI_Msk                    ((uint32_t)0x00080000)
#define IO_BANK0_GPIO_STATUS_INTOPERI                        IO_BANK0_GPIO_STATUS_INTOPERI_Msk
#define IO_BANK0_GPIO_STATUS_INFROMPAD_Pos                   17
#define IO_BANK0_GPIO_STATUS_INFROMPAD_Msk                   ((uint32_t)0x00020000)
#define IO_BANK0_GPIO_STATUS_INFROMPAD                       IO_BANK0_GPIO_STATUS_INFROMPAD_Msk
#define IO_BANK0_GPIO_STATUS_OETOPAD_Pos                     13
#define IO_BANK0_GPIO_STATUS_OETOPAD_Msk                     ((uint32_t)0x00002000)
#define IO_BANK0_GPIO_STATUS_OETOPAD                         IO_BANK0_GPIO_STATUS_OETOPAD_Msk
#define IO_BANK0_GPIO_STATUS_OEFROMPERI_Pos                  12
#define IO_BANK0_GPIO_STATUS_OEFROMPERI_Msk                  ((uint32_t)0x00001000)
#define IO_BANK0_GPIO_STATUS_OEFROMPERI                      IO_BANK0_GPIO_STATUS_OEFROMPERI_Msk
#define IO_BANK0_GPIO_STATUS_OUTTOPAD_Pos                    9
#define IO_BANK0_GPIO_STATUS_OUTTOPAD_Msk                    ((uint32_t)0x00000200)
#define IO_BANK0_GPIO_STATUS_OUTTOPAD                        IO_BANK0_GPIO_STATUS_OUTTOPAD_Msk
#define IO_BANK0_GPIO_STATUS_OUTFROMPERI_Pos                 8
#define IO_BANK0_GPIO_STATUS_OUTFROMPERI_Msk                 ((uint32_t)0x00000100)
#define IO_BANK0_GPIO_STATUS_OUTFROMPERI                     IO_BANK0_GPIO_STATUS_OUTFROMPERI_Msk
// IO_BANK0->GPIO[n].CTRL
#define IO_BANK0_GPIO_CTRL_IRQOVER_Pos                       28
#define IO_BANK0_GPIO_CTRL_IRQOVER_Msk                       ((uint32_t)0x30000000)
#define IO_BANK0_GPIO_CTRL_IRQOVER                           IO_BANK0_GPIO_CTRL_IRQOVER_Msk
#define IO_BANK0_GPIO_CTRL_INOVER_Pos                        16
#define IO_BANK0_GPIO_CTRL_INOVER_Msk                        ((uint32_t)0x00030000)
#define IO_BANK0_GPIO_CTRL_INOVER                            IO_BANK0_GPIO_CTRL_INOVER_Msk
#define IO_BANK0_GPIO_CTRL_OEOVER_Pos                        12
#define IO_BANK0_GPIO_CTRL_OEOVER_Msk                        ((uint32_t)0x00003000)
#define IO_BANK0_GPIO_CTRL_OEOVER                            IO_BANK0_GPIO_CTRL_OEOVER_Msk
#define IO_BANK0_GPIO_CTRL_OUTOVER_Pos                       8
#define IO_BANK0_GPIO_CTRL_OUTOVER_Msk                       ((uint32_t)0x00000300)
#define IO_BANK0_GPIO_CTRL_OUTOVER                           IO_BANK0_GPIO_CTRL_OUTOVER_Msk
#define IO_BANK0_GPIO_CTRL_FUNCSEL_Pos                       0
#define IO_BANK0_GPIO_CTRL_FUNCSEL_Msk                       ((uint32_t)0x0000001F)
#define IO_BANK0_GPIO_CTRL_FUNCSEL                           IO_BANK0_GPIO_CTRL_FUNCSEL_Msk
// IO_BANK0->INTR
#define IO_BANK0_INTR_GPIO7_EDGE_HIGH_Pos                    31
#define IO_BANK0_INTR_GPIO7_EDGE_HIGH_Msk                    ((uint32_t)0x80000000)
#define IO_BANK0_INTR_GPIO7_EDGE_HIGH                        IO_BANK0_INTR_GPIO7_EDGE_HIGH_Msk
#define IO_BANK0_INTR_GPIO7_EDGE_LOW_Pos                     30
#define IO_BANK0_INTR_GPIO7_EDGE_LOW_Msk                     ((uint32_t)0x40000000)
#define IO_BANK0_INTR_GPIO7_EDGE_LOW                         IO_BANK0_INTR_GPIO7_EDGE_LOW_Msk
#define IO_BANK0_INTR_GPIO7_LEVEL_HIGH_Pos                   29
#define IO_BANK0_INTR_GPIO7_LEVEL_HIGH_Msk                   ((uint32_t)0x20000000)
#define IO_BANK0_INTR_GPIO7_LEVEL_HIGH                       IO_BANK0_INTR_GPIO7_LEVEL_HIGH_Msk
#define IO_BANK0_INTR_GPIO7_LEVEL_LOW_Pos                    28
#define IO_BANK0_INTR_GPIO7_LEVEL_LOW_Msk                    ((uint32_t)0x10000000)
#define IO_BANK0_INTR_GPIO7_LEVEL_LOW                        IO_BANK0_INTR_GPIO7_LEVEL_LOW_Msk
#define IO_BANK0_INTR_GPIO6_EDGE_HIGH_Pos                    27
#define IO_BANK0_INTR_GPIO6_EDGE_HIGH_Msk                    ((uint32_t)0x08000000)
#define IO_BANK0_INTR_GPIO6_EDGE_HIGH                        IO_BANK0_INTR_GPIO6_EDGE_HIGH_Msk
#define IO_BANK0_INTR_GPIO6_EDGE_LOW_Pos                     26
#define IO_BANK0_INTR_GPIO6_EDGE_LOW_Msk                     ((uint32_t)0x04000000)
#define IO_BANK0_INTR_GPIO6_EDGE_LOW                         IO_BANK0_INTR_GPIO6_EDGE_LOW_Msk
#define IO_BANK0_INTR_GPIO6_LEVEL_HIGH_Pos                   25
#define IO_BANK0_INTR_GPIO6_LEVEL_HIGH_Msk                   ((uint32_t)0x02000000)
#define IO_BANK0_INTR_GPIO6_LEVEL_HIGH                       IO_BANK0_INTR_GPIO6_LEVEL_HIGH_Msk
#define IO_BANK0_INTR_GPIO6_LEVEL_LOW_Pos                    24
#define IO_BANK0_INTR_GPIO6_LEVEL_LOW_Msk                    ((uint32_t)0x01000000)
#define IO_BANK0_INTR_GPIO6_LEVEL_LOW                        IO_BANK0_INTR_GPIO6_LEVEL_LOW_Msk
#define IO_BANK0_INTR_GPIO5_EDGE_HIGH_Pos                    23
#define IO_BANK0_INTR_GPIO5_EDGE_HIGH_Msk                    ((uint32_t)0x00800000)
#define IO_BANK0_INTR_GPIO5_EDGE_HIGH                        IO_BANK0_INTR_GPIO5_EDGE_HIGH_Msk
#define IO_BANK0_INTR_GPIO5_EDGE_LOW_Pos                     22
#define IO_BANK0_INTR_GPIO5_EDGE_LOW_Msk                     ((uint32_t)0x00400000)
#define IO_BANK0_INTR_GPIO5_EDGE_LOW                         IO_BANK0_INTR_GPIO5_EDGE_LOW_Msk
#define IO_BANK0_INTR_GPIO5_LEVEL_HIGH_Pos                   21
#define IO_BANK0_INTR_GPIO5_LEVEL_HIGH_Msk                   ((uint32_t)0x00200000)
#define IO_BANK0_INTR_GPIO5_LEVEL_HIGH                       IO_BANK0_INTR_GPIO5_LEVEL_HIGH_Msk
#define IO_BANK0_INTR_GPIO5_LEVEL_LOW_Pos                    20
#define IO_BANK0_INTR_GPIO5_LEVEL_LOW_Msk                    ((uint32_t)0x00100000)
#define IO_BANK0_INTR_GPIO5_LEVEL_LOW                        IO_BANK0_INTR_GPIO5_LEVEL_LOW_Msk
#define IO_BANK0_INTR_GPIO4_EDGE_HIGH_Pos                    19
#define IO_BANK0_INTR_GPIO4_EDGE_HIGH_Msk                    ((uint32_t)0x00080000)
#define IO_BANK0_INTR_GPIO4_EDGE_HIGH                        IO_BANK0_INTR_GPIO4_EDGE_HIGH_Msk
#define IO_BANK0_INTR_GPIO4_EDGE_LOW_Pos                     18
#define IO_BANK0_INTR_GPIO4_EDGE_LOW_Msk                     ((uint32_t)0x00040000)
#define IO_BANK0_INTR_GPIO4_EDGE_LOW                         IO_BANK0_INTR_GPIO4_EDGE_LOW_Msk
#define IO_BANK0_INTR_GPIO4_LEVEL_HIGH_Pos                   17
#define IO_BANK0_INTR_GPIO4_LEVEL_HIGH_Msk                   ((uint32_t)0x00020000)
#define IO_BANK0_INTR_GPIO4_LEVEL_HIGH                       IO_BANK0_INTR_GPIO4_LEVEL_HIGH_Msk
#define IO_BANK0_INTR_GPIO4_LEVEL_LOW_Pos                    16
#define IO_BANK0_INTR_GPIO4_LEVEL_LOW_Msk                    ((uint32_t)0x00010000)
#define IO_BANK0_INTR_GPIO4_LEVEL_LOW                        IO_BANK0_INTR_GPIO4_LEVEL_LOW_Msk
#define IO_BANK0_INTR_GPIO3_EDGE_HIGH_Pos                    15
#define IO_BANK0_INTR_GPIO3_EDGE_HIGH_Msk                    ((uint32_t)0x00008000)
#define IO_BANK0_INTR_GPIO3_EDGE_HIGH                        IO_BANK0_INTR_GPIO3_EDGE_HIGH_Msk
#define IO_BANK0_INTR_GPIO3_EDGE_LOW_Pos                     14
#define IO_BANK0_INTR_GPIO3_EDGE_LOW_Msk                     ((uint32_t)0x00004000)
#define IO_BANK0_INTR_GPIO3_EDGE_LOW                         IO_BANK0_INTR_GPIO3_EDGE_LOW_Msk
#define IO_BANK0_INTR_GPIO3_LEVEL_HIGH_Pos                   13
#define IO_BANK0_INTR_GPIO3_LEVEL_HIGH_Msk                   ((uint32_t)0x00002000)
#define IO_BANK0_INTR_GPIO3_LEVEL_HIGH                       IO_BANK0_INTR_GPIO3_LEVEL_HIGH_Msk
#define IO_BANK0_INTR_GPIO3_LEVEL_LOW_Pos                    12
#define IO_BANK0_INTR_GPIO3_LEVEL_LOW_Msk                    ((uint32_t)0x00001000)
#define IO_BANK0_INTR_GPIO3_LEVEL_LOW                        IO_BANK0_INTR_GPIO3_LEVEL_LOW_Msk
#define IO_BANK0_INTR_GPIO2_EDGE_HIGH_Pos                    11
#define IO_BANK0_INTR_GPIO2_EDGE_HIGH_Msk                    ((uint32_t)0x00000800)
#define IO_BANK0_INTR_GPIO2_EDGE_HIGH                        IO_BANK0_INTR_GPIO2_EDGE_HIGH_Msk
#define IO_BANK0_INTR_GPIO2_EDGE_LOW_Pos                     10
#define IO_BANK0_INTR_GPIO2_EDGE_LOW_Msk                     ((uint32_t)0x00000400)
#define IO_BANK0_INTR_GPIO2_EDGE_LOW                         IO_BANK0_INTR_GPIO2_EDGE_LOW_Msk
#define IO_BANK0_INTR_GPIO2_LEVEL_HIGH_Pos                   9
#define IO_BANK0_INTR_GPIO2_LEVEL_HIGH_Msk                   ((uint32_t)0x00000200)
#define IO_BANK0_INTR_GPIO2_LEVEL_HIGH                       IO_BANK0_INTR_GPIO2_LEVEL_HIGH_Msk
#define IO_BANK0_INTR_GPIO2_LEVEL_LOW_Pos                    8
#define IO_BANK0_INTR_GPIO2_LEVEL_LOW_Msk                    ((uint32_t)0x00000100)
#define IO_BANK0_INTR_GPIO2_LEVEL_LOW                        IO_BANK0_INTR_GPIO2_LEVEL_LOW_Msk
#define IO_BANK0_INTR_GPIO1_EDGE_HIGH_Pos                    7
#define IO_BANK0_INTR_GPIO1_EDGE_HIGH_Msk                    ((uint32_t)0x00000080)
#define IO_BANK0_INTR_GPIO1_EDGE_HIGH                        IO_BANK0_INTR_GPIO1_EDGE_HIGH_Msk
#define IO_BANK0_INTR_GPIO1_EDGE_LOW_Pos                     6
#define IO_BANK0_INTR_GPIO1_EDGE_LOW_Msk                     ((uint32_t)0x00000040)
#define IO_BANK0_INTR_GPIO1_EDGE_LOW                         IO_BANK0_INTR_GPIO1_EDGE_LOW_Msk
#define IO_BANK0_INTR_GPIO1_LEVEL_HIGH_Pos                   5
#define IO_BANK0_INTR_GPIO1_LEVEL_HIGH_Msk                   ((uint32_t)0x00000020)
#define IO_BANK0_INTR_GPIO1_LEVEL_HIGH                       IO_BANK0_INTR_GPIO1_LEVEL_HIGH_Msk
#define IO_BANK0_INTR_GPIO1_LEVEL_LOW_Pos                    4
#define IO_BANK0_INTR_GPIO1_LEVEL_LOW_Msk                    ((uint32_t)0x00000010)
#define IO_BANK0_INTR_GPIO1_LEVEL_LOW                        IO_BANK0_INTR_GPIO1_LEVEL_LOW_Msk
#define IO_BANK0_INTR_GPIO0_EDGE_HIGH_Pos                    3
#define IO_BANK0_INTR_GPIO0_EDGE_HIGH_Msk                    ((uint32_t)0x00000008)
#define IO_BANK0_INTR_GPIO0_EDGE_HIGH                        IO_BANK0_INTR_GPIO0_EDGE_HIGH_Msk
#define IO_BANK0_INTR_GPIO0_EDGE_LOW_Pos                     2
#define IO_BANK0_INTR_GPIO0_EDGE_LOW_Msk                     ((uint32_t)0x00000004)
#define IO_BANK0_INTR_GPIO0_EDGE_LOW                         IO_BANK0_INTR_GPIO0_EDGE_LOW_Msk
#define IO_BANK0_INTR_GPIO0_LEVEL_HIGH_Pos                   1
#define IO_BANK0_INTR_GPIO0_LEVEL_HIGH_Msk                   ((uint32_t)0x00000002)
#define IO_BANK0_INTR_GPIO0_LEVEL_HIGH                       IO_BANK0_INTR_GPIO0_LEVEL_HIGH_Msk
#define IO_BANK0_INTR_GPIO0_LEVEL_LOW_Pos                    0
#define IO_BANK0_INTR_GPIO0_LEVEL_LOW_Msk                    ((uint32_t)0x00000001)
#define IO_BANK0_INTR_GPIO0_LEVEL_LOW                        IO_BANK0_INTR_GPIO0_LEVEL_LOW_Msk
// IO_BANK0->PROC[n].INTE
#define IO_BANK0_PROC_INTE_GPIO7_EDGE_HIGH_Pos               31
#define IO_BANK0_PROC_INTE_GPIO7_EDGE_HIGH_Msk               ((uint32_t)0x80000000)
#define IO_BANK0_PROC_INTE_GPIO7_EDGE_HIGH                   IO_BANK0_PROC_INTE_GPIO7_EDGE_HIGH_Msk
#define IO_BANK0_PROC_INTE_GPIO7_EDGE_LOW_Pos                30
#define IO_BANK0_PROC_INTE_GPIO7_EDGE_LOW_Msk                ((uint32_t)0x40000000)
#define IO_BANK0_PROC_INTE_GPIO7_EDGE_LOW                    IO_BANK0_PROC_INTE_GPIO7_EDGE_LOW_Msk
#define IO_BANK0_PROC_INTE_GPIO7_LEVEL_HIGH_Pos              29
#define IO_BANK0_PROC_INTE_GPIO7_LEVEL_HIGH_Msk              ((uint32_t)0x20000000)
#define IO_BANK0_PROC_INTE_GPIO7_LEVEL_HIGH                  IO_BANK0_PROC_INTE_GPIO7_LEVEL_HIGH_Msk
#define IO_BANK0_PROC_INTE_GPIO7_LEVEL_LOW_Pos               28
#define IO_BANK0_PROC_INTE_GPIO7_LEVEL_LOW_Msk               ((uint32_t)0x10000000)
#define IO_BANK0_PROC_INTE_GPIO7_LEVEL_LOW                   IO_BANK0_PROC_INTE_GPIO7_LEVEL_LOW_Msk
#define IO_BANK0_PROC_INTE_GPIO6_EDGE_HIGH_Pos               27
#define IO_BANK0_PROC_INTE_GPIO6_EDGE_HIGH_Msk               ((uint32_t)0x08000000)
#define IO_BANK0_PROC_INTE_GPIO6_EDGE_HIGH                   IO_BANK0_PROC_INTE_GPIO6_EDGE_HIGH_Msk
#define IO_BANK0_PROC_INTE_GPIO6_EDGE_LOW_Pos                26
#define IO_BANK0_PROC_INTE_GPIO6_EDGE_LOW_Msk                ((uint32_t)0x04000000)
#define IO_BANK0_PROC_INTE_GPIO6_EDGE_LOW                    IO_BANK0_PROC_INTE_GPIO6_EDGE_LOW_Msk
#define IO_BANK0_PROC_INTE_GPIO6_LEVEL_HIGH_Pos              25
#define IO_BANK0_PROC_INTE_GPIO6_LEVEL_HIGH_Msk              ((uint32_t)0x02000000)
#define IO_BANK0_PROC_INTE_GPIO6_LEVEL_HIGH                  IO_BANK0_PROC_INTE_GPIO6_LEVEL_HIGH_Msk
#define IO_BANK0_PROC_INTE_GPIO6_LEVEL_LOW_Pos               24
#define IO_BANK0_PROC_INTE_GPIO6_LEVEL_LOW_Msk               ((uint32_t)0x01000000)
#define IO_BANK0_PROC_INTE_GPIO6_LEVEL_LOW                   IO_BANK0_PROC_INTE_GPIO6_LEVEL_LOW_Msk
#define IO_BANK0_PROC_INTE_GPIO5_EDGE_HIGH_Pos               23
#define IO_BANK0_PROC_INTE_GPIO5_EDGE_HIGH_Msk               ((uint32_t)0x00800000)
#define IO_BANK0_PROC_INTE_GPIO5_EDGE_HIGH                   IO_BANK0_PROC_INTE_GPIO5_EDGE_HIGH_Msk
#define IO_BANK0_PROC_INTE_GPIO5_EDGE_LOW_Pos                22
#define IO_BANK0_PROC_INTE_GPIO5_EDGE_LOW_Msk                ((uint32_t)0x00400000)
#define IO_BANK0_PROC_INTE_GPIO5_EDGE_LOW                    IO_BANK0_PROC_INTE_GPIO5_EDGE_LOW_Msk
#define IO_BANK0_PROC_INTE_GPIO5_LEVEL_HIGH_Pos              21
#define IO_BANK0_PROC_INTE_GPIO5_LEVEL_HIGH_Msk              ((uint32_t)0x00200000)
#define IO_BANK0_PROC_INTE_GPIO5_LEVEL_HIGH                  IO_BANK0_PROC_INTE_GPIO5_LEVEL_HIGH_Msk
#define IO_BANK0_PROC_INTE_GPIO5_LEVEL_LOW_Pos               20
#define IO_BANK0_PROC_INTE_GPIO5_LEVEL_LOW_Msk               ((uint32_t)0x00100000)
#define IO_BANK0_PROC_INTE_GPIO5_LEVEL_LOW                   IO_BANK0_PROC_INTE_GPIO5_LEVEL_LOW_Msk
#define IO_BANK0_PROC_INTE_GPIO4_EDGE_HIGH_Pos               19
#define IO_BANK0_PROC_INTE_GPIO4_EDGE_HIGH_Msk               ((uint32_t)0x00080000)
#define IO_BANK0_PROC_INTE_GPIO4_EDGE_HIGH                   IO_BANK0_PROC_INTE_GPIO4_EDGE_HIGH_Msk
#define IO_BANK0_PROC_INTE_GPIO4_EDGE_LOW_Pos                18
#define IO_BANK0_PROC_INTE_GPIO4_EDGE_LOW_Msk                ((uint32_t)0x00040000)
#define IO_BANK0_PROC_INTE_GPIO4_EDGE_LOW                    IO_BANK0_PROC_INTE_GPIO4_EDGE_LOW_Msk
#define IO_BANK0_PROC_INTE_GPIO4_LEVEL_HIGH_Pos              17
#define IO_BANK0_PROC_INTE_GPIO4_LEVEL_HIGH_Msk              ((uint32_t)0x00020000)
#define IO_BANK0_PROC_INTE_GPIO4_LEVEL_HIGH                  IO_BANK0_PROC_INTE_GPIO4_LEVEL_HIGH_Msk
#define IO_BANK0_PROC_INTE_GPIO4_LEVEL_LOW_Pos               16
#define IO_BANK0_PROC_INTE_GPIO4_LEVEL_LOW_Msk               ((uint32_t)0x00010000)
#define IO_BANK0_PROC_INTE_GPIO4_LEVEL_LOW                   IO_BANK0_PROC_INTE_GPIO4_LEVEL_LOW_Msk
#define IO_BANK0_PROC_INTE_GPIO3_EDGE_HIGH_Pos               15
#define IO_BANK0_PROC_INTE_GPIO3_EDGE_HIGH_Msk               ((uint32_t)0x00008000)
#define IO_BANK0_PROC_INTE_GPIO3_EDGE_HIGH                   IO_BANK0_PROC_INTE_GPIO3_EDGE_HIGH_Msk
#define IO_BANK0_PROC_INTE_GPIO3_EDGE_LOW_Pos                14
#define IO_BANK0_PROC_INTE_GPIO3_EDGE_LOW_Msk                ((uint32_t)0x00004000)
#define IO_BANK0_PROC_INTE_GPIO3_EDGE_LOW                    IO_BANK0_PROC_INTE_GPIO3_EDGE_LOW_Msk
#define IO_BANK0_PROC_INTE_GPIO3_LEVEL_HIGH_Pos              13
#define IO_BANK0_PROC_INTE_GPIO3_LEVEL_HIGH_Msk              ((uint32_t)0x00002000)
#define IO_BANK0_PROC_INTE_GPIO3_LEVEL_HIGH                  IO_BANK0_PROC_INTE_GPIO3_LEVEL_HIGH_Msk
#define IO_BANK0_PROC_INTE_GPIO3_LEVEL_LOW_Pos               12
#define IO_BANK0_PROC_INTE_GPIO3_LEVEL_LOW_Msk               ((uint32_t)0x00001000)
#define IO_BANK0_PROC_INTE_GPIO3_LEVEL_LOW                   IO_BANK0_PROC_INTE_GPIO3_LEVEL_LOW_Msk
#define IO_BANK0_PROC_INTE_GPIO2_EDGE_HIGH_Pos               11
#define IO_BANK0_PROC_INTE_GPIO2_EDGE_HIGH_Msk               ((uint32_t)0x00000800)
#define IO_BANK0_PROC_INTE_GPIO2_EDGE_HIGH                   IO_BANK0_PROC_INTE_GPIO2_EDGE_HIGH_Msk
#define IO_BANK0_PROC_INTE_GPIO2_EDGE_LOW_Pos                10
#define IO_BANK0_PROC_INTE_GPIO2_EDGE_LOW_Msk                ((uint32_t)0x00000400)
#define IO_BANK0_PROC_INTE_GPIO2_EDGE_LOW                    IO_BANK0_PROC_INTE_GPIO2_EDGE_LOW_Msk
#define IO_BANK0_PROC_INTE_GPIO2_LEVEL_HIGH_Pos              9
#define IO_BANK0_PROC_INTE_GPIO2_LEVEL_HIGH_Msk              ((uint32_t)0x00000200)
#define IO_BANK0_PROC_INTE_GPIO2_LEVEL_HIGH                  IO_BANK0_PROC_INTE_GPIO2_LEVEL_HIGH_Msk
#define IO_BANK0_PROC_INTE_GPIO2_LEVEL_LOW_Pos               8
#define IO_BANK0_PROC_INTE_GPIO2_LEVEL_LOW_Msk               ((uint32_t)0x00000100)
#define IO_BANK0_PROC_INTE_GPIO2_LEVEL_LOW                   IO_BANK0_PROC_INTE_GPIO2_LEVEL_LOW_Msk
#define IO_BANK0_PROC_INTE_GPIO1_EDGE_HIGH_Pos               7
#define IO_BANK0_PROC_INTE_GPIO1_EDGE_HIGH_Msk               ((uint32_t)0x00000080)
#define IO_BANK0_PROC_INTE_GPIO1_EDGE_HIGH                   IO_BANK0_PROC_INTE_GPIO1_EDGE_HIGH_Msk
#define IO_BANK0_PROC_INTE_GPIO1_EDGE_LOW_Pos                6
#define IO_BANK0_PROC_INTE_GPIO1_EDGE_LOW_Msk                ((uint32_t)0x00000040)
#define IO_BANK0_PROC_INTE_GPIO1_EDGE_LOW                    IO_BANK0_PROC_INTE_GPIO1_EDGE_LOW_Msk
#define IO_BANK0_PROC_INTE_GPIO1_LEVEL_HIGH_Pos              5
#define IO_BANK0_PROC_INTE_GPIO1_LEVEL_HIGH_Msk              ((uint32_t)0x00000020)
#define IO_BANK0_PROC_INTE_GPIO1_LEVEL_HIGH                  IO_BANK0_PROC_INTE_GPIO1_LEVEL_HIGH_Msk
#define IO_BANK0_PROC_INTE_GPIO1_LEVEL_LOW_Pos               4
#define IO_BANK0_PROC_INTE_GPIO1_LEVEL_LOW_Msk               ((uint32_t)0x00000010)
#define IO_BANK0_PROC_INTE_GPIO1_LEVEL_LOW                   IO_BANK0_PROC_INTE_GPIO1_LEVEL_LOW_Msk
#define IO_BANK0_PROC_INTE_GPIO0_EDGE_HIGH_Pos               3
#define IO_BANK0_PROC_INTE_GPIO0_EDGE_HIGH_Msk               ((uint32_t)0x00000008)
#define IO_BANK0_PROC_INTE_GPIO0_EDGE_HIGH                   IO_BANK0_PROC_INTE_GPIO0_EDGE_HIGH_Msk
#define IO_BANK0_PROC_INTE_GPIO0_EDGE_LOW_Pos                2
#define IO_BANK0_PROC_INTE_GPIO0_EDGE_LOW_Msk                ((uint32_t)0x00000004)
#define IO_BANK0_PROC_INTE_GPIO0_EDGE_LOW                    IO_BANK0_PROC_INTE_GPIO0_EDGE_LOW_Msk
#define IO_BANK0_PROC_INTE_GPIO0_LEVEL_HIGH_Pos              1
#define IO_BANK0_PROC_INTE_GPIO0_LEVEL_HIGH_Msk              ((uint32_t)0x00000002)
#define IO_BANK0_PROC_INTE_GPIO0_LEVEL_HIGH                  IO_BANK0_PROC_INTE_GPIO0_LEVEL_HIGH_Msk
#define IO_BANK0_PROC_INTE_GPIO0_LEVEL_LOW_Pos               0
#define IO_BANK0_PROC_INTE_GPIO0_LEVEL_LOW_Msk               ((uint32_t)0x00000001)
#define IO_BANK0_PROC_INTE_GPIO0_LEVEL_LOW                   IO_BANK0_PROC_INTE_GPIO0_LEVEL_LOW_Msk
// IO_BANK0->PROC[n].INTF
#define IO_BANK0_PROC_INTF_GPIO7_EDGE_HIGH_Pos               31
#define IO_BANK0_PROC_INTF_GPIO7_EDGE_HIGH_Msk               ((uint32_t)0x80000000)
#define IO_BANK0_PROC_INTF_GPIO7_EDGE_HIGH                   IO_BANK0_PROC_INTF_GPIO7_EDGE_HIGH_Msk
#define IO_BANK0_PROC_INTF_GPIO7_EDGE_LOW_Pos                30
#define IO_BANK0_PROC_INTF_GPIO7_EDGE_LOW_Msk                ((uint32_t)0x40000000)
#define IO_BANK0_PROC_INTF_GPIO7_EDGE_LOW                    IO_BANK0_PROC_INTF_GPIO7_EDGE_LOW_Msk
#define IO_BANK0_PROC_INTF_GPIO7_LEVEL_HIGH_Pos              29
#define IO_BANK0_PROC_INTF_GPIO7_LEVEL_HIGH_Msk              ((uint32_t)0x20000000)
#define IO_BANK0_PROC_INTF_GPIO7_LEVEL_HIGH                  IO_BANK0_PROC_INTF_GPIO7_LEVEL_HIGH_Msk
#define IO_BANK0_PROC_INTF_GPIO7_LEVEL_LOW_Pos               28
#define IO_BANK0_PROC_INTF_GPIO7_LEVEL_LOW_Msk               ((uint32_t)0x10000000)
#define IO_BANK0_PROC_INTF_GPIO7_LEVEL_LOW                   IO_BANK0_PROC_INTF_GPIO7_LEVEL_LOW_Msk
#define IO_BANK0_PROC_INTF_GPIO6_EDGE_HIGH_Pos               27
#define IO_BANK0_PROC_INTF_GPIO6_EDGE_HIGH_Msk               ((uint32_t)0x08000000)
#define IO_BANK0_PROC_INTF_GPIO6_EDGE_HIGH                   IO_BANK0_PROC_INTF_GPIO6_EDGE_HIGH_Msk
#define IO_BANK0_PROC_INTF_GPIO6_EDGE_LOW_Pos                26
#define IO_BANK0_PROC_INTF_GPIO6_EDGE_LOW_Msk                ((uint32_t)0x04000000)
#define IO_BANK0_PROC_INTF_GPIO6_EDGE_LOW                    IO_BANK0_PROC_INTF_GPIO6_EDGE_LOW_Msk
#define IO_BANK0_PROC_INTF_GPIO6_LEVEL_HIGH_Pos              25
#define IO_BANK0_PROC_INTF_GPIO6_LEVEL_HIGH_Msk              ((uint32_t)0x02000000)
#define IO_BANK0_PROC_INTF_GPIO6_LEVEL_HIGH                  IO_BANK0_PROC_INTF_GPIO6_LEVEL_HIGH_Msk
#define IO_BANK0_PROC_INTF_GPIO6_LEVEL_LOW_Pos               24
#define IO_BANK0_PROC_INTF_GPIO6_LEVEL_LOW_Msk               ((uint32_t)0x01000000)
#define IO_BANK0_PROC_INTF_GPIO6_LEVEL_LOW                   IO_BANK0_PROC_INTF_GPIO6_LEVEL_LOW_Msk
#define IO_BANK0_PROC_INTF_GPIO5_EDGE_HIGH_Pos               23
#define IO_BANK0_PROC_INTF_GPIO5_EDGE_HIGH_Msk               ((uint32_t)0x00800000)
#define IO_BANK0_PROC_INTF_GPIO5_EDGE_HIGH                   IO_BANK0_PROC_INTF_GPIO5_EDGE_HIGH_Msk
#define IO_BANK0_PROC_INTF_GPIO5_EDGE_LOW_Pos                22
#define IO_BANK0_PROC_INTF_GPIO5_EDGE_LOW_Msk                ((uint32_t)0x00400000)
#define IO_BANK0_PROC_INTF_GPIO5_EDGE_LOW                    IO_BANK0_PROC_INTF_GPIO5_EDGE_LOW_Msk
#define IO_BANK0_PROC_INTF_GPIO5_LEVEL_HIGH_Pos              21
#define IO_BANK0_PROC_INTF_GPIO5_LEVEL_HIGH_Msk              ((uint32_t)0x00200000)
#define IO_BANK0_PROC_INTF_GPIO5_LEVEL_HIGH                  IO_BANK0_PROC_INTF_GPIO5_LEVEL_HIGH_Msk
#define IO_BANK0_PROC_INTF_GPIO5_LEVEL_LOW_Pos               20
#define IO_BANK0_PROC_INTF_GPIO5_LEVEL_LOW_Msk               ((uint32_t)0x00100000)
#define IO_BANK0_PROC_INTF_GPIO5_LEVEL_LOW                   IO_BANK0_PROC_INTF_GPIO5_LEVEL_LOW_Msk
#define IO_BANK0_PROC_INTF_GPIO4_EDGE_HIGH_Pos               19
#define IO_BANK0_PROC_INTF_GPIO4_EDGE_HIGH_Msk               ((uint32_t)0x00080000)
#define IO_BANK0_PROC_INTF_GPIO4_EDGE_HIGH                   IO_BANK0_PROC_INTF_GPIO4_EDGE_HIGH_Msk
#define IO_BANK0_PROC_INTF_GPIO4_EDGE_LOW_Pos                18
#define IO_BANK0_PROC_INTF_GPIO4_EDGE_LOW_Msk                ((uint32_t)0x00040000)
#define IO_BANK0_PROC_INTF_GPIO4_EDGE_LOW                    IO_BANK0_PROC_INTF_GPIO4_EDGE_LOW_Msk
#define IO_BANK0_PROC_INTF_GPIO4_LEVEL_HIGH_Pos              17
#define IO_BANK0_PROC_INTF_GPIO4_LEVEL_HIGH_Msk              ((uint32_t)0x00020000)
#define IO_BANK0_PROC_INTF_GPIO4_LEVEL_HIGH                  IO_BANK0_PROC_INTF_GPIO4_LEVEL_HIGH_Msk
#define IO_BANK0_PROC_INTF_GPIO4_LEVEL_LOW_Pos               16
#define IO_BANK0_PROC_INTF_GPIO4_LEVEL_LOW_Msk               ((uint32_t)0x00010000)
#define IO_BANK0_PROC_INTF_GPIO4_LEVEL_LOW                   IO_BANK0_PROC_INTF_GPIO4_LEVEL_LOW_Msk
#define IO_BANK0_PROC_INTF_GPIO3_EDGE_HIGH_Pos               15
#define IO_BANK0_PROC_INTF_GPIO3_EDGE_HIGH_Msk               ((uint32_t)0x00008000)
#define IO_BANK0_PROC_INTF_GPIO3_EDGE_HIGH                   IO_BANK0_PROC_INTF_GPIO3_EDGE_HIGH_Msk
#define IO_BANK0_PROC_INTF_GPIO3_EDGE_LOW_Pos                14
#define IO_BANK0_PROC_INTF_GPIO3_EDGE_LOW_Msk                ((uint32_t)0x00004000)
#define IO_BANK0_PROC_INTF_GPIO3_EDGE_LOW                    IO_BANK0_PROC_INTF_GPIO3_EDGE_LOW_Msk
#define IO_BANK0_PROC_INTF_GPIO3_LEVEL_HIGH_Pos              13
#define IO_BANK0_PROC_INTF_GPIO3_LEVEL_HIGH_Msk              ((uint32_t)0x00002000)
#define IO_BANK0_PROC_INTF_GPIO3_LEVEL_HIGH                  IO_BANK0_PROC_INTF_GPIO3_LEVEL_HIGH_Msk
#define IO_BANK0_PROC_INTF_GPIO3_LEVEL_LOW_Pos               12
#define IO_BANK0_PROC_INTF_GPIO3_LEVEL_LOW_Msk               ((uint32_t)0x00001000)
#define IO_BANK0_PROC_INTF_GPIO3_LEVEL_LOW                   IO_BANK0_PROC_INTF_GPIO3_LEVEL_LOW_Msk
#define IO_BANK0_PROC_INTF_GPIO2_EDGE_HIGH_Pos               11
#define IO_BANK0_PROC_INTF_GPIO2_EDGE_HIGH_Msk               ((uint32_t)0x00000800)
#define IO_BANK0_PROC_INTF_GPIO2_EDGE_HIGH                   IO_BANK0_PROC_INTF_GPIO2_EDGE_HIGH_Msk
#define IO_BANK0_PROC_INTF_GPIO2_EDGE_LOW_Pos                10
#define IO_BANK0_PROC_INTF_GPIO2_EDGE_LOW_Msk                ((uint32_t)0x00000400)
#define IO_BANK0_PROC_INTF_GPIO2_EDGE_LOW                    IO_BANK0_PROC_INTF_GPIO2_EDGE_LOW_Msk
#define IO_BANK0_PROC_INTF_GPIO2_LEVEL_HIGH_Pos              9
#define IO_BANK0_PROC_INTF_GPIO2_LEVEL_HIGH_Msk              ((uint32_t)0x00000200)
#define IO_BANK0_PROC_INTF_GPIO2_LEVEL_HIGH                  IO_BANK0_PROC_INTF_GPIO2_LEVEL_HIGH_Msk
#define IO_BANK0_PROC_INTF_GPIO2_LEVEL_LOW_Pos               8
#define IO_BANK0_PROC_INTF_GPIO2_LEVEL_LOW_Msk               ((uint32_t)0x00000100)
#define IO_BANK0_PROC_INTF_GPIO2_LEVEL_LOW                   IO_BANK0_PROC_INTF_GPIO2_LEVEL_LOW_Msk
#define IO_BANK0_PROC_INTF_GPIO1_EDGE_HIGH_Pos               7
#define IO_BANK0_PROC_INTF_GPIO1_EDGE_HIGH_Msk               ((uint32_t)0x00000080)
#define IO_BANK0_PROC_INTF_GPIO1_EDGE_HIGH                   IO_BANK0_PROC_INTF_GPIO1_EDGE_HIGH_Msk
#define IO_BANK0_PROC_INTF_GPIO1_EDGE_LOW_Pos                6
#define IO_BANK0_PROC_INTF_GPIO1_EDGE_LOW_Msk                ((uint32_t)0x00000040)
#define IO_BANK0_PROC_INTF_GPIO1_EDGE_LOW                    IO_BANK0_PROC_INTF_GPIO1_EDGE_LOW_Msk
#define IO_BANK0_PROC_INTF_GPIO1_LEVEL_HIGH_Pos              5
#define IO_BANK0_PROC_INTF_GPIO1_LEVEL_HIGH_Msk              ((uint32_t)0x00000020)
#define IO_BANK0_PROC_INTF_GPIO1_LEVEL_HIGH                  IO_BANK0_PROC_INTF_GPIO1_LEVEL_HIGH_Msk
#define IO_BANK0_PROC_INTF_GPIO1_LEVEL_LOW_Pos               4
#define IO_BANK0_PROC_INTF_GPIO1_LEVEL_LOW_Msk               ((uint32_t)0x00000010)
#define IO_BANK0_PROC_INTF_GPIO1_LEVEL_LOW                   IO_BANK0_PROC_INTF_GPIO1_LEVEL_LOW_Msk
#define IO_BANK0_PROC_INTF_GPIO0_EDGE_HIGH_Pos               3
#define IO_BANK0_PROC_INTF_GPIO0_EDGE_HIGH_Msk               ((uint32_t)0x00000008)
#define IO_BANK0_PROC_INTF_GPIO0_EDGE_HIGH                   IO_BANK0_PROC_INTF_GPIO0_EDGE_HIGH_Msk
#define IO_BANK0_PROC_INTF_GPIO0_EDGE_LOW_Pos                2
#define IO_BANK0_PROC_INTF_GPIO0_EDGE_LOW_Msk                ((uint32_t)0x00000004)
#define IO_BANK0_PROC_INTF_GPIO0_EDGE_LOW                    IO_BANK0_PROC_INTF_GPIO0_EDGE_LOW_Msk
#define IO_BANK0_PROC_INTF_GPIO0_LEVEL_HIGH_Pos              1
#define IO_BANK0_PROC_INTF_GPIO0_LEVEL_HIGH_Msk              ((uint32_t)0x00000002)
#define IO_BANK0_PROC_INTF_GPIO0_LEVEL_HIGH                  IO_BANK0_PROC_INTF_GPIO0_LEVEL_HIGH_Msk
#define IO_BANK0_PROC_INTF_GPIO0_LEVEL_LOW_Pos               0
#define IO_BANK0_PROC_INTF_GPIO0_LEVEL_LOW_Msk               ((uint32_t)0x00000001)
#define IO_BANK0_PROC_INTF_GPIO0_LEVEL_LOW                   IO_BANK0_PROC_INTF_GPIO0_LEVEL_LOW_Msk
// IO_BANK0->PROC[n].INTS
#define IO_BANK0_PROC_INTS_GPIO7_EDGE_HIGH_Pos               31
#define IO_BANK0_PROC_INTS_GPIO7_EDGE_HIGH_Msk               ((uint32_t)0x80000000)
#define IO_BANK0_PROC_INTS_GPIO7_EDGE_HIGH                   IO_BANK0_PROC_INTS_GPIO7_EDGE_HIGH_Msk
#define IO_BANK0_PROC_INTS_GPIO7_EDGE_LOW_Pos                30
#define IO_BANK0_PROC_INTS_GPIO7_EDGE_LOW_Msk                ((uint32_t)0x40000000)
#define IO_BANK0_PROC_INTS_GPIO7_EDGE_LOW                    IO_BANK0_PROC_INTS_GPIO7_EDGE_LOW_Msk
#define IO_BANK0_PROC_INTS_GPIO7_LEVEL_HIGH_Pos              29
#define IO_BANK0_PROC_INTS_GPIO7_LEVEL_HIGH_Msk              ((uint32_t)0x20000000)
#define IO_BANK0_PROC_INTS_GPIO7_LEVEL_HIGH                  IO_BANK0_PROC_INTS_GPIO7_LEVEL_HIGH_Msk
#define IO_BANK0_PROC_INTS_GPIO7_LEVEL_LOW_Pos               28
#define IO_BANK0_PROC_INTS_GPIO7_LEVEL_LOW_Msk               ((uint32_t)0x10000000)
#define IO_BANK0_PROC_INTS_GPIO7_LEVEL_LOW                   IO_BANK0_PROC_INTS_GPIO7_LEVEL_LOW_Msk
#define IO_BANK0_PROC_INTS_GPIO6_EDGE_HIGH_Pos               27
#define IO_BANK0_PROC_INTS_GPIO6_EDGE_HIGH_Msk               ((uint32_t)0x08000000)
#define IO_BANK0_PROC_INTS_GPIO6_EDGE_HIGH                   IO_BANK0_PROC_INTS_GPIO6_EDGE_HIGH_Msk
#define IO_BANK0_PROC_INTS_GPIO6_EDGE_LOW_Pos                26
#define IO_BANK0_PROC_INTS_GPIO6_EDGE_LOW_Msk                ((uint32_t)0x04000000)
#define IO_BANK0_PROC_INTS_GPIO6_EDGE_LOW                    IO_BANK0_PROC_INTS_GPIO6_EDGE_LOW_Msk
#define IO_BANK0_PROC_INTS_GPIO6_LEVEL_HIGH_Pos              25
#define IO_BANK0_PROC_INTS_GPIO6_LEVEL_HIGH_Msk              ((uint32_t)0x02000000)
#define IO_BANK0_PROC_INTS_GPIO6_LEVEL_HIGH                  IO_BANK0_PROC_INTS_GPIO6_LEVEL_HIGH_Msk
#define IO_BANK0_PROC_INTS_GPIO6_LEVEL_LOW_Pos               24
#define IO_BANK0_PROC_INTS_GPIO6_LEVEL_LOW_Msk               ((uint32_t)0x01000000)
#define IO_BANK0_PROC_INTS_GPIO6_LEVEL_LOW                   IO_BANK0_PROC_INTS_GPIO6_LEVEL_LOW_Msk
#define IO_BANK0_PROC_INTS_GPIO5_EDGE_HIGH_Pos               23
#define IO_BANK0_PROC_INTS_GPIO5_EDGE_HIGH_Msk               ((uint32_t)0x00800000)
#define IO_BANK0_PROC_INTS_GPIO5_EDGE_HIGH                   IO_BANK0_PROC_INTS_GPIO5_EDGE_HIGH_Msk
#define IO_BANK0_PROC_INTS_GPIO5_EDGE_LOW_Pos                22
#define IO_BANK0_PROC_INTS_GPIO5_EDGE_LOW_Msk                ((uint32_t)0x00400000)
#define IO_BANK0_PROC_INTS_GPIO5_EDGE_LOW                    IO_BANK0_PROC_INTS_GPIO5_EDGE_LOW_Msk
#define IO_BANK0_PROC_INTS_GPIO5_LEVEL_HIGH_Pos              21
#define IO_BANK0_PROC_INTS_GPIO5_LEVEL_HIGH_Msk              ((uint32_t)0x00200000)
#define IO_BANK0_PROC_INTS_GPIO5_LEVEL_HIGH                  IO_BANK0_PROC_INTS_GPIO5_LEVEL_HIGH_Msk
#define IO_BANK0_PROC_INTS_GPIO5_LEVEL_LOW_Pos               20
#define IO_BANK0_PROC_INTS_GPIO5_LEVEL_LOW_Msk               ((uint32_t)0x00100000)
#define IO_BANK0_PROC_INTS_GPIO5_LEVEL_LOW                   IO_BANK0_PROC_INTS_GPIO5_LEVEL_LOW_Msk
#define IO_BANK0_PROC_INTS_GPIO4_EDGE_HIGH_Pos               19
#define IO_BANK0_PROC_INTS_GPIO4_EDGE_HIGH_Msk               ((uint32_t)0x00080000)
#define IO_BANK0_PROC_INTS_GPIO4_EDGE_HIGH                   IO_BANK0_PROC_INTS_GPIO4_EDGE_HIGH_Msk
#define IO_BANK0_PROC_INTS_GPIO4_EDGE_LOW_Pos                18
#define IO_BANK0_PROC_INTS_GPIO4_EDGE_LOW_Msk                ((uint32_t)0x00040000)
#define IO_BANK0_PROC_INTS_GPIO4_EDGE_LOW                    IO_BANK0_PROC_INTS_GPIO4_EDGE_LOW_Msk
#define IO_BANK0_PROC_INTS_GPIO4_LEVEL_HIGH_Pos              17
#define IO_BANK0_PROC_INTS_GPIO4_LEVEL_HIGH_Msk              ((uint32_t)0x00020000)
#define IO_BANK0_PROC_INTS_GPIO4_LEVEL_HIGH                  IO_BANK0_PROC_INTS_GPIO4_LEVEL_HIGH_Msk
#define IO_BANK0_PROC_INTS_GPIO4_LEVEL_LOW_Pos               16
#define IO_BANK0_PROC_INTS_GPIO4_LEVEL_LOW_Msk               ((uint32_t)0x00010000)
#define IO_BANK0_PROC_INTS_GPIO4_LEVEL_LOW                   IO_BANK0_PROC_INTS_GPIO4_LEVEL_LOW_Msk
#define IO_BANK0_PROC_INTS_GPIO3_EDGE_HIGH_Pos               15
#define IO_BANK0_PROC_INTS_GPIO3_EDGE_HIGH_Msk               ((uint32_t)0x00008000)
#define IO_BANK0_PROC_INTS_GPIO3_EDGE_HIGH                   IO_BANK0_PROC_INTS_GPIO3_EDGE_HIGH_Msk
#define IO_BANK0_PROC_INTS_GPIO3_EDGE_LOW_Pos                14
#define IO_BANK0_PROC_INTS_GPIO3_EDGE_LOW_Msk                ((uint32_t)0x00004000)
#define IO_BANK0_PROC_INTS_GPIO3_EDGE_LOW                    IO_BANK0_PROC_INTS_GPIO3_EDGE_LOW_Msk
#define IO_BANK0_PROC_INTS_GPIO3_LEVEL_HIGH_Pos              13
#define IO_BANK0_PROC_INTS_GPIO3_LEVEL_HIGH_Msk              ((uint32_t)0x00002000)
#define IO_BANK0_PROC_INTS_GPIO3_LEVEL_HIGH                  IO_BANK0_PROC_INTS_GPIO3_LEVEL_HIGH_Msk
#define IO_BANK0_PROC_INTS_GPIO3_LEVEL_LOW_Pos               12
#define IO_BANK0_PROC_INTS_GPIO3_LEVEL_LOW_Msk               ((uint32_t)0x00001000)
#define IO_BANK0_PROC_INTS_GPIO3_LEVEL_LOW                   IO_BANK0_PROC_INTS_GPIO3_LEVEL_LOW_Msk
#define IO_BANK0_PROC_INTS_GPIO2_EDGE_HIGH_Pos               11
#define IO_BANK0_PROC_INTS_GPIO2_EDGE_HIGH_Msk               ((uint32_t)0x00000800)
#define IO_BANK0_PROC_INTS_GPIO2_EDGE_HIGH                   IO_BANK0_PROC_INTS_GPIO2_EDGE_HIGH_Msk
#define IO_BANK0_PROC_INTS_GPIO2_EDGE_LOW_Pos                10
#define IO_BANK0_PROC_INTS_GPIO2_EDGE_LOW_Msk                ((uint32_t)0x00000400)
#define IO_BANK0_PROC_INTS_GPIO2_EDGE_LOW                    IO_BANK0_PROC_INTS_GPIO2_EDGE_LOW_Msk
#define IO_BANK0_PROC_INTS_GPIO2_LEVEL_HIGH_Pos              9
#define IO_BANK0_PROC_INTS_GPIO2_LEVEL_HIGH_Msk              ((uint32_t)0x00000200)
#define IO_BANK0_PROC_INTS_GPIO2_LEVEL_HIGH                  IO_BANK0_PROC_INTS_GPIO2_LEVEL_HIGH_Msk
#define IO_BANK0_PROC_INTS_GPIO2_LEVEL_LOW_Pos               8
#define IO_BANK0_PROC_INTS_GPIO2_LEVEL_LOW_Msk               ((uint32_t)0x00000100)
#define IO_BANK0_PROC_INTS_GPIO2_LEVEL_LOW                   IO_BANK0_PROC_INTS_GPIO2_LEVEL_LOW_Msk
#define IO_BANK0_PROC_INTS_GPIO1_EDGE_HIGH_Pos               7
#define IO_BANK0_PROC_INTS_GPIO1_EDGE_HIGH_Msk               ((uint32_t)0x00000080)
#define IO_BANK0_PROC_INTS_GPIO1_EDGE_HIGH                   IO_BANK0_PROC_INTS_GPIO1_EDGE_HIGH_Msk
#define IO_BANK0_PROC_INTS_GPIO1_EDGE_LOW_Pos                6
#define IO_BANK0_PROC_INTS_GPIO1_EDGE_LOW_Msk                ((uint32_t)0x00000040)
#define IO_BANK0_PROC_INTS_GPIO1_EDGE_LOW                    IO_BANK0_PROC_INTS_GPIO1_EDGE_LOW_Msk
#define IO_BANK0_PROC_INTS_GPIO1_LEVEL_HIGH_Pos              5
#define IO_BANK0_PROC_INTS_GPIO1_LEVEL_HIGH_Msk              ((uint32_t)0x00000020)
#define IO_BANK0_PROC_INTS_GPIO1_LEVEL_HIGH                  IO_BANK0_PROC_INTS_GPIO1_LEVEL_HIGH_Msk
#define IO_BANK0_PROC_INTS_GPIO1_LEVEL_LOW_Pos               4
#define IO_BANK0_PROC_INTS_GPIO1_LEVEL_LOW_Msk               ((uint32_t)0x00000010)
#define IO_BANK0_PROC_INTS_GPIO1_LEVEL_LOW                   IO_BANK0_PROC_INTS_GPIO1_LEVEL_LOW_Msk
#define IO_BANK0_PROC_INTS_GPIO0_EDGE_HIGH_Pos               3
#define IO_BANK0_PROC_INTS_GPIO0_EDGE_HIGH_Msk               ((uint32_t)0x00000008)
#define IO_BANK0_PROC_INTS_GPIO0_EDGE_HIGH                   IO_BANK0_PROC_INTS_GPIO0_EDGE_HIGH_Msk
#define IO_BANK0_PROC_INTS_GPIO0_EDGE_LOW_Pos                2
#define IO_BANK0_PROC_INTS_GPIO0_EDGE_LOW_Msk                ((uint32_t)0x00000004)
#define IO_BANK0_PROC_INTS_GPIO0_EDGE_LOW                    IO_BANK0_PROC_INTS_GPIO0_EDGE_LOW_Msk
#define IO_BANK0_PROC_INTS_GPIO0_LEVEL_HIGH_Pos              1
#define IO_BANK0_PROC_INTS_GPIO0_LEVEL_HIGH_Msk              ((uint32_t)0x00000002)
#define IO_BANK0_PROC_INTS_GPIO0_LEVEL_HIGH                  IO_BANK0_PROC_INTS_GPIO0_LEVEL_HIGH_Msk
#define IO_BANK0_PROC_INTS_GPIO0_LEVEL_LOW_Pos               0
#define IO_BANK0_PROC_INTS_GPIO0_LEVEL_LOW_Msk               ((uint32_t)0x00000001)
#define IO_BANK0_PROC_INTS_GPIO0_LEVEL_LOW                   IO_BANK0_PROC_INTS_GPIO0_LEVEL_LOW_Msk
// IO_BANK0->DORMANT_WAKE_INTE
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO7_EDGE_HIGH_Pos       31
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO7_EDGE_HIGH_Msk       ((uint32_t)0x80000000)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO7_EDGE_HIGH           IO_BANK0_DORMANT_WAKE_INTE_GPIO7_EDGE_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO7_EDGE_LOW_Pos        30
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO7_EDGE_LOW_Msk        ((uint32_t)0x40000000)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO7_EDGE_LOW            IO_BANK0_DORMANT_WAKE_INTE_GPIO7_EDGE_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO7_LEVEL_HIGH_Pos      29
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO7_LEVEL_HIGH_Msk      ((uint32_t)0x20000000)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO7_LEVEL_HIGH          IO_BANK0_DORMANT_WAKE_INTE_GPIO7_LEVEL_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO7_LEVEL_LOW_Pos       28
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO7_LEVEL_LOW_Msk       ((uint32_t)0x10000000)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO7_LEVEL_LOW           IO_BANK0_DORMANT_WAKE_INTE_GPIO7_LEVEL_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO6_EDGE_HIGH_Pos       27
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO6_EDGE_HIGH_Msk       ((uint32_t)0x08000000)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO6_EDGE_HIGH           IO_BANK0_DORMANT_WAKE_INTE_GPIO6_EDGE_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO6_EDGE_LOW_Pos        26
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO6_EDGE_LOW_Msk        ((uint32_t)0x04000000)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO6_EDGE_LOW            IO_BANK0_DORMANT_WAKE_INTE_GPIO6_EDGE_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO6_LEVEL_HIGH_Pos      25
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO6_LEVEL_HIGH_Msk      ((uint32_t)0x02000000)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO6_LEVEL_HIGH          IO_BANK0_DORMANT_WAKE_INTE_GPIO6_LEVEL_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO6_LEVEL_LOW_Pos       24
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO6_LEVEL_LOW_Msk       ((uint32_t)0x01000000)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO6_LEVEL_LOW           IO_BANK0_DORMANT_WAKE_INTE_GPIO6_LEVEL_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO5_EDGE_HIGH_Pos       23
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO5_EDGE_HIGH_Msk       ((uint32_t)0x00800000)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO5_EDGE_HIGH           IO_BANK0_DORMANT_WAKE_INTE_GPIO5_EDGE_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO5_EDGE_LOW_Pos        22
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO5_EDGE_LOW_Msk        ((uint32_t)0x00400000)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO5_EDGE_LOW            IO_BANK0_DORMANT_WAKE_INTE_GPIO5_EDGE_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO5_LEVEL_HIGH_Pos      21
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO5_LEVEL_HIGH_Msk      ((uint32_t)0x00200000)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO5_LEVEL_HIGH          IO_BANK0_DORMANT_WAKE_INTE_GPIO5_LEVEL_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO5_LEVEL_LOW_Pos       20
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO5_LEVEL_LOW_Msk       ((uint32_t)0x00100000)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO5_LEVEL_LOW           IO_BANK0_DORMANT_WAKE_INTE_GPIO5_LEVEL_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO4_EDGE_HIGH_Pos       19
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO4_EDGE_HIGH_Msk       ((uint32_t)0x00080000)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO4_EDGE_HIGH           IO_BANK0_DORMANT_WAKE_INTE_GPIO4_EDGE_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO4_EDGE_LOW_Pos        18
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO4_EDGE_LOW_Msk        ((uint32_t)0x00040000)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO4_EDGE_LOW            IO_BANK0_DORMANT_WAKE_INTE_GPIO4_EDGE_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO4_LEVEL_HIGH_Pos      17
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO4_LEVEL_HIGH_Msk      ((uint32_t)0x00020000)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO4_LEVEL_HIGH          IO_BANK0_DORMANT_WAKE_INTE_GPIO4_LEVEL_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO4_LEVEL_LOW_Pos       16
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO4_LEVEL_LOW_Msk       ((uint32_t)0x00010000)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO4_LEVEL_LOW           IO_BANK0_DORMANT_WAKE_INTE_GPIO4_LEVEL_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO3_EDGE_HIGH_Pos       15
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO3_EDGE_HIGH_Msk       ((uint32_t)0x00008000)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO3_EDGE_HIGH           IO_BANK0_DORMANT_WAKE_INTE_GPIO3_EDGE_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO3_EDGE_LOW_Pos        14
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO3_EDGE_LOW_Msk        ((uint32_t)0x00004000)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO3_EDGE_LOW            IO_BANK0_DORMANT_WAKE_INTE_GPIO3_EDGE_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO3_LEVEL_HIGH_Pos      13
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO3_LEVEL_HIGH_Msk      ((uint32_t)0x00002000)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO3_LEVEL_HIGH          IO_BANK0_DORMANT_WAKE_INTE_GPIO3_LEVEL_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO3_LEVEL_LOW_Pos       12
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO3_LEVEL_LOW_Msk       ((uint32_t)0x00001000)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO3_LEVEL_LOW           IO_BANK0_DORMANT_WAKE_INTE_GPIO3_LEVEL_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO2_EDGE_HIGH_Pos       11
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO2_EDGE_HIGH_Msk       ((uint32_t)0x00000800)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO2_EDGE_HIGH           IO_BANK0_DORMANT_WAKE_INTE_GPIO2_EDGE_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO2_EDGE_LOW_Pos        10
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO2_EDGE_LOW_Msk        ((uint32_t)0x00000400)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO2_EDGE_LOW            IO_BANK0_DORMANT_WAKE_INTE_GPIO2_EDGE_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO2_LEVEL_HIGH_Pos      9
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO2_LEVEL_HIGH_Msk      ((uint32_t)0x00000200)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO2_LEVEL_HIGH          IO_BANK0_DORMANT_WAKE_INTE_GPIO2_LEVEL_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO2_LEVEL_LOW_Pos       8
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO2_LEVEL_LOW_Msk       ((uint32_t)0x00000100)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO2_LEVEL_LOW           IO_BANK0_DORMANT_WAKE_INTE_GPIO2_LEVEL_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO1_EDGE_HIGH_Pos       7
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO1_EDGE_HIGH_Msk       ((uint32_t)0x00000080)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO1_EDGE_HIGH           IO_BANK0_DORMANT_WAKE_INTE_GPIO1_EDGE_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO1_EDGE_LOW_Pos        6
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO1_EDGE_LOW_Msk        ((uint32_t)0x00000040)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO1_EDGE_LOW            IO_BANK0_DORMANT_WAKE_INTE_GPIO1_EDGE_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO1_LEVEL_HIGH_Pos      5
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO1_LEVEL_HIGH_Msk      ((uint32_t)0x00000020)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO1_LEVEL_HIGH          IO_BANK0_DORMANT_WAKE_INTE_GPIO1_LEVEL_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO1_LEVEL_LOW_Pos       4
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO1_LEVEL_LOW_Msk       ((uint32_t)0x00000010)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO1_LEVEL_LOW           IO_BANK0_DORMANT_WAKE_INTE_GPIO1_LEVEL_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO0_EDGE_HIGH_Pos       3
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO0_EDGE_HIGH_Msk       ((uint32_t)0x00000008)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO0_EDGE_HIGH           IO_BANK0_DORMANT_WAKE_INTE_GPIO0_EDGE_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO0_EDGE_LOW_Pos        2
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO0_EDGE_LOW_Msk        ((uint32_t)0x00000004)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO0_EDGE_LOW            IO_BANK0_DORMANT_WAKE_INTE_GPIO0_EDGE_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO0_LEVEL_HIGH_Pos      1
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO0_LEVEL_HIGH_Msk      ((uint32_t)0x00000002)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO0_LEVEL_HIGH          IO_BANK0_DORMANT_WAKE_INTE_GPIO0_LEVEL_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO0_LEVEL_LOW_Pos       0
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO0_LEVEL_LOW_Msk       ((uint32_t)0x00000001)
#define IO_BANK0_DORMANT_WAKE_INTE_GPIO0_LEVEL_LOW           IO_BANK0_DORMANT_WAKE_INTE_GPIO0_LEVEL_LOW_Msk
// IO_BANK0->DORMANT_WAKE_INTF
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO7_EDGE_HIGH_Pos       31
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO7_EDGE_HIGH_Msk       ((uint32_t)0x80000000)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO7_EDGE_HIGH           IO_BANK0_DORMANT_WAKE_INTF_GPIO7_EDGE_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO7_EDGE_LOW_Pos        30
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO7_EDGE_LOW_Msk        ((uint32_t)0x40000000)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO7_EDGE_LOW            IO_BANK0_DORMANT_WAKE_INTF_GPIO7_EDGE_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO7_LEVEL_HIGH_Pos      29
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO7_LEVEL_HIGH_Msk      ((uint32_t)0x20000000)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO7_LEVEL_HIGH          IO_BANK0_DORMANT_WAKE_INTF_GPIO7_LEVEL_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO7_LEVEL_LOW_Pos       28
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO7_LEVEL_LOW_Msk       ((uint32_t)0x10000000)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO7_LEVEL_LOW           IO_BANK0_DORMANT_WAKE_INTF_GPIO7_LEVEL_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO6_EDGE_HIGH_Pos       27
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO6_EDGE_HIGH_Msk       ((uint32_t)0x08000000)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO6_EDGE_HIGH           IO_BANK0_DORMANT_WAKE_INTF_GPIO6_EDGE_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO6_EDGE_LOW_Pos        26
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO6_EDGE_LOW_Msk        ((uint32_t)0x04000000)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO6_EDGE_LOW            IO_BANK0_DORMANT_WAKE_INTF_GPIO6_EDGE_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO6_LEVEL_HIGH_Pos      25
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO6_LEVEL_HIGH_Msk      ((uint32_t)0x02000000)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO6_LEVEL_HIGH          IO_BANK0_DORMANT_WAKE_INTF_GPIO6_LEVEL_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO6_LEVEL_LOW_Pos       24
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO6_LEVEL_LOW_Msk       ((uint32_t)0x01000000)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO6_LEVEL_LOW           IO_BANK0_DORMANT_WAKE_INTF_GPIO6_LEVEL_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO5_EDGE_HIGH_Pos       23
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO5_EDGE_HIGH_Msk       ((uint32_t)0x00800000)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO5_EDGE_HIGH           IO_BANK0_DORMANT_WAKE_INTF_GPIO5_EDGE_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO5_EDGE_LOW_Pos        22
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO5_EDGE_LOW_Msk        ((uint32_t)0x00400000)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO5_EDGE_LOW            IO_BANK0_DORMANT_WAKE_INTF_GPIO5_EDGE_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO5_LEVEL_HIGH_Pos      21
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO5_LEVEL_HIGH_Msk      ((uint32_t)0x00200000)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO5_LEVEL_HIGH          IO_BANK0_DORMANT_WAKE_INTF_GPIO5_LEVEL_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO5_LEVEL_LOW_Pos       20
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO5_LEVEL_LOW_Msk       ((uint32_t)0x00100000)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO5_LEVEL_LOW           IO_BANK0_DORMANT_WAKE_INTF_GPIO5_LEVEL_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO4_EDGE_HIGH_Pos       19
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO4_EDGE_HIGH_Msk       ((uint32_t)0x00080000)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO4_EDGE_HIGH           IO_BANK0_DORMANT_WAKE_INTF_GPIO4_EDGE_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO4_EDGE_LOW_Pos        18
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO4_EDGE_LOW_Msk        ((uint32_t)0x00040000)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO4_EDGE_LOW            IO_BANK0_DORMANT_WAKE_INTF_GPIO4_EDGE_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO4_LEVEL_HIGH_Pos      17
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO4_LEVEL_HIGH_Msk      ((uint32_t)0x00020000)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO4_LEVEL_HIGH          IO_BANK0_DORMANT_WAKE_INTF_GPIO4_LEVEL_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO4_LEVEL_LOW_Pos       16
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO4_LEVEL_LOW_Msk       ((uint32_t)0x00010000)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO4_LEVEL_LOW           IO_BANK0_DORMANT_WAKE_INTF_GPIO4_LEVEL_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO3_EDGE_HIGH_Pos       15
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO3_EDGE_HIGH_Msk       ((uint32_t)0x00008000)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO3_EDGE_HIGH           IO_BANK0_DORMANT_WAKE_INTF_GPIO3_EDGE_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO3_EDGE_LOW_Pos        14
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO3_EDGE_LOW_Msk        ((uint32_t)0x00004000)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO3_EDGE_LOW            IO_BANK0_DORMANT_WAKE_INTF_GPIO3_EDGE_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO3_LEVEL_HIGH_Pos      13
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO3_LEVEL_HIGH_Msk      ((uint32_t)0x00002000)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO3_LEVEL_HIGH          IO_BANK0_DORMANT_WAKE_INTF_GPIO3_LEVEL_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO3_LEVEL_LOW_Pos       12
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO3_LEVEL_LOW_Msk       ((uint32_t)0x00001000)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO3_LEVEL_LOW           IO_BANK0_DORMANT_WAKE_INTF_GPIO3_LEVEL_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO2_EDGE_HIGH_Pos       11
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO2_EDGE_HIGH_Msk       ((uint32_t)0x00000800)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO2_EDGE_HIGH           IO_BANK0_DORMANT_WAKE_INTF_GPIO2_EDGE_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO2_EDGE_LOW_Pos        10
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO2_EDGE_LOW_Msk        ((uint32_t)0x00000400)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO2_EDGE_LOW            IO_BANK0_DORMANT_WAKE_INTF_GPIO2_EDGE_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO2_LEVEL_HIGH_Pos      9
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO2_LEVEL_HIGH_Msk      ((uint32_t)0x00000200)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO2_LEVEL_HIGH          IO_BANK0_DORMANT_WAKE_INTF_GPIO2_LEVEL_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO2_LEVEL_LOW_Pos       8
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO2_LEVEL_LOW_Msk       ((uint32_t)0x00000100)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO2_LEVEL_LOW           IO_BANK0_DORMANT_WAKE_INTF_GPIO2_LEVEL_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO1_EDGE_HIGH_Pos       7
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO1_EDGE_HIGH_Msk       ((uint32_t)0x00000080)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO1_EDGE_HIGH           IO_BANK0_DORMANT_WAKE_INTF_GPIO1_EDGE_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO1_EDGE_LOW_Pos        6
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO1_EDGE_LOW_Msk        ((uint32_t)0x00000040)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO1_EDGE_LOW            IO_BANK0_DORMANT_WAKE_INTF_GPIO1_EDGE_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO1_LEVEL_HIGH_Pos      5
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO1_LEVEL_HIGH_Msk      ((uint32_t)0x00000020)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO1_LEVEL_HIGH          IO_BANK0_DORMANT_WAKE_INTF_GPIO1_LEVEL_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO1_LEVEL_LOW_Pos       4
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO1_LEVEL_LOW_Msk       ((uint32_t)0x00000010)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO1_LEVEL_LOW           IO_BANK0_DORMANT_WAKE_INTF_GPIO1_LEVEL_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO0_EDGE_HIGH_Pos       3
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO0_EDGE_HIGH_Msk       ((uint32_t)0x00000008)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO0_EDGE_HIGH           IO_BANK0_DORMANT_WAKE_INTF_GPIO0_EDGE_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO0_EDGE_LOW_Pos        2
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO0_EDGE_LOW_Msk        ((uint32_t)0x00000004)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO0_EDGE_LOW            IO_BANK0_DORMANT_WAKE_INTF_GPIO0_EDGE_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO0_LEVEL_HIGH_Pos      1
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO0_LEVEL_HIGH_Msk      ((uint32_t)0x00000002)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO0_LEVEL_HIGH          IO_BANK0_DORMANT_WAKE_INTF_GPIO0_LEVEL_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO0_LEVEL_LOW_Pos       0
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO0_LEVEL_LOW_Msk       ((uint32_t)0x00000001)
#define IO_BANK0_DORMANT_WAKE_INTF_GPIO0_LEVEL_LOW           IO_BANK0_DORMANT_WAKE_INTF_GPIO0_LEVEL_LOW_Msk
// IO_BANK0->DORMANT_WAKE_INTS
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO7_EDGE_HIGH_Pos       31
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO7_EDGE_HIGH_Msk       ((uint32_t)0x80000000)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO7_EDGE_HIGH           IO_BANK0_DORMANT_WAKE_INTS_GPIO7_EDGE_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO7_EDGE_LOW_Pos        30
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO7_EDGE_LOW_Msk        ((uint32_t)0x40000000)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO7_EDGE_LOW            IO_BANK0_DORMANT_WAKE_INTS_GPIO7_EDGE_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO7_LEVEL_HIGH_Pos      29
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO7_LEVEL_HIGH_Msk      ((uint32_t)0x20000000)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO7_LEVEL_HIGH          IO_BANK0_DORMANT_WAKE_INTS_GPIO7_LEVEL_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO7_LEVEL_LOW_Pos       28
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO7_LEVEL_LOW_Msk       ((uint32_t)0x10000000)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO7_LEVEL_LOW           IO_BANK0_DORMANT_WAKE_INTS_GPIO7_LEVEL_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO6_EDGE_HIGH_Pos       27
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO6_EDGE_HIGH_Msk       ((uint32_t)0x08000000)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO6_EDGE_HIGH           IO_BANK0_DORMANT_WAKE_INTS_GPIO6_EDGE_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO6_EDGE_LOW_Pos        26
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO6_EDGE_LOW_Msk        ((uint32_t)0x04000000)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO6_EDGE_LOW            IO_BANK0_DORMANT_WAKE_INTS_GPIO6_EDGE_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO6_LEVEL_HIGH_Pos      25
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO6_LEVEL_HIGH_Msk      ((uint32_t)0x02000000)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO6_LEVEL_HIGH          IO_BANK0_DORMANT_WAKE_INTS_GPIO6_LEVEL_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO6_LEVEL_LOW_Pos       24
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO6_LEVEL_LOW_Msk       ((uint32_t)0x01000000)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO6_LEVEL_LOW           IO_BANK0_DORMANT_WAKE_INTS_GPIO6_LEVEL_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO5_EDGE_HIGH_Pos       23
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO5_EDGE_HIGH_Msk       ((uint32_t)0x00800000)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO5_EDGE_HIGH           IO_BANK0_DORMANT_WAKE_INTS_GPIO5_EDGE_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO5_EDGE_LOW_Pos        22
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO5_EDGE_LOW_Msk        ((uint32_t)0x00400000)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO5_EDGE_LOW            IO_BANK0_DORMANT_WAKE_INTS_GPIO5_EDGE_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO5_LEVEL_HIGH_Pos      21
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO5_LEVEL_HIGH_Msk      ((uint32_t)0x00200000)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO5_LEVEL_HIGH          IO_BANK0_DORMANT_WAKE_INTS_GPIO5_LEVEL_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO5_LEVEL_LOW_Pos       20
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO5_LEVEL_LOW_Msk       ((uint32_t)0x00100000)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO5_LEVEL_LOW           IO_BANK0_DORMANT_WAKE_INTS_GPIO5_LEVEL_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO4_EDGE_HIGH_Pos       19
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO4_EDGE_HIGH_Msk       ((uint32_t)0x00080000)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO4_EDGE_HIGH           IO_BANK0_DORMANT_WAKE_INTS_GPIO4_EDGE_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO4_EDGE_LOW_Pos        18
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO4_EDGE_LOW_Msk        ((uint32_t)0x00040000)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO4_EDGE_LOW            IO_BANK0_DORMANT_WAKE_INTS_GPIO4_EDGE_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO4_LEVEL_HIGH_Pos      17
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO4_LEVEL_HIGH_Msk      ((uint32_t)0x00020000)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO4_LEVEL_HIGH          IO_BANK0_DORMANT_WAKE_INTS_GPIO4_LEVEL_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO4_LEVEL_LOW_Pos       16
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO4_LEVEL_LOW_Msk       ((uint32_t)0x00010000)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO4_LEVEL_LOW           IO_BANK0_DORMANT_WAKE_INTS_GPIO4_LEVEL_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO3_EDGE_HIGH_Pos       15
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO3_EDGE_HIGH_Msk       ((uint32_t)0x00008000)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO3_EDGE_HIGH           IO_BANK0_DORMANT_WAKE_INTS_GPIO3_EDGE_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO3_EDGE_LOW_Pos        14
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO3_EDGE_LOW_Msk        ((uint32_t)0x00004000)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO3_EDGE_LOW            IO_BANK0_DORMANT_WAKE_INTS_GPIO3_EDGE_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO3_LEVEL_HIGH_Pos      13
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO3_LEVEL_HIGH_Msk      ((uint32_t)0x00002000)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO3_LEVEL_HIGH          IO_BANK0_DORMANT_WAKE_INTS_GPIO3_LEVEL_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO3_LEVEL_LOW_Pos       12
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO3_LEVEL_LOW_Msk       ((uint32_t)0x00001000)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO3_LEVEL_LOW           IO_BANK0_DORMANT_WAKE_INTS_GPIO3_LEVEL_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO2_EDGE_HIGH_Pos       11
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO2_EDGE_HIGH_Msk       ((uint32_t)0x00000800)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO2_EDGE_HIGH           IO_BANK0_DORMANT_WAKE_INTS_GPIO2_EDGE_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO2_EDGE_LOW_Pos        10
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO2_EDGE_LOW_Msk        ((uint32_t)0x00000400)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO2_EDGE_LOW            IO_BANK0_DORMANT_WAKE_INTS_GPIO2_EDGE_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO2_LEVEL_HIGH_Pos      9
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO2_LEVEL_HIGH_Msk      ((uint32_t)0x00000200)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO2_LEVEL_HIGH          IO_BANK0_DORMANT_WAKE_INTS_GPIO2_LEVEL_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO2_LEVEL_LOW_Pos       8
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO2_LEVEL_LOW_Msk       ((uint32_t)0x00000100)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO2_LEVEL_LOW           IO_BANK0_DORMANT_WAKE_INTS_GPIO2_LEVEL_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO1_EDGE_HIGH_Pos       7
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO1_EDGE_HIGH_Msk       ((uint32_t)0x00000080)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO1_EDGE_HIGH           IO_BANK0_DORMANT_WAKE_INTS_GPIO1_EDGE_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO1_EDGE_LOW_Pos        6
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO1_EDGE_LOW_Msk        ((uint32_t)0x00000040)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO1_EDGE_LOW            IO_BANK0_DORMANT_WAKE_INTS_GPIO1_EDGE_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO1_LEVEL_HIGH_Pos      5
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO1_LEVEL_HIGH_Msk      ((uint32_t)0x00000020)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO1_LEVEL_HIGH          IO_BANK0_DORMANT_WAKE_INTS_GPIO1_LEVEL_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO1_LEVEL_LOW_Pos       4
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO1_LEVEL_LOW_Msk       ((uint32_t)0x00000010)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO1_LEVEL_LOW           IO_BANK0_DORMANT_WAKE_INTS_GPIO1_LEVEL_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO0_EDGE_HIGH_Pos       3
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO0_EDGE_HIGH_Msk       ((uint32_t)0x00000008)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO0_EDGE_HIGH           IO_BANK0_DORMANT_WAKE_INTS_GPIO0_EDGE_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO0_EDGE_LOW_Pos        2
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO0_EDGE_LOW_Msk        ((uint32_t)0x00000004)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO0_EDGE_LOW            IO_BANK0_DORMANT_WAKE_INTS_GPIO0_EDGE_LOW_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO0_LEVEL_HIGH_Pos      1
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO0_LEVEL_HIGH_Msk      ((uint32_t)0x00000002)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO0_LEVEL_HIGH          IO_BANK0_DORMANT_WAKE_INTS_GPIO0_LEVEL_HIGH_Msk
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO0_LEVEL_LOW_Pos       0
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO0_LEVEL_LOW_Msk       ((uint32_t)0x00000001)
#define IO_BANK0_DORMANT_WAKE_INTS_GPIO0_LEVEL_LOW           IO_BANK0_DORMANT_WAKE_INTS_GPIO0_LEVEL_LOW_Msk

// IO_QSPI->GPIO_QSPI_SCLK_STATUS
#define IO_QSPI_GPIO_QSPI_SCLK_STATUS_IRQTOPROC_Pos          26
#define IO_QSPI_GPIO_QSPI_SCLK_STATUS_IRQTOPROC_Msk          ((uint32_t)0x04000000)
#define IO_QSPI_GPIO_QSPI_SCLK_STATUS_IRQTOPROC              IO_QSPI_GPIO_QSPI_SCLK_STATUS_IRQTOPROC_Msk
#define IO_QSPI_GPIO_QSPI_SCLK_STATUS_IRQFROMPAD_Pos         24
#define IO_QSPI_GPIO_QSPI_SCLK_STATUS_IRQFROMPAD_Msk         ((uint32_t)0x01000000)
#define IO_QSPI_GPIO_QSPI_SCLK_STATUS_IRQFROMPAD             IO_QSPI_GPIO_QSPI_SCLK_STATUS_IRQFROMPAD_Msk
#define IO_QSPI_GPIO_QSPI_SCLK_STATUS_INTOPERI_Pos           19
#define IO_QSPI_GPIO_QSPI_SCLK_STATUS_INTOPERI_Msk           ((uint32_t)0x00080000)
#define IO_QSPI_GPIO_QSPI_SCLK_STATUS_INTOPERI               IO_QSPI_GPIO_QSPI_SCLK_STATUS_INTOPERI_Msk
#define IO_QSPI_GPIO_QSPI_SCLK_STATUS_INFROMPAD_Pos          17
#define IO_QSPI_GPIO_QSPI_SCLK_STATUS_INFROMPAD_Msk          ((uint32_t)0x00020000)
#define IO_QSPI_GPIO_QSPI_SCLK_STATUS_INFROMPAD              IO_QSPI_GPIO_QSPI_SCLK_STATUS_INFROMPAD_Msk
#define IO_QSPI_GPIO_QSPI_SCLK_STATUS_OETOPAD_Pos            13
#define IO_QSPI_GPIO_QSPI_SCLK_STATUS_OETOPAD_Msk            ((uint32_t)0x00002000)
#define IO_QSPI_GPIO_QSPI_SCLK_STATUS_OETOPAD                IO_QSPI_GPIO_QSPI_SCLK_STATUS_OETOPAD_Msk
#define IO_QSPI_GPIO_QSPI_SCLK_STATUS_OEFROMPERI_Pos         12
#define IO_QSPI_GPIO_QSPI_SCLK_STATUS_OEFROMPERI_Msk         ((uint32_t)0x00001000)
#define IO_QSPI_GPIO_QSPI_SCLK_STATUS_OEFROMPERI             IO_QSPI_GPIO_QSPI_SCLK_STATUS_OEFROMPERI_Msk
#define IO_QSPI_GPIO_QSPI_SCLK_STATUS_OUTTOPAD_Pos           9
#define IO_QSPI_GPIO_QSPI_SCLK_STATUS_OUTTOPAD_Msk           ((uint32_t)0x00000200)
#define IO_QSPI_GPIO_QSPI_SCLK_STATUS_OUTTOPAD               IO_QSPI_GPIO_QSPI_SCLK_STATUS_OUTTOPAD_Msk
#define IO_QSPI_GPIO_QSPI_SCLK_STATUS_OUTFROMPERI_Pos        8
#define IO_QSPI_GPIO_QSPI_SCLK_STATUS_OUTFROMPERI_Msk        ((uint32_t)0x00000100)
#define IO_QSPI_GPIO_QSPI_SCLK_STATUS_OUTFROMPERI            IO_QSPI_GPIO_QSPI_SCLK_STATUS_OUTFROMPERI_Msk
// IO_QSPI->GPIO_QSPI_SCLK_CTRL
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_IRQOVER_Pos              28
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_IRQOVER_Msk              ((uint32_t)0x30000000)
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_IRQOVER                  IO_QSPI_GPIO_QSPI_SCLK_CTRL_IRQOVER_Msk
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_IRQOVER_NORMAL           ((uint32_t)0x00000000)
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_IRQOVER_INVERT           ((uint32_t)0x10000000)
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_IRQOVER_LOW              ((uint32_t)0x20000000)
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_IRQOVER_HIGH             ((uint32_t)0x30000000)
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_INOVER_Pos               16
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_INOVER_Msk               ((uint32_t)0x00030000)
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_INOVER                   IO_QSPI_GPIO_QSPI_SCLK_CTRL_INOVER_Msk
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_INOVER_NORMAL            ((uint32_t)0x00000000)
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_INOVER_INVERT            ((uint32_t)0x00010000)
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_INOVER_LOW               ((uint32_t)0x00020000)
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_INOVER_HIGH              ((uint32_t)0x00030000)
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_OEOVER_Pos               12
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_OEOVER_Msk               ((uint32_t)0x00003000)
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_OEOVER                   IO_QSPI_GPIO_QSPI_SCLK_CTRL_OEOVER_Msk
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_OEOVER_NORMAL            ((uint32_t)0x00000000)
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_OEOVER_INVERT            ((uint32_t)0x00001000)
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_OEOVER_DISABLE           ((uint32_t)0x00002000)
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_OEOVER_ENABLE            ((uint32_t)0x00003000)
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_OUTOVER_Pos              8
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_OUTOVER_Msk              ((uint32_t)0x00000300)
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_OUTOVER                  IO_QSPI_GPIO_QSPI_SCLK_CTRL_OUTOVER_Msk
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_OUTOVER_NORMAL           ((uint32_t)0x00000000)
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_OUTOVER_INVERT           ((uint32_t)0x00000100)
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_OUTOVER_LOW              ((uint32_t)0x00000200)
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_OUTOVER_HIGH             ((uint32_t)0x00000300)
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_FUNCSEL_Pos              0
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_FUNCSEL_Msk              ((uint32_t)0x0000001F)
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_FUNCSEL                  IO_QSPI_GPIO_QSPI_SCLK_CTRL_FUNCSEL_Msk
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_FUNCSEL_XIP_SCLK         ((uint32_t)0x00000000)
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_FUNCSEL_SIO_30           ((uint32_t)0x00000005)
#define IO_QSPI_GPIO_QSPI_SCLK_CTRL_FUNCSEL_NULL             ((uint32_t)0x0000001F)
// IO_QSPI->GPIO_QSPI_SS_STATUS
#define IO_QSPI_GPIO_QSPI_SS_STATUS_IRQTOPROC_Pos            26
#define IO_QSPI_GPIO_QSPI_SS_STATUS_IRQTOPROC_Msk            ((uint32_t)0x04000000)
#define IO_QSPI_GPIO_QSPI_SS_STATUS_IRQTOPROC                IO_QSPI_GPIO_QSPI_SS_STATUS_IRQTOPROC_Msk
#define IO_QSPI_GPIO_QSPI_SS_STATUS_IRQFROMPAD_Pos           24
#define IO_QSPI_GPIO_QSPI_SS_STATUS_IRQFROMPAD_Msk           ((uint32_t)0x01000000)
#define IO_QSPI_GPIO_QSPI_SS_STATUS_IRQFROMPAD               IO_QSPI_GPIO_QSPI_SS_STATUS_IRQFROMPAD_Msk
#define IO_QSPI_GPIO_QSPI_SS_STATUS_INTOPERI_Pos             19
#define IO_QSPI_GPIO_QSPI_SS_STATUS_INTOPERI_Msk             ((uint32_t)0x00080000)
#define IO_QSPI_GPIO_QSPI_SS_STATUS_INTOPERI                 IO_QSPI_GPIO_QSPI_SS_STATUS_INTOPERI_Msk
#define IO_QSPI_GPIO_QSPI_SS_STATUS_INFROMPAD_Pos            17
#define IO_QSPI_GPIO_QSPI_SS_STATUS_INFROMPAD_Msk            ((uint32_t)0x00020000)
#define IO_QSPI_GPIO_QSPI_SS_STATUS_INFROMPAD                IO_QSPI_GPIO_QSPI_SS_STATUS_INFROMPAD_Msk
#define IO_QSPI_GPIO_QSPI_SS_STATUS_OETOPAD_Pos              13
#define IO_QSPI_GPIO_QSPI_SS_STATUS_OETOPAD_Msk              ((uint32_t)0x00002000)
#define IO_QSPI_GPIO_QSPI_SS_STATUS_OETOPAD                  IO_QSPI_GPIO_QSPI_SS_STATUS_OETOPAD_Msk
#define IO_QSPI_GPIO_QSPI_SS_STATUS_OEFROMPERI_Pos           12
#define IO_QSPI_GPIO_QSPI_SS_STATUS_OEFROMPERI_Msk           ((uint32_t)0x00001000)
#define IO_QSPI_GPIO_QSPI_SS_STATUS_OEFROMPERI               IO_QSPI_GPIO_QSPI_SS_STATUS_OEFROMPERI_Msk
#define IO_QSPI_GPIO_QSPI_SS_STATUS_OUTTOPAD_Pos             9
#define IO_QSPI_GPIO_QSPI_SS_STATUS_OUTTOPAD_Msk             ((uint32_t)0x00000200)
#define IO_QSPI_GPIO_QSPI_SS_STATUS_OUTTOPAD                 IO_QSPI_GPIO_QSPI_SS_STATUS_OUTTOPAD_Msk
#define IO_QSPI_GPIO_QSPI_SS_STATUS_OUTFROMPERI_Pos          8
#define IO_QSPI_GPIO_QSPI_SS_STATUS_OUTFROMPERI_Msk          ((uint32_t)0x00000100)
#define IO_QSPI_GPIO_QSPI_SS_STATUS_OUTFROMPERI              IO_QSPI_GPIO_QSPI_SS_STATUS_OUTFROMPERI_Msk
// IO_QSPI->GPIO_QSPI_SS_CTRL
#define IO_QSPI_GPIO_QSPI_SS_CTRL_IRQOVER_Pos                28
#define IO_QSPI_GPIO_QSPI_SS_CTRL_IRQOVER_Msk                ((uint32_t)0x30000000)
#define IO_QSPI_GPIO_QSPI_SS_CTRL_IRQOVER                    IO_QSPI_GPIO_QSPI_SS_CTRL_IRQOVER_Msk
#define IO_QSPI_GPIO_QSPI_SS_CTRL_IRQOVER_NORMAL             ((uint32_t)0x00000000)
#define IO_QSPI_GPIO_QSPI_SS_CTRL_IRQOVER_INVERT             ((uint32_t)0x10000000)
#define IO_QSPI_GPIO_QSPI_SS_CTRL_IRQOVER_LOW                ((uint32_t)0x20000000)
#define IO_QSPI_GPIO_QSPI_SS_CTRL_IRQOVER_HIGH               ((uint32_t)0x30000000)
#define IO_QSPI_GPIO_QSPI_SS_CTRL_INOVER_Pos                 16
#define IO_QSPI_GPIO_QSPI_SS_CTRL_INOVER_Msk                 ((uint32_t)0x00030000)
#define IO_QSPI_GPIO_QSPI_SS_CTRL_INOVER                     IO_QSPI_GPIO_QSPI_SS_CTRL_INOVER_Msk
#define IO_QSPI_GPIO_QSPI_SS_CTRL_INOVER_NORMAL              ((uint32_t)0x00000000)
#define IO_QSPI_GPIO_QSPI_SS_CTRL_INOVER_INVERT              ((uint32_t)0x00010000)
#define IO_QSPI_GPIO_QSPI_SS_CTRL_INOVER_LOW                 ((uint32_t)0x00020000)
#define IO_QSPI_GPIO_QSPI_SS_CTRL_INOVER_HIGH                ((uint32_t)0x00030000)
#define IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_Pos                 12
#define IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_Msk                 ((uint32_t)0x00003000)
#define IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER                     IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_Msk
#define IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_NORMAL              ((uint32_t)0x00000000)
#define IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_INVERT              ((uint32_t)0x00001000)
#define IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_DISABLE             ((uint32_t)0x00002000)
#define IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_ENABLE              ((uint32_t)0x00003000)
#define IO_QSPI_GPIO_QSPI_SS_CTRL_OUTOVER_Pos                8
#define IO_QSPI_GPIO_QSPI_SS_CTRL_OUTOVER_Msk                ((uint32_t)0x00000300)
#define IO_QSPI_GPIO_QSPI_SS_CTRL_OUTOVER                    IO_QSPI_GPIO_QSPI_SS_CTRL_OUTOVER_Msk
#define IO_QSPI_GPIO_QSPI_SS_CTRL_OUTOVER_NORMAL             ((uint32_t)0x00000000)
#define IO_QSPI_GPIO_QSPI_SS_CTRL_OUTOVER_INVERT             ((uint32_t)0x00000100)
#define IO_QSPI_GPIO_QSPI_SS_CTRL_OUTOVER_LOW                ((uint32_t)0x00000200)
#define IO_QSPI_GPIO_QSPI_SS_CTRL_OUTOVER_HIGH               ((uint32_t)0x00000300)
#define IO_QSPI_GPIO_QSPI_SS_CTRL_FUNCSEL_Pos                0
#define IO_QSPI_GPIO_QSPI_SS_CTRL_FUNCSEL_Msk                ((uint32_t)0x0000001F)
#define IO_QSPI_GPIO_QSPI_SS_CTRL_FUNCSEL                    IO_QSPI_GPIO_QSPI_SS_CTRL_FUNCSEL_Msk
#define IO_QSPI_GPIO_QSPI_SS_CTRL_FUNCSEL_XIP_SS_N           ((uint32_t)0x00000000)
#define IO_QSPI_GPIO_QSPI_SS_CTRL_FUNCSEL_SIO_31             ((uint32_t)0x00000005)
#define IO_QSPI_GPIO_QSPI_SS_CTRL_FUNCSEL_NULL               ((uint32_t)0x0000001F)
// IO_QSPI->GPIO_QSPI_SD[n].STATUS
#define IO_QSPI_GPIO_QSPI_SD_STATUS_IRQTOPROC_Pos            26
#define IO_QSPI_GPIO_QSPI_SD_STATUS_IRQTOPROC_Msk            ((uint32_t)0x04000000)
#define IO_QSPI_GPIO_QSPI_SD_STATUS_IRQTOPROC                IO_QSPI_GPIO_QSPI_SD_STATUS_IRQTOPROC_Msk
#define IO_QSPI_GPIO_QSPI_SD_STATUS_IRQFROMPAD_Pos           24
#define IO_QSPI_GPIO_QSPI_SD_STATUS_IRQFROMPAD_Msk           ((uint32_t)0x01000000)
#define IO_QSPI_GPIO_QSPI_SD_STATUS_IRQFROMPAD               IO_QSPI_GPIO_QSPI_SD_STATUS_IRQFROMPAD_Msk
#define IO_QSPI_GPIO_QSPI_SD_STATUS_INTOPERI_Pos             19
#define IO_QSPI_GPIO_QSPI_SD_STATUS_INTOPERI_Msk             ((uint32_t)0x00080000)
#define IO_QSPI_GPIO_QSPI_SD_STATUS_INTOPERI                 IO_QSPI_GPIO_QSPI_SD_STATUS_INTOPERI_Msk
#define IO_QSPI_GPIO_QSPI_SD_STATUS_INFROMPAD_Pos            17
#define IO_QSPI_GPIO_QSPI_SD_STATUS_INFROMPAD_Msk            ((uint32_t)0x00020000)
#define IO_QSPI_GPIO_QSPI_SD_STATUS_INFROMPAD                IO_QSPI_GPIO_QSPI_SD_STATUS_INFROMPAD_Msk
#define IO_QSPI_GPIO_QSPI_SD_STATUS_OETOPAD_Pos              13
#define IO_QSPI_GPIO_QSPI_SD_STATUS_OETOPAD_Msk              ((uint32_t)0x00002000)
#define IO_QSPI_GPIO_QSPI_SD_STATUS_OETOPAD                  IO_QSPI_GPIO_QSPI_SD_STATUS_OETOPAD_Msk
#define IO_QSPI_GPIO_QSPI_SD_STATUS_OEFROMPERI_Pos           12
#define IO_QSPI_GPIO_QSPI_SD_STATUS_OEFROMPERI_Msk           ((uint32_t)0x00001000)
#define IO_QSPI_GPIO_QSPI_SD_STATUS_OEFROMPERI               IO_QSPI_GPIO_QSPI_SD_STATUS_OEFROMPERI_Msk
#define IO_QSPI_GPIO_QSPI_SD_STATUS_OUTTOPAD_Pos             9
#define IO_QSPI_GPIO_QSPI_SD_STATUS_OUTTOPAD_Msk             ((uint32_t)0x00000200)
#define IO_QSPI_GPIO_QSPI_SD_STATUS_OUTTOPAD                 IO_QSPI_GPIO_QSPI_SD_STATUS_OUTTOPAD_Msk
#define IO_QSPI_GPIO_QSPI_SD_STATUS_OUTFROMPERI_Pos          8
#define IO_QSPI_GPIO_QSPI_SD_STATUS_OUTFROMPERI_Msk          ((uint32_t)0x00000100)
#define IO_QSPI_GPIO_QSPI_SD_STATUS_OUTFROMPERI              IO_QSPI_GPIO_QSPI_SD_STATUS_OUTFROMPERI_Msk
// IO_QSPI->GPIO_QSPI_SD[n].CTRL
#define IO_QSPI_GPIO_QSPI_SD_CTRL_IRQOVER_Pos                28
#define IO_QSPI_GPIO_QSPI_SD_CTRL_IRQOVER_Msk                ((uint32_t)0x30000000)
#define IO_QSPI_GPIO_QSPI_SD_CTRL_IRQOVER                    IO_QSPI_GPIO_QSPI_SD_CTRL_IRQOVER_Msk
#define IO_QSPI_GPIO_QSPI_SD_CTRL_INOVER_Pos                 16
#define IO_QSPI_GPIO_QSPI_SD_CTRL_INOVER_Msk                 ((uint32_t)0x00030000)
#define IO_QSPI_GPIO_QSPI_SD_CTRL_INOVER                     IO_QSPI_GPIO_QSPI_SD_CTRL_INOVER_Msk
#define IO_QSPI_GPIO_QSPI_SD_CTRL_OEOVER_Pos                 12
#define IO_QSPI_GPIO_QSPI_SD_CTRL_OEOVER_Msk                 ((uint32_t)0x00003000)
#define IO_QSPI_GPIO_QSPI_SD_CTRL_OEOVER                     IO_QSPI_GPIO_QSPI_SD_CTRL_OEOVER_Msk
#define IO_QSPI_GPIO_QSPI_SD_CTRL_OUTOVER_Pos                8
#define IO_QSPI_GPIO_QSPI_SD_CTRL_OUTOVER_Msk                ((uint32_t)0x00000300)
#define IO_QSPI_GPIO_QSPI_SD_CTRL_OUTOVER                    IO_QSPI_GPIO_QSPI_SD_CTRL_OUTOVER_Msk
#define IO_QSPI_GPIO_QSPI_SD_CTRL_FUNCSEL_Pos                0
#define IO_QSPI_GPIO_QSPI_SD_CTRL_FUNCSEL_Msk                ((uint32_t)0x0000001F)
#define IO_QSPI_GPIO_QSPI_SD_CTRL_FUNCSEL                    IO_QSPI_GPIO_QSPI_SD_CTRL_FUNCSEL_Msk
// IO_QSPI->INTR
#define IO_QSPI_INTR_GPIO_QSPI_SD3_EDGE_HIGH_Pos             23
#define IO_QSPI_INTR_GPIO_QSPI_SD3_EDGE_HIGH_Msk             ((uint32_t)0x00800000)
#define IO_QSPI_INTR_GPIO_QSPI_SD3_EDGE_HIGH                 IO_QSPI_INTR_GPIO_QSPI_SD3_EDGE_HIGH_Msk
#define IO_QSPI_INTR_GPIO_QSPI_SD3_EDGE_LOW_Pos              22
#define IO_QSPI_INTR_GPIO_QSPI_SD3_EDGE_LOW_Msk              ((uint32_t)0x00400000)
#define IO_QSPI_INTR_GPIO_QSPI_SD3_EDGE_LOW                  IO_QSPI_INTR_GPIO_QSPI_SD3_EDGE_LOW_Msk
#define IO_QSPI_INTR_GPIO_QSPI_SD3_LEVEL_HIGH_Pos            21
#define IO_QSPI_INTR_GPIO_QSPI_SD3_LEVEL_HIGH_Msk            ((uint32_t)0x00200000)
#define IO_QSPI_INTR_GPIO_QSPI_SD3_LEVEL_HIGH                IO_QSPI_INTR_GPIO_QSPI_SD3_LEVEL_HIGH_Msk
#define IO_QSPI_INTR_GPIO_QSPI_SD3_LEVEL_LOW_Pos             20
#define IO_QSPI_INTR_GPIO_QSPI_SD3_LEVEL_LOW_Msk             ((uint32_t)0x00100000)
#define IO_QSPI_INTR_GPIO_QSPI_SD3_LEVEL_LOW                 IO_QSPI_INTR_GPIO_QSPI_SD3_LEVEL_LOW_Msk
#define IO_QSPI_INTR_GPIO_QSPI_SD2_EDGE_HIGH_Pos             19
#define IO_QSPI_INTR_GPIO_QSPI_SD2_EDGE_HIGH_Msk             ((uint32_t)0x00080000)
#define IO_QSPI_INTR_GPIO_QSPI_SD2_EDGE_HIGH                 IO_QSPI_INTR_GPIO_QSPI_SD2_EDGE_HIGH_Msk
#define IO_QSPI_INTR_GPIO_QSPI_SD2_EDGE_LOW_Pos              18
#define IO_QSPI_INTR_GPIO_QSPI_SD2_EDGE_LOW_Msk              ((uint32_t)0x00040000)
#define IO_QSPI_INTR_GPIO_QSPI_SD2_EDGE_LOW                  IO_QSPI_INTR_GPIO_QSPI_SD2_EDGE_LOW_Msk
#define IO_QSPI_INTR_GPIO_QSPI_SD2_LEVEL_HIGH_Pos            17
#define IO_QSPI_INTR_GPIO_QSPI_SD2_LEVEL_HIGH_Msk            ((uint32_t)0x00020000)
#define IO_QSPI_INTR_GPIO_QSPI_SD2_LEVEL_HIGH                IO_QSPI_INTR_GPIO_QSPI_SD2_LEVEL_HIGH_Msk
#define IO_QSPI_INTR_GPIO_QSPI_SD2_LEVEL_LOW_Pos             16
#define IO_QSPI_INTR_GPIO_QSPI_SD2_LEVEL_LOW_Msk             ((uint32_t)0x00010000)
#define IO_QSPI_INTR_GPIO_QSPI_SD2_LEVEL_LOW                 IO_QSPI_INTR_GPIO_QSPI_SD2_LEVEL_LOW_Msk
#define IO_QSPI_INTR_GPIO_QSPI_SD1_EDGE_HIGH_Pos             15
#define IO_QSPI_INTR_GPIO_QSPI_SD1_EDGE_HIGH_Msk             ((uint32_t)0x00008000)
#define IO_QSPI_INTR_GPIO_QSPI_SD1_EDGE_HIGH                 IO_QSPI_INTR_GPIO_QSPI_SD1_EDGE_HIGH_Msk
#define IO_QSPI_INTR_GPIO_QSPI_SD1_EDGE_LOW_Pos              14
#define IO_QSPI_INTR_GPIO_QSPI_SD1_EDGE_LOW_Msk              ((uint32_t)0x00004000)
#define IO_QSPI_INTR_GPIO_QSPI_SD1_EDGE_LOW                  IO_QSPI_INTR_GPIO_QSPI_SD1_EDGE_LOW_Msk
#define IO_QSPI_INTR_GPIO_QSPI_SD1_LEVEL_HIGH_Pos            13
#define IO_QSPI_INTR_GPIO_QSPI_SD1_LEVEL_HIGH_Msk            ((uint32_t)0x00002000)
#define IO_QSPI_INTR_GPIO_QSPI_SD1_LEVEL_HIGH                IO_QSPI_INTR_GPIO_QSPI_SD1_LEVEL_HIGH_Msk
#define IO_QSPI_INTR_GPIO_QSPI_SD1_LEVEL_LOW_Pos             12
#define IO_QSPI_INTR_GPIO_QSPI_SD1_LEVEL_LOW_Msk             ((uint32_t)0x00001000)
#define IO_QSPI_INTR_GPIO_QSPI_SD1_LEVEL_LOW                 IO_QSPI_INTR_GPIO_QSPI_SD1_LEVEL_LOW_Msk
#define IO_QSPI_INTR_GPIO_QSPI_SD0_EDGE_HIGH_Pos             11
#define IO_QSPI_INTR_GPIO_QSPI_SD0_EDGE_HIGH_Msk             ((uint32_t)0x00000800)
#define IO_QSPI_INTR_GPIO_QSPI_SD0_EDGE_HIGH                 IO_QSPI_INTR_GPIO_QSPI_SD0_EDGE_HIGH_Msk
#define IO_QSPI_INTR_GPIO_QSPI_SD0_EDGE_LOW_Pos              10
#define IO_QSPI_INTR_GPIO_QSPI_SD0_EDGE_LOW_Msk              ((uint32_t)0x00000400)
#define IO_QSPI_INTR_GPIO_QSPI_SD0_EDGE_LOW                  IO_QSPI_INTR_GPIO_QSPI_SD0_EDGE_LOW_Msk
#define IO_QSPI_INTR_GPIO_QSPI_SD0_LEVEL_HIGH_Pos            9
#define IO_QSPI_INTR_GPIO_QSPI_SD0_LEVEL_HIGH_Msk            ((uint32_t)0x00000200)
#define IO_QSPI_INTR_GPIO_QSPI_SD0_LEVEL_HIGH                IO_QSPI_INTR_GPIO_QSPI_SD0_LEVEL_HIGH_Msk
#define IO_QSPI_INTR_GPIO_QSPI_SD0_LEVEL_LOW_Pos             8
#define IO_QSPI_INTR_GPIO_QSPI_SD0_LEVEL_LOW_Msk             ((uint32_t)0x00000100)
#define IO_QSPI_INTR_GPIO_QSPI_SD0_LEVEL_LOW                 IO_QSPI_INTR_GPIO_QSPI_SD0_LEVEL_LOW_Msk
#define IO_QSPI_INTR_GPIO_QSPI_SS_EDGE_HIGH_Pos              7
#define IO_QSPI_INTR_GPIO_QSPI_SS_EDGE_HIGH_Msk              ((uint32_t)0x00000080)
#define IO_QSPI_INTR_GPIO_QSPI_SS_EDGE_HIGH                  IO_QSPI_INTR_GPIO_QSPI_SS_EDGE_HIGH_Msk
#define IO_QSPI_INTR_GPIO_QSPI_SS_EDGE_LOW_Pos               6
#define IO_QSPI_INTR_GPIO_QSPI_SS_EDGE_LOW_Msk               ((uint32_t)0x00000040)
#define IO_QSPI_INTR_GPIO_QSPI_SS_EDGE_LOW                   IO_QSPI_INTR_GPIO_QSPI_SS_EDGE_LOW_Msk
#define IO_QSPI_INTR_GPIO_QSPI_SS_LEVEL_HIGH_Pos             5
#define IO_QSPI_INTR_GPIO_QSPI_SS_LEVEL_HIGH_Msk             ((uint32_t)0x00000020)
#define IO_QSPI_INTR_GPIO_QSPI_SS_LEVEL_HIGH                 IO_QSPI_INTR_GPIO_QSPI_SS_LEVEL_HIGH_Msk
#define IO_QSPI_INTR_GPIO_QSPI_SS_LEVEL_LOW_Pos              4
#define IO_QSPI_INTR_GPIO_QSPI_SS_LEVEL_LOW_Msk              ((uint32_t)0x00000010)
#define IO_QSPI_INTR_GPIO_QSPI_SS_LEVEL_LOW                  IO_QSPI_INTR_GPIO_QSPI_SS_LEVEL_LOW_Msk
#define IO_QSPI_INTR_GPIO_QSPI_SCLK_EDGE_HIGH_Pos            3
#define IO_QSPI_INTR_GPIO_QSPI_SCLK_EDGE_HIGH_Msk            ((uint32_t)0x00000008)
#define IO_QSPI_INTR_GPIO_QSPI_SCLK_EDGE_HIGH                IO_QSPI_INTR_GPIO_QSPI_SCLK_EDGE_HIGH_Msk
#define IO_QSPI_INTR_GPIO_QSPI_SCLK_EDGE_LOW_Pos             2
#define IO_QSPI_INTR_GPIO_QSPI_SCLK_EDGE_LOW_Msk             ((uint32_t)0x00000004)
#define IO_QSPI_INTR_GPIO_QSPI_SCLK_EDGE_LOW                 IO_QSPI_INTR_GPIO_QSPI_SCLK_EDGE_LOW_Msk
#define IO_QSPI_INTR_GPIO_QSPI_SCLK_LEVEL_HIGH_Pos           1
#define IO_QSPI_INTR_GPIO_QSPI_SCLK_LEVEL_HIGH_Msk           ((uint32_t)0x00000002)
#define IO_QSPI_INTR_GPIO_QSPI_SCLK_LEVEL_HIGH               IO_QSPI_INTR_GPIO_QSPI_SCLK_LEVEL_HIGH_Msk
#define IO_QSPI_INTR_GPIO_QSPI_SCLK_LEVEL_LOW_Pos            0
#define IO_QSPI_INTR_GPIO_QSPI_SCLK_LEVEL_LOW_Msk            ((uint32_t)0x00000001)
#define IO_QSPI_INTR_GPIO_QSPI_SCLK_LEVEL_LOW                IO_QSPI_INTR_GPIO_QSPI_SCLK_LEVEL_LOW_Msk
// IO_QSPI->PROC[n].INTE
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD3_EDGE_HIGH_Pos        23
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD3_EDGE_HIGH_Msk        ((uint32_t)0x00800000)
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD3_EDGE_HIGH            IO_QSPI_PROC_INTE_GPIO_QSPI_SD3_EDGE_HIGH_Msk
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD3_EDGE_LOW_Pos         22
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD3_EDGE_LOW_Msk         ((uint32_t)0x00400000)
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD3_EDGE_LOW             IO_QSPI_PROC_INTE_GPIO_QSPI_SD3_EDGE_LOW_Msk
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD3_LEVEL_HIGH_Pos       21
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD3_LEVEL_HIGH_Msk       ((uint32_t)0x00200000)
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD3_LEVEL_HIGH           IO_QSPI_PROC_INTE_GPIO_QSPI_SD3_LEVEL_HIGH_Msk
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD3_LEVEL_LOW_Pos        20
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD3_LEVEL_LOW_Msk        ((uint32_t)0x00100000)
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD3_LEVEL_LOW            IO_QSPI_PROC_INTE_GPIO_QSPI_SD3_LEVEL_LOW_Msk
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD2_EDGE_HIGH_Pos        19
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD2_EDGE_HIGH_Msk        ((uint32_t)0x00080000)
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD2_EDGE_HIGH            IO_QSPI_PROC_INTE_GPIO_QSPI_SD2_EDGE_HIGH_Msk
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD2_EDGE_LOW_Pos         18
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD2_EDGE_LOW_Msk         ((uint32_t)0x00040000)
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD2_EDGE_LOW             IO_QSPI_PROC_INTE_GPIO_QSPI_SD2_EDGE_LOW_Msk
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD2_LEVEL_HIGH_Pos       17
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD2_LEVEL_HIGH_Msk       ((uint32_t)0x00020000)
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD2_LEVEL_HIGH           IO_QSPI_PROC_INTE_GPIO_QSPI_SD2_LEVEL_HIGH_Msk
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD2_LEVEL_LOW_Pos        16
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD2_LEVEL_LOW_Msk        ((uint32_t)0x00010000)
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD2_LEVEL_LOW            IO_QSPI_PROC_INTE_GPIO_QSPI_SD2_LEVEL_LOW_Msk
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD1_EDGE_HIGH_Pos        15
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD1_EDGE_HIGH_Msk        ((uint32_t)0x00008000)
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD1_EDGE_HIGH            IO_QSPI_PROC_INTE_GPIO_QSPI_SD1_EDGE_HIGH_Msk
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD1_EDGE_LOW_Pos         14
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD1_EDGE_LOW_Msk         ((uint32_t)0x00004000)
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD1_EDGE_LOW             IO_QSPI_PROC_INTE_GPIO_QSPI_SD1_EDGE_LOW_Msk
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD1_LEVEL_HIGH_Pos       13
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD1_LEVEL_HIGH_Msk       ((uint32_t)0x00002000)
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD1_LEVEL_HIGH           IO_QSPI_PROC_INTE_GPIO_QSPI_SD1_LEVEL_HIGH_Msk
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD1_LEVEL_LOW_Pos        12
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD1_LEVEL_LOW_Msk        ((uint32_t)0x00001000)
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD1_LEVEL_LOW            IO_QSPI_PROC_INTE_GPIO_QSPI_SD1_LEVEL_LOW_Msk
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD0_EDGE_HIGH_Pos        11
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD0_EDGE_HIGH_Msk        ((uint32_t)0x00000800)
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD0_EDGE_HIGH            IO_QSPI_PROC_INTE_GPIO_QSPI_SD0_EDGE_HIGH_Msk
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD0_EDGE_LOW_Pos         10
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD0_EDGE_LOW_Msk         ((uint32_t)0x00000400)
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD0_EDGE_LOW             IO_QSPI_PROC_INTE_GPIO_QSPI_SD0_EDGE_LOW_Msk
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD0_LEVEL_HIGH_Pos       9
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD0_LEVEL_HIGH_Msk       ((uint32_t)0x00000200)
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD0_LEVEL_HIGH           IO_QSPI_PROC_INTE_GPIO_QSPI_SD0_LEVEL_HIGH_Msk
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD0_LEVEL_LOW_Pos        8
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD0_LEVEL_LOW_Msk        ((uint32_t)0x00000100)
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SD0_LEVEL_LOW            IO_QSPI_PROC_INTE_GPIO_QSPI_SD0_LEVEL_LOW_Msk
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SS_EDGE_HIGH_Pos         7
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SS_EDGE_HIGH_Msk         ((uint32_t)0x00000080)
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SS_EDGE_HIGH             IO_QSPI_PROC_INTE_GPIO_QSPI_SS_EDGE_HIGH_Msk
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SS_EDGE_LOW_Pos          6
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SS_EDGE_LOW_Msk          ((uint32_t)0x00000040)
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SS_EDGE_LOW              IO_QSPI_PROC_INTE_GPIO_QSPI_SS_EDGE_LOW_Msk
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SS_LEVEL_HIGH_Pos        5
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SS_LEVEL_HIGH_Msk        ((uint32_t)0x00000020)
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SS_LEVEL_HIGH            IO_QSPI_PROC_INTE_GPIO_QSPI_SS_LEVEL_HIGH_Msk
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SS_LEVEL_LOW_Pos         4
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SS_LEVEL_LOW_Msk         ((uint32_t)0x00000010)
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SS_LEVEL_LOW             IO_QSPI_PROC_INTE_GPIO_QSPI_SS_LEVEL_LOW_Msk
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SCLK_EDGE_HIGH_Pos       3
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SCLK_EDGE_HIGH_Msk       ((uint32_t)0x00000008)
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SCLK_EDGE_HIGH           IO_QSPI_PROC_INTE_GPIO_QSPI_SCLK_EDGE_HIGH_Msk
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SCLK_EDGE_LOW_Pos        2
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SCLK_EDGE_LOW_Msk        ((uint32_t)0x00000004)
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SCLK_EDGE_LOW            IO_QSPI_PROC_INTE_GPIO_QSPI_SCLK_EDGE_LOW_Msk
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SCLK_LEVEL_HIGH_Pos      1
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SCLK_LEVEL_HIGH_Msk      ((uint32_t)0x00000002)
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SCLK_LEVEL_HIGH          IO_QSPI_PROC_INTE_GPIO_QSPI_SCLK_LEVEL_HIGH_Msk
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SCLK_LEVEL_LOW_Pos       0
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SCLK_LEVEL_LOW_Msk       ((uint32_t)0x00000001)
#define IO_QSPI_PROC_INTE_GPIO_QSPI_SCLK_LEVEL_LOW           IO_QSPI_PROC_INTE_GPIO_QSPI_SCLK_LEVEL_LOW_Msk
// IO_QSPI->PROC[n].INTF
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD3_EDGE_HIGH_Pos        23
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD3_EDGE_HIGH_Msk        ((uint32_t)0x00800000)
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD3_EDGE_HIGH            IO_QSPI_PROC_INTF_GPIO_QSPI_SD3_EDGE_HIGH_Msk
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD3_EDGE_LOW_Pos         22
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD3_EDGE_LOW_Msk         ((uint32_t)0x00400000)
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD3_EDGE_LOW             IO_QSPI_PROC_INTF_GPIO_QSPI_SD3_EDGE_LOW_Msk
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD3_LEVEL_HIGH_Pos       21
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD3_LEVEL_HIGH_Msk       ((uint32_t)0x00200000)
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD3_LEVEL_HIGH           IO_QSPI_PROC_INTF_GPIO_QSPI_SD3_LEVEL_HIGH_Msk
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD3_LEVEL_LOW_Pos        20
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD3_LEVEL_LOW_Msk        ((uint32_t)0x00100000)
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD3_LEVEL_LOW            IO_QSPI_PROC_INTF_GPIO_QSPI_SD3_LEVEL_LOW_Msk
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD2_EDGE_HIGH_Pos        19
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD2_EDGE_HIGH_Msk        ((uint32_t)0x00080000)
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD2_EDGE_HIGH            IO_QSPI_PROC_INTF_GPIO_QSPI_SD2_EDGE_HIGH_Msk
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD2_EDGE_LOW_Pos         18
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD2_EDGE_LOW_Msk         ((uint32_t)0x00040000)
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD2_EDGE_LOW             IO_QSPI_PROC_INTF_GPIO_QSPI_SD2_EDGE_LOW_Msk
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD2_LEVEL_HIGH_Pos       17
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD2_LEVEL_HIGH_Msk       ((uint32_t)0x00020000)
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD2_LEVEL_HIGH           IO_QSPI_PROC_INTF_GPIO_QSPI_SD2_LEVEL_HIGH_Msk
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD2_LEVEL_LOW_Pos        16
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD2_LEVEL_LOW_Msk        ((uint32_t)0x00010000)
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD2_LEVEL_LOW            IO_QSPI_PROC_INTF_GPIO_QSPI_SD2_LEVEL_LOW_Msk
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD1_EDGE_HIGH_Pos        15
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD1_EDGE_HIGH_Msk        ((uint32_t)0x00008000)
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD1_EDGE_HIGH            IO_QSPI_PROC_INTF_GPIO_QSPI_SD1_EDGE_HIGH_Msk
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD1_EDGE_LOW_Pos         14
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD1_EDGE_LOW_Msk         ((uint32_t)0x00004000)
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD1_EDGE_LOW             IO_QSPI_PROC_INTF_GPIO_QSPI_SD1_EDGE_LOW_Msk
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD1_LEVEL_HIGH_Pos       13
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD1_LEVEL_HIGH_Msk       ((uint32_t)0x00002000)
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD1_LEVEL_HIGH           IO_QSPI_PROC_INTF_GPIO_QSPI_SD1_LEVEL_HIGH_Msk
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD1_LEVEL_LOW_Pos        12
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD1_LEVEL_LOW_Msk        ((uint32_t)0x00001000)
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD1_LEVEL_LOW            IO_QSPI_PROC_INTF_GPIO_QSPI_SD1_LEVEL_LOW_Msk
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD0_EDGE_HIGH_Pos        11
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD0_EDGE_HIGH_Msk        ((uint32_t)0x00000800)
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD0_EDGE_HIGH            IO_QSPI_PROC_INTF_GPIO_QSPI_SD0_EDGE_HIGH_Msk
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD0_EDGE_LOW_Pos         10
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD0_EDGE_LOW_Msk         ((uint32_t)0x00000400)
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD0_EDGE_LOW             IO_QSPI_PROC_INTF_GPIO_QSPI_SD0_EDGE_LOW_Msk
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD0_LEVEL_HIGH_Pos       9
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD0_LEVEL_HIGH_Msk       ((uint32_t)0x00000200)
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD0_LEVEL_HIGH           IO_QSPI_PROC_INTF_GPIO_QSPI_SD0_LEVEL_HIGH_Msk
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD0_LEVEL_LOW_Pos        8
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD0_LEVEL_LOW_Msk        ((uint32_t)0x00000100)
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SD0_LEVEL_LOW            IO_QSPI_PROC_INTF_GPIO_QSPI_SD0_LEVEL_LOW_Msk
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SS_EDGE_HIGH_Pos         7
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SS_EDGE_HIGH_Msk         ((uint32_t)0x00000080)
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SS_EDGE_HIGH             IO_QSPI_PROC_INTF_GPIO_QSPI_SS_EDGE_HIGH_Msk
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SS_EDGE_LOW_Pos          6
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SS_EDGE_LOW_Msk          ((uint32_t)0x00000040)
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SS_EDGE_LOW              IO_QSPI_PROC_INTF_GPIO_QSPI_SS_EDGE_LOW_Msk
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SS_LEVEL_HIGH_Pos        5
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SS_LEVEL_HIGH_Msk        ((uint32_t)0x00000020)
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SS_LEVEL_HIGH            IO_QSPI_PROC_INTF_GPIO_QSPI_SS_LEVEL_HIGH_Msk
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SS_LEVEL_LOW_Pos         4
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SS_LEVEL_LOW_Msk         ((uint32_t)0x00000010)
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SS_LEVEL_LOW             IO_QSPI_PROC_INTF_GPIO_QSPI_SS_LEVEL_LOW_Msk
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SCLK_EDGE_HIGH_Pos       3
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SCLK_EDGE_HIGH_Msk       ((uint32_t)0x00000008)
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SCLK_EDGE_HIGH           IO_QSPI_PROC_INTF_GPIO_QSPI_SCLK_EDGE_HIGH_Msk
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SCLK_EDGE_LOW_Pos        2
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SCLK_EDGE_LOW_Msk        ((uint32_t)0x00000004)
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SCLK_EDGE_LOW            IO_QSPI_PROC_INTF_GPIO_QSPI_SCLK_EDGE_LOW_Msk
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SCLK_LEVEL_HIGH_Pos      1
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SCLK_LEVEL_HIGH_Msk      ((uint32_t)0x00000002)
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SCLK_LEVEL_HIGH          IO_QSPI_PROC_INTF_GPIO_QSPI_SCLK_LEVEL_HIGH_Msk
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SCLK_LEVEL_LOW_Pos       0
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SCLK_LEVEL_LOW_Msk       ((uint32_t)0x00000001)
#define IO_QSPI_PROC_INTF_GPIO_QSPI_SCLK_LEVEL_LOW           IO_QSPI_PROC_INTF_GPIO_QSPI_SCLK_LEVEL_LOW_Msk
// IO_QSPI->PROC[n].INTS
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD3_EDGE_HIGH_Pos        23
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD3_EDGE_HIGH_Msk        ((uint32_t)0x00800000)
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD3_EDGE_HIGH            IO_QSPI_PROC_INTS_GPIO_QSPI_SD3_EDGE_HIGH_Msk
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD3_EDGE_LOW_Pos         22
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD3_EDGE_LOW_Msk         ((uint32_t)0x00400000)
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD3_EDGE_LOW             IO_QSPI_PROC_INTS_GPIO_QSPI_SD3_EDGE_LOW_Msk
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD3_LEVEL_HIGH_Pos       21
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD3_LEVEL_HIGH_Msk       ((uint32_t)0x00200000)
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD3_LEVEL_HIGH           IO_QSPI_PROC_INTS_GPIO_QSPI_SD3_LEVEL_HIGH_Msk
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD3_LEVEL_LOW_Pos        20
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD3_LEVEL_LOW_Msk        ((uint32_t)0x00100000)
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD3_LEVEL_LOW            IO_QSPI_PROC_INTS_GPIO_QSPI_SD3_LEVEL_LOW_Msk
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD2_EDGE_HIGH_Pos        19
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD2_EDGE_HIGH_Msk        ((uint32_t)0x00080000)
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD2_EDGE_HIGH            IO_QSPI_PROC_INTS_GPIO_QSPI_SD2_EDGE_HIGH_Msk
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD2_EDGE_LOW_Pos         18
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD2_EDGE_LOW_Msk         ((uint32_t)0x00040000)
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD2_EDGE_LOW             IO_QSPI_PROC_INTS_GPIO_QSPI_SD2_EDGE_LOW_Msk
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD2_LEVEL_HIGH_Pos       17
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD2_LEVEL_HIGH_Msk       ((uint32_t)0x00020000)
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD2_LEVEL_HIGH           IO_QSPI_PROC_INTS_GPIO_QSPI_SD2_LEVEL_HIGH_Msk
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD2_LEVEL_LOW_Pos        16
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD2_LEVEL_LOW_Msk        ((uint32_t)0x00010000)
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD2_LEVEL_LOW            IO_QSPI_PROC_INTS_GPIO_QSPI_SD2_LEVEL_LOW_Msk
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD1_EDGE_HIGH_Pos        15
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD1_EDGE_HIGH_Msk        ((uint32_t)0x00008000)
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD1_EDGE_HIGH            IO_QSPI_PROC_INTS_GPIO_QSPI_SD1_EDGE_HIGH_Msk
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD1_EDGE_LOW_Pos         14
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD1_EDGE_LOW_Msk         ((uint32_t)0x00004000)
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD1_EDGE_LOW             IO_QSPI_PROC_INTS_GPIO_QSPI_SD1_EDGE_LOW_Msk
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD1_LEVEL_HIGH_Pos       13
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD1_LEVEL_HIGH_Msk       ((uint32_t)0x00002000)
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD1_LEVEL_HIGH           IO_QSPI_PROC_INTS_GPIO_QSPI_SD1_LEVEL_HIGH_Msk
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD1_LEVEL_LOW_Pos        12
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD1_LEVEL_LOW_Msk        ((uint32_t)0x00001000)
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD1_LEVEL_LOW            IO_QSPI_PROC_INTS_GPIO_QSPI_SD1_LEVEL_LOW_Msk
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD0_EDGE_HIGH_Pos        11
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD0_EDGE_HIGH_Msk        ((uint32_t)0x00000800)
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD0_EDGE_HIGH            IO_QSPI_PROC_INTS_GPIO_QSPI_SD0_EDGE_HIGH_Msk
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD0_EDGE_LOW_Pos         10
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD0_EDGE_LOW_Msk         ((uint32_t)0x00000400)
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD0_EDGE_LOW             IO_QSPI_PROC_INTS_GPIO_QSPI_SD0_EDGE_LOW_Msk
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD0_LEVEL_HIGH_Pos       9
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD0_LEVEL_HIGH_Msk       ((uint32_t)0x00000200)
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD0_LEVEL_HIGH           IO_QSPI_PROC_INTS_GPIO_QSPI_SD0_LEVEL_HIGH_Msk
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD0_LEVEL_LOW_Pos        8
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD0_LEVEL_LOW_Msk        ((uint32_t)0x00000100)
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SD0_LEVEL_LOW            IO_QSPI_PROC_INTS_GPIO_QSPI_SD0_LEVEL_LOW_Msk
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SS_EDGE_HIGH_Pos         7
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SS_EDGE_HIGH_Msk         ((uint32_t)0x00000080)
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SS_EDGE_HIGH             IO_QSPI_PROC_INTS_GPIO_QSPI_SS_EDGE_HIGH_Msk
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SS_EDGE_LOW_Pos          6
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SS_EDGE_LOW_Msk          ((uint32_t)0x00000040)
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SS_EDGE_LOW              IO_QSPI_PROC_INTS_GPIO_QSPI_SS_EDGE_LOW_Msk
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SS_LEVEL_HIGH_Pos        5
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SS_LEVEL_HIGH_Msk        ((uint32_t)0x00000020)
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SS_LEVEL_HIGH            IO_QSPI_PROC_INTS_GPIO_QSPI_SS_LEVEL_HIGH_Msk
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SS_LEVEL_LOW_Pos         4
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SS_LEVEL_LOW_Msk         ((uint32_t)0x00000010)
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SS_LEVEL_LOW             IO_QSPI_PROC_INTS_GPIO_QSPI_SS_LEVEL_LOW_Msk
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SCLK_EDGE_HIGH_Pos       3
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SCLK_EDGE_HIGH_Msk       ((uint32_t)0x00000008)
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SCLK_EDGE_HIGH           IO_QSPI_PROC_INTS_GPIO_QSPI_SCLK_EDGE_HIGH_Msk
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SCLK_EDGE_LOW_Pos        2
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SCLK_EDGE_LOW_Msk        ((uint32_t)0x00000004)
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SCLK_EDGE_LOW            IO_QSPI_PROC_INTS_GPIO_QSPI_SCLK_EDGE_LOW_Msk
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SCLK_LEVEL_HIGH_Pos      1
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SCLK_LEVEL_HIGH_Msk      ((uint32_t)0x00000002)
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SCLK_LEVEL_HIGH          IO_QSPI_PROC_INTS_GPIO_QSPI_SCLK_LEVEL_HIGH_Msk
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SCLK_LEVEL_LOW_Pos       0
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SCLK_LEVEL_LOW_Msk       ((uint32_t)0x00000001)
#define IO_QSPI_PROC_INTS_GPIO_QSPI_SCLK_LEVEL_LOW           IO_QSPI_PROC_INTS_GPIO_QSPI_SCLK_LEVEL_LOW_Msk
// IO_QSPI->DORMANT_WAKE_INTE
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD3_EDGE_HIGH_Pos 23
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD3_EDGE_HIGH_Msk ((uint32_t)0x00800000)
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD3_EDGE_HIGH    IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD3_EDGE_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD3_EDGE_LOW_Pos 22
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD3_EDGE_LOW_Msk ((uint32_t)0x00400000)
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD3_EDGE_LOW     IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD3_EDGE_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD3_LEVEL_HIGH_Pos 21
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD3_LEVEL_HIGH_Msk ((uint32_t)0x00200000)
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD3_LEVEL_HIGH   IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD3_LEVEL_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD3_LEVEL_LOW_Pos 20
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD3_LEVEL_LOW_Msk ((uint32_t)0x00100000)
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD3_LEVEL_LOW    IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD3_LEVEL_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD2_EDGE_HIGH_Pos 19
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD2_EDGE_HIGH_Msk ((uint32_t)0x00080000)
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD2_EDGE_HIGH    IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD2_EDGE_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD2_EDGE_LOW_Pos 18
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD2_EDGE_LOW_Msk ((uint32_t)0x00040000)
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD2_EDGE_LOW     IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD2_EDGE_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD2_LEVEL_HIGH_Pos 17
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD2_LEVEL_HIGH_Msk ((uint32_t)0x00020000)
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD2_LEVEL_HIGH   IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD2_LEVEL_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD2_LEVEL_LOW_Pos 16
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD2_LEVEL_LOW_Msk ((uint32_t)0x00010000)
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD2_LEVEL_LOW    IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD2_LEVEL_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD1_EDGE_HIGH_Pos 15
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD1_EDGE_HIGH_Msk ((uint32_t)0x00008000)
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD1_EDGE_HIGH    IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD1_EDGE_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD1_EDGE_LOW_Pos 14
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD1_EDGE_LOW_Msk ((uint32_t)0x00004000)
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD1_EDGE_LOW     IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD1_EDGE_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD1_LEVEL_HIGH_Pos 13
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD1_LEVEL_HIGH_Msk ((uint32_t)0x00002000)
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD1_LEVEL_HIGH   IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD1_LEVEL_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD1_LEVEL_LOW_Pos 12
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD1_LEVEL_LOW_Msk ((uint32_t)0x00001000)
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD1_LEVEL_LOW    IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD1_LEVEL_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD0_EDGE_HIGH_Pos 11
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD0_EDGE_HIGH_Msk ((uint32_t)0x00000800)
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD0_EDGE_HIGH    IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD0_EDGE_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD0_EDGE_LOW_Pos 10
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD0_EDGE_LOW_Msk ((uint32_t)0x00000400)
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD0_EDGE_LOW     IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD0_EDGE_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD0_LEVEL_HIGH_Pos 9
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD0_LEVEL_HIGH_Msk ((uint32_t)0x00000200)
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD0_LEVEL_HIGH   IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD0_LEVEL_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD0_LEVEL_LOW_Pos 8
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD0_LEVEL_LOW_Msk ((uint32_t)0x00000100)
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD0_LEVEL_LOW    IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SD0_LEVEL_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SS_EDGE_HIGH_Pos 7
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SS_EDGE_HIGH_Msk ((uint32_t)0x00000080)
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SS_EDGE_HIGH     IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SS_EDGE_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SS_EDGE_LOW_Pos  6
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SS_EDGE_LOW_Msk  ((uint32_t)0x00000040)
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SS_EDGE_LOW      IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SS_EDGE_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SS_LEVEL_HIGH_Pos 5
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SS_LEVEL_HIGH_Msk ((uint32_t)0x00000020)
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SS_LEVEL_HIGH    IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SS_LEVEL_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SS_LEVEL_LOW_Pos 4
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SS_LEVEL_LOW_Msk ((uint32_t)0x00000010)
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SS_LEVEL_LOW     IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SS_LEVEL_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SCLK_EDGE_HIGH_Pos 3
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SCLK_EDGE_HIGH_Msk ((uint32_t)0x00000008)
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SCLK_EDGE_HIGH   IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SCLK_EDGE_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SCLK_EDGE_LOW_Pos 2
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SCLK_EDGE_LOW_Msk ((uint32_t)0x00000004)
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SCLK_EDGE_LOW    IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SCLK_EDGE_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SCLK_LEVEL_HIGH_Pos 1
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SCLK_LEVEL_HIGH_Msk ((uint32_t)0x00000002)
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SCLK_LEVEL_HIGH  IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SCLK_LEVEL_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SCLK_LEVEL_LOW_Pos 0
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SCLK_LEVEL_LOW_Msk ((uint32_t)0x00000001)
#define IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SCLK_LEVEL_LOW   IO_QSPI_DORMANT_WAKE_INTE_GPIO_QSPI_SCLK_LEVEL_LOW_Msk
// IO_QSPI->DORMANT_WAKE_INTF
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD3_EDGE_HIGH_Pos 23
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD3_EDGE_HIGH_Msk ((uint32_t)0x00800000)
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD3_EDGE_HIGH    IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD3_EDGE_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD3_EDGE_LOW_Pos 22
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD3_EDGE_LOW_Msk ((uint32_t)0x00400000)
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD3_EDGE_LOW     IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD3_EDGE_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD3_LEVEL_HIGH_Pos 21
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD3_LEVEL_HIGH_Msk ((uint32_t)0x00200000)
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD3_LEVEL_HIGH   IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD3_LEVEL_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD3_LEVEL_LOW_Pos 20
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD3_LEVEL_LOW_Msk ((uint32_t)0x00100000)
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD3_LEVEL_LOW    IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD3_LEVEL_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD2_EDGE_HIGH_Pos 19
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD2_EDGE_HIGH_Msk ((uint32_t)0x00080000)
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD2_EDGE_HIGH    IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD2_EDGE_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD2_EDGE_LOW_Pos 18
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD2_EDGE_LOW_Msk ((uint32_t)0x00040000)
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD2_EDGE_LOW     IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD2_EDGE_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD2_LEVEL_HIGH_Pos 17
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD2_LEVEL_HIGH_Msk ((uint32_t)0x00020000)
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD2_LEVEL_HIGH   IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD2_LEVEL_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD2_LEVEL_LOW_Pos 16
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD2_LEVEL_LOW_Msk ((uint32_t)0x00010000)
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD2_LEVEL_LOW    IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD2_LEVEL_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD1_EDGE_HIGH_Pos 15
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD1_EDGE_HIGH_Msk ((uint32_t)0x00008000)
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD1_EDGE_HIGH    IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD1_EDGE_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD1_EDGE_LOW_Pos 14
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD1_EDGE_LOW_Msk ((uint32_t)0x00004000)
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD1_EDGE_LOW     IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD1_EDGE_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD1_LEVEL_HIGH_Pos 13
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD1_LEVEL_HIGH_Msk ((uint32_t)0x00002000)
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD1_LEVEL_HIGH   IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD1_LEVEL_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD1_LEVEL_LOW_Pos 12
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD1_LEVEL_LOW_Msk ((uint32_t)0x00001000)
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD1_LEVEL_LOW    IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD1_LEVEL_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD0_EDGE_HIGH_Pos 11
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD0_EDGE_HIGH_Msk ((uint32_t)0x00000800)
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD0_EDGE_HIGH    IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD0_EDGE_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD0_EDGE_LOW_Pos 10
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD0_EDGE_LOW_Msk ((uint32_t)0x00000400)
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD0_EDGE_LOW     IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD0_EDGE_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD0_LEVEL_HIGH_Pos 9
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD0_LEVEL_HIGH_Msk ((uint32_t)0x00000200)
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD0_LEVEL_HIGH   IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD0_LEVEL_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD0_LEVEL_LOW_Pos 8
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD0_LEVEL_LOW_Msk ((uint32_t)0x00000100)
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD0_LEVEL_LOW    IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SD0_LEVEL_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SS_EDGE_HIGH_Pos 7
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SS_EDGE_HIGH_Msk ((uint32_t)0x00000080)
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SS_EDGE_HIGH     IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SS_EDGE_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SS_EDGE_LOW_Pos  6
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SS_EDGE_LOW_Msk  ((uint32_t)0x00000040)
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SS_EDGE_LOW      IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SS_EDGE_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SS_LEVEL_HIGH_Pos 5
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SS_LEVEL_HIGH_Msk ((uint32_t)0x00000020)
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SS_LEVEL_HIGH    IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SS_LEVEL_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SS_LEVEL_LOW_Pos 4
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SS_LEVEL_LOW_Msk ((uint32_t)0x00000010)
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SS_LEVEL_LOW     IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SS_LEVEL_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SCLK_EDGE_HIGH_Pos 3
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SCLK_EDGE_HIGH_Msk ((uint32_t)0x00000008)
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SCLK_EDGE_HIGH   IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SCLK_EDGE_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SCLK_EDGE_LOW_Pos 2
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SCLK_EDGE_LOW_Msk ((uint32_t)0x00000004)
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SCLK_EDGE_LOW    IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SCLK_EDGE_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SCLK_LEVEL_HIGH_Pos 1
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SCLK_LEVEL_HIGH_Msk ((uint32_t)0x00000002)
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SCLK_LEVEL_HIGH  IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SCLK_LEVEL_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SCLK_LEVEL_LOW_Pos 0
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SCLK_LEVEL_LOW_Msk ((uint32_t)0x00000001)
#define IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SCLK_LEVEL_LOW   IO_QSPI_DORMANT_WAKE_INTF_GPIO_QSPI_SCLK_LEVEL_LOW_Msk
// IO_QSPI->DORMANT_WAKE_INTS
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD3_EDGE_HIGH_Pos 23
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD3_EDGE_HIGH_Msk ((uint32_t)0x00800000)
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD3_EDGE_HIGH    IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD3_EDGE_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD3_EDGE_LOW_Pos 22
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD3_EDGE_LOW_Msk ((uint32_t)0x00400000)
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD3_EDGE_LOW     IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD3_EDGE_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD3_LEVEL_HIGH_Pos 21
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD3_LEVEL_HIGH_Msk ((uint32_t)0x00200000)
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD3_LEVEL_HIGH   IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD3_LEVEL_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD3_LEVEL_LOW_Pos 20
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD3_LEVEL_LOW_Msk ((uint32_t)0x00100000)
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD3_LEVEL_LOW    IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD3_LEVEL_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD2_EDGE_HIGH_Pos 19
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD2_EDGE_HIGH_Msk ((uint32_t)0x00080000)
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD2_EDGE_HIGH    IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD2_EDGE_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD2_EDGE_LOW_Pos 18
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD2_EDGE_LOW_Msk ((uint32_t)0x00040000)
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD2_EDGE_LOW     IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD2_EDGE_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD2_LEVEL_HIGH_Pos 17
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD2_LEVEL_HIGH_Msk ((uint32_t)0x00020000)
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD2_LEVEL_HIGH   IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD2_LEVEL_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD2_LEVEL_LOW_Pos 16
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD2_LEVEL_LOW_Msk ((uint32_t)0x00010000)
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD2_LEVEL_LOW    IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD2_LEVEL_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD1_EDGE_HIGH_Pos 15
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD1_EDGE_HIGH_Msk ((uint32_t)0x00008000)
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD1_EDGE_HIGH    IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD1_EDGE_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD1_EDGE_LOW_Pos 14
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD1_EDGE_LOW_Msk ((uint32_t)0x00004000)
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD1_EDGE_LOW     IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD1_EDGE_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD1_LEVEL_HIGH_Pos 13
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD1_LEVEL_HIGH_Msk ((uint32_t)0x00002000)
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD1_LEVEL_HIGH   IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD1_LEVEL_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD1_LEVEL_LOW_Pos 12
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD1_LEVEL_LOW_Msk ((uint32_t)0x00001000)
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD1_LEVEL_LOW    IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD1_LEVEL_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD0_EDGE_HIGH_Pos 11
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD0_EDGE_HIGH_Msk ((uint32_t)0x00000800)
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD0_EDGE_HIGH    IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD0_EDGE_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD0_EDGE_LOW_Pos 10
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD0_EDGE_LOW_Msk ((uint32_t)0x00000400)
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD0_EDGE_LOW     IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD0_EDGE_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD0_LEVEL_HIGH_Pos 9
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD0_LEVEL_HIGH_Msk ((uint32_t)0x00000200)
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD0_LEVEL_HIGH   IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD0_LEVEL_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD0_LEVEL_LOW_Pos 8
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD0_LEVEL_LOW_Msk ((uint32_t)0x00000100)
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD0_LEVEL_LOW    IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SD0_LEVEL_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SS_EDGE_HIGH_Pos 7
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SS_EDGE_HIGH_Msk ((uint32_t)0x00000080)
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SS_EDGE_HIGH     IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SS_EDGE_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SS_EDGE_LOW_Pos  6
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SS_EDGE_LOW_Msk  ((uint32_t)0x00000040)
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SS_EDGE_LOW      IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SS_EDGE_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SS_LEVEL_HIGH_Pos 5
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SS_LEVEL_HIGH_Msk ((uint32_t)0x00000020)
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SS_LEVEL_HIGH    IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SS_LEVEL_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SS_LEVEL_LOW_Pos 4
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SS_LEVEL_LOW_Msk ((uint32_t)0x00000010)
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SS_LEVEL_LOW     IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SS_LEVEL_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SCLK_EDGE_HIGH_Pos 3
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SCLK_EDGE_HIGH_Msk ((uint32_t)0x00000008)
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SCLK_EDGE_HIGH   IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SCLK_EDGE_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SCLK_EDGE_LOW_Pos 2
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SCLK_EDGE_LOW_Msk ((uint32_t)0x00000004)
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SCLK_EDGE_LOW    IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SCLK_EDGE_LOW_Msk
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SCLK_LEVEL_HIGH_Pos 1
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SCLK_LEVEL_HIGH_Msk ((uint32_t)0x00000002)
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SCLK_LEVEL_HIGH  IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SCLK_LEVEL_HIGH_Msk
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SCLK_LEVEL_LOW_Pos 0
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SCLK_LEVEL_LOW_Msk ((uint32_t)0x00000001)
#define IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SCLK_LEVEL_LOW   IO_QSPI_DORMANT_WAKE_INTS_GPIO_QSPI_SCLK_LEVEL_LOW_Msk

// PADS_BANK0->VOLTAGE_SELECT
#define PADS_BANK0_VOLTAGE_SELECT_VOLTAGE_SELECT_Pos         0
#define PADS_BANK0_VOLTAGE_SELECT_VOLTAGE_SELECT_Msk         ((uint32_t)0x00000001)
#define PADS_BANK0_VOLTAGE_SELECT_VOLTAGE_SELECT             PADS_BANK0_VOLTAGE_SELECT_VOLTAGE_SELECT_Msk
#define PADS_BANK0_VOLTAGE_SELECT_VOLTAGE_SELECT_3V3         ((uint32_t)0x00000000)
#define PADS_BANK0_VOLTAGE_SELECT_VOLTAGE_SELECT_1V8         ((uint32_t)0x00000001)
// PADS_BANK0->GPIO
#define PADS_BANK0_GPIO_OD_Pos                               7
#define PADS_BANK0_GPIO_OD_Msk                               ((uint32_t)0x00000080)
#define PADS_BANK0_GPIO_OD                                   PADS_BANK0_GPIO_OD_Msk
#define PADS_BANK0_GPIO_IE_Pos                               6
#define PADS_BANK0_GPIO_IE_Msk                               ((uint32_t)0x00000040)
#define PADS_BANK0_GPIO_IE                                   PADS_BANK0_GPIO_IE_Msk
#define PADS_BANK0_GPIO_DRIVE_Pos                            4
#define PADS_BANK0_GPIO_DRIVE_Msk                            ((uint32_t)0x00000030)
#define PADS_BANK0_GPIO_DRIVE                                PADS_BANK0_GPIO_DRIVE_Msk
#define PADS_BANK0_GPIO_PUE_Pos                              3
#define PADS_BANK0_GPIO_PUE_Msk                              ((uint32_t)0x00000008)
#define PADS_BANK0_GPIO_PUE                                  PADS_BANK0_GPIO_PUE_Msk
#define PADS_BANK0_GPIO_PDE_Pos                              2
#define PADS_BANK0_GPIO_PDE_Msk                              ((uint32_t)0x00000004)
#define PADS_BANK0_GPIO_PDE                                  PADS_BANK0_GPIO_PDE_Msk
#define PADS_BANK0_GPIO_SCHMITT_Pos                          1
#define PADS_BANK0_GPIO_SCHMITT_Msk                          ((uint32_t)0x00000002)
#define PADS_BANK0_GPIO_SCHMITT                              PADS_BANK0_GPIO_SCHMITT_Msk
#define PADS_BANK0_GPIO_SLEWFAST_Pos                         0
#define PADS_BANK0_GPIO_SLEWFAST_Msk                         ((uint32_t)0x00000001)
#define PADS_BANK0_GPIO_SLEWFAST                             PADS_BANK0_GPIO_SLEWFAST_Msk
// PADS_BANK0->SWCLK
#define PADS_BANK0_SWCLK_OD_Pos                              7
#define PADS_BANK0_SWCLK_OD_Msk                              ((uint32_t)0x00000080)
#define PADS_BANK0_SWCLK_OD                                  PADS_BANK0_SWCLK_OD_Msk
#define PADS_BANK0_SWCLK_IE_Pos                              6
#define PADS_BANK0_SWCLK_IE_Msk                              ((uint32_t)0x00000040)
#define PADS_BANK0_SWCLK_IE                                  PADS_BANK0_SWCLK_IE_Msk
#define PADS_BANK0_SWCLK_DRIVE_Pos                           4
#define PADS_BANK0_SWCLK_DRIVE_Msk                           ((uint32_t)0x00000030)
#define PADS_BANK0_SWCLK_DRIVE                               PADS_BANK0_SWCLK_DRIVE_Msk
#define PADS_BANK0_SWCLK_DRIVE_2MA                           ((uint32_t)0x00000000)
#define PADS_BANK0_SWCLK_DRIVE_4MA                           ((uint32_t)0x00000010)
#define PADS_BANK0_SWCLK_DRIVE_8MA                           ((uint32_t)0x00000020)
#define PADS_BANK0_SWCLK_DRIVE_12MA                          ((uint32_t)0x00000030)
#define PADS_BANK0_SWCLK_PUE_Pos                             3
#define PADS_BANK0_SWCLK_PUE_Msk                             ((uint32_t)0x00000008)
#define PADS_BANK0_SWCLK_PUE                                 PADS_BANK0_SWCLK_PUE_Msk
#define PADS_BANK0_SWCLK_PDE_Pos                             2
#define PADS_BANK0_SWCLK_PDE_Msk                             ((uint32_t)0x00000004)
#define PADS_BANK0_SWCLK_PDE                                 PADS_BANK0_SWCLK_PDE_Msk
#define PADS_BANK0_SWCLK_SCHMITT_Pos                         1
#define PADS_BANK0_SWCLK_SCHMITT_Msk                         ((uint32_t)0x00000002)
#define PADS_BANK0_SWCLK_SCHMITT                             PADS_BANK0_SWCLK_SCHMITT_Msk
#define PADS_BANK0_SWCLK_SLEWFAST_Pos                        0
#define PADS_BANK0_SWCLK_SLEWFAST_Msk                        ((uint32_t)0x00000001)
#define PADS_BANK0_SWCLK_SLEWFAST                            PADS_BANK0_SWCLK_SLEWFAST_Msk
// PADS_BANK0->SWD
#define PADS_BANK0_SWD_OD_Pos                                7
#define PADS_BANK0_SWD_OD_Msk                                ((uint32_t)0x00000080)
#define PADS_BANK0_SWD_OD                                    PADS_BANK0_SWD_OD_Msk
#define PADS_BANK0_SWD_IE_Pos                                6
#define PADS_BANK0_SWD_IE_Msk                                ((uint32_t)0x00000040)
#define PADS_BANK0_SWD_IE                                    PADS_BANK0_SWD_IE_Msk
#define PADS_BANK0_SWD_DRIVE_Pos                             4
#define PADS_BANK0_SWD_DRIVE_Msk                             ((uint32_t)0x00000030)
#define PADS_BANK0_SWD_DRIVE                                 PADS_BANK0_SWD_DRIVE_Msk
#define PADS_BANK0_SWD_DRIVE_2MA                             ((uint32_t)0x00000000)
#define PADS_BANK0_SWD_DRIVE_4MA                             ((uint32_t)0x00000010)
#define PADS_BANK0_SWD_DRIVE_8MA                             ((uint32_t)0x00000020)
#define PADS_BANK0_SWD_DRIVE_12MA                            ((uint32_t)0x00000030)
#define PADS_BANK0_SWD_PUE_Pos                               3
#define PADS_BANK0_SWD_PUE_Msk                               ((uint32_t)0x00000008)
#define PADS_BANK0_SWD_PUE                                   PADS_BANK0_SWD_PUE_Msk
#define PADS_BANK0_SWD_PDE_Pos                               2
#define PADS_BANK0_SWD_PDE_Msk                               ((uint32_t)0x00000004)
#define PADS_BANK0_SWD_PDE                                   PADS_BANK0_SWD_PDE_Msk
#define PADS_BANK0_SWD_SCHMITT_Pos                           1
#define PADS_BANK0_SWD_SCHMITT_Msk                           ((uint32_t)0x00000002)
#define PADS_BANK0_SWD_SCHMITT                               PADS_BANK0_SWD_SCHMITT_Msk
#define PADS_BANK0_SWD_SLEWFAST_Pos                          0
#define PADS_BANK0_SWD_SLEWFAST_Msk                          ((uint32_t)0x00000001)
#define PADS_BANK0_SWD_SLEWFAST                              PADS_BANK0_SWD_SLEWFAST_Msk

// PADS_QSPI->VOLTAGE_SELECT
#define PADS_QSPI_VOLTAGE_SELECT_VOLTAGE_SELECT_Pos          0
#define PADS_QSPI_VOLTAGE_SELECT_VOLTAGE_SELECT_Msk          ((uint32_t)0x00000001)
#define PADS_QSPI_VOLTAGE_SELECT_VOLTAGE_SELECT              PADS_QSPI_VOLTAGE_SELECT_VOLTAGE_SELECT_Msk
#define PADS_QSPI_VOLTAGE_SELECT_VOLTAGE_SELECT_3V3          ((uint32_t)0x00000000)
#define PADS_QSPI_VOLTAGE_SELECT_VOLTAGE_SELECT_1V8          ((uint32_t)0x00000001)
// PADS_QSPI->GPIO_QSPI_SCLK
#define PADS_QSPI_GPIO_QSPI_SCLK_OD_Pos                      7
#define PADS_QSPI_GPIO_QSPI_SCLK_OD_Msk                      ((uint32_t)0x00000080)
#define PADS_QSPI_GPIO_QSPI_SCLK_OD                          PADS_QSPI_GPIO_QSPI_SCLK_OD_Msk
#define PADS_QSPI_GPIO_QSPI_SCLK_IE_Pos                      6
#define PADS_QSPI_GPIO_QSPI_SCLK_IE_Msk                      ((uint32_t)0x00000040)
#define PADS_QSPI_GPIO_QSPI_SCLK_IE                          PADS_QSPI_GPIO_QSPI_SCLK_IE_Msk
#define PADS_QSPI_GPIO_QSPI_SCLK_DRIVE_Pos                   4
#define PADS_QSPI_GPIO_QSPI_SCLK_DRIVE_Msk                   ((uint32_t)0x00000030)
#define PADS_QSPI_GPIO_QSPI_SCLK_DRIVE                       PADS_QSPI_GPIO_QSPI_SCLK_DRIVE_Msk
#define PADS_QSPI_GPIO_QSPI_SCLK_DRIVE_2MA                   ((uint32_t)0x00000000)
#define PADS_QSPI_GPIO_QSPI_SCLK_DRIVE_4MA                   ((uint32_t)0x00000010)
#define PADS_QSPI_GPIO_QSPI_SCLK_DRIVE_8MA                   ((uint32_t)0x00000020)
#define PADS_QSPI_GPIO_QSPI_SCLK_DRIVE_12MA                  ((uint32_t)0x00000030)
#define PADS_QSPI_GPIO_QSPI_SCLK_PUE_Pos                     3
#define PADS_QSPI_GPIO_QSPI_SCLK_PUE_Msk                     ((uint32_t)0x00000008)
#define PADS_QSPI_GPIO_QSPI_SCLK_PUE                         PADS_QSPI_GPIO_QSPI_SCLK_PUE_Msk
#define PADS_QSPI_GPIO_QSPI_SCLK_PDE_Pos                     2
#define PADS_QSPI_GPIO_QSPI_SCLK_PDE_Msk                     ((uint32_t)0x00000004)
#define PADS_QSPI_GPIO_QSPI_SCLK_PDE                         PADS_QSPI_GPIO_QSPI_SCLK_PDE_Msk
#define PADS_QSPI_GPIO_QSPI_SCLK_SCHMITT_Pos                 1
#define PADS_QSPI_GPIO_QSPI_SCLK_SCHMITT_Msk                 ((uint32_t)0x00000002)
#define PADS_QSPI_GPIO_QSPI_SCLK_SCHMITT                     PADS_QSPI_GPIO_QSPI_SCLK_SCHMITT_Msk
#define PADS_QSPI_GPIO_QSPI_SCLK_SLEWFAST_Pos                0
#define PADS_QSPI_GPIO_QSPI_SCLK_SLEWFAST_Msk                ((uint32_t)0x00000001)
#define PADS_QSPI_GPIO_QSPI_SCLK_SLEWFAST                    PADS_QSPI_GPIO_QSPI_SCLK_SLEWFAST_Msk
// PADS_QSPI->GPIO_QSPI_SD
#define PADS_QSPI_GPIO_QSPI_SD_OD_Pos                        7
#define PADS_QSPI_GPIO_QSPI_SD_OD_Msk                        ((uint32_t)0x00000080)
#define PADS_QSPI_GPIO_QSPI_SD_OD                            PADS_QSPI_GPIO_QSPI_SD_OD_Msk
#define PADS_QSPI_GPIO_QSPI_SD_IE_Pos                        6
#define PADS_QSPI_GPIO_QSPI_SD_IE_Msk                        ((uint32_t)0x00000040)
#define PADS_QSPI_GPIO_QSPI_SD_IE                            PADS_QSPI_GPIO_QSPI_SD_IE_Msk
#define PADS_QSPI_GPIO_QSPI_SD_DRIVE_Pos                     4
#define PADS_QSPI_GPIO_QSPI_SD_DRIVE_Msk                     ((uint32_t)0x00000030)
#define PADS_QSPI_GPIO_QSPI_SD_DRIVE                         PADS_QSPI_GPIO_QSPI_SD_DRIVE_Msk
#define PADS_QSPI_GPIO_QSPI_SD_PUE_Pos                       3
#define PADS_QSPI_GPIO_QSPI_SD_PUE_Msk                       ((uint32_t)0x00000008)
#define PADS_QSPI_GPIO_QSPI_SD_PUE                           PADS_QSPI_GPIO_QSPI_SD_PUE_Msk
#define PADS_QSPI_GPIO_QSPI_SD_PDE_Pos                       2
#define PADS_QSPI_GPIO_QSPI_SD_PDE_Msk                       ((uint32_t)0x00000004)
#define PADS_QSPI_GPIO_QSPI_SD_PDE                           PADS_QSPI_GPIO_QSPI_SD_PDE_Msk
#define PADS_QSPI_GPIO_QSPI_SD_SCHMITT_Pos                   1
#define PADS_QSPI_GPIO_QSPI_SD_SCHMITT_Msk                   ((uint32_t)0x00000002)
#define PADS_QSPI_GPIO_QSPI_SD_SCHMITT                       PADS_QSPI_GPIO_QSPI_SD_SCHMITT_Msk
#define PADS_QSPI_GPIO_QSPI_SD_SLEWFAST_Pos                  0
#define PADS_QSPI_GPIO_QSPI_SD_SLEWFAST_Msk                  ((uint32_t)0x00000001)
#define PADS_QSPI_GPIO_QSPI_SD_SLEWFAST                      PADS_QSPI_GPIO_QSPI_SD_SLEWFAST_Msk
// PADS_QSPI->GPIO_QSPI_SS
#define PADS_QSPI_GPIO_QSPI_SS_OD_Pos                        7
#define PADS_QSPI_GPIO_QSPI_SS_OD_Msk                        ((uint32_t)0x00000080)
#define PADS_QSPI_GPIO_QSPI_SS_OD                            PADS_QSPI_GPIO_QSPI_SS_OD_Msk
#define PADS_QSPI_GPIO_QSPI_SS_IE_Pos                        6
#define PADS_QSPI_GPIO_QSPI_SS_IE_Msk                        ((uint32_t)0x00000040)
#define PADS_QSPI_GPIO_QSPI_SS_IE                            PADS_QSPI_GPIO_QSPI_SS_IE_Msk
#define PADS_QSPI_GPIO_QSPI_SS_DRIVE_Pos                     4
#define PADS_QSPI_GPIO_QSPI_SS_DRIVE_Msk                     ((uint32_t)0x00000030)
#define PADS_QSPI_GPIO_QSPI_SS_DRIVE                         PADS_QSPI_GPIO_QSPI_SS_DRIVE_Msk
#define PADS_QSPI_GPIO_QSPI_SS_DRIVE_2MA                     ((uint32_t)0x00000000)
#define PADS_QSPI_GPIO_QSPI_SS_DRIVE_4MA                     ((uint32_t)0x00000010)
#define PADS_QSPI_GPIO_QSPI_SS_DRIVE_8MA                     ((uint32_t)0x00000020)
#define PADS_QSPI_GPIO_QSPI_SS_DRIVE_12MA                    ((uint32_t)0x00000030)
#define PADS_QSPI_GPIO_QSPI_SS_PUE_Pos                       3
#define PADS_QSPI_GPIO_QSPI_SS_PUE_Msk                       ((uint32_t)0x00000008)
#define PADS_QSPI_GPIO_QSPI_SS_PUE                           PADS_QSPI_GPIO_QSPI_SS_PUE_Msk
#define PADS_QSPI_GPIO_QSPI_SS_PDE_Pos                       2
#define PADS_QSPI_GPIO_QSPI_SS_PDE_Msk                       ((uint32_t)0x00000004)
#define PADS_QSPI_GPIO_QSPI_SS_PDE                           PADS_QSPI_GPIO_QSPI_SS_PDE_Msk
#define PADS_QSPI_GPIO_QSPI_SS_SCHMITT_Pos                   1
#define PADS_QSPI_GPIO_QSPI_SS_SCHMITT_Msk                   ((uint32_t)0x00000002)
#define PADS_QSPI_GPIO_QSPI_SS_SCHMITT                       PADS_QSPI_GPIO_QSPI_SS_SCHMITT_Msk
#define PADS_QSPI_GPIO_QSPI_SS_SLEWFAST_Pos                  0
#define PADS_QSPI_GPIO_QSPI_SS_SLEWFAST_Msk                  ((uint32_t)0x00000001)
#define PADS_QSPI_GPIO_QSPI_SS_SLEWFAST                      PADS_QSPI_GPIO_QSPI_SS_SLEWFAST_Msk

// PIO->CTRL
#define PIO_CTRL_CLKDIV_RESTART_Pos                          8
#define PIO_CTRL_CLKDIV_RESTART_Msk                          ((uint32_t)0x00000F00)
#define PIO_CTRL_CLKDIV_RESTART                              PIO_CTRL_CLKDIV_RESTART_Msk
#define PIO_CTRL_SM_RESTART_Pos                              4
#define PIO_CTRL_SM_RESTART_Msk                              ((uint32_t)0x000000F0)
#define PIO_CTRL_SM_RESTART                                  PIO_CTRL_SM_RESTART_Msk
#define PIO_CTRL_SM_ENABLE_Pos                               0
#define PIO_CTRL_SM_ENABLE_Msk                               ((uint32_t)0x0000000F)
#define PIO_CTRL_SM_ENABLE                                   PIO_CTRL_SM_ENABLE_Msk
// PIO->FSTAT
#define PIO_FSTAT_TXEMPTY_Pos                                24
#define PIO_FSTAT_TXEMPTY_Msk                                ((uint32_t)0x0F000000)
#define PIO_FSTAT_TXEMPTY                                    PIO_FSTAT_TXEMPTY_Msk
#define PIO_FSTAT_TXFULL_Pos                                 16
#define PIO_FSTAT_TXFULL_Msk                                 ((uint32_t)0x000F0000)
#define PIO_FSTAT_TXFULL                                     PIO_FSTAT_TXFULL_Msk
#define PIO_FSTAT_RXEMPTY_Pos                                8
#define PIO_FSTAT_RXEMPTY_Msk                                ((uint32_t)0x00000F00)
#define PIO_FSTAT_RXEMPTY                                    PIO_FSTAT_RXEMPTY_Msk
#define PIO_FSTAT_RXFULL_Pos                                 0
#define PIO_FSTAT_RXFULL_Msk                                 ((uint32_t)0x0000000F)
#define PIO_FSTAT_RXFULL                                     PIO_FSTAT_RXFULL_Msk
// PIO->FDEBUG
#define PIO_FDEBUG_TXSTALL_Pos                               24
#define PIO_FDEBUG_TXSTALL_Msk                               ((uint32_t)0x0F000000)
#define PIO_FDEBUG_TXSTALL                                   PIO_FDEBUG_TXSTALL_Msk
#define PIO_FDEBUG_TXOVER_Pos                                16
#define PIO_FDEBUG_TXOVER_Msk                                ((uint32_t)0x000F0000)
#define PIO_FDEBUG_TXOVER                                    PIO_FDEBUG_TXOVER_Msk
#define PIO_FDEBUG_RXUNDER_Pos                               8
#define PIO_FDEBUG_RXUNDER_Msk                               ((uint32_t)0x00000F00)
#define PIO_FDEBUG_RXUNDER                                   PIO_FDEBUG_RXUNDER_Msk
#define PIO_FDEBUG_RXSTALL_Pos                               0
#define PIO_FDEBUG_RXSTALL_Msk                               ((uint32_t)0x0000000F)
#define PIO_FDEBUG_RXSTALL                                   PIO_FDEBUG_RXSTALL_Msk
// PIO->FLEVEL
#define PIO_FLEVEL_RX3_Pos                                   28
#define PIO_FLEVEL_RX3_Msk                                   ((uint32_t)0xF0000000)
#define PIO_FLEVEL_RX3                                       PIO_FLEVEL_RX3_Msk
#define PIO_FLEVEL_TX3_Pos                                   24
#define PIO_FLEVEL_TX3_Msk                                   ((uint32_t)0x0F000000)
#define PIO_FLEVEL_TX3                                       PIO_FLEVEL_TX3_Msk
#define PIO_FLEVEL_RX2_Pos                                   20
#define PIO_FLEVEL_RX2_Msk                                   ((uint32_t)0x00F00000)
#define PIO_FLEVEL_RX2                                       PIO_FLEVEL_RX2_Msk
#define PIO_FLEVEL_TX2_Pos                                   16
#define PIO_FLEVEL_TX2_Msk                                   ((uint32_t)0x000F0000)
#define PIO_FLEVEL_TX2                                       PIO_FLEVEL_TX2_Msk
#define PIO_FLEVEL_RX1_Pos                                   12
#define PIO_FLEVEL_RX1_Msk                                   ((uint32_t)0x0000F000)
#define PIO_FLEVEL_RX1                                       PIO_FLEVEL_RX1_Msk
#define PIO_FLEVEL_TX1_Pos                                   8
#define PIO_FLEVEL_TX1_Msk                                   ((uint32_t)0x00000F00)
#define PIO_FLEVEL_TX1                                       PIO_FLEVEL_TX1_Msk
#define PIO_FLEVEL_RX0_Pos                                   4
#define PIO_FLEVEL_RX0_Msk                                   ((uint32_t)0x000000F0)
#define PIO_FLEVEL_RX0                                       PIO_FLEVEL_RX0_Msk
#define PIO_FLEVEL_TX0_Pos                                   0
#define PIO_FLEVEL_TX0_Msk                                   ((uint32_t)0x0000000F)
#define PIO_FLEVEL_TX0                                       PIO_FLEVEL_TX0_Msk
// PIO->TXF
#define PIO_TXF_TXF0_Pos                                     0
#define PIO_TXF_TXF0_Msk                                     ((uint32_t)0xFFFFFFFF)
#define PIO_TXF_TXF0                                         PIO_TXF_TXF0_Msk
// PIO->RXF
#define PIO_RXF_RXF0_Pos                                     0
#define PIO_RXF_RXF0_Msk                                     ((uint32_t)0xFFFFFFFF)
#define PIO_RXF_RXF0                                         PIO_RXF_RXF0_Msk
// PIO->IRQ
#define PIO_IRQ_IRQ_Pos                                      0
#define PIO_IRQ_IRQ_Msk                                      ((uint32_t)0x000000FF)
#define PIO_IRQ_IRQ                                          PIO_IRQ_IRQ_Msk
// PIO->IRQ_FORCE
#define PIO_IRQ_FORCE_IRQ_FORCE_Pos                          0
#define PIO_IRQ_FORCE_IRQ_FORCE_Msk                          ((uint32_t)0x000000FF)
#define PIO_IRQ_FORCE_IRQ_FORCE                              PIO_IRQ_FORCE_IRQ_FORCE_Msk
// PIO->INPUT_SYNC_BYPASS
#define PIO_INPUT_SYNC_BYPASS_INPUT_SYNC_BYPASS_Pos          0
#define PIO_INPUT_SYNC_BYPASS_INPUT_SYNC_BYPASS_Msk          ((uint32_t)0xFFFFFFFF)
#define PIO_INPUT_SYNC_BYPASS_INPUT_SYNC_BYPASS              PIO_INPUT_SYNC_BYPASS_INPUT_SYNC_BYPASS_Msk
// PIO->DBG_PADOUT
#define PIO_DBG_PADOUT_DBG_PADOUT_Pos                        0
#define PIO_DBG_PADOUT_DBG_PADOUT_Msk                        ((uint32_t)0xFFFFFFFF)
#define PIO_DBG_PADOUT_DBG_PADOUT                            PIO_DBG_PADOUT_DBG_PADOUT_Msk
// PIO->DBG_PADOE
#define PIO_DBG_PADOE_DBG_PADOE_Pos                          0
#define PIO_DBG_PADOE_DBG_PADOE_Msk                          ((uint32_t)0xFFFFFFFF)
#define PIO_DBG_PADOE_DBG_PADOE                              PIO_DBG_PADOE_DBG_PADOE_Msk
// PIO->DBG_CFGINFO
#define PIO_DBG_CFGINFO_IMEM_SIZE_Pos                        16
#define PIO_DBG_CFGINFO_IMEM_SIZE_Msk                        ((uint32_t)0x003F0000)
#define PIO_DBG_CFGINFO_IMEM_SIZE                            PIO_DBG_CFGINFO_IMEM_SIZE_Msk
#define PIO_DBG_CFGINFO_SM_COUNT_Pos                         8
#define PIO_DBG_CFGINFO_SM_COUNT_Msk                         ((uint32_t)0x00000F00)
#define PIO_DBG_CFGINFO_SM_COUNT                             PIO_DBG_CFGINFO_SM_COUNT_Msk
#define PIO_DBG_CFGINFO_FIFO_DEPTH_Pos                       0
#define PIO_DBG_CFGINFO_FIFO_DEPTH_Msk                       ((uint32_t)0x0000003F)
#define PIO_DBG_CFGINFO_FIFO_DEPTH                           PIO_DBG_CFGINFO_FIFO_DEPTH_Msk
// PIO->INSTR_MEM
#define PIO_INSTR_MEM_INSTR_MEM0_Pos                         0
#define PIO_INSTR_MEM_INSTR_MEM0_Msk                         ((uint32_t)0x0000FFFF)
#define PIO_INSTR_MEM_INSTR_MEM0                             PIO_INSTR_MEM_INSTR_MEM0_Msk
// PIO->SM[n].CLKDIV
#define PIO_SM_CLKDIV_INT_Pos                                16
#define PIO_SM_CLKDIV_INT_Msk                                ((uint32_t)0xFFFF0000)
#define PIO_SM_CLKDIV_INT                                    PIO_SM_CLKDIV_INT_Msk
#define PIO_SM_CLKDIV_FRAC_Pos                               8
#define PIO_SM_CLKDIV_FRAC_Msk                               ((uint32_t)0x0000FF00)
#define PIO_SM_CLKDIV_FRAC                                   PIO_SM_CLKDIV_FRAC_Msk
// PIO->SM[n].EXECCTRL
#define PIO_SM_EXECCTRL_EXEC_STALLED_Pos                     31
#define PIO_SM_EXECCTRL_EXEC_STALLED_Msk                     ((uint32_t)0x80000000)
#define PIO_SM_EXECCTRL_EXEC_STALLED                         PIO_SM_EXECCTRL_EXEC_STALLED_Msk
#define PIO_SM_EXECCTRL_SIDE_EN_Pos                          30
#define PIO_SM_EXECCTRL_SIDE_EN_Msk                          ((uint32_t)0x40000000)
#define PIO_SM_EXECCTRL_SIDE_EN                              PIO_SM_EXECCTRL_SIDE_EN_Msk
#define PIO_SM_EXECCTRL_SIDE_PINDIR_Pos                      29
#define PIO_SM_EXECCTRL_SIDE_PINDIR_Msk                      ((uint32_t)0x20000000)
#define PIO_SM_EXECCTRL_SIDE_PINDIR                          PIO_SM_EXECCTRL_SIDE_PINDIR_Msk
#define PIO_SM_EXECCTRL_JMP_PIN_Pos                          24
#define PIO_SM_EXECCTRL_JMP_PIN_Msk                          ((uint32_t)0x1F000000)
#define PIO_SM_EXECCTRL_JMP_PIN                              PIO_SM_EXECCTRL_JMP_PIN_Msk
#define PIO_SM_EXECCTRL_OUT_EN_SEL_Pos                       19
#define PIO_SM_EXECCTRL_OUT_EN_SEL_Msk                       ((uint32_t)0x00F80000)
#define PIO_SM_EXECCTRL_OUT_EN_SEL                           PIO_SM_EXECCTRL_OUT_EN_SEL_Msk
#define PIO_SM_EXECCTRL_INLINE_OUT_EN_Pos                    18
#define PIO_SM_EXECCTRL_INLINE_OUT_EN_Msk                    ((uint32_t)0x00040000)
#define PIO_SM_EXECCTRL_INLINE_OUT_EN                        PIO_SM_EXECCTRL_INLINE_OUT_EN_Msk
#define PIO_SM_EXECCTRL_OUT_STICKY_Pos                       17
#define PIO_SM_EXECCTRL_OUT_STICKY_Msk                       ((uint32_t)0x00020000)
#define PIO_SM_EXECCTRL_OUT_STICKY                           PIO_SM_EXECCTRL_OUT_STICKY_Msk
#define PIO_SM_EXECCTRL_WRAP_TOP_Pos                         12
#define PIO_SM_EXECCTRL_WRAP_TOP_Msk                         ((uint32_t)0x0001F000)
#define PIO_SM_EXECCTRL_WRAP_TOP                             PIO_SM_EXECCTRL_WRAP_TOP_Msk
#define PIO_SM_EXECCTRL_WRAP_BOTTOM_Pos                      7
#define PIO_SM_EXECCTRL_WRAP_BOTTOM_Msk                      ((uint32_t)0x00000F80)
#define PIO_SM_EXECCTRL_WRAP_BOTTOM                          PIO_SM_EXECCTRL_WRAP_BOTTOM_Msk
#define PIO_SM_EXECCTRL_STATUS_SEL_Pos                       4
#define PIO_SM_EXECCTRL_STATUS_SEL_Msk                       ((uint32_t)0x00000010)
#define PIO_SM_EXECCTRL_STATUS_SEL                           PIO_SM_EXECCTRL_STATUS_SEL_Msk
#define PIO_SM_EXECCTRL_STATUS_N_Pos                         0
#define PIO_SM_EXECCTRL_STATUS_N_Msk                         ((uint32_t)0x0000000F)
#define PIO_SM_EXECCTRL_STATUS_N                             PIO_SM_EXECCTRL_STATUS_N_Msk
// PIO->SM[n].SHIFTCTRL
#define PIO_SM_SHIFTCTRL_FJOIN_RX_Pos                        31
#define PIO_SM_SHIFTCTRL_FJOIN_RX_Msk                        ((uint32_t)0x80000000)
#define PIO_SM_SHIFTCTRL_FJOIN_RX                            PIO_SM_SHIFTCTRL_FJOIN_RX_Msk
#define PIO_SM_SHIFTCTRL_FJOIN_TX_Pos                        30
#define PIO_SM_SHIFTCTRL_FJOIN_TX_Msk                        ((uint32_t)0x40000000)
#define PIO_SM_SHIFTCTRL_FJOIN_TX                            PIO_SM_SHIFTCTRL_FJOIN_TX_Msk
#define PIO_SM_SHIFTCTRL_PULL_THRESH_Pos                     25
#define PIO_SM_SHIFTCTRL_PULL_THRESH_Msk                     ((uint32_t)0x3E000000)
#define PIO_SM_SHIFTCTRL_PULL_THRESH                         PIO_SM_SHIFTCTRL_PULL_THRESH_Msk
#define PIO_SM_SHIFTCTRL_PUSH_THRESH_Pos                     20
#define PIO_SM_SHIFTCTRL_PUSH_THRESH_Msk                     ((uint32_t)0x01F00000)
#define PIO_SM_SHIFTCTRL_PUSH_THRESH                         PIO_SM_SHIFTCTRL_PUSH_THRESH_Msk
#define PIO_SM_SHIFTCTRL_OUT_SHIFTDIR_Pos                    19
#define PIO_SM_SHIFTCTRL_OUT_SHIFTDIR_Msk                    ((uint32_t)0x00080000)
#define PIO_SM_SHIFTCTRL_OUT_SHIFTDIR                        PIO_SM_SHIFTCTRL_OUT_SHIFTDIR_Msk
#define PIO_SM_SHIFTCTRL_IN_SHIFTDIR_Pos                     18
#define PIO_SM_SHIFTCTRL_IN_SHIFTDIR_Msk                     ((uint32_t)0x00040000)
#define PIO_SM_SHIFTCTRL_IN_SHIFTDIR                         PIO_SM_SHIFTCTRL_IN_SHIFTDIR_Msk
#define PIO_SM_SHIFTCTRL_AUTOPULL_Pos                        17
#define PIO_SM_SHIFTCTRL_AUTOPULL_Msk                        ((uint32_t)0x00020000)
#define PIO_SM_SHIFTCTRL_AUTOPULL                            PIO_SM_SHIFTCTRL_AUTOPULL_Msk
#define PIO_SM_SHIFTCTRL_AUTOPUSH_Pos                        16
#define PIO_SM_SHIFTCTRL_AUTOPUSH_Msk                        ((uint32_t)0x00010000)
#define PIO_SM_SHIFTCTRL_AUTOPUSH                            PIO_SM_SHIFTCTRL_AUTOPUSH_Msk
// PIO->SM[n].ADDR
#define PIO_SM_ADDR_SM0_ADDR_Pos                             0
#define PIO_SM_ADDR_SM0_ADDR_Msk                             ((uint32_t)0x0000001F)
#define PIO_SM_ADDR_SM0_ADDR                                 PIO_SM_ADDR_SM0_ADDR_Msk
// PIO->SM[n].INSTR
#define PIO_SM_INSTR_SM0_INSTR_Pos                           0
#define PIO_SM_INSTR_SM0_INSTR_Msk                           ((uint32_t)0x0000FFFF)
#define PIO_SM_INSTR_SM0_INSTR                               PIO_SM_INSTR_SM0_INSTR_Msk
// PIO->SM[n].PINCTRL
#define PIO_SM_PINCTRL_SIDESET_COUNT_Pos                     29
#define PIO_SM_PINCTRL_SIDESET_COUNT_Msk                     ((uint32_t)0xE0000000)
#define PIO_SM_PINCTRL_SIDESET_COUNT                         PIO_SM_PINCTRL_SIDESET_COUNT_Msk
#define PIO_SM_PINCTRL_SET_COUNT_Pos                         26
#define PIO_SM_PINCTRL_SET_COUNT_Msk                         ((uint32_t)0x1C000000)
#define PIO_SM_PINCTRL_SET_COUNT                             PIO_SM_PINCTRL_SET_COUNT_Msk
#define PIO_SM_PINCTRL_OUT_COUNT_Pos                         20
#define PIO_SM_PINCTRL_OUT_COUNT_Msk                         ((uint32_t)0x03F00000)
#define PIO_SM_PINCTRL_OUT_COUNT                             PIO_SM_PINCTRL_OUT_COUNT_Msk
#define PIO_SM_PINCTRL_IN_BASE_Pos                           15
#define PIO_SM_PINCTRL_IN_BASE_Msk                           ((uint32_t)0x000F8000)
#define PIO_SM_PINCTRL_IN_BASE                               PIO_SM_PINCTRL_IN_BASE_Msk
#define PIO_SM_PINCTRL_SIDESET_BASE_Pos                      10
#define PIO_SM_PINCTRL_SIDESET_BASE_Msk                      ((uint32_t)0x00007C00)
#define PIO_SM_PINCTRL_SIDESET_BASE                          PIO_SM_PINCTRL_SIDESET_BASE_Msk
#define PIO_SM_PINCTRL_SET_BASE_Pos                          5
#define PIO_SM_PINCTRL_SET_BASE_Msk                          ((uint32_t)0x000003E0)
#define PIO_SM_PINCTRL_SET_BASE                              PIO_SM_PINCTRL_SET_BASE_Msk
#define PIO_SM_PINCTRL_OUT_BASE_Pos                          0
#define PIO_SM_PINCTRL_OUT_BASE_Msk                          ((uint32_t)0x0000001F)
#define PIO_SM_PINCTRL_OUT_BASE                              PIO_SM_PINCTRL_OUT_BASE_Msk
// PIO->INTR
#define PIO_INTR_SM3_Pos                                     11
#define PIO_INTR_SM3_Msk                                     ((uint32_t)0x00000800)
#define PIO_INTR_SM3                                         PIO_INTR_SM3_Msk
#define PIO_INTR_SM2_Pos                                     10
#define PIO_INTR_SM2_Msk                                     ((uint32_t)0x00000400)
#define PIO_INTR_SM2                                         PIO_INTR_SM2_Msk
#define PIO_INTR_SM1_Pos                                     9
#define PIO_INTR_SM1_Msk                                     ((uint32_t)0x00000200)
#define PIO_INTR_SM1                                         PIO_INTR_SM1_Msk
#define PIO_INTR_SM0_Pos                                     8
#define PIO_INTR_SM0_Msk                                     ((uint32_t)0x00000100)
#define PIO_INTR_SM0                                         PIO_INTR_SM0_Msk
#define PIO_INTR_SM3_TXNFULL_Pos                             7
#define PIO_INTR_SM3_TXNFULL_Msk                             ((uint32_t)0x00000080)
#define PIO_INTR_SM3_TXNFULL                                 PIO_INTR_SM3_TXNFULL_Msk
#define PIO_INTR_SM2_TXNFULL_Pos                             6
#define PIO_INTR_SM2_TXNFULL_Msk                             ((uint32_t)0x00000040)
#define PIO_INTR_SM2_TXNFULL                                 PIO_INTR_SM2_TXNFULL_Msk
#define PIO_INTR_SM1_TXNFULL_Pos                             5
#define PIO_INTR_SM1_TXNFULL_Msk                             ((uint32_t)0x00000020)
#define PIO_INTR_SM1_TXNFULL                                 PIO_INTR_SM1_TXNFULL_Msk
#define PIO_INTR_SM0_TXNFULL_Pos                             4
#define PIO_INTR_SM0_TXNFULL_Msk                             ((uint32_t)0x00000010)
#define PIO_INTR_SM0_TXNFULL                                 PIO_INTR_SM0_TXNFULL_Msk
#define PIO_INTR_SM3_RXNEMPTY_Pos                            3
#define PIO_INTR_SM3_RXNEMPTY_Msk                            ((uint32_t)0x00000008)
#define PIO_INTR_SM3_RXNEMPTY                                PIO_INTR_SM3_RXNEMPTY_Msk
#define PIO_INTR_SM2_RXNEMPTY_Pos                            2
#define PIO_INTR_SM2_RXNEMPTY_Msk                            ((uint32_t)0x00000004)
#define PIO_INTR_SM2_RXNEMPTY                                PIO_INTR_SM2_RXNEMPTY_Msk
#define PIO_INTR_SM1_RXNEMPTY_Pos                            1
#define PIO_INTR_SM1_RXNEMPTY_Msk                            ((uint32_t)0x00000002)
#define PIO_INTR_SM1_RXNEMPTY                                PIO_INTR_SM1_RXNEMPTY_Msk
#define PIO_INTR_SM0_RXNEMPTY_Pos                            0
#define PIO_INTR_SM0_RXNEMPTY_Msk                            ((uint32_t)0x00000001)
#define PIO_INTR_SM0_RXNEMPTY                                PIO_INTR_SM0_RXNEMPTY_Msk
// PIO->IRQ_CTRL[n].INTE
#define PIO_IRQ_CTRL_INTE_SM3_Pos                            11
#define PIO_IRQ_CTRL_INTE_SM3_Msk                            ((uint32_t)0x00000800)
#define PIO_IRQ_CTRL_INTE_SM3                                PIO_IRQ_CTRL_INTE_SM3_Msk
#define PIO_IRQ_CTRL_INTE_SM2_Pos                            10
#define PIO_IRQ_CTRL_INTE_SM2_Msk                            ((uint32_t)0x00000400)
#define PIO_IRQ_CTRL_INTE_SM2                                PIO_IRQ_CTRL_INTE_SM2_Msk
#define PIO_IRQ_CTRL_INTE_SM1_Pos                            9
#define PIO_IRQ_CTRL_INTE_SM1_Msk                            ((uint32_t)0x00000200)
#define PIO_IRQ_CTRL_INTE_SM1                                PIO_IRQ_CTRL_INTE_SM1_Msk
#define PIO_IRQ_CTRL_INTE_SM0_Pos                            8
#define PIO_IRQ_CTRL_INTE_SM0_Msk                            ((uint32_t)0x00000100)
#define PIO_IRQ_CTRL_INTE_SM0                                PIO_IRQ_CTRL_INTE_SM0_Msk
#define PIO_IRQ_CTRL_INTE_SM3_TXNFULL_Pos                    7
#define PIO_IRQ_CTRL_INTE_SM3_TXNFULL_Msk                    ((uint32_t)0x00000080)
#define PIO_IRQ_CTRL_INTE_SM3_TXNFULL                        PIO_IRQ_CTRL_INTE_SM3_TXNFULL_Msk
#define PIO_IRQ_CTRL_INTE_SM2_TXNFULL_Pos                    6
#define PIO_IRQ_CTRL_INTE_SM2_TXNFULL_Msk                    ((uint32_t)0x00000040)
#define PIO_IRQ_CTRL_INTE_SM2_TXNFULL                        PIO_IRQ_CTRL_INTE_SM2_TXNFULL_Msk
#define PIO_IRQ_CTRL_INTE_SM1_TXNFULL_Pos                    5
#define PIO_IRQ_CTRL_INTE_SM1_TXNFULL_Msk                    ((uint32_t)0x00000020)
#define PIO_IRQ_CTRL_INTE_SM1_TXNFULL                        PIO_IRQ_CTRL_INTE_SM1_TXNFULL_Msk
#define PIO_IRQ_CTRL_INTE_SM0_TXNFULL_Pos                    4
#define PIO_IRQ_CTRL_INTE_SM0_TXNFULL_Msk                    ((uint32_t)0x00000010)
#define PIO_IRQ_CTRL_INTE_SM0_TXNFULL                        PIO_IRQ_CTRL_INTE_SM0_TXNFULL_Msk
#define PIO_IRQ_CTRL_INTE_SM3_RXNEMPTY_Pos                   3
#define PIO_IRQ_CTRL_INTE_SM3_RXNEMPTY_Msk                   ((uint32_t)0x00000008)
#define PIO_IRQ_CTRL_INTE_SM3_RXNEMPTY                       PIO_IRQ_CTRL_INTE_SM3_RXNEMPTY_Msk
#define PIO_IRQ_CTRL_INTE_SM2_RXNEMPTY_Pos                   2
#define PIO_IRQ_CTRL_INTE_SM2_RXNEMPTY_Msk                   ((uint32_t)0x00000004)
#define PIO_IRQ_CTRL_INTE_SM2_RXNEMPTY                       PIO_IRQ_CTRL_INTE_SM2_RXNEMPTY_Msk
#define PIO_IRQ_CTRL_INTE_SM1_RXNEMPTY_Pos                   1
#define PIO_IRQ_CTRL_INTE_SM1_RXNEMPTY_Msk                   ((uint32_t)0x00000002)
#define PIO_IRQ_CTRL_INTE_SM1_RXNEMPTY                       PIO_IRQ_CTRL_INTE_SM1_RXNEMPTY_Msk
#define PIO_IRQ_CTRL_INTE_SM0_RXNEMPTY_Pos                   0
#define PIO_IRQ_CTRL_INTE_SM0_RXNEMPTY_Msk                   ((uint32_t)0x00000001)
#define PIO_IRQ_CTRL_INTE_SM0_RXNEMPTY                       PIO_IRQ_CTRL_INTE_SM0_RXNEMPTY_Msk
// PIO->IRQ_CTRL[n].INTF
#define PIO_IRQ_CTRL_INTF_SM3_Pos                            11
#define PIO_IRQ_CTRL_INTF_SM3_Msk                            ((uint32_t)0x00000800)
#define PIO_IRQ_CTRL_INTF_SM3                                PIO_IRQ_CTRL_INTF_SM3_Msk
#define PIO_IRQ_CTRL_INTF_SM2_Pos                            10
#define PIO_IRQ_CTRL_INTF_SM2_Msk                            ((uint32_t)0x00000400)
#define PIO_IRQ_CTRL_INTF_SM2                                PIO_IRQ_CTRL_INTF_SM2_Msk
#define PIO_IRQ_CTRL_INTF_SM1_Pos                            9
#define PIO_IRQ_CTRL_INTF_SM1_Msk                            ((uint32_t)0x00000200)
#define PIO_IRQ_CTRL_INTF_SM1                                PIO_IRQ_CTRL_INTF_SM1_Msk
#define PIO_IRQ_CTRL_INTF_SM0_Pos                            8
#define PIO_IRQ_CTRL_INTF_SM0_Msk                            ((uint32_t)0x00000100)
#define PIO_IRQ_CTRL_INTF_SM0                                PIO_IRQ_CTRL_INTF_SM0_Msk
#define PIO_IRQ_CTRL_INTF_SM3_TXNFULL_Pos                    7
#define PIO_IRQ_CTRL_INTF_SM3_TXNFULL_Msk                    ((uint32_t)0x00000080)
#define PIO_IRQ_CTRL_INTF_SM3_TXNFULL                        PIO_IRQ_CTRL_INTF_SM3_TXNFULL_Msk
#define PIO_IRQ_CTRL_INTF_SM2_TXNFULL_Pos                    6
#define PIO_IRQ_CTRL_INTF_SM2_TXNFULL_Msk                    ((uint32_t)0x00000040)
#define PIO_IRQ_CTRL_INTF_SM2_TXNFULL                        PIO_IRQ_CTRL_INTF_SM2_TXNFULL_Msk
#define PIO_IRQ_CTRL_INTF_SM1_TXNFULL_Pos                    5
#define PIO_IRQ_CTRL_INTF_SM1_TXNFULL_Msk                    ((uint32_t)0x00000020)
#define PIO_IRQ_CTRL_INTF_SM1_TXNFULL                        PIO_IRQ_CTRL_INTF_SM1_TXNFULL_Msk
#define PIO_IRQ_CTRL_INTF_SM0_TXNFULL_Pos                    4
#define PIO_IRQ_CTRL_INTF_SM0_TXNFULL_Msk                    ((uint32_t)0x00000010)
#define PIO_IRQ_CTRL_INTF_SM0_TXNFULL                        PIO_IRQ_CTRL_INTF_SM0_TXNFULL_Msk
#define PIO_IRQ_CTRL_INTF_SM3_RXNEMPTY_Pos                   3
#define PIO_IRQ_CTRL_INTF_SM3_RXNEMPTY_Msk                   ((uint32_t)0x00000008)
#define PIO_IRQ_CTRL_INTF_SM3_RXNEMPTY                       PIO_IRQ_CTRL_INTF_SM3_RXNEMPTY_Msk
#define PIO_IRQ_CTRL_INTF_SM2_RXNEMPTY_Pos                   2
#define PIO_IRQ_CTRL_INTF_SM2_RXNEMPTY_Msk                   ((uint32_t)0x00000004)
#define PIO_IRQ_CTRL_INTF_SM2_RXNEMPTY                       PIO_IRQ_CTRL_INTF_SM2_RXNEMPTY_Msk
#define PIO_IRQ_CTRL_INTF_SM1_RXNEMPTY_Pos                   1
#define PIO_IRQ_CTRL_INTF_SM1_RXNEMPTY_Msk                   ((uint32_t)0x00000002)
#define PIO_IRQ_CTRL_INTF_SM1_RXNEMPTY                       PIO_IRQ_CTRL_INTF_SM1_RXNEMPTY_Msk
#define PIO_IRQ_CTRL_INTF_SM0_RXNEMPTY_Pos                   0
#define PIO_IRQ_CTRL_INTF_SM0_RXNEMPTY_Msk                   ((uint32_t)0x00000001)
#define PIO_IRQ_CTRL_INTF_SM0_RXNEMPTY                       PIO_IRQ_CTRL_INTF_SM0_RXNEMPTY_Msk
// PIO->IRQ_CTRL[n].INTS
#define PIO_IRQ_CTRL_INTS_SM3_Pos                            11
#define PIO_IRQ_CTRL_INTS_SM3_Msk                            ((uint32_t)0x00000800)
#define PIO_IRQ_CTRL_INTS_SM3                                PIO_IRQ_CTRL_INTS_SM3_Msk
#define PIO_IRQ_CTRL_INTS_SM2_Pos                            10
#define PIO_IRQ_CTRL_INTS_SM2_Msk                            ((uint32_t)0x00000400)
#define PIO_IRQ_CTRL_INTS_SM2                                PIO_IRQ_CTRL_INTS_SM2_Msk
#define PIO_IRQ_CTRL_INTS_SM1_Pos                            9
#define PIO_IRQ_CTRL_INTS_SM1_Msk                            ((uint32_t)0x00000200)
#define PIO_IRQ_CTRL_INTS_SM1                                PIO_IRQ_CTRL_INTS_SM1_Msk
#define PIO_IRQ_CTRL_INTS_SM0_Pos                            8
#define PIO_IRQ_CTRL_INTS_SM0_Msk                            ((uint32_t)0x00000100)
#define PIO_IRQ_CTRL_INTS_SM0                                PIO_IRQ_CTRL_INTS_SM0_Msk
#define PIO_IRQ_CTRL_INTS_SM3_TXNFULL_Pos                    7
#define PIO_IRQ_CTRL_INTS_SM3_TXNFULL_Msk                    ((uint32_t)0x00000080)
#define PIO_IRQ_CTRL_INTS_SM3_TXNFULL                        PIO_IRQ_CTRL_INTS_SM3_TXNFULL_Msk
#define PIO_IRQ_CTRL_INTS_SM2_TXNFULL_Pos                    6
#define PIO_IRQ_CTRL_INTS_SM2_TXNFULL_Msk                    ((uint32_t)0x00000040)
#define PIO_IRQ_CTRL_INTS_SM2_TXNFULL                        PIO_IRQ_CTRL_INTS_SM2_TXNFULL_Msk
#define PIO_IRQ_CTRL_INTS_SM1_TXNFULL_Pos                    5
#define PIO_IRQ_CTRL_INTS_SM1_TXNFULL_Msk                    ((uint32_t)0x00000020)
#define PIO_IRQ_CTRL_INTS_SM1_TXNFULL                        PIO_IRQ_CTRL_INTS_SM1_TXNFULL_Msk
#define PIO_IRQ_CTRL_INTS_SM0_TXNFULL_Pos                    4
#define PIO_IRQ_CTRL_INTS_SM0_TXNFULL_Msk                    ((uint32_t)0x00000010)
#define PIO_IRQ_CTRL_INTS_SM0_TXNFULL                        PIO_IRQ_CTRL_INTS_SM0_TXNFULL_Msk
#define PIO_IRQ_CTRL_INTS_SM3_RXNEMPTY_Pos                   3
#define PIO_IRQ_CTRL_INTS_SM3_RXNEMPTY_Msk                   ((uint32_t)0x00000008)
#define PIO_IRQ_CTRL_INTS_SM3_RXNEMPTY                       PIO_IRQ_CTRL_INTS_SM3_RXNEMPTY_Msk
#define PIO_IRQ_CTRL_INTS_SM2_RXNEMPTY_Pos                   2
#define PIO_IRQ_CTRL_INTS_SM2_RXNEMPTY_Msk                   ((uint32_t)0x00000004)
#define PIO_IRQ_CTRL_INTS_SM2_RXNEMPTY                       PIO_IRQ_CTRL_INTS_SM2_RXNEMPTY_Msk
#define PIO_IRQ_CTRL_INTS_SM1_RXNEMPTY_Pos                   1
#define PIO_IRQ_CTRL_INTS_SM1_RXNEMPTY_Msk                   ((uint32_t)0x00000002)
#define PIO_IRQ_CTRL_INTS_SM1_RXNEMPTY                       PIO_IRQ_CTRL_INTS_SM1_RXNEMPTY_Msk
#define PIO_IRQ_CTRL_INTS_SM0_RXNEMPTY_Pos                   0
#define PIO_IRQ_CTRL_INTS_SM0_RXNEMPTY_Msk                   ((uint32_t)0x00000001)
#define PIO_IRQ_CTRL_INTS_SM0_RXNEMPTY                       PIO_IRQ_CTRL_INTS_SM0_RXNEMPTY_Msk

// PLL->CS
#define PLL_CS_LOCK_Pos                                      31
#define PLL_CS_LOCK_Msk                                      ((uint32_t)0x80000000)
#define PLL_CS_LOCK                                          PLL_CS_LOCK_Msk
#define PLL_CS_BYPASS_Pos                                    8
#define PLL_CS_BYPASS_Msk                                    ((uint32_t)0x00000100)
#define PLL_CS_BYPASS                                        PLL_CS_BYPASS_Msk
#define PLL_CS_REFDIV_Pos                                    0
#define PLL_CS_REFDIV_Msk                                    ((uint32_t)0x0000003F)
#define PLL_CS_REFDIV                                        PLL_CS_REFDIV_Msk
// PLL->PWR
#define PLL_PWR_VCOPD_Pos                                    5
#define PLL_PWR_VCOPD_Msk                                    ((uint32_t)0x00000020)
#define PLL_PWR_VCOPD                                        PLL_PWR_VCOPD_Msk
#define PLL_PWR_POSTDIVPD_Pos                                3
#define PLL_PWR_POSTDIVPD_Msk                                ((uint32_t)0x00000008)
#define PLL_PWR_POSTDIVPD                                    PLL_PWR_POSTDIVPD_Msk
#define PLL_PWR_DSMPD_Pos                                    2
#define PLL_PWR_DSMPD_Msk                                    ((uint32_t)0x00000004)
#define PLL_PWR_DSMPD                                        PLL_PWR_DSMPD_Msk
#define PLL_PWR_PD_Pos                                       0
#define PLL_PWR_PD_Msk                                       ((uint32_t)0x00000001)
#define PLL_PWR_PD                                           PLL_PWR_PD_Msk
// PLL->FBDIV_INT
#define PLL_FBDIV_INT_FBDIV_INT_Pos                          0
#define PLL_FBDIV_INT_FBDIV_INT_Msk                          ((uint32_t)0x00000FFF)
#define PLL_FBDIV_INT_FBDIV_INT                              PLL_FBDIV_INT_FBDIV_INT_Msk
// PLL->PRIM
#define PLL_PRIM_POSTDIV1_Pos                                16
#define PLL_PRIM_POSTDIV1_Msk                                ((uint32_t)0x00070000)
#define PLL_PRIM_POSTDIV1                                    PLL_PRIM_POSTDIV1_Msk
#define PLL_PRIM_POSTDIV2_Pos                                12
#define PLL_PRIM_POSTDIV2_Msk                                ((uint32_t)0x00007000)
#define PLL_PRIM_POSTDIV2                                    PLL_PRIM_POSTDIV2_Msk

// PSM->FRCE_ON
#define PSM_FRCE_ON_PROC1_Pos                                16
#define PSM_FRCE_ON_PROC1_Msk                                ((uint32_t)0x00010000)
#define PSM_FRCE_ON_PROC1                                    PSM_FRCE_ON_PROC1_Msk
#define PSM_FRCE_ON_PROC0_Pos                                15
#define PSM_FRCE_ON_PROC0_Msk                                ((uint32_t)0x00008000)
#define PSM_FRCE_ON_PROC0                                    PSM_FRCE_ON_PROC0_Msk
#define PSM_FRCE_ON_SIO_Pos                                  14
#define PSM_FRCE_ON_SIO_Msk                                  ((uint32_t)0x00004000)
#define PSM_FRCE_ON_SIO                                      PSM_FRCE_ON_SIO_Msk
#define PSM_FRCE_ON_VREG_AND_CHIP_RESET_Pos                  13
#define PSM_FRCE_ON_VREG_AND_CHIP_RESET_Msk                  ((uint32_t)0x00002000)
#define PSM_FRCE_ON_VREG_AND_CHIP_RESET                      PSM_FRCE_ON_VREG_AND_CHIP_RESET_Msk
#define PSM_FRCE_ON_XIP_Pos                                  12
#define PSM_FRCE_ON_XIP_Msk                                  ((uint32_t)0x00001000)
#define PSM_FRCE_ON_XIP                                      PSM_FRCE_ON_XIP_Msk
#define PSM_FRCE_ON_SRAM5_Pos                                11
#define PSM_FRCE_ON_SRAM5_Msk                                ((uint32_t)0x00000800)
#define PSM_FRCE_ON_SRAM5                                    PSM_FRCE_ON_SRAM5_Msk
#define PSM_FRCE_ON_SRAM4_Pos                                10
#define PSM_FRCE_ON_SRAM4_Msk                                ((uint32_t)0x00000400)
#define PSM_FRCE_ON_SRAM4                                    PSM_FRCE_ON_SRAM4_Msk
#define PSM_FRCE_ON_SRAM3_Pos                                9
#define PSM_FRCE_ON_SRAM3_Msk                                ((uint32_t)0x00000200)
#define PSM_FRCE_ON_SRAM3                                    PSM_FRCE_ON_SRAM3_Msk
#define PSM_FRCE_ON_SRAM2_Pos                                8
#define PSM_FRCE_ON_SRAM2_Msk                                ((uint32_t)0x00000100)
#define PSM_FRCE_ON_SRAM2                                    PSM_FRCE_ON_SRAM2_Msk
#define PSM_FRCE_ON_SRAM1_Pos                                7
#define PSM_FRCE_ON_SRAM1_Msk                                ((uint32_t)0x00000080)
#define PSM_FRCE_ON_SRAM1                                    PSM_FRCE_ON_SRAM1_Msk
#define PSM_FRCE_ON_SRAM0_Pos                                6
#define PSM_FRCE_ON_SRAM0_Msk                                ((uint32_t)0x00000040)
#define PSM_FRCE_ON_SRAM0                                    PSM_FRCE_ON_SRAM0_Msk
#define PSM_FRCE_ON_ROM_Pos                                  5
#define PSM_FRCE_ON_ROM_Msk                                  ((uint32_t)0x00000020)
#define PSM_FRCE_ON_ROM                                      PSM_FRCE_ON_ROM_Msk
#define PSM_FRCE_ON_BUSFABRIC_Pos                            4
#define PSM_FRCE_ON_BUSFABRIC_Msk                            ((uint32_t)0x00000010)
#define PSM_FRCE_ON_BUSFABRIC                                PSM_FRCE_ON_BUSFABRIC_Msk
#define PSM_FRCE_ON_RESETS_Pos                               3
#define PSM_FRCE_ON_RESETS_Msk                               ((uint32_t)0x00000008)
#define PSM_FRCE_ON_RESETS                                   PSM_FRCE_ON_RESETS_Msk
#define PSM_FRCE_ON_CLOCKS_Pos                               2
#define PSM_FRCE_ON_CLOCKS_Msk                               ((uint32_t)0x00000004)
#define PSM_FRCE_ON_CLOCKS                                   PSM_FRCE_ON_CLOCKS_Msk
#define PSM_FRCE_ON_XOSC_Pos                                 1
#define PSM_FRCE_ON_XOSC_Msk                                 ((uint32_t)0x00000002)
#define PSM_FRCE_ON_XOSC                                     PSM_FRCE_ON_XOSC_Msk
#define PSM_FRCE_ON_ROSC_Pos                                 0
#define PSM_FRCE_ON_ROSC_Msk                                 ((uint32_t)0x00000001)
#define PSM_FRCE_ON_ROSC                                     PSM_FRCE_ON_ROSC_Msk
// PSM->FRCE_OFF
#define PSM_FRCE_OFF_PROC1_Pos                               16
#define PSM_FRCE_OFF_PROC1_Msk                               ((uint32_t)0x00010000)
#define PSM_FRCE_OFF_PROC1                                   PSM_FRCE_OFF_PROC1_Msk
#define PSM_FRCE_OFF_PROC0_Pos                               15
#define PSM_FRCE_OFF_PROC0_Msk                               ((uint32_t)0x00008000)
#define PSM_FRCE_OFF_PROC0                                   PSM_FRCE_OFF_PROC0_Msk
#define PSM_FRCE_OFF_SIO_Pos                                 14
#define PSM_FRCE_OFF_SIO_Msk                                 ((uint32_t)0x00004000)
#define PSM_FRCE_OFF_SIO                                     PSM_FRCE_OFF_SIO_Msk
#define PSM_FRCE_OFF_VREG_AND_CHIP_RESET_Pos                 13
#define PSM_FRCE_OFF_VREG_AND_CHIP_RESET_Msk                 ((uint32_t)0x00002000)
#define PSM_FRCE_OFF_VREG_AND_CHIP_RESET                     PSM_FRCE_OFF_VREG_AND_CHIP_RESET_Msk
#define PSM_FRCE_OFF_XIP_Pos                                 12
#define PSM_FRCE_OFF_XIP_Msk                                 ((uint32_t)0x00001000)
#define PSM_FRCE_OFF_XIP                                     PSM_FRCE_OFF_XIP_Msk
#define PSM_FRCE_OFF_SRAM5_Pos                               11
#define PSM_FRCE_OFF_SRAM5_Msk                               ((uint32_t)0x00000800)
#define PSM_FRCE_OFF_SRAM5                                   PSM_FRCE_OFF_SRAM5_Msk
#define PSM_FRCE_OFF_SRAM4_Pos                               10
#define PSM_FRCE_OFF_SRAM4_Msk                               ((uint32_t)0x00000400)
#define PSM_FRCE_OFF_SRAM4                                   PSM_FRCE_OFF_SRAM4_Msk
#define PSM_FRCE_OFF_SRAM3_Pos                               9
#define PSM_FRCE_OFF_SRAM3_Msk                               ((uint32_t)0x00000200)
#define PSM_FRCE_OFF_SRAM3                                   PSM_FRCE_OFF_SRAM3_Msk
#define PSM_FRCE_OFF_SRAM2_Pos                               8
#define PSM_FRCE_OFF_SRAM2_Msk                               ((uint32_t)0x00000100)
#define PSM_FRCE_OFF_SRAM2                                   PSM_FRCE_OFF_SRAM2_Msk
#define PSM_FRCE_OFF_SRAM1_Pos                               7
#define PSM_FRCE_OFF_SRAM1_Msk                               ((uint32_t)0x00000080)
#define PSM_FRCE_OFF_SRAM1                                   PSM_FRCE_OFF_SRAM1_Msk
#define PSM_FRCE_OFF_SRAM0_Pos                               6
#define PSM_FRCE_OFF_SRAM0_Msk                               ((uint32_t)0x00000040)
#define PSM_FRCE_OFF_SRAM0                                   PSM_FRCE_OFF_SRAM0_Msk
#define PSM_FRCE_OFF_ROM_Pos                                 5
#define PSM_FRCE_OFF_ROM_Msk                                 ((uint32_t)0x00000020)
#define PSM_FRCE_OFF_ROM                                     PSM_FRCE_OFF_ROM_Msk
#define PSM_FRCE_OFF_BUSFABRIC_Pos                           4
#define PSM_FRCE_OFF_BUSFABRIC_Msk                           ((uint32_t)0x00000010)
#define PSM_FRCE_OFF_BUSFABRIC                               PSM_FRCE_OFF_BUSFABRIC_Msk
#define PSM_FRCE_OFF_RESETS_Pos                              3
#define PSM_FRCE_OFF_RESETS_Msk                              ((uint32_t)0x00000008)
#define PSM_FRCE_OFF_RESETS                                  PSM_FRCE_OFF_RESETS_Msk
#define PSM_FRCE_OFF_CLOCKS_Pos                              2
#define PSM_FRCE_OFF_CLOCKS_Msk                              ((uint32_t)0x00000004)
#define PSM_FRCE_OFF_CLOCKS                                  PSM_FRCE_OFF_CLOCKS_Msk
#define PSM_FRCE_OFF_XOSC_Pos                                1
#define PSM_FRCE_OFF_XOSC_Msk                                ((uint32_t)0x00000002)
#define PSM_FRCE_OFF_XOSC                                    PSM_FRCE_OFF_XOSC_Msk
#define PSM_FRCE_OFF_ROSC_Pos                                0
#define PSM_FRCE_OFF_ROSC_Msk                                ((uint32_t)0x00000001)
#define PSM_FRCE_OFF_ROSC                                    PSM_FRCE_OFF_ROSC_Msk
// PSM->WDSEL
#define PSM_WDSEL_PROC1_Pos                                  16
#define PSM_WDSEL_PROC1_Msk                                  ((uint32_t)0x00010000)
#define PSM_WDSEL_PROC1                                      PSM_WDSEL_PROC1_Msk
#define PSM_WDSEL_PROC0_Pos                                  15
#define PSM_WDSEL_PROC0_Msk                                  ((uint32_t)0x00008000)
#define PSM_WDSEL_PROC0                                      PSM_WDSEL_PROC0_Msk
#define PSM_WDSEL_SIO_Pos                                    14
#define PSM_WDSEL_SIO_Msk                                    ((uint32_t)0x00004000)
#define PSM_WDSEL_SIO                                        PSM_WDSEL_SIO_Msk
#define PSM_WDSEL_VREG_AND_CHIP_RESET_Pos                    13
#define PSM_WDSEL_VREG_AND_CHIP_RESET_Msk                    ((uint32_t)0x00002000)
#define PSM_WDSEL_VREG_AND_CHIP_RESET                        PSM_WDSEL_VREG_AND_CHIP_RESET_Msk
#define PSM_WDSEL_XIP_Pos                                    12
#define PSM_WDSEL_XIP_Msk                                    ((uint32_t)0x00001000)
#define PSM_WDSEL_XIP                                        PSM_WDSEL_XIP_Msk
#define PSM_WDSEL_SRAM5_Pos                                  11
#define PSM_WDSEL_SRAM5_Msk                                  ((uint32_t)0x00000800)
#define PSM_WDSEL_SRAM5                                      PSM_WDSEL_SRAM5_Msk
#define PSM_WDSEL_SRAM4_Pos                                  10
#define PSM_WDSEL_SRAM4_Msk                                  ((uint32_t)0x00000400)
#define PSM_WDSEL_SRAM4                                      PSM_WDSEL_SRAM4_Msk
#define PSM_WDSEL_SRAM3_Pos                                  9
#define PSM_WDSEL_SRAM3_Msk                                  ((uint32_t)0x00000200)
#define PSM_WDSEL_SRAM3                                      PSM_WDSEL_SRAM3_Msk
#define PSM_WDSEL_SRAM2_Pos                                  8
#define PSM_WDSEL_SRAM2_Msk                                  ((uint32_t)0x00000100)
#define PSM_WDSEL_SRAM2                                      PSM_WDSEL_SRAM2_Msk
#define PSM_WDSEL_SRAM1_Pos                                  7
#define PSM_WDSEL_SRAM1_Msk                                  ((uint32_t)0x00000080)
#define PSM_WDSEL_SRAM1                                      PSM_WDSEL_SRAM1_Msk
#define PSM_WDSEL_SRAM0_Pos                                  6
#define PSM_WDSEL_SRAM0_Msk                                  ((uint32_t)0x00000040)
#define PSM_WDSEL_SRAM0                                      PSM_WDSEL_SRAM0_Msk
#define PSM_WDSEL_ROM_Pos                                    5
#define PSM_WDSEL_ROM_Msk                                    ((uint32_t)0x00000020)
#define PSM_WDSEL_ROM                                        PSM_WDSEL_ROM_Msk
#define PSM_WDSEL_BUSFABRIC_Pos                              4
#define PSM_WDSEL_BUSFABRIC_Msk                              ((uint32_t)0x00000010)
#define PSM_WDSEL_BUSFABRIC                                  PSM_WDSEL_BUSFABRIC_Msk
#define PSM_WDSEL_RESETS_Pos                                 3
#define PSM_WDSEL_RESETS_Msk                                 ((uint32_t)0x00000008)
#define PSM_WDSEL_RESETS                                     PSM_WDSEL_RESETS_Msk
#define PSM_WDSEL_CLOCKS_Pos                                 2
#define PSM_WDSEL_CLOCKS_Msk                                 ((uint32_t)0x00000004)
#define PSM_WDSEL_CLOCKS                                     PSM_WDSEL_CLOCKS_Msk
#define PSM_WDSEL_XOSC_Pos                                   1
#define PSM_WDSEL_XOSC_Msk                                   ((uint32_t)0x00000002)
#define PSM_WDSEL_XOSC                                       PSM_WDSEL_XOSC_Msk
#define PSM_WDSEL_ROSC_Pos                                   0
#define PSM_WDSEL_ROSC_Msk                                   ((uint32_t)0x00000001)
#define PSM_WDSEL_ROSC                                       PSM_WDSEL_ROSC_Msk
// PSM->DONE
#define PSM_DONE_PROC1_Pos                                   16
#define PSM_DONE_PROC1_Msk                                   ((uint32_t)0x00010000)
#define PSM_DONE_PROC1                                       PSM_DONE_PROC1_Msk
#define PSM_DONE_PROC0_Pos                                   15
#define PSM_DONE_PROC0_Msk                                   ((uint32_t)0x00008000)
#define PSM_DONE_PROC0                                       PSM_DONE_PROC0_Msk
#define PSM_DONE_SIO_Pos                                     14
#define PSM_DONE_SIO_Msk                                     ((uint32_t)0x00004000)
#define PSM_DONE_SIO                                         PSM_DONE_SIO_Msk
#define PSM_DONE_VREG_AND_CHIP_RESET_Pos                     13
#define PSM_DONE_VREG_AND_CHIP_RESET_Msk                     ((uint32_t)0x00002000)
#define PSM_DONE_VREG_AND_CHIP_RESET                         PSM_DONE_VREG_AND_CHIP_RESET_Msk
#define PSM_DONE_XIP_Pos                                     12
#define PSM_DONE_XIP_Msk                                     ((uint32_t)0x00001000)
#define PSM_DONE_XIP                                         PSM_DONE_XIP_Msk
#define PSM_DONE_SRAM5_Pos                                   11
#define PSM_DONE_SRAM5_Msk                                   ((uint32_t)0x00000800)
#define PSM_DONE_SRAM5                                       PSM_DONE_SRAM5_Msk
#define PSM_DONE_SRAM4_Pos                                   10
#define PSM_DONE_SRAM4_Msk                                   ((uint32_t)0x00000400)
#define PSM_DONE_SRAM4                                       PSM_DONE_SRAM4_Msk
#define PSM_DONE_SRAM3_Pos                                   9
#define PSM_DONE_SRAM3_Msk                                   ((uint32_t)0x00000200)
#define PSM_DONE_SRAM3                                       PSM_DONE_SRAM3_Msk
#define PSM_DONE_SRAM2_Pos                                   8
#define PSM_DONE_SRAM2_Msk                                   ((uint32_t)0x00000100)
#define PSM_DONE_SRAM2                                       PSM_DONE_SRAM2_Msk
#define PSM_DONE_SRAM1_Pos                                   7
#define PSM_DONE_SRAM1_Msk                                   ((uint32_t)0x00000080)
#define PSM_DONE_SRAM1                                       PSM_DONE_SRAM1_Msk
#define PSM_DONE_SRAM0_Pos                                   6
#define PSM_DONE_SRAM0_Msk                                   ((uint32_t)0x00000040)
#define PSM_DONE_SRAM0                                       PSM_DONE_SRAM0_Msk
#define PSM_DONE_ROM_Pos                                     5
#define PSM_DONE_ROM_Msk                                     ((uint32_t)0x00000020)
#define PSM_DONE_ROM                                         PSM_DONE_ROM_Msk
#define PSM_DONE_BUSFABRIC_Pos                               4
#define PSM_DONE_BUSFABRIC_Msk                               ((uint32_t)0x00000010)
#define PSM_DONE_BUSFABRIC                                   PSM_DONE_BUSFABRIC_Msk
#define PSM_DONE_RESETS_Pos                                  3
#define PSM_DONE_RESETS_Msk                                  ((uint32_t)0x00000008)
#define PSM_DONE_RESETS                                      PSM_DONE_RESETS_Msk
#define PSM_DONE_CLOCKS_Pos                                  2
#define PSM_DONE_CLOCKS_Msk                                  ((uint32_t)0x00000004)
#define PSM_DONE_CLOCKS                                      PSM_DONE_CLOCKS_Msk
#define PSM_DONE_XOSC_Pos                                    1
#define PSM_DONE_XOSC_Msk                                    ((uint32_t)0x00000002)
#define PSM_DONE_XOSC                                        PSM_DONE_XOSC_Msk
#define PSM_DONE_ROSC_Pos                                    0
#define PSM_DONE_ROSC_Msk                                    ((uint32_t)0x00000001)
#define PSM_DONE_ROSC                                        PSM_DONE_ROSC_Msk

// PWM->CH[n].CSR
#define PWM_CH_CSR_PH_ADV_Pos                                7
#define PWM_CH_CSR_PH_ADV_Msk                                ((uint32_t)0x00000080)
#define PWM_CH_CSR_PH_ADV                                    PWM_CH_CSR_PH_ADV_Msk
#define PWM_CH_CSR_PH_RET_Pos                                6
#define PWM_CH_CSR_PH_RET_Msk                                ((uint32_t)0x00000040)
#define PWM_CH_CSR_PH_RET                                    PWM_CH_CSR_PH_RET_Msk
#define PWM_CH_CSR_DIVMODE_Pos                               4
#define PWM_CH_CSR_DIVMODE_Msk                               ((uint32_t)0x00000030)
#define PWM_CH_CSR_DIVMODE                                   PWM_CH_CSR_DIVMODE_Msk
#define PWM_CH_CSR_B_INV_Pos                                 3
#define PWM_CH_CSR_B_INV_Msk                                 ((uint32_t)0x00000008)
#define PWM_CH_CSR_B_INV                                     PWM_CH_CSR_B_INV_Msk
#define PWM_CH_CSR_A_INV_Pos                                 2
#define PWM_CH_CSR_A_INV_Msk                                 ((uint32_t)0x00000004)
#define PWM_CH_CSR_A_INV                                     PWM_CH_CSR_A_INV_Msk
#define PWM_CH_CSR_PH_CORRECT_Pos                            1
#define PWM_CH_CSR_PH_CORRECT_Msk                            ((uint32_t)0x00000002)
#define PWM_CH_CSR_PH_CORRECT                                PWM_CH_CSR_PH_CORRECT_Msk
#define PWM_CH_CSR_EN_Pos                                    0
#define PWM_CH_CSR_EN_Msk                                    ((uint32_t)0x00000001)
#define PWM_CH_CSR_EN                                        PWM_CH_CSR_EN_Msk
// PWM->CH[n].DIV
#define PWM_CH_DIV_INT_Pos                                   4
#define PWM_CH_DIV_INT_Msk                                   ((uint32_t)0x00000FF0)
#define PWM_CH_DIV_INT                                       PWM_CH_DIV_INT_Msk
#define PWM_CH_DIV_FRAC_Pos                                  0
#define PWM_CH_DIV_FRAC_Msk                                  ((uint32_t)0x0000000F)
#define PWM_CH_DIV_FRAC                                      PWM_CH_DIV_FRAC_Msk
// PWM->CH[n].CTR
#define PWM_CH_CTR_CH0_CTR_Pos                               0
#define PWM_CH_CTR_CH0_CTR_Msk                               ((uint32_t)0x0000FFFF)
#define PWM_CH_CTR_CH0_CTR                                   PWM_CH_CTR_CH0_CTR_Msk
// PWM->CH[n].CC
#define PWM_CH_CC_B_Pos                                      16
#define PWM_CH_CC_B_Msk                                      ((uint32_t)0xFFFF0000)
#define PWM_CH_CC_B                                          PWM_CH_CC_B_Msk
#define PWM_CH_CC_A_Pos                                      0
#define PWM_CH_CC_A_Msk                                      ((uint32_t)0x0000FFFF)
#define PWM_CH_CC_A                                          PWM_CH_CC_A_Msk
// PWM->CH[n].TOP
#define PWM_CH_TOP_CH0_TOP_Pos                               0
#define PWM_CH_TOP_CH0_TOP_Msk                               ((uint32_t)0x0000FFFF)
#define PWM_CH_TOP_CH0_TOP                                   PWM_CH_TOP_CH0_TOP_Msk
// PWM->EN
#define PWM_EN_CH7_Pos                                       7
#define PWM_EN_CH7_Msk                                       ((uint32_t)0x00000080)
#define PWM_EN_CH7                                           PWM_EN_CH7_Msk
#define PWM_EN_CH6_Pos                                       6
#define PWM_EN_CH6_Msk                                       ((uint32_t)0x00000040)
#define PWM_EN_CH6                                           PWM_EN_CH6_Msk
#define PWM_EN_CH5_Pos                                       5
#define PWM_EN_CH5_Msk                                       ((uint32_t)0x00000020)
#define PWM_EN_CH5                                           PWM_EN_CH5_Msk
#define PWM_EN_CH4_Pos                                       4
#define PWM_EN_CH4_Msk                                       ((uint32_t)0x00000010)
#define PWM_EN_CH4                                           PWM_EN_CH4_Msk
#define PWM_EN_CH3_Pos                                       3
#define PWM_EN_CH3_Msk                                       ((uint32_t)0x00000008)
#define PWM_EN_CH3                                           PWM_EN_CH3_Msk
#define PWM_EN_CH2_Pos                                       2
#define PWM_EN_CH2_Msk                                       ((uint32_t)0x00000004)
#define PWM_EN_CH2                                           PWM_EN_CH2_Msk
#define PWM_EN_CH1_Pos                                       1
#define PWM_EN_CH1_Msk                                       ((uint32_t)0x00000002)
#define PWM_EN_CH1                                           PWM_EN_CH1_Msk
#define PWM_EN_CH0_Pos                                       0
#define PWM_EN_CH0_Msk                                       ((uint32_t)0x00000001)
#define PWM_EN_CH0                                           PWM_EN_CH0_Msk
// PWM->INTR
#define PWM_INTR_CH7_Pos                                     7
#define PWM_INTR_CH7_Msk                                     ((uint32_t)0x00000080)
#define PWM_INTR_CH7                                         PWM_INTR_CH7_Msk
#define PWM_INTR_CH6_Pos                                     6
#define PWM_INTR_CH6_Msk                                     ((uint32_t)0x00000040)
#define PWM_INTR_CH6                                         PWM_INTR_CH6_Msk
#define PWM_INTR_CH5_Pos                                     5
#define PWM_INTR_CH5_Msk                                     ((uint32_t)0x00000020)
#define PWM_INTR_CH5                                         PWM_INTR_CH5_Msk
#define PWM_INTR_CH4_Pos                                     4
#define PWM_INTR_CH4_Msk                                     ((uint32_t)0x00000010)
#define PWM_INTR_CH4                                         PWM_INTR_CH4_Msk
#define PWM_INTR_CH3_Pos                                     3
#define PWM_INTR_CH3_Msk                                     ((uint32_t)0x00000008)
#define PWM_INTR_CH3                                         PWM_INTR_CH3_Msk
#define PWM_INTR_CH2_Pos                                     2
#define PWM_INTR_CH2_Msk                                     ((uint32_t)0x00000004)
#define PWM_INTR_CH2                                         PWM_INTR_CH2_Msk
#define PWM_INTR_CH1_Pos                                     1
#define PWM_INTR_CH1_Msk                                     ((uint32_t)0x00000002)
#define PWM_INTR_CH1                                         PWM_INTR_CH1_Msk
#define PWM_INTR_CH0_Pos                                     0
#define PWM_INTR_CH0_Msk                                     ((uint32_t)0x00000001)
#define PWM_INTR_CH0                                         PWM_INTR_CH0_Msk
// PWM->INTE
#define PWM_INTE_CH7_Pos                                     7
#define PWM_INTE_CH7_Msk                                     ((uint32_t)0x00000080)
#define PWM_INTE_CH7                                         PWM_INTE_CH7_Msk
#define PWM_INTE_CH6_Pos                                     6
#define PWM_INTE_CH6_Msk                                     ((uint32_t)0x00000040)
#define PWM_INTE_CH6                                         PWM_INTE_CH6_Msk
#define PWM_INTE_CH5_Pos                                     5
#define PWM_INTE_CH5_Msk                                     ((uint32_t)0x00000020)
#define PWM_INTE_CH5                                         PWM_INTE_CH5_Msk
#define PWM_INTE_CH4_Pos                                     4
#define PWM_INTE_CH4_Msk                                     ((uint32_t)0x00000010)
#define PWM_INTE_CH4                                         PWM_INTE_CH4_Msk
#define PWM_INTE_CH3_Pos                                     3
#define PWM_INTE_CH3_Msk                                     ((uint32_t)0x00000008)
#define PWM_INTE_CH3                                         PWM_INTE_CH3_Msk
#define PWM_INTE_CH2_Pos                                     2
#define PWM_INTE_CH2_Msk                                     ((uint32_t)0x00000004)
#define PWM_INTE_CH2                                         PWM_INTE_CH2_Msk
#define PWM_INTE_CH1_Pos                                     1
#define PWM_INTE_CH1_Msk                                     ((uint32_t)0x00000002)
#define PWM_INTE_CH1                                         PWM_INTE_CH1_Msk
#define PWM_INTE_CH0_Pos                                     0
#define PWM_INTE_CH0_Msk                                     ((uint32_t)0x00000001)
#define PWM_INTE_CH0                                         PWM_INTE_CH0_Msk
// PWM->INTF
#define PWM_INTF_CH7_Pos                                     7
#define PWM_INTF_CH7_Msk                                     ((uint32_t)0x00000080)
#define PWM_INTF_CH7                                         PWM_INTF_CH7_Msk
#define PWM_INTF_CH6_Pos                                     6
#define PWM_INTF_CH6_Msk                                     ((uint32_t)0x00000040)
#define PWM_INTF_CH6                                         PWM_INTF_CH6_Msk
#define PWM_INTF_CH5_Pos                                     5
#define PWM_INTF_CH5_Msk                                     ((uint32_t)0x00000020)
#define PWM_INTF_CH5                                         PWM_INTF_CH5_Msk
#define PWM_INTF_CH4_Pos                                     4
#define PWM_INTF_CH4_Msk                                     ((uint32_t)0x00000010)
#define PWM_INTF_CH4                                         PWM_INTF_CH4_Msk
#define PWM_INTF_CH3_Pos                                     3
#define PWM_INTF_CH3_Msk                                     ((uint32_t)0x00000008)
#define PWM_INTF_CH3                                         PWM_INTF_CH3_Msk
#define PWM_INTF_CH2_Pos                                     2
#define PWM_INTF_CH2_Msk                                     ((uint32_t)0x00000004)
#define PWM_INTF_CH2                                         PWM_INTF_CH2_Msk
#define PWM_INTF_CH1_Pos                                     1
#define PWM_INTF_CH1_Msk                                     ((uint32_t)0x00000002)
#define PWM_INTF_CH1                                         PWM_INTF_CH1_Msk
#define PWM_INTF_CH0_Pos                                     0
#define PWM_INTF_CH0_Msk                                     ((uint32_t)0x00000001)
#define PWM_INTF_CH0                                         PWM_INTF_CH0_Msk
// PWM->INTS
#define PWM_INTS_CH7_Pos                                     7
#define PWM_INTS_CH7_Msk                                     ((uint32_t)0x00000080)
#define PWM_INTS_CH7                                         PWM_INTS_CH7_Msk
#define PWM_INTS_CH6_Pos                                     6
#define PWM_INTS_CH6_Msk                                     ((uint32_t)0x00000040)
#define PWM_INTS_CH6                                         PWM_INTS_CH6_Msk
#define PWM_INTS_CH5_Pos                                     5
#define PWM_INTS_CH5_Msk                                     ((uint32_t)0x00000020)
#define PWM_INTS_CH5                                         PWM_INTS_CH5_Msk
#define PWM_INTS_CH4_Pos                                     4
#define PWM_INTS_CH4_Msk                                     ((uint32_t)0x00000010)
#define PWM_INTS_CH4                                         PWM_INTS_CH4_Msk
#define PWM_INTS_CH3_Pos                                     3
#define PWM_INTS_CH3_Msk                                     ((uint32_t)0x00000008)
#define PWM_INTS_CH3                                         PWM_INTS_CH3_Msk
#define PWM_INTS_CH2_Pos                                     2
#define PWM_INTS_CH2_Msk                                     ((uint32_t)0x00000004)
#define PWM_INTS_CH2                                         PWM_INTS_CH2_Msk
#define PWM_INTS_CH1_Pos                                     1
#define PWM_INTS_CH1_Msk                                     ((uint32_t)0x00000002)
#define PWM_INTS_CH1                                         PWM_INTS_CH1_Msk
#define PWM_INTS_CH0_Pos                                     0
#define PWM_INTS_CH0_Msk                                     ((uint32_t)0x00000001)
#define PWM_INTS_CH0                                         PWM_INTS_CH0_Msk

// RESETS->RESET
#define RESETS_RESET_USBCTRL_Pos                             24
#define RESETS_RESET_USBCTRL_Msk                             ((uint32_t)0x01000000)
#define RESETS_RESET_USBCTRL                                 RESETS_RESET_USBCTRL_Msk
#define RESETS_RESET_UART1_Pos                               23
#define RESETS_RESET_UART1_Msk                               ((uint32_t)0x00800000)
#define RESETS_RESET_UART1                                   RESETS_RESET_UART1_Msk
#define RESETS_RESET_UART0_Pos                               22
#define RESETS_RESET_UART0_Msk                               ((uint32_t)0x00400000)
#define RESETS_RESET_UART0                                   RESETS_RESET_UART0_Msk
#define RESETS_RESET_TIMER_Pos                               21
#define RESETS_RESET_TIMER_Msk                               ((uint32_t)0x00200000)
#define RESETS_RESET_TIMER                                   RESETS_RESET_TIMER_Msk
#define RESETS_RESET_TBMAN_Pos                               20
#define RESETS_RESET_TBMAN_Msk                               ((uint32_t)0x00100000)
#define RESETS_RESET_TBMAN                                   RESETS_RESET_TBMAN_Msk
#define RESETS_RESET_SYSINFO_Pos                             19
#define RESETS_RESET_SYSINFO_Msk                             ((uint32_t)0x00080000)
#define RESETS_RESET_SYSINFO                                 RESETS_RESET_SYSINFO_Msk
#define RESETS_RESET_SYSCFG_Pos                              18
#define RESETS_RESET_SYSCFG_Msk                              ((uint32_t)0x00040000)
#define RESETS_RESET_SYSCFG                                  RESETS_RESET_SYSCFG_Msk
#define RESETS_RESET_SPI1_Pos                                17
#define RESETS_RESET_SPI1_Msk                                ((uint32_t)0x00020000)
#define RESETS_RESET_SPI1                                    RESETS_RESET_SPI1_Msk
#define RESETS_RESET_SPI0_Pos                                16
#define RESETS_RESET_SPI0_Msk                                ((uint32_t)0x00010000)
#define RESETS_RESET_SPI0                                    RESETS_RESET_SPI0_Msk
#define RESETS_RESET_RTC_Pos                                 15
#define RESETS_RESET_RTC_Msk                                 ((uint32_t)0x00008000)
#define RESETS_RESET_RTC                                     RESETS_RESET_RTC_Msk
#define RESETS_RESET_PWM_Pos                                 14
#define RESETS_RESET_PWM_Msk                                 ((uint32_t)0x00004000)
#define RESETS_RESET_PWM                                     RESETS_RESET_PWM_Msk
#define RESETS_RESET_PLL_USB_Pos                             13
#define RESETS_RESET_PLL_USB_Msk                             ((uint32_t)0x00002000)
#define RESETS_RESET_PLL_USB                                 RESETS_RESET_PLL_USB_Msk
#define RESETS_RESET_PLL_SYS_Pos                             12
#define RESETS_RESET_PLL_SYS_Msk                             ((uint32_t)0x00001000)
#define RESETS_RESET_PLL_SYS                                 RESETS_RESET_PLL_SYS_Msk
#define RESETS_RESET_PIO1_Pos                                11
#define RESETS_RESET_PIO1_Msk                                ((uint32_t)0x00000800)
#define RESETS_RESET_PIO1                                    RESETS_RESET_PIO1_Msk
#define RESETS_RESET_PIO0_Pos                                10
#define RESETS_RESET_PIO0_Msk                                ((uint32_t)0x00000400)
#define RESETS_RESET_PIO0                                    RESETS_RESET_PIO0_Msk
#define RESETS_RESET_PADS_QSPI_Pos                           9
#define RESETS_RESET_PADS_QSPI_Msk                           ((uint32_t)0x00000200)
#define RESETS_RESET_PADS_QSPI                               RESETS_RESET_PADS_QSPI_Msk
#define RESETS_RESET_PADS_BANK0_Pos                          8
#define RESETS_RESET_PADS_BANK0_Msk                          ((uint32_t)0x00000100)
#define RESETS_RESET_PADS_BANK0                              RESETS_RESET_PADS_BANK0_Msk
#define RESETS_RESET_JTAG_Pos                                7
#define RESETS_RESET_JTAG_Msk                                ((uint32_t)0x00000080)
#define RESETS_RESET_JTAG                                    RESETS_RESET_JTAG_Msk
#define RESETS_RESET_IO_QSPI_Pos                             6
#define RESETS_RESET_IO_QSPI_Msk                             ((uint32_t)0x00000040)
#define RESETS_RESET_IO_QSPI                                 RESETS_RESET_IO_QSPI_Msk
#define RESETS_RESET_IO_BANK0_Pos                            5
#define RESETS_RESET_IO_BANK0_Msk                            ((uint32_t)0x00000020)
#define RESETS_RESET_IO_BANK0                                RESETS_RESET_IO_BANK0_Msk
#define RESETS_RESET_I2C1_Pos                                4
#define RESETS_RESET_I2C1_Msk                                ((uint32_t)0x00000010)
#define RESETS_RESET_I2C1                                    RESETS_RESET_I2C1_Msk
#define RESETS_RESET_I2C0_Pos                                3
#define RESETS_RESET_I2C0_Msk                                ((uint32_t)0x00000008)
#define RESETS_RESET_I2C0                                    RESETS_RESET_I2C0_Msk
#define RESETS_RESET_DMA_Pos                                 2
#define RESETS_RESET_DMA_Msk                                 ((uint32_t)0x00000004)
#define RESETS_RESET_DMA                                     RESETS_RESET_DMA_Msk
#define RESETS_RESET_BUSCTRL_Pos                             1
#define RESETS_RESET_BUSCTRL_Msk                             ((uint32_t)0x00000002)
#define RESETS_RESET_BUSCTRL                                 RESETS_RESET_BUSCTRL_Msk
#define RESETS_RESET_ADC_Pos                                 0
#define RESETS_RESET_ADC_Msk                                 ((uint32_t)0x00000001)
#define RESETS_RESET_ADC                                     RESETS_RESET_ADC_Msk
// RESETS->WDSEL
#define RESETS_WDSEL_USBCTRL_Pos                             24
#define RESETS_WDSEL_USBCTRL_Msk                             ((uint32_t)0x01000000)
#define RESETS_WDSEL_USBCTRL                                 RESETS_WDSEL_USBCTRL_Msk
#define RESETS_WDSEL_UART1_Pos                               23
#define RESETS_WDSEL_UART1_Msk                               ((uint32_t)0x00800000)
#define RESETS_WDSEL_UART1                                   RESETS_WDSEL_UART1_Msk
#define RESETS_WDSEL_UART0_Pos                               22
#define RESETS_WDSEL_UART0_Msk                               ((uint32_t)0x00400000)
#define RESETS_WDSEL_UART0                                   RESETS_WDSEL_UART0_Msk
#define RESETS_WDSEL_TIMER_Pos                               21
#define RESETS_WDSEL_TIMER_Msk                               ((uint32_t)0x00200000)
#define RESETS_WDSEL_TIMER                                   RESETS_WDSEL_TIMER_Msk
#define RESETS_WDSEL_TBMAN_Pos                               20
#define RESETS_WDSEL_TBMAN_Msk                               ((uint32_t)0x00100000)
#define RESETS_WDSEL_TBMAN                                   RESETS_WDSEL_TBMAN_Msk
#define RESETS_WDSEL_SYSINFO_Pos                             19
#define RESETS_WDSEL_SYSINFO_Msk                             ((uint32_t)0x00080000)
#define RESETS_WDSEL_SYSINFO                                 RESETS_WDSEL_SYSINFO_Msk
#define RESETS_WDSEL_SYSCFG_Pos                              18
#define RESETS_WDSEL_SYSCFG_Msk                              ((uint32_t)0x00040000)
#define RESETS_WDSEL_SYSCFG                                  RESETS_WDSEL_SYSCFG_Msk
#define RESETS_WDSEL_SPI1_Pos                                17
#define RESETS_WDSEL_SPI1_Msk                                ((uint32_t)0x00020000)
#define RESETS_WDSEL_SPI1                                    RESETS_WDSEL_SPI1_Msk
#define RESETS_WDSEL_SPI0_Pos                                16
#define RESETS_WDSEL_SPI0_Msk                                ((uint32_t)0x00010000)
#define RESETS_WDSEL_SPI0                                    RESETS_WDSEL_SPI0_Msk
#define RESETS_WDSEL_RTC_Pos                                 15
#define RESETS_WDSEL_RTC_Msk                                 ((uint32_t)0x00008000)
#define RESETS_WDSEL_RTC                                     RESETS_WDSEL_RTC_Msk
#define RESETS_WDSEL_PWM_Pos                                 14
#define RESETS_WDSEL_PWM_Msk                                 ((uint32_t)0x00004000)
#define RESETS_WDSEL_PWM                                     RESETS_WDSEL_PWM_Msk
#define RESETS_WDSEL_PLL_USB_Pos                             13
#define RESETS_WDSEL_PLL_USB_Msk                             ((uint32_t)0x00002000)
#define RESETS_WDSEL_PLL_USB                                 RESETS_WDSEL_PLL_USB_Msk
#define RESETS_WDSEL_PLL_SYS_Pos                             12
#define RESETS_WDSEL_PLL_SYS_Msk                             ((uint32_t)0x00001000)
#define RESETS_WDSEL_PLL_SYS                                 RESETS_WDSEL_PLL_SYS_Msk
#define RESETS_WDSEL_PIO1_Pos                                11
#define RESETS_WDSEL_PIO1_Msk                                ((uint32_t)0x00000800)
#define RESETS_WDSEL_PIO1                                    RESETS_WDSEL_PIO1_Msk
#define RESETS_WDSEL_PIO0_Pos                                10
#define RESETS_WDSEL_PIO0_Msk                                ((uint32_t)0x00000400)
#define RESETS_WDSEL_PIO0                                    RESETS_WDSEL_PIO0_Msk
#define RESETS_WDSEL_PADS_QSPI_Pos                           9
#define RESETS_WDSEL_PADS_QSPI_Msk                           ((uint32_t)0x00000200)
#define RESETS_WDSEL_PADS_QSPI                               RESETS_WDSEL_PADS_QSPI_Msk
#define RESETS_WDSEL_PADS_BANK0_Pos                          8
#define RESETS_WDSEL_PADS_BANK0_Msk                          ((uint32_t)0x00000100)
#define RESETS_WDSEL_PADS_BANK0                              RESETS_WDSEL_PADS_BANK0_Msk
#define RESETS_WDSEL_JTAG_Pos                                7
#define RESETS_WDSEL_JTAG_Msk                                ((uint32_t)0x00000080)
#define RESETS_WDSEL_JTAG                                    RESETS_WDSEL_JTAG_Msk
#define RESETS_WDSEL_IO_QSPI_Pos                             6
#define RESETS_WDSEL_IO_QSPI_Msk                             ((uint32_t)0x00000040)
#define RESETS_WDSEL_IO_QSPI                                 RESETS_WDSEL_IO_QSPI_Msk
#define RESETS_WDSEL_IO_BANK0_Pos                            5
#define RESETS_WDSEL_IO_BANK0_Msk                            ((uint32_t)0x00000020)
#define RESETS_WDSEL_IO_BANK0                                RESETS_WDSEL_IO_BANK0_Msk
#define RESETS_WDSEL_I2C1_Pos                                4
#define RESETS_WDSEL_I2C1_Msk                                ((uint32_t)0x00000010)
#define RESETS_WDSEL_I2C1                                    RESETS_WDSEL_I2C1_Msk
#define RESETS_WDSEL_I2C0_Pos                                3
#define RESETS_WDSEL_I2C0_Msk                                ((uint32_t)0x00000008)
#define RESETS_WDSEL_I2C0                                    RESETS_WDSEL_I2C0_Msk
#define RESETS_WDSEL_DMA_Pos                                 2
#define RESETS_WDSEL_DMA_Msk                                 ((uint32_t)0x00000004)
#define RESETS_WDSEL_DMA                                     RESETS_WDSEL_DMA_Msk
#define RESETS_WDSEL_BUSCTRL_Pos                             1
#define RESETS_WDSEL_BUSCTRL_Msk                             ((uint32_t)0x00000002)
#define RESETS_WDSEL_BUSCTRL                                 RESETS_WDSEL_BUSCTRL_Msk
#define RESETS_WDSEL_ADC_Pos                                 0
#define RESETS_WDSEL_ADC_Msk                                 ((uint32_t)0x00000001)
#define RESETS_WDSEL_ADC                                     RESETS_WDSEL_ADC_Msk
// RESETS->RESET_DONE
#define RESETS_RESET_DONE_USBCTRL_Pos                        24
#define RESETS_RESET_DONE_USBCTRL_Msk                        ((uint32_t)0x01000000)
#define RESETS_RESET_DONE_USBCTRL                            RESETS_RESET_DONE_USBCTRL_Msk
#define RESETS_RESET_DONE_UART1_Pos                          23
#define RESETS_RESET_DONE_UART1_Msk                          ((uint32_t)0x00800000)
#define RESETS_RESET_DONE_UART1                              RESETS_RESET_DONE_UART1_Msk
#define RESETS_RESET_DONE_UART0_Pos                          22
#define RESETS_RESET_DONE_UART0_Msk                          ((uint32_t)0x00400000)
#define RESETS_RESET_DONE_UART0                              RESETS_RESET_DONE_UART0_Msk
#define RESETS_RESET_DONE_TIMER_Pos                          21
#define RESETS_RESET_DONE_TIMER_Msk                          ((uint32_t)0x00200000)
#define RESETS_RESET_DONE_TIMER                              RESETS_RESET_DONE_TIMER_Msk
#define RESETS_RESET_DONE_TBMAN_Pos                          20
#define RESETS_RESET_DONE_TBMAN_Msk                          ((uint32_t)0x00100000)
#define RESETS_RESET_DONE_TBMAN                              RESETS_RESET_DONE_TBMAN_Msk
#define RESETS_RESET_DONE_SYSINFO_Pos                        19
#define RESETS_RESET_DONE_SYSINFO_Msk                        ((uint32_t)0x00080000)
#define RESETS_RESET_DONE_SYSINFO                            RESETS_RESET_DONE_SYSINFO_Msk
#define RESETS_RESET_DONE_SYSCFG_Pos                         18
#define RESETS_RESET_DONE_SYSCFG_Msk                         ((uint32_t)0x00040000)
#define RESETS_RESET_DONE_SYSCFG                             RESETS_RESET_DONE_SYSCFG_Msk
#define RESETS_RESET_DONE_SPI1_Pos                           17
#define RESETS_RESET_DONE_SPI1_Msk                           ((uint32_t)0x00020000)
#define RESETS_RESET_DONE_SPI1                               RESETS_RESET_DONE_SPI1_Msk
#define RESETS_RESET_DONE_SPI0_Pos                           16
#define RESETS_RESET_DONE_SPI0_Msk                           ((uint32_t)0x00010000)
#define RESETS_RESET_DONE_SPI0                               RESETS_RESET_DONE_SPI0_Msk
#define RESETS_RESET_DONE_RTC_Pos                            15
#define RESETS_RESET_DONE_RTC_Msk                            ((uint32_t)0x00008000)
#define RESETS_RESET_DONE_RTC                                RESETS_RESET_DONE_RTC_Msk
#define RESETS_RESET_DONE_PWM_Pos                            14
#define RESETS_RESET_DONE_PWM_Msk                            ((uint32_t)0x00004000)
#define RESETS_RESET_DONE_PWM                                RESETS_RESET_DONE_PWM_Msk
#define RESETS_RESET_DONE_PLL_USB_Pos                        13
#define RESETS_RESET_DONE_PLL_USB_Msk                        ((uint32_t)0x00002000)
#define RESETS_RESET_DONE_PLL_USB                            RESETS_RESET_DONE_PLL_USB_Msk
#define RESETS_RESET_DONE_PLL_SYS_Pos                        12
#define RESETS_RESET_DONE_PLL_SYS_Msk                        ((uint32_t)0x00001000)
#define RESETS_RESET_DONE_PLL_SYS                            RESETS_RESET_DONE_PLL_SYS_Msk
#define RESETS_RESET_DONE_PIO1_Pos                           11
#define RESETS_RESET_DONE_PIO1_Msk                           ((uint32_t)0x00000800)
#define RESETS_RESET_DONE_PIO1                               RESETS_RESET_DONE_PIO1_Msk
#define RESETS_RESET_DONE_PIO0_Pos                           10
#define RESETS_RESET_DONE_PIO0_Msk                           ((uint32_t)0x00000400)
#define RESETS_RESET_DONE_PIO0                               RESETS_RESET_DONE_PIO0_Msk
#define RESETS_RESET_DONE_PADS_QSPI_Pos                      9
#define RESETS_RESET_DONE_PADS_QSPI_Msk                      ((uint32_t)0x00000200)
#define RESETS_RESET_DONE_PADS_QSPI                          RESETS_RESET_DONE_PADS_QSPI_Msk
#define RESETS_RESET_DONE_PADS_BANK0_Pos                     8
#define RESETS_RESET_DONE_PADS_BANK0_Msk                     ((uint32_t)0x00000100)
#define RESETS_RESET_DONE_PADS_BANK0                         RESETS_RESET_DONE_PADS_BANK0_Msk
#define RESETS_RESET_DONE_JTAG_Pos                           7
#define RESETS_RESET_DONE_JTAG_Msk                           ((uint32_t)0x00000080)
#define RESETS_RESET_DONE_JTAG                               RESETS_RESET_DONE_JTAG_Msk
#define RESETS_RESET_DONE_IO_QSPI_Pos                        6
#define RESETS_RESET_DONE_IO_QSPI_Msk                        ((uint32_t)0x00000040)
#define RESETS_RESET_DONE_IO_QSPI                            RESETS_RESET_DONE_IO_QSPI_Msk
#define RESETS_RESET_DONE_IO_BANK0_Pos                       5
#define RESETS_RESET_DONE_IO_BANK0_Msk                       ((uint32_t)0x00000020)
#define RESETS_RESET_DONE_IO_BANK0                           RESETS_RESET_DONE_IO_BANK0_Msk
#define RESETS_RESET_DONE_I2C1_Pos                           4
#define RESETS_RESET_DONE_I2C1_Msk                           ((uint32_t)0x00000010)
#define RESETS_RESET_DONE_I2C1                               RESETS_RESET_DONE_I2C1_Msk
#define RESETS_RESET_DONE_I2C0_Pos                           3
#define RESETS_RESET_DONE_I2C0_Msk                           ((uint32_t)0x00000008)
#define RESETS_RESET_DONE_I2C0                               RESETS_RESET_DONE_I2C0_Msk
#define RESETS_RESET_DONE_DMA_Pos                            2
#define RESETS_RESET_DONE_DMA_Msk                            ((uint32_t)0x00000004)
#define RESETS_RESET_DONE_DMA                                RESETS_RESET_DONE_DMA_Msk
#define RESETS_RESET_DONE_BUSCTRL_Pos                        1
#define RESETS_RESET_DONE_BUSCTRL_Msk                        ((uint32_t)0x00000002)
#define RESETS_RESET_DONE_BUSCTRL                            RESETS_RESET_DONE_BUSCTRL_Msk
#define RESETS_RESET_DONE_ADC_Pos                            0
#define RESETS_RESET_DONE_ADC_Msk                            ((uint32_t)0x00000001)
#define RESETS_RESET_DONE_ADC                                RESETS_RESET_DONE_ADC_Msk

// ROSC->CTRL
#define ROSC_CTRL_ENABLE_Pos                                 12
#define ROSC_CTRL_ENABLE_Msk                                 ((uint32_t)0x00FFF000)
#define ROSC_CTRL_ENABLE                                     ROSC_CTRL_ENABLE_Msk
#define ROSC_CTRL_ENABLE_DISABLE                             ((uint32_t)0x00D1E000)
#define ROSC_CTRL_ENABLE_ENABLE                              ((uint32_t)0x00FAB000)
#define ROSC_CTRL_FREQ_RANGE_Pos                             0
#define ROSC_CTRL_FREQ_RANGE_Msk                             ((uint32_t)0x00000FFF)
#define ROSC_CTRL_FREQ_RANGE                                 ROSC_CTRL_FREQ_RANGE_Msk
#define ROSC_CTRL_FREQ_RANGE_LOW                             ((uint32_t)0x00000FA4)
#define ROSC_CTRL_FREQ_RANGE_MEDIUM                          ((uint32_t)0x00000FA5)
#define ROSC_CTRL_FREQ_RANGE_HIGH                            ((uint32_t)0x00000FA7)
#define ROSC_CTRL_FREQ_RANGE_TOOHIGH                         ((uint32_t)0x00000FA6)
// ROSC->FREQA
#define ROSC_FREQA_PASSWD_Pos                                16
#define ROSC_FREQA_PASSWD_Msk                                ((uint32_t)0xFFFF0000)
#define ROSC_FREQA_PASSWD                                    ROSC_FREQA_PASSWD_Msk
#define ROSC_FREQA_PASSWD_PASS                               ((uint32_t)0x96960000)
#define ROSC_FREQA_DS3_Pos                                   12
#define ROSC_FREQA_DS3_Msk                                   ((uint32_t)0x00007000)
#define ROSC_FREQA_DS3                                       ROSC_FREQA_DS3_Msk
#define ROSC_FREQA_DS2_Pos                                   8
#define ROSC_FREQA_DS2_Msk                                   ((uint32_t)0x00000700)
#define ROSC_FREQA_DS2                                       ROSC_FREQA_DS2_Msk
#define ROSC_FREQA_DS1_Pos                                   4
#define ROSC_FREQA_DS1_Msk                                   ((uint32_t)0x00000070)
#define ROSC_FREQA_DS1                                       ROSC_FREQA_DS1_Msk
#define ROSC_FREQA_DS0_Pos                                   0
#define ROSC_FREQA_DS0_Msk                                   ((uint32_t)0x00000007)
#define ROSC_FREQA_DS0                                       ROSC_FREQA_DS0_Msk
// ROSC->FREQB
#define ROSC_FREQB_PASSWD_Pos                                16
#define ROSC_FREQB_PASSWD_Msk                                ((uint32_t)0xFFFF0000)
#define ROSC_FREQB_PASSWD                                    ROSC_FREQB_PASSWD_Msk
#define ROSC_FREQB_PASSWD_PASS                               ((uint32_t)0x96960000)
#define ROSC_FREQB_DS7_Pos                                   12
#define ROSC_FREQB_DS7_Msk                                   ((uint32_t)0x00007000)
#define ROSC_FREQB_DS7                                       ROSC_FREQB_DS7_Msk
#define ROSC_FREQB_DS6_Pos                                   8
#define ROSC_FREQB_DS6_Msk                                   ((uint32_t)0x00000700)
#define ROSC_FREQB_DS6                                       ROSC_FREQB_DS6_Msk
#define ROSC_FREQB_DS5_Pos                                   4
#define ROSC_FREQB_DS5_Msk                                   ((uint32_t)0x00000070)
#define ROSC_FREQB_DS5                                       ROSC_FREQB_DS5_Msk
#define ROSC_FREQB_DS4_Pos                                   0
#define ROSC_FREQB_DS4_Msk                                   ((uint32_t)0x00000007)
#define ROSC_FREQB_DS4                                       ROSC_FREQB_DS4_Msk
// ROSC->DORMANT
#define ROSC_DORMANT_DORMANT_Pos                             0
#define ROSC_DORMANT_DORMANT_Msk                             ((uint32_t)0xFFFFFFFF)
#define ROSC_DORMANT_DORMANT                                 ROSC_DORMANT_DORMANT_Msk
#define ROSC_DORMANT_DORMANT_DORMANT                         ((uint32_t)0x636F6D61)
#define ROSC_DORMANT_DORMANT_WAKE                            ((uint32_t)0x77616B65)
// ROSC->DIV
#define ROSC_DIV_DIV_Pos                                     0
#define ROSC_DIV_DIV_Msk                                     ((uint32_t)0x00000FFF)
#define ROSC_DIV_DIV                                         ROSC_DIV_DIV_Msk
#define ROSC_DIV_DIV_PASS                                    ((uint32_t)0x00000AA0)
// ROSC->PHASE
#define ROSC_PHASE_PASSWD_Pos                                4
#define ROSC_PHASE_PASSWD_Msk                                ((uint32_t)0x00000FF0)
#define ROSC_PHASE_PASSWD                                    ROSC_PHASE_PASSWD_Msk
#define ROSC_PHASE_ENABLE_Pos                                3
#define ROSC_PHASE_ENABLE_Msk                                ((uint32_t)0x00000008)
#define ROSC_PHASE_ENABLE                                    ROSC_PHASE_ENABLE_Msk
#define ROSC_PHASE_FLIP_Pos                                  2
#define ROSC_PHASE_FLIP_Msk                                  ((uint32_t)0x00000004)
#define ROSC_PHASE_FLIP                                      ROSC_PHASE_FLIP_Msk
#define ROSC_PHASE_SHIFT_Pos                                 0
#define ROSC_PHASE_SHIFT_Msk                                 ((uint32_t)0x00000003)
#define ROSC_PHASE_SHIFT                                     ROSC_PHASE_SHIFT_Msk
// ROSC->STATUS
#define ROSC_STATUS_STABLE_Pos                               31
#define ROSC_STATUS_STABLE_Msk                               ((uint32_t)0x80000000)
#define ROSC_STATUS_STABLE                                   ROSC_STATUS_STABLE_Msk
#define ROSC_STATUS_BADWRITE_Pos                             24
#define ROSC_STATUS_BADWRITE_Msk                             ((uint32_t)0x01000000)
#define ROSC_STATUS_BADWRITE                                 ROSC_STATUS_BADWRITE_Msk
#define ROSC_STATUS_DIV_RUNNING_Pos                          16
#define ROSC_STATUS_DIV_RUNNING_Msk                          ((uint32_t)0x00010000)
#define ROSC_STATUS_DIV_RUNNING                              ROSC_STATUS_DIV_RUNNING_Msk
#define ROSC_STATUS_ENABLED_Pos                              12
#define ROSC_STATUS_ENABLED_Msk                              ((uint32_t)0x00001000)
#define ROSC_STATUS_ENABLED                                  ROSC_STATUS_ENABLED_Msk
// ROSC->RANDOMBIT
#define ROSC_RANDOMBIT_RANDOMBIT_Pos                         0
#define ROSC_RANDOMBIT_RANDOMBIT_Msk                         ((uint32_t)0x00000001)
#define ROSC_RANDOMBIT_RANDOMBIT                             ROSC_RANDOMBIT_RANDOMBIT_Msk
// ROSC->COUNT
#define ROSC_COUNT_COUNT_Pos                                 0
#define ROSC_COUNT_COUNT_Msk                                 ((uint32_t)0x000000FF)
#define ROSC_COUNT_COUNT                                     ROSC_COUNT_COUNT_Msk

// RTC->CLKDIV_M1
#define RTC_CLKDIV_M1_CLKDIV_M1_Pos                          0
#define RTC_CLKDIV_M1_CLKDIV_M1_Msk                          ((uint32_t)0x0000FFFF)
#define RTC_CLKDIV_M1_CLKDIV_M1                              RTC_CLKDIV_M1_CLKDIV_M1_Msk
// RTC->SETUP_0
#define RTC_SETUP_0_YEAR_Pos                                 12
#define RTC_SETUP_0_YEAR_Msk                                 ((uint32_t)0x00FFF000)
#define RTC_SETUP_0_YEAR                                     RTC_SETUP_0_YEAR_Msk
#define RTC_SETUP_0_MONTH_Pos                                8
#define RTC_SETUP_0_MONTH_Msk                                ((uint32_t)0x00000F00)
#define RTC_SETUP_0_MONTH                                    RTC_SETUP_0_MONTH_Msk
#define RTC_SETUP_0_DAY_Pos                                  0
#define RTC_SETUP_0_DAY_Msk                                  ((uint32_t)0x0000001F)
#define RTC_SETUP_0_DAY                                      RTC_SETUP_0_DAY_Msk
// RTC->SETUP_1
#define RTC_SETUP_1_DOTW_Pos                                 24
#define RTC_SETUP_1_DOTW_Msk                                 ((uint32_t)0x07000000)
#define RTC_SETUP_1_DOTW                                     RTC_SETUP_1_DOTW_Msk
#define RTC_SETUP_1_HOUR_Pos                                 16
#define RTC_SETUP_1_HOUR_Msk                                 ((uint32_t)0x001F0000)
#define RTC_SETUP_1_HOUR                                     RTC_SETUP_1_HOUR_Msk
#define RTC_SETUP_1_MIN_Pos                                  8
#define RTC_SETUP_1_MIN_Msk                                  ((uint32_t)0x00003F00)
#define RTC_SETUP_1_MIN                                      RTC_SETUP_1_MIN_Msk
#define RTC_SETUP_1_SEC_Pos                                  0
#define RTC_SETUP_1_SEC_Msk                                  ((uint32_t)0x0000003F)
#define RTC_SETUP_1_SEC                                      RTC_SETUP_1_SEC_Msk
// RTC->CTRL
#define RTC_CTRL_FORCE_NOTLEAPYEAR_Pos                       8
#define RTC_CTRL_FORCE_NOTLEAPYEAR_Msk                       ((uint32_t)0x00000100)
#define RTC_CTRL_FORCE_NOTLEAPYEAR                           RTC_CTRL_FORCE_NOTLEAPYEAR_Msk
#define RTC_CTRL_LOAD_Pos                                    4
#define RTC_CTRL_LOAD_Msk                                    ((uint32_t)0x00000010)
#define RTC_CTRL_LOAD                                        RTC_CTRL_LOAD_Msk
#define RTC_CTRL_RTC_ACTIVE_Pos                              1
#define RTC_CTRL_RTC_ACTIVE_Msk                              ((uint32_t)0x00000002)
#define RTC_CTRL_RTC_ACTIVE                                  RTC_CTRL_RTC_ACTIVE_Msk
#define RTC_CTRL_RTC_ENABLE_Pos                              0
#define RTC_CTRL_RTC_ENABLE_Msk                              ((uint32_t)0x00000001)
#define RTC_CTRL_RTC_ENABLE                                  RTC_CTRL_RTC_ENABLE_Msk
// RTC->IRQ_SETUP_0
#define RTC_IRQ_SETUP_0_MATCH_ACTIVE_Pos                     29
#define RTC_IRQ_SETUP_0_MATCH_ACTIVE_Msk                     ((uint32_t)0x20000000)
#define RTC_IRQ_SETUP_0_MATCH_ACTIVE                         RTC_IRQ_SETUP_0_MATCH_ACTIVE_Msk
#define RTC_IRQ_SETUP_0_MATCH_ENA_Pos                        28
#define RTC_IRQ_SETUP_0_MATCH_ENA_Msk                        ((uint32_t)0x10000000)
#define RTC_IRQ_SETUP_0_MATCH_ENA                            RTC_IRQ_SETUP_0_MATCH_ENA_Msk
#define RTC_IRQ_SETUP_0_YEAR_ENA_Pos                         26
#define RTC_IRQ_SETUP_0_YEAR_ENA_Msk                         ((uint32_t)0x04000000)
#define RTC_IRQ_SETUP_0_YEAR_ENA                             RTC_IRQ_SETUP_0_YEAR_ENA_Msk
#define RTC_IRQ_SETUP_0_MONTH_ENA_Pos                        25
#define RTC_IRQ_SETUP_0_MONTH_ENA_Msk                        ((uint32_t)0x02000000)
#define RTC_IRQ_SETUP_0_MONTH_ENA                            RTC_IRQ_SETUP_0_MONTH_ENA_Msk
#define RTC_IRQ_SETUP_0_DAY_ENA_Pos                          24
#define RTC_IRQ_SETUP_0_DAY_ENA_Msk                          ((uint32_t)0x01000000)
#define RTC_IRQ_SETUP_0_DAY_ENA                              RTC_IRQ_SETUP_0_DAY_ENA_Msk
#define RTC_IRQ_SETUP_0_YEAR_Pos                             12
#define RTC_IRQ_SETUP_0_YEAR_Msk                             ((uint32_t)0x00FFF000)
#define RTC_IRQ_SETUP_0_YEAR                                 RTC_IRQ_SETUP_0_YEAR_Msk
#define RTC_IRQ_SETUP_0_MONTH_Pos                            8
#define RTC_IRQ_SETUP_0_MONTH_Msk                            ((uint32_t)0x00000F00)
#define RTC_IRQ_SETUP_0_MONTH                                RTC_IRQ_SETUP_0_MONTH_Msk
#define RTC_IRQ_SETUP_0_DAY_Pos                              0
#define RTC_IRQ_SETUP_0_DAY_Msk                              ((uint32_t)0x0000001F)
#define RTC_IRQ_SETUP_0_DAY                                  RTC_IRQ_SETUP_0_DAY_Msk
// RTC->IRQ_SETUP_1
#define RTC_IRQ_SETUP_1_DOTW_ENA_Pos                         31
#define RTC_IRQ_SETUP_1_DOTW_ENA_Msk                         ((uint32_t)0x80000000)
#define RTC_IRQ_SETUP_1_DOTW_ENA                             RTC_IRQ_SETUP_1_DOTW_ENA_Msk
#define RTC_IRQ_SETUP_1_HOUR_ENA_Pos                         30
#define RTC_IRQ_SETUP_1_HOUR_ENA_Msk                         ((uint32_t)0x40000000)
#define RTC_IRQ_SETUP_1_HOUR_ENA                             RTC_IRQ_SETUP_1_HOUR_ENA_Msk
#define RTC_IRQ_SETUP_1_MIN_ENA_Pos                          29
#define RTC_IRQ_SETUP_1_MIN_ENA_Msk                          ((uint32_t)0x20000000)
#define RTC_IRQ_SETUP_1_MIN_ENA                              RTC_IRQ_SETUP_1_MIN_ENA_Msk
#define RTC_IRQ_SETUP_1_SEC_ENA_Pos                          28
#define RTC_IRQ_SETUP_1_SEC_ENA_Msk                          ((uint32_t)0x10000000)
#define RTC_IRQ_SETUP_1_SEC_ENA                              RTC_IRQ_SETUP_1_SEC_ENA_Msk
#define RTC_IRQ_SETUP_1_DOTW_Pos                             24
#define RTC_IRQ_SETUP_1_DOTW_Msk                             ((uint32_t)0x07000000)
#define RTC_IRQ_SETUP_1_DOTW                                 RTC_IRQ_SETUP_1_DOTW_Msk
#define RTC_IRQ_SETUP_1_HOUR_Pos                             16
#define RTC_IRQ_SETUP_1_HOUR_Msk                             ((uint32_t)0x001F0000)
#define RTC_IRQ_SETUP_1_HOUR                                 RTC_IRQ_SETUP_1_HOUR_Msk
#define RTC_IRQ_SETUP_1_MIN_Pos                              8
#define RTC_IRQ_SETUP_1_MIN_Msk                              ((uint32_t)0x00003F00)
#define RTC_IRQ_SETUP_1_MIN                                  RTC_IRQ_SETUP_1_MIN_Msk
#define RTC_IRQ_SETUP_1_SEC_Pos                              0
#define RTC_IRQ_SETUP_1_SEC_Msk                              ((uint32_t)0x0000003F)
#define RTC_IRQ_SETUP_1_SEC                                  RTC_IRQ_SETUP_1_SEC_Msk
// RTC->RTC_1
#define RTC_RTC_1_YEAR_Pos                                   12
#define RTC_RTC_1_YEAR_Msk                                   ((uint32_t)0x00FFF000)
#define RTC_RTC_1_YEAR                                       RTC_RTC_1_YEAR_Msk
#define RTC_RTC_1_MONTH_Pos                                  8
#define RTC_RTC_1_MONTH_Msk                                  ((uint32_t)0x00000F00)
#define RTC_RTC_1_MONTH                                      RTC_RTC_1_MONTH_Msk
#define RTC_RTC_1_DAY_Pos                                    0
#define RTC_RTC_1_DAY_Msk                                    ((uint32_t)0x0000001F)
#define RTC_RTC_1_DAY                                        RTC_RTC_1_DAY_Msk
// RTC->RTC_0
#define RTC_RTC_0_DOTW_Pos                                   24
#define RTC_RTC_0_DOTW_Msk                                   ((uint32_t)0x07000000)
#define RTC_RTC_0_DOTW                                       RTC_RTC_0_DOTW_Msk
#define RTC_RTC_0_HOUR_Pos                                   16
#define RTC_RTC_0_HOUR_Msk                                   ((uint32_t)0x001F0000)
#define RTC_RTC_0_HOUR                                       RTC_RTC_0_HOUR_Msk
#define RTC_RTC_0_MIN_Pos                                    8
#define RTC_RTC_0_MIN_Msk                                    ((uint32_t)0x00003F00)
#define RTC_RTC_0_MIN                                        RTC_RTC_0_MIN_Msk
#define RTC_RTC_0_SEC_Pos                                    0
#define RTC_RTC_0_SEC_Msk                                    ((uint32_t)0x0000003F)
#define RTC_RTC_0_SEC                                        RTC_RTC_0_SEC_Msk
// RTC->INTR
#define RTC_INTR_RTC_Pos                                     0
#define RTC_INTR_RTC_Msk                                     ((uint32_t)0x00000001)
#define RTC_INTR_RTC                                         RTC_INTR_RTC_Msk
// RTC->INTE
#define RTC_INTE_RTC_Pos                                     0
#define RTC_INTE_RTC_Msk                                     ((uint32_t)0x00000001)
#define RTC_INTE_RTC                                         RTC_INTE_RTC_Msk
// RTC->INTF
#define RTC_INTF_RTC_Pos                                     0
#define RTC_INTF_RTC_Msk                                     ((uint32_t)0x00000001)
#define RTC_INTF_RTC                                         RTC_INTF_RTC_Msk
// RTC->INTS
#define RTC_INTS_RTC_Pos                                     0
#define RTC_INTS_RTC_Msk                                     ((uint32_t)0x00000001)
#define RTC_INTS_RTC                                         RTC_INTS_RTC_Msk

// SIO->CPUID
#define SIO_CPUID_CPUID_Pos                                  0
#define SIO_CPUID_CPUID_Msk                                  ((uint32_t)0xFFFFFFFF)
#define SIO_CPUID_CPUID                                      SIO_CPUID_CPUID_Msk
// SIO->GPIO_IN
#define SIO_GPIO_IN_GPIO_IN_Pos                              0
#define SIO_GPIO_IN_GPIO_IN_Msk                              ((uint32_t)0x3FFFFFFF)
#define SIO_GPIO_IN_GPIO_IN                                  SIO_GPIO_IN_GPIO_IN_Msk
// SIO->GPIO_HI_IN
#define SIO_GPIO_HI_IN_GPIO_HI_IN_Pos                        0
#define SIO_GPIO_HI_IN_GPIO_HI_IN_Msk                        ((uint32_t)0x0000003F)
#define SIO_GPIO_HI_IN_GPIO_HI_IN                            SIO_GPIO_HI_IN_GPIO_HI_IN_Msk
// SIO->GPIO_OUT
#define SIO_GPIO_OUT_GPIO_OUT_Pos                            0
#define SIO_GPIO_OUT_GPIO_OUT_Msk                            ((uint32_t)0x3FFFFFFF)
#define SIO_GPIO_OUT_GPIO_OUT                                SIO_GPIO_OUT_GPIO_OUT_Msk
// SIO->GPIO_OUT_SET
#define SIO_GPIO_OUT_SET_GPIO_OUT_SET_Pos                    0
#define SIO_GPIO_OUT_SET_GPIO_OUT_SET_Msk                    ((uint32_t)0x3FFFFFFF)
#define SIO_GPIO_OUT_SET_GPIO_OUT_SET                        SIO_GPIO_OUT_SET_GPIO_OUT_SET_Msk
// SIO->GPIO_OUT_CLR
#define SIO_GPIO_OUT_CLR_GPIO_OUT_CLR_Pos                    0
#define SIO_GPIO_OUT_CLR_GPIO_OUT_CLR_Msk                    ((uint32_t)0x3FFFFFFF)
#define SIO_GPIO_OUT_CLR_GPIO_OUT_CLR                        SIO_GPIO_OUT_CLR_GPIO_OUT_CLR_Msk
// SIO->GPIO_OUT_XOR
#define SIO_GPIO_OUT_XOR_GPIO_OUT_XOR_Pos                    0
#define SIO_GPIO_OUT_XOR_GPIO_OUT_XOR_Msk                    ((uint32_t)0x3FFFFFFF)
#define SIO_GPIO_OUT_XOR_GPIO_OUT_XOR                        SIO_GPIO_OUT_XOR_GPIO_OUT_XOR_Msk
// SIO->GPIO_OE
#define SIO_GPIO_OE_GPIO_OE_Pos                              0
#define SIO_GPIO_OE_GPIO_OE_Msk                              ((uint32_t)0x3FFFFFFF)
#define SIO_GPIO_OE_GPIO_OE                                  SIO_GPIO_OE_GPIO_OE_Msk
// SIO->GPIO_OE_SET
#define SIO_GPIO_OE_SET_GPIO_OE_SET_Pos                      0
#define SIO_GPIO_OE_SET_GPIO_OE_SET_Msk                      ((uint32_t)0x3FFFFFFF)
#define SIO_GPIO_OE_SET_GPIO_OE_SET                          SIO_GPIO_OE_SET_GPIO_OE_SET_Msk
// SIO->GPIO_OE_CLR
#define SIO_GPIO_OE_CLR_GPIO_OE_CLR_Pos                      0
#define SIO_GPIO_OE_CLR_GPIO_OE_CLR_Msk                      ((uint32_t)0x3FFFFFFF)
#define SIO_GPIO_OE_CLR_GPIO_OE_CLR                          SIO_GPIO_OE_CLR_GPIO_OE_CLR_Msk
// SIO->GPIO_OE_XOR
#define SIO_GPIO_OE_XOR_GPIO_OE_XOR_Pos                      0
#define SIO_GPIO_OE_XOR_GPIO_OE_XOR_Msk                      ((uint32_t)0x3FFFFFFF)
#define SIO_GPIO_OE_XOR_GPIO_OE_XOR                          SIO_GPIO_OE_XOR_GPIO_OE_XOR_Msk
// SIO->GPIO_HI_OUT
#define SIO_GPIO_HI_OUT_GPIO_HI_OUT_Pos                      0
#define SIO_GPIO_HI_OUT_GPIO_HI_OUT_Msk                      ((uint32_t)0x0000003F)
#define SIO_GPIO_HI_OUT_GPIO_HI_OUT                          SIO_GPIO_HI_OUT_GPIO_HI_OUT_Msk
// SIO->GPIO_HI_OUT_SET
#define SIO_GPIO_HI_OUT_SET_GPIO_HI_OUT_SET_Pos              0
#define SIO_GPIO_HI_OUT_SET_GPIO_HI_OUT_SET_Msk              ((uint32_t)0x0000003F)
#define SIO_GPIO_HI_OUT_SET_GPIO_HI_OUT_SET                  SIO_GPIO_HI_OUT_SET_GPIO_HI_OUT_SET_Msk
// SIO->GPIO_HI_OUT_CLR
#define SIO_GPIO_HI_OUT_CLR_GPIO_HI_OUT_CLR_Pos              0
#define SIO_GPIO_HI_OUT_CLR_GPIO_HI_OUT_CLR_Msk              ((uint32_t)0x0000003F)
#define SIO_GPIO_HI_OUT_CLR_GPIO_HI_OUT_CLR                  SIO_GPIO_HI_OUT_CLR_GPIO_HI_OUT_CLR_Msk
// SIO->GPIO_HI_OUT_XOR
#define SIO_GPIO_HI_OUT_XOR_GPIO_HI_OUT_XOR_Pos              0
#define SIO_GPIO_HI_OUT_XOR_GPIO_HI_OUT_XOR_Msk              ((uint32_t)0x0000003F)
#define SIO_GPIO_HI_OUT_XOR_GPIO_HI_OUT_XOR                  SIO_GPIO_HI_OUT_XOR_GPIO_HI_OUT_XOR_Msk
// SIO->GPIO_HI_OE
#define SIO_GPIO_HI_OE_GPIO_HI_OE_Pos                        0
#define SIO_GPIO_HI_OE_GPIO_HI_OE_Msk                        ((uint32_t)0x0000003F)
#define SIO_GPIO_HI_OE_GPIO_HI_OE                            SIO_GPIO_HI_OE_GPIO_HI_OE_Msk
// SIO->GPIO_HI_OE_SET
#define SIO_GPIO_HI_OE_SET_GPIO_HI_OE_SET_Pos                0
#define SIO_GPIO_HI_OE_SET_GPIO_HI_OE_SET_Msk                ((uint32_t)0x0000003F)
#define SIO_GPIO_HI_OE_SET_GPIO_HI_OE_SET                    SIO_GPIO_HI_OE_SET_GPIO_HI_OE_SET_Msk
// SIO->GPIO_HI_OE_CLR
#define SIO_GPIO_HI_OE_CLR_GPIO_HI_OE_CLR_Pos                0
#define SIO_GPIO_HI_OE_CLR_GPIO_HI_OE_CLR_Msk                ((uint32_t)0x0000003F)
#define SIO_GPIO_HI_OE_CLR_GPIO_HI_OE_CLR                    SIO_GPIO_HI_OE_CLR_GPIO_HI_OE_CLR_Msk
// SIO->GPIO_HI_OE_XOR
#define SIO_GPIO_HI_OE_XOR_GPIO_HI_OE_XOR_Pos                0
#define SIO_GPIO_HI_OE_XOR_GPIO_HI_OE_XOR_Msk                ((uint32_t)0x0000003F)
#define SIO_GPIO_HI_OE_XOR_GPIO_HI_OE_XOR                    SIO_GPIO_HI_OE_XOR_GPIO_HI_OE_XOR_Msk
// SIO->FIFO_ST
#define SIO_FIFO_ST_ROE_Pos                                  3
#define SIO_FIFO_ST_ROE_Msk                                  ((uint32_t)0x00000008)
#define SIO_FIFO_ST_ROE                                      SIO_FIFO_ST_ROE_Msk
#define SIO_FIFO_ST_WOF_Pos                                  2
#define SIO_FIFO_ST_WOF_Msk                                  ((uint32_t)0x00000004)
#define SIO_FIFO_ST_WOF                                      SIO_FIFO_ST_WOF_Msk
#define SIO_FIFO_ST_RDY_Pos                                  1
#define SIO_FIFO_ST_RDY_Msk                                  ((uint32_t)0x00000002)
#define SIO_FIFO_ST_RDY                                      SIO_FIFO_ST_RDY_Msk
#define SIO_FIFO_ST_VLD_Pos                                  0
#define SIO_FIFO_ST_VLD_Msk                                  ((uint32_t)0x00000001)
#define SIO_FIFO_ST_VLD                                      SIO_FIFO_ST_VLD_Msk
// SIO->FIFO_WR
#define SIO_FIFO_WR_FIFO_WR_Pos                              0
#define SIO_FIFO_WR_FIFO_WR_Msk                              ((uint32_t)0xFFFFFFFF)
#define SIO_FIFO_WR_FIFO_WR                                  SIO_FIFO_WR_FIFO_WR_Msk
// SIO->FIFO_RD
#define SIO_FIFO_RD_FIFO_RD_Pos                              0
#define SIO_FIFO_RD_FIFO_RD_Msk                              ((uint32_t)0xFFFFFFFF)
#define SIO_FIFO_RD_FIFO_RD                                  SIO_FIFO_RD_FIFO_RD_Msk
// SIO->SPINLOCK_ST
#define SIO_SPINLOCK_ST_SPINLOCK_ST_Pos                      0
#define SIO_SPINLOCK_ST_SPINLOCK_ST_Msk                      ((uint32_t)0xFFFFFFFF)
#define SIO_SPINLOCK_ST_SPINLOCK_ST                          SIO_SPINLOCK_ST_SPINLOCK_ST_Msk
// SIO->DIV_UDIVIDEND
#define SIO_DIV_UDIVIDEND_DIV_UDIVIDEND_Pos                  0
#define SIO_DIV_UDIVIDEND_DIV_UDIVIDEND_Msk                  ((uint32_t)0xFFFFFFFF)
#define SIO_DIV_UDIVIDEND_DIV_UDIVIDEND                      SIO_DIV_UDIVIDEND_DIV_UDIVIDEND_Msk
// SIO->DIV_UDIVISOR
#define SIO_DIV_UDIVISOR_DIV_UDIVISOR_Pos                    0
#define SIO_DIV_UDIVISOR_DIV_UDIVISOR_Msk                    ((uint32_t)0xFFFFFFFF)
#define SIO_DIV_UDIVISOR_DIV_UDIVISOR                        SIO_DIV_UDIVISOR_DIV_UDIVISOR_Msk
// SIO->DIV_SDIVIDEND
#define SIO_DIV_SDIVIDEND_DIV_SDIVIDEND_Pos                  0
#define SIO_DIV_SDIVIDEND_DIV_SDIVIDEND_Msk                  ((uint32_t)0xFFFFFFFF)
#define SIO_DIV_SDIVIDEND_DIV_SDIVIDEND                      SIO_DIV_SDIVIDEND_DIV_SDIVIDEND_Msk
// SIO->DIV_SDIVISOR
#define SIO_DIV_SDIVISOR_DIV_SDIVISOR_Pos                    0
#define SIO_DIV_SDIVISOR_DIV_SDIVISOR_Msk                    ((uint32_t)0xFFFFFFFF)
#define SIO_DIV_SDIVISOR_DIV_SDIVISOR                        SIO_DIV_SDIVISOR_DIV_SDIVISOR_Msk
// SIO->DIV_QUOTIENT
#define SIO_DIV_QUOTIENT_DIV_QUOTIENT_Pos                    0
#define SIO_DIV_QUOTIENT_DIV_QUOTIENT_Msk                    ((uint32_t)0xFFFFFFFF)
#define SIO_DIV_QUOTIENT_DIV_QUOTIENT                        SIO_DIV_QUOTIENT_DIV_QUOTIENT_Msk
// SIO->DIV_REMAINDER
#define SIO_DIV_REMAINDER_DIV_REMAINDER_Pos                  0
#define SIO_DIV_REMAINDER_DIV_REMAINDER_Msk                  ((uint32_t)0xFFFFFFFF)
#define SIO_DIV_REMAINDER_DIV_REMAINDER                      SIO_DIV_REMAINDER_DIV_REMAINDER_Msk
// SIO->DIV_CSR
#define SIO_DIV_CSR_DIRTY_Pos                                1
#define SIO_DIV_CSR_DIRTY_Msk                                ((uint32_t)0x00000002)
#define SIO_DIV_CSR_DIRTY                                    SIO_DIV_CSR_DIRTY_Msk
#define SIO_DIV_CSR_READY_Pos                                0
#define SIO_DIV_CSR_READY_Msk                                ((uint32_t)0x00000001)
#define SIO_DIV_CSR_READY                                    SIO_DIV_CSR_READY_Msk
// SIO->INTERP[n].ACCUM
#define SIO_INTERP_ACCUM_INTERP0_ACCUM0_Pos                  0
#define SIO_INTERP_ACCUM_INTERP0_ACCUM0_Msk                  ((uint32_t)0xFFFFFFFF)
#define SIO_INTERP_ACCUM_INTERP0_ACCUM0                      SIO_INTERP_ACCUM_INTERP0_ACCUM0_Msk
// SIO->INTERP[n].BASE
#define SIO_INTERP_BASE_INTERP0_BASE0_Pos                    0
#define SIO_INTERP_BASE_INTERP0_BASE0_Msk                    ((uint32_t)0xFFFFFFFF)
#define SIO_INTERP_BASE_INTERP0_BASE0                        SIO_INTERP_BASE_INTERP0_BASE0_Msk
// SIO->INTERP[n].POP_LANE
#define SIO_INTERP_POP_LANE_INTERP0_POP_LANE0_Pos            0
#define SIO_INTERP_POP_LANE_INTERP0_POP_LANE0_Msk            ((uint32_t)0xFFFFFFFF)
#define SIO_INTERP_POP_LANE_INTERP0_POP_LANE0                SIO_INTERP_POP_LANE_INTERP0_POP_LANE0_Msk
// SIO->INTERP[n].POP_FULL
#define SIO_INTERP_POP_FULL_INTERP0_POP_FULL_Pos             0
#define SIO_INTERP_POP_FULL_INTERP0_POP_FULL_Msk             ((uint32_t)0xFFFFFFFF)
#define SIO_INTERP_POP_FULL_INTERP0_POP_FULL                 SIO_INTERP_POP_FULL_INTERP0_POP_FULL_Msk
// SIO->INTERP[n].PEEK_LANE
#define SIO_INTERP_PEEK_LANE_INTERP0_PEEK_LANE0_Pos          0
#define SIO_INTERP_PEEK_LANE_INTERP0_PEEK_LANE0_Msk          ((uint32_t)0xFFFFFFFF)
#define SIO_INTERP_PEEK_LANE_INTERP0_PEEK_LANE0              SIO_INTERP_PEEK_LANE_INTERP0_PEEK_LANE0_Msk
// SIO->INTERP[n].PEEK_FULL
#define SIO_INTERP_PEEK_FULL_INTERP0_PEEK_FULL_Pos           0
#define SIO_INTERP_PEEK_FULL_INTERP0_PEEK_FULL_Msk           ((uint32_t)0xFFFFFFFF)
#define SIO_INTERP_PEEK_FULL_INTERP0_PEEK_FULL               SIO_INTERP_PEEK_FULL_INTERP0_PEEK_FULL_Msk
// SIO->INTERP[n].CTRL_LANE
#define SIO_INTERP_CTRL_LANE_OVERF_Pos                       25
#define SIO_INTERP_CTRL_LANE_OVERF_Msk                       ((uint32_t)0x02000000)
#define SIO_INTERP_CTRL_LANE_OVERF                           SIO_INTERP_CTRL_LANE_OVERF_Msk
#define SIO_INTERP_CTRL_LANE_OVERF1_Pos                      24
#define SIO_INTERP_CTRL_LANE_OVERF1_Msk                      ((uint32_t)0x01000000)
#define SIO_INTERP_CTRL_LANE_OVERF1                          SIO_INTERP_CTRL_LANE_OVERF1_Msk
#define SIO_INTERP_CTRL_LANE_OVERF0_Pos                      23
#define SIO_INTERP_CTRL_LANE_OVERF0_Msk                      ((uint32_t)0x00800000)
#define SIO_INTERP_CTRL_LANE_OVERF0                          SIO_INTERP_CTRL_LANE_OVERF0_Msk
#define SIO_INTERP_CTRL_LANE_BLEND_Pos                       21
#define SIO_INTERP_CTRL_LANE_BLEND_Msk                       ((uint32_t)0x00200000)
#define SIO_INTERP_CTRL_LANE_BLEND                           SIO_INTERP_CTRL_LANE_BLEND_Msk
#define SIO_INTERP_CTRL_LANE_FORCE_MSB_Pos                   19
#define SIO_INTERP_CTRL_LANE_FORCE_MSB_Msk                   ((uint32_t)0x00180000)
#define SIO_INTERP_CTRL_LANE_FORCE_MSB                       SIO_INTERP_CTRL_LANE_FORCE_MSB_Msk
#define SIO_INTERP_CTRL_LANE_ADD_RAW_Pos                     18
#define SIO_INTERP_CTRL_LANE_ADD_RAW_Msk                     ((uint32_t)0x00040000)
#define SIO_INTERP_CTRL_LANE_ADD_RAW                         SIO_INTERP_CTRL_LANE_ADD_RAW_Msk
#define SIO_INTERP_CTRL_LANE_CROSS_RESULT_Pos                17
#define SIO_INTERP_CTRL_LANE_CROSS_RESULT_Msk                ((uint32_t)0x00020000)
#define SIO_INTERP_CTRL_LANE_CROSS_RESULT                    SIO_INTERP_CTRL_LANE_CROSS_RESULT_Msk
#define SIO_INTERP_CTRL_LANE_CROSS_INPUT_Pos                 16
#define SIO_INTERP_CTRL_LANE_CROSS_INPUT_Msk                 ((uint32_t)0x00010000)
#define SIO_INTERP_CTRL_LANE_CROSS_INPUT                     SIO_INTERP_CTRL_LANE_CROSS_INPUT_Msk
#define SIO_INTERP_CTRL_LANE_SIGNED_Pos                      15
#define SIO_INTERP_CTRL_LANE_SIGNED_Msk                      ((uint32_t)0x00008000)
#define SIO_INTERP_CTRL_LANE_SIGNED                          SIO_INTERP_CTRL_LANE_SIGNED_Msk
#define SIO_INTERP_CTRL_LANE_MASK_MSB_Pos                    10
#define SIO_INTERP_CTRL_LANE_MASK_MSB_Msk                    ((uint32_t)0x00007C00)
#define SIO_INTERP_CTRL_LANE_MASK_MSB                        SIO_INTERP_CTRL_LANE_MASK_MSB_Msk
#define SIO_INTERP_CTRL_LANE_MASK_LSB_Pos                    5
#define SIO_INTERP_CTRL_LANE_MASK_LSB_Msk                    ((uint32_t)0x000003E0)
#define SIO_INTERP_CTRL_LANE_MASK_LSB                        SIO_INTERP_CTRL_LANE_MASK_LSB_Msk
#define SIO_INTERP_CTRL_LANE_SHIFT_Pos                       0
#define SIO_INTERP_CTRL_LANE_SHIFT_Msk                       ((uint32_t)0x0000001F)
#define SIO_INTERP_CTRL_LANE_SHIFT                           SIO_INTERP_CTRL_LANE_SHIFT_Msk
// SIO->INTERP[n].ACCUM0_ADD
#define SIO_INTERP_ACCUM0_ADD_INTERP0_ACCUM0_ADD_Pos         0
#define SIO_INTERP_ACCUM0_ADD_INTERP0_ACCUM0_ADD_Msk         ((uint32_t)0x00FFFFFF)
#define SIO_INTERP_ACCUM0_ADD_INTERP0_ACCUM0_ADD             SIO_INTERP_ACCUM0_ADD_INTERP0_ACCUM0_ADD_Msk
// SIO->INTERP[n].ACCUM1_ADD
#define SIO_INTERP_ACCUM1_ADD_INTERP0_ACCUM1_ADD_Pos         0
#define SIO_INTERP_ACCUM1_ADD_INTERP0_ACCUM1_ADD_Msk         ((uint32_t)0x00FFFFFF)
#define SIO_INTERP_ACCUM1_ADD_INTERP0_ACCUM1_ADD             SIO_INTERP_ACCUM1_ADD_INTERP0_ACCUM1_ADD_Msk
// SIO->INTERP[n].BASE_1AND0
#define SIO_INTERP_BASE_1AND0_INTERP0_BASE_1AND0_Pos         0
#define SIO_INTERP_BASE_1AND0_INTERP0_BASE_1AND0_Msk         ((uint32_t)0xFFFFFFFF)
#define SIO_INTERP_BASE_1AND0_INTERP0_BASE_1AND0             SIO_INTERP_BASE_1AND0_INTERP0_BASE_1AND0_Msk
// SIO->SPINLOCK
#define SIO_SPINLOCK_SPINLOCK0_Pos                           0
#define SIO_SPINLOCK_SPINLOCK0_Msk                           ((uint32_t)0xFFFFFFFF)
#define SIO_SPINLOCK_SPINLOCK0                               SIO_SPINLOCK_SPINLOCK0_Msk

// SPI->SSPCR0
#define SPI_SSPCR0_SCR_Pos                                   8
#define SPI_SSPCR0_SCR_Msk                                   ((uint32_t)0x0000FF00)
#define SPI_SSPCR0_SCR                                       SPI_SSPCR0_SCR_Msk
#define SPI_SSPCR0_SPH_Pos                                   7
#define SPI_SSPCR0_SPH_Msk                                   ((uint32_t)0x00000080)
#define SPI_SSPCR0_SPH                                       SPI_SSPCR0_SPH_Msk
#define SPI_SSPCR0_SPO_Pos                                   6
#define SPI_SSPCR0_SPO_Msk                                   ((uint32_t)0x00000040)
#define SPI_SSPCR0_SPO                                       SPI_SSPCR0_SPO_Msk
#define SPI_SSPCR0_FRF_Pos                                   4
#define SPI_SSPCR0_FRF_Msk                                   ((uint32_t)0x00000030)
#define SPI_SSPCR0_FRF                                       SPI_SSPCR0_FRF_Msk
#define SPI_SSPCR0_DSS_Pos                                   0
#define SPI_SSPCR0_DSS_Msk                                   ((uint32_t)0x0000000F)
#define SPI_SSPCR0_DSS                                       SPI_SSPCR0_DSS_Msk
// SPI->SSPCR1
#define SPI_SSPCR1_SOD_Pos                                   3
#define SPI_SSPCR1_SOD_Msk                                   ((uint32_t)0x00000008)
#define SPI_SSPCR1_SOD                                       SPI_SSPCR1_SOD_Msk
#define SPI_SSPCR1_MS_Pos                                    2
#define SPI_SSPCR1_MS_Msk                                    ((uint32_t)0x00000004)
#define SPI_SSPCR1_MS                                        SPI_SSPCR1_MS_Msk
#define SPI_SSPCR1_SSE_Pos                                   1
#define SPI_SSPCR1_SSE_Msk                                   ((uint32_t)0x00000002)
#define SPI_SSPCR1_SSE                                       SPI_SSPCR1_SSE_Msk
#define SPI_SSPCR1_LBM_Pos                                   0
#define SPI_SSPCR1_LBM_Msk                                   ((uint32_t)0x00000001)
#define SPI_SSPCR1_LBM                                       SPI_SSPCR1_LBM_Msk
// SPI->SSPDR
#define SPI_SSPDR_DATA_Pos                                   0
#define SPI_SSPDR_DATA_Msk                                   ((uint32_t)0x0000FFFF)
#define SPI_SSPDR_DATA                                       SPI_SSPDR_DATA_Msk
// SPI->SSPSR
#define SPI_SSPSR_BSY_Pos                                    4
#define SPI_SSPSR_BSY_Msk                                    ((uint32_t)0x00000010)
#define SPI_SSPSR_BSY                                        SPI_SSPSR_BSY_Msk
#define SPI_SSPSR_RFF_Pos                                    3
#define SPI_SSPSR_RFF_Msk                                    ((uint32_t)0x00000008)
#define SPI_SSPSR_RFF                                        SPI_SSPSR_RFF_Msk
#define SPI_SSPSR_RNE_Pos                                    2
#define SPI_SSPSR_RNE_Msk                                    ((uint32_t)0x00000004)
#define SPI_SSPSR_RNE                                        SPI_SSPSR_RNE_Msk
#define SPI_SSPSR_TNF_Pos                                    1
#define SPI_SSPSR_TNF_Msk                                    ((uint32_t)0x00000002)
#define SPI_SSPSR_TNF                                        SPI_SSPSR_TNF_Msk
#define SPI_SSPSR_TFE_Pos                                    0
#define SPI_SSPSR_TFE_Msk                                    ((uint32_t)0x00000001)
#define SPI_SSPSR_TFE                                        SPI_SSPSR_TFE_Msk
// SPI->SSPCPSR
#define SPI_SSPCPSR_CPSDVSR_Pos                              0
#define SPI_SSPCPSR_CPSDVSR_Msk                              ((uint32_t)0x000000FF)
#define SPI_SSPCPSR_CPSDVSR                                  SPI_SSPCPSR_CPSDVSR_Msk
// SPI->SSPIMSC
#define SPI_SSPIMSC_TXIM_Pos                                 3
#define SPI_SSPIMSC_TXIM_Msk                                 ((uint32_t)0x00000008)
#define SPI_SSPIMSC_TXIM                                     SPI_SSPIMSC_TXIM_Msk
#define SPI_SSPIMSC_RXIM_Pos                                 2
#define SPI_SSPIMSC_RXIM_Msk                                 ((uint32_t)0x00000004)
#define SPI_SSPIMSC_RXIM                                     SPI_SSPIMSC_RXIM_Msk
#define SPI_SSPIMSC_RTIM_Pos                                 1
#define SPI_SSPIMSC_RTIM_Msk                                 ((uint32_t)0x00000002)
#define SPI_SSPIMSC_RTIM                                     SPI_SSPIMSC_RTIM_Msk
#define SPI_SSPIMSC_RORIM_Pos                                0
#define SPI_SSPIMSC_RORIM_Msk                                ((uint32_t)0x00000001)
#define SPI_SSPIMSC_RORIM                                    SPI_SSPIMSC_RORIM_Msk
// SPI->SSPRIS
#define SPI_SSPRIS_TXRIS_Pos                                 3
#define SPI_SSPRIS_TXRIS_Msk                                 ((uint32_t)0x00000008)
#define SPI_SSPRIS_TXRIS                                     SPI_SSPRIS_TXRIS_Msk
#define SPI_SSPRIS_RXRIS_Pos                                 2
#define SPI_SSPRIS_RXRIS_Msk                                 ((uint32_t)0x00000004)
#define SPI_SSPRIS_RXRIS                                     SPI_SSPRIS_RXRIS_Msk
#define SPI_SSPRIS_RTRIS_Pos                                 1
#define SPI_SSPRIS_RTRIS_Msk                                 ((uint32_t)0x00000002)
#define SPI_SSPRIS_RTRIS                                     SPI_SSPRIS_RTRIS_Msk
#define SPI_SSPRIS_RORRIS_Pos                                0
#define SPI_SSPRIS_RORRIS_Msk                                ((uint32_t)0x00000001)
#define SPI_SSPRIS_RORRIS                                    SPI_SSPRIS_RORRIS_Msk
// SPI->SSPMIS
#define SPI_SSPMIS_TXMIS_Pos                                 3
#define SPI_SSPMIS_TXMIS_Msk                                 ((uint32_t)0x00000008)
#define SPI_SSPMIS_TXMIS                                     SPI_SSPMIS_TXMIS_Msk
#define SPI_SSPMIS_RXMIS_Pos                                 2
#define SPI_SSPMIS_RXMIS_Msk                                 ((uint32_t)0x00000004)
#define SPI_SSPMIS_RXMIS                                     SPI_SSPMIS_RXMIS_Msk
#define SPI_SSPMIS_RTMIS_Pos                                 1
#define SPI_SSPMIS_RTMIS_Msk                                 ((uint32_t)0x00000002)
#define SPI_SSPMIS_RTMIS                                     SPI_SSPMIS_RTMIS_Msk
#define SPI_SSPMIS_RORMIS_Pos                                0
#define SPI_SSPMIS_RORMIS_Msk                                ((uint32_t)0x00000001)
#define SPI_SSPMIS_RORMIS                                    SPI_SSPMIS_RORMIS_Msk
// SPI->SSPICR
#define SPI_SSPICR_RTIC_Pos                                  1
#define SPI_SSPICR_RTIC_Msk                                  ((uint32_t)0x00000002)
#define SPI_SSPICR_RTIC                                      SPI_SSPICR_RTIC_Msk
#define SPI_SSPICR_RORIC_Pos                                 0
#define SPI_SSPICR_RORIC_Msk                                 ((uint32_t)0x00000001)
#define SPI_SSPICR_RORIC                                     SPI_SSPICR_RORIC_Msk
// SPI->SSPDMACR
#define SPI_SSPDMACR_TXDMAE_Pos                              1
#define SPI_SSPDMACR_TXDMAE_Msk                              ((uint32_t)0x00000002)
#define SPI_SSPDMACR_TXDMAE                                  SPI_SSPDMACR_TXDMAE_Msk
#define SPI_SSPDMACR_RXDMAE_Pos                              0
#define SPI_SSPDMACR_RXDMAE_Msk                              ((uint32_t)0x00000001)
#define SPI_SSPDMACR_RXDMAE                                  SPI_SSPDMACR_RXDMAE_Msk
// SPI->SSPPERIPHID0
#define SPI_SSPPERIPHID0_PARTNUMBER0_Pos                     0
#define SPI_SSPPERIPHID0_PARTNUMBER0_Msk                     ((uint32_t)0x000000FF)
#define SPI_SSPPERIPHID0_PARTNUMBER0                         SPI_SSPPERIPHID0_PARTNUMBER0_Msk
// SPI->SSPPERIPHID1
#define SPI_SSPPERIPHID1_DESIGNER0_Pos                       4
#define SPI_SSPPERIPHID1_DESIGNER0_Msk                       ((uint32_t)0x000000F0)
#define SPI_SSPPERIPHID1_DESIGNER0                           SPI_SSPPERIPHID1_DESIGNER0_Msk
#define SPI_SSPPERIPHID1_PARTNUMBER1_Pos                     0
#define SPI_SSPPERIPHID1_PARTNUMBER1_Msk                     ((uint32_t)0x0000000F)
#define SPI_SSPPERIPHID1_PARTNUMBER1                         SPI_SSPPERIPHID1_PARTNUMBER1_Msk
// SPI->SSPPERIPHID2
#define SPI_SSPPERIPHID2_REVISION_Pos                        4
#define SPI_SSPPERIPHID2_REVISION_Msk                        ((uint32_t)0x000000F0)
#define SPI_SSPPERIPHID2_REVISION                            SPI_SSPPERIPHID2_REVISION_Msk
#define SPI_SSPPERIPHID2_DESIGNER1_Pos                       0
#define SPI_SSPPERIPHID2_DESIGNER1_Msk                       ((uint32_t)0x0000000F)
#define SPI_SSPPERIPHID2_DESIGNER1                           SPI_SSPPERIPHID2_DESIGNER1_Msk
// SPI->SSPPERIPHID3
#define SPI_SSPPERIPHID3_CONFIGURATION_Pos                   0
#define SPI_SSPPERIPHID3_CONFIGURATION_Msk                   ((uint32_t)0x000000FF)
#define SPI_SSPPERIPHID3_CONFIGURATION                       SPI_SSPPERIPHID3_CONFIGURATION_Msk
// SPI->SSPPCELLID
#define SPI_SSPPCELLID_SSPPCELLID0_Pos                       0
#define SPI_SSPPCELLID_SSPPCELLID0_Msk                       ((uint32_t)0x000000FF)
#define SPI_SSPPCELLID_SSPPCELLID0                           SPI_SSPPCELLID_SSPPCELLID0_Msk

// SSI->CTRLR0
#define SSI_CTRLR0_SSTE_Pos                                  24
#define SSI_CTRLR0_SSTE_Msk                                  ((uint32_t)0x01000000)
#define SSI_CTRLR0_SSTE                                      SSI_CTRLR0_SSTE_Msk
#define SSI_CTRLR0_SPI_FRF_Pos                               21
#define SSI_CTRLR0_SPI_FRF_Msk                               ((uint32_t)0x00600000)
#define SSI_CTRLR0_SPI_FRF                                   SSI_CTRLR0_SPI_FRF_Msk
#define SSI_CTRLR0_SPI_FRF_STD                               ((uint32_t)0x00000000)
#define SSI_CTRLR0_SPI_FRF_DUAL                              ((uint32_t)0x00200000)
#define SSI_CTRLR0_SPI_FRF_QUAD                              ((uint32_t)0x00400000)
#define SSI_CTRLR0_DFS_32_Pos                                16
#define SSI_CTRLR0_DFS_32_Msk                                ((uint32_t)0x001F0000)
#define SSI_CTRLR0_DFS_32                                    SSI_CTRLR0_DFS_32_Msk
#define SSI_CTRLR0_CFS_Pos                                   12
#define SSI_CTRLR0_CFS_Msk                                   ((uint32_t)0x0000F000)
#define SSI_CTRLR0_CFS                                       SSI_CTRLR0_CFS_Msk
#define SSI_CTRLR0_SRL_Pos                                   11
#define SSI_CTRLR0_SRL_Msk                                   ((uint32_t)0x00000800)
#define SSI_CTRLR0_SRL                                       SSI_CTRLR0_SRL_Msk
#define SSI_CTRLR0_SLV_OE_Pos                                10
#define SSI_CTRLR0_SLV_OE_Msk                                ((uint32_t)0x00000400)
#define SSI_CTRLR0_SLV_OE                                    SSI_CTRLR0_SLV_OE_Msk
#define SSI_CTRLR0_TMOD_Pos                                  8
#define SSI_CTRLR0_TMOD_Msk                                  ((uint32_t)0x00000300)
#define SSI_CTRLR0_TMOD                                      SSI_CTRLR0_TMOD_Msk
#define SSI_CTRLR0_TMOD_TX_AND_RX                            ((uint32_t)0x00000000)
#define SSI_CTRLR0_TMOD_TX_ONLY                              ((uint32_t)0x00000100)
#define SSI_CTRLR0_TMOD_RX_ONLY                              ((uint32_t)0x00000200)
#define SSI_CTRLR0_TMOD_EEPROM_READ                          ((uint32_t)0x00000300)
#define SSI_CTRLR0_SCPOL_Pos                                 7
#define SSI_CTRLR0_SCPOL_Msk                                 ((uint32_t)0x00000080)
#define SSI_CTRLR0_SCPOL                                     SSI_CTRLR0_SCPOL_Msk
#define SSI_CTRLR0_SCPH_Pos                                  6
#define SSI_CTRLR0_SCPH_Msk                                  ((uint32_t)0x00000040)
#define SSI_CTRLR0_SCPH                                      SSI_CTRLR0_SCPH_Msk
#define SSI_CTRLR0_FRF_Pos                                   4
#define SSI_CTRLR0_FRF_Msk                                   ((uint32_t)0x00000030)
#define SSI_CTRLR0_FRF                                       SSI_CTRLR0_FRF_Msk
#define SSI_CTRLR0_DFS_Pos                                   0
#define SSI_CTRLR0_DFS_Msk                                   ((uint32_t)0x0000000F)
#define SSI_CTRLR0_DFS                                       SSI_CTRLR0_DFS_Msk
// SSI->CTRLR1
#define SSI_CTRLR1_NDF_Pos                                   0
#define SSI_CTRLR1_NDF_Msk                                   ((uint32_t)0x0000FFFF)
#define SSI_CTRLR1_NDF                                       SSI_CTRLR1_NDF_Msk
// SSI->SSIENR
#define SSI_SSIENR_SSI_EN_Pos                                0
#define SSI_SSIENR_SSI_EN_Msk                                ((uint32_t)0x00000001)
#define SSI_SSIENR_SSI_EN                                    SSI_SSIENR_SSI_EN_Msk
// SSI->MWCR
#define SSI_MWCR_MHS_Pos                                     2
#define SSI_MWCR_MHS_Msk                                     ((uint32_t)0x00000004)
#define SSI_MWCR_MHS                                         SSI_MWCR_MHS_Msk
#define SSI_MWCR_MDD_Pos                                     1
#define SSI_MWCR_MDD_Msk                                     ((uint32_t)0x00000002)
#define SSI_MWCR_MDD                                         SSI_MWCR_MDD_Msk
#define SSI_MWCR_MWMOD_Pos                                   0
#define SSI_MWCR_MWMOD_Msk                                   ((uint32_t)0x00000001)
#define SSI_MWCR_MWMOD                                       SSI_MWCR_MWMOD_Msk
// SSI->SER
#define SSI_SER_SER_Pos                                      0
#define SSI_SER_SER_Msk                                      ((uint32_t)0x00000001)
#define SSI_SER_SER                                          SSI_SER_SER_Msk
// SSI->BAUDR
#define SSI_BAUDR_SCKDV_Pos                                  0
#define SSI_BAUDR_SCKDV_Msk                                  ((uint32_t)0x0000FFFF)
#define SSI_BAUDR_SCKDV                                      SSI_BAUDR_SCKDV_Msk
// SSI->TXFTLR
#define SSI_TXFTLR_TFT_Pos                                   0
#define SSI_TXFTLR_TFT_Msk                                   ((uint32_t)0x000000FF)
#define SSI_TXFTLR_TFT                                       SSI_TXFTLR_TFT_Msk
// SSI->RXFTLR
#define SSI_RXFTLR_RFT_Pos                                   0
#define SSI_RXFTLR_RFT_Msk                                   ((uint32_t)0x000000FF)
#define SSI_RXFTLR_RFT                                       SSI_RXFTLR_RFT_Msk
// SSI->TXFLR
#define SSI_TXFLR_TFTFL_Pos                                  0
#define SSI_TXFLR_TFTFL_Msk                                  ((uint32_t)0x000000FF)
#define SSI_TXFLR_TFTFL                                      SSI_TXFLR_TFTFL_Msk
// SSI->RXFLR
#define SSI_RXFLR_RXTFL_Pos                                  0
#define SSI_RXFLR_RXTFL_Msk                                  ((uint32_t)0x000000FF)
#define SSI_RXFLR_RXTFL                                      SSI_RXFLR_RXTFL_Msk
// SSI->SR
#define SSI_SR_DCOL_Pos                                      6
#define SSI_SR_DCOL_Msk                                      ((uint32_t)0x00000040)
#define SSI_SR_DCOL                                          SSI_SR_DCOL_Msk
#define SSI_SR_TXE_Pos                                       5
#define SSI_SR_TXE_Msk                                       ((uint32_t)0x00000020)
#define SSI_SR_TXE                                           SSI_SR_TXE_Msk
#define SSI_SR_RFF_Pos                                       4
#define SSI_SR_RFF_Msk                                       ((uint32_t)0x00000010)
#define SSI_SR_RFF                                           SSI_SR_RFF_Msk
#define SSI_SR_RFNE_Pos                                      3
#define SSI_SR_RFNE_Msk                                      ((uint32_t)0x00000008)
#define SSI_SR_RFNE                                          SSI_SR_RFNE_Msk
#define SSI_SR_TFE_Pos                                       2
#define SSI_SR_TFE_Msk                                       ((uint32_t)0x00000004)
#define SSI_SR_TFE                                           SSI_SR_TFE_Msk
#define SSI_SR_TFNF_Pos                                      1
#define SSI_SR_TFNF_Msk                                      ((uint32_t)0x00000002)
#define SSI_SR_TFNF                                          SSI_SR_TFNF_Msk
#define SSI_SR_BUSY_Pos                                      0
#define SSI_SR_BUSY_Msk                                      ((uint32_t)0x00000001)
#define SSI_SR_BUSY                                          SSI_SR_BUSY_Msk
// SSI->IMR
#define SSI_IMR_MSTIM_Pos                                    5
#define SSI_IMR_MSTIM_Msk                                    ((uint32_t)0x00000020)
#define SSI_IMR_MSTIM                                        SSI_IMR_MSTIM_Msk
#define SSI_IMR_RXFIM_Pos                                    4
#define SSI_IMR_RXFIM_Msk                                    ((uint32_t)0x00000010)
#define SSI_IMR_RXFIM                                        SSI_IMR_RXFIM_Msk
#define SSI_IMR_RXOIM_Pos                                    3
#define SSI_IMR_RXOIM_Msk                                    ((uint32_t)0x00000008)
#define SSI_IMR_RXOIM                                        SSI_IMR_RXOIM_Msk
#define SSI_IMR_RXUIM_Pos                                    2
#define SSI_IMR_RXUIM_Msk                                    ((uint32_t)0x00000004)
#define SSI_IMR_RXUIM                                        SSI_IMR_RXUIM_Msk
#define SSI_IMR_TXOIM_Pos                                    1
#define SSI_IMR_TXOIM_Msk                                    ((uint32_t)0x00000002)
#define SSI_IMR_TXOIM                                        SSI_IMR_TXOIM_Msk
#define SSI_IMR_TXEIM_Pos                                    0
#define SSI_IMR_TXEIM_Msk                                    ((uint32_t)0x00000001)
#define SSI_IMR_TXEIM                                        SSI_IMR_TXEIM_Msk
// SSI->ISR
#define SSI_ISR_MSTIS_Pos                                    5
#define SSI_ISR_MSTIS_Msk                                    ((uint32_t)0x00000020)
#define SSI_ISR_MSTIS                                        SSI_ISR_MSTIS_Msk
#define SSI_ISR_RXFIS_Pos                                    4
#define SSI_ISR_RXFIS_Msk                                    ((uint32_t)0x00000010)
#define SSI_ISR_RXFIS                                        SSI_ISR_RXFIS_Msk
#define SSI_ISR_RXOIS_Pos                                    3
#define SSI_ISR_RXOIS_Msk                                    ((uint32_t)0x00000008)
#define SSI_ISR_RXOIS                                        SSI_ISR_RXOIS_Msk
#define SSI_ISR_RXUIS_Pos                                    2
#define SSI_ISR_RXUIS_Msk                                    ((uint32_t)0x00000004)
#define SSI_ISR_RXUIS                                        SSI_ISR_RXUIS_Msk
#define SSI_ISR_TXOIS_Pos                                    1
#define SSI_ISR_TXOIS_Msk                                    ((uint32_t)0x00000002)
#define SSI_ISR_TXOIS                                        SSI_ISR_TXOIS_Msk
#define SSI_ISR_TXEIS_Pos                                    0
#define SSI_ISR_TXEIS_Msk                                    ((uint32_t)0x00000001)
#define SSI_ISR_TXEIS                                        SSI_ISR_TXEIS_Msk
// SSI->RISR
#define SSI_RISR_MSTIR_Pos                                   5
#define SSI_RISR_MSTIR_Msk                                   ((uint32_t)0x00000020)
#define SSI_RISR_MSTIR                                       SSI_RISR_MSTIR_Msk
#define SSI_RISR_RXFIR_Pos                                   4
#define SSI_RISR_RXFIR_Msk                                   ((uint32_t)0x00000010)
#define SSI_RISR_RXFIR                                       SSI_RISR_RXFIR_Msk
#define SSI_RISR_RXOIR_Pos                                   3
#define SSI_RISR_RXOIR_Msk                                   ((uint32_t)0x00000008)
#define SSI_RISR_RXOIR                                       SSI_RISR_RXOIR_Msk
#define SSI_RISR_RXUIR_Pos                                   2
#define SSI_RISR_RXUIR_Msk                                   ((uint32_t)0x00000004)
#define SSI_RISR_RXUIR                                       SSI_RISR_RXUIR_Msk
#define SSI_RISR_TXOIR_Pos                                   1
#define SSI_RISR_TXOIR_Msk                                   ((uint32_t)0x00000002)
#define SSI_RISR_TXOIR                                       SSI_RISR_TXOIR_Msk
#define SSI_RISR_TXEIR_Pos                                   0
#define SSI_RISR_TXEIR_Msk                                   ((uint32_t)0x00000001)
#define SSI_RISR_TXEIR                                       SSI_RISR_TXEIR_Msk
// SSI->TXOICR
#define SSI_TXOICR_TXOICR_Pos                                0
#define SSI_TXOICR_TXOICR_Msk                                ((uint32_t)0x00000001)
#define SSI_TXOICR_TXOICR                                    SSI_TXOICR_TXOICR_Msk
// SSI->RXOICR
#define SSI_RXOICR_RXOICR_Pos                                0
#define SSI_RXOICR_RXOICR_Msk                                ((uint32_t)0x00000001)
#define SSI_RXOICR_RXOICR                                    SSI_RXOICR_RXOICR_Msk
// SSI->RXUICR
#define SSI_RXUICR_RXUICR_Pos                                0
#define SSI_RXUICR_RXUICR_Msk                                ((uint32_t)0x00000001)
#define SSI_RXUICR_RXUICR                                    SSI_RXUICR_RXUICR_Msk
// SSI->MSTICR
#define SSI_MSTICR_MSTICR_Pos                                0
#define SSI_MSTICR_MSTICR_Msk                                ((uint32_t)0x00000001)
#define SSI_MSTICR_MSTICR                                    SSI_MSTICR_MSTICR_Msk
// SSI->ICR
#define SSI_ICR_ICR_Pos                                      0
#define SSI_ICR_ICR_Msk                                      ((uint32_t)0x00000001)
#define SSI_ICR_ICR                                          SSI_ICR_ICR_Msk
// SSI->DMACR
#define SSI_DMACR_TDMAE_Pos                                  1
#define SSI_DMACR_TDMAE_Msk                                  ((uint32_t)0x00000002)
#define SSI_DMACR_TDMAE                                      SSI_DMACR_TDMAE_Msk
#define SSI_DMACR_RDMAE_Pos                                  0
#define SSI_DMACR_RDMAE_Msk                                  ((uint32_t)0x00000001)
#define SSI_DMACR_RDMAE                                      SSI_DMACR_RDMAE_Msk
// SSI->DMATDLR
#define SSI_DMATDLR_DMATDL_Pos                               0
#define SSI_DMATDLR_DMATDL_Msk                               ((uint32_t)0x000000FF)
#define SSI_DMATDLR_DMATDL                                   SSI_DMATDLR_DMATDL_Msk
// SSI->DMARDLR
#define SSI_DMARDLR_DMARDL_Pos                               0
#define SSI_DMARDLR_DMARDL_Msk                               ((uint32_t)0x000000FF)
#define SSI_DMARDLR_DMARDL                                   SSI_DMARDLR_DMARDL_Msk
// SSI->IDR
#define SSI_IDR_IDCODE_Pos                                   0
#define SSI_IDR_IDCODE_Msk                                   ((uint32_t)0xFFFFFFFF)
#define SSI_IDR_IDCODE                                       SSI_IDR_IDCODE_Msk
// SSI->SSI_VERSION_ID
#define SSI_SSI_VERSION_ID_SSI_COMP_VERSION_Pos              0
#define SSI_SSI_VERSION_ID_SSI_COMP_VERSION_Msk              ((uint32_t)0xFFFFFFFF)
#define SSI_SSI_VERSION_ID_SSI_COMP_VERSION                  SSI_SSI_VERSION_ID_SSI_COMP_VERSION_Msk
// SSI->DR0
#define SSI_DR0_DR_Pos                                       0
#define SSI_DR0_DR_Msk                                       ((uint32_t)0xFFFFFFFF)
#define SSI_DR0_DR                                           SSI_DR0_DR_Msk
// SSI->RX_SAMPLE_DLY
#define SSI_RX_SAMPLE_DLY_RSD_Pos                            0
#define SSI_RX_SAMPLE_DLY_RSD_Msk                            ((uint32_t)0x000000FF)
#define SSI_RX_SAMPLE_DLY_RSD                                SSI_RX_SAMPLE_DLY_RSD_Msk
// SSI->SPI_CTRLR0
#define SSI_SPI_CTRLR0_XIP_CMD_Pos                           24
#define SSI_SPI_CTRLR0_XIP_CMD_Msk                           ((uint32_t)0xFF000000)
#define SSI_SPI_CTRLR0_XIP_CMD                               SSI_SPI_CTRLR0_XIP_CMD_Msk
#define SSI_SPI_CTRLR0_SPI_RXDS_EN_Pos                       18
#define SSI_SPI_CTRLR0_SPI_RXDS_EN_Msk                       ((uint32_t)0x00040000)
#define SSI_SPI_CTRLR0_SPI_RXDS_EN                           SSI_SPI_CTRLR0_SPI_RXDS_EN_Msk
#define SSI_SPI_CTRLR0_INST_DDR_EN_Pos                       17
#define SSI_SPI_CTRLR0_INST_DDR_EN_Msk                       ((uint32_t)0x00020000)
#define SSI_SPI_CTRLR0_INST_DDR_EN                           SSI_SPI_CTRLR0_INST_DDR_EN_Msk
#define SSI_SPI_CTRLR0_SPI_DDR_EN_Pos                        16
#define SSI_SPI_CTRLR0_SPI_DDR_EN_Msk                        ((uint32_t)0x00010000)
#define SSI_SPI_CTRLR0_SPI_DDR_EN                            SSI_SPI_CTRLR0_SPI_DDR_EN_Msk
#define SSI_SPI_CTRLR0_WAIT_CYCLES_Pos                       11
#define SSI_SPI_CTRLR0_WAIT_CYCLES_Msk                       ((uint32_t)0x0000F800)
#define SSI_SPI_CTRLR0_WAIT_CYCLES                           SSI_SPI_CTRLR0_WAIT_CYCLES_Msk
#define SSI_SPI_CTRLR0_INST_L_Pos                            8
#define SSI_SPI_CTRLR0_INST_L_Msk                            ((uint32_t)0x00000300)
#define SSI_SPI_CTRLR0_INST_L                                SSI_SPI_CTRLR0_INST_L_Msk
#define SSI_SPI_CTRLR0_INST_L_NONE                           ((uint32_t)0x00000000)
#define SSI_SPI_CTRLR0_INST_L_4B                             ((uint32_t)0x00000100)
#define SSI_SPI_CTRLR0_INST_L_8B                             ((uint32_t)0x00000200)
#define SSI_SPI_CTRLR0_INST_L_16B                            ((uint32_t)0x00000300)
#define SSI_SPI_CTRLR0_ADDR_L_Pos                            2
#define SSI_SPI_CTRLR0_ADDR_L_Msk                            ((uint32_t)0x0000003C)
#define SSI_SPI_CTRLR0_ADDR_L                                SSI_SPI_CTRLR0_ADDR_L_Msk
#define SSI_SPI_CTRLR0_TRANS_TYPE_Pos                        0
#define SSI_SPI_CTRLR0_TRANS_TYPE_Msk                        ((uint32_t)0x00000003)
#define SSI_SPI_CTRLR0_TRANS_TYPE                            SSI_SPI_CTRLR0_TRANS_TYPE_Msk
#define SSI_SPI_CTRLR0_TRANS_TYPE_1C1A                       ((uint32_t)0x00000000)
#define SSI_SPI_CTRLR0_TRANS_TYPE_1C2A                       ((uint32_t)0x00000001)
#define SSI_SPI_CTRLR0_TRANS_TYPE_2C2A                       ((uint32_t)0x00000002)
// SSI->TXD_DRIVE_EDGE
#define SSI_TXD_DRIVE_EDGE_TDE_Pos                           0
#define SSI_TXD_DRIVE_EDGE_TDE_Msk                           ((uint32_t)0x000000FF)
#define SSI_TXD_DRIVE_EDGE_TDE                               SSI_TXD_DRIVE_EDGE_TDE_Msk

// SYSCFG->PROC_NMI_MASK
#define SYSCFG_PROC_NMI_MASK_PROC0_NMI_MASK_Pos              0
#define SYSCFG_PROC_NMI_MASK_PROC0_NMI_MASK_Msk              ((uint32_t)0xFFFFFFFF)
#define SYSCFG_PROC_NMI_MASK_PROC0_NMI_MASK                  SYSCFG_PROC_NMI_MASK_PROC0_NMI_MASK_Msk
// SYSCFG->PROC_CONFIG
#define SYSCFG_PROC_CONFIG_PROC1_DAP_INSTID_Pos              28
#define SYSCFG_PROC_CONFIG_PROC1_DAP_INSTID_Msk              ((uint32_t)0xF0000000)
#define SYSCFG_PROC_CONFIG_PROC1_DAP_INSTID                  SYSCFG_PROC_CONFIG_PROC1_DAP_INSTID_Msk
#define SYSCFG_PROC_CONFIG_PROC0_DAP_INSTID_Pos              24
#define SYSCFG_PROC_CONFIG_PROC0_DAP_INSTID_Msk              ((uint32_t)0x0F000000)
#define SYSCFG_PROC_CONFIG_PROC0_DAP_INSTID                  SYSCFG_PROC_CONFIG_PROC0_DAP_INSTID_Msk
#define SYSCFG_PROC_CONFIG_PROC1_HALTED_Pos                  1
#define SYSCFG_PROC_CONFIG_PROC1_HALTED_Msk                  ((uint32_t)0x00000002)
#define SYSCFG_PROC_CONFIG_PROC1_HALTED                      SYSCFG_PROC_CONFIG_PROC1_HALTED_Msk
#define SYSCFG_PROC_CONFIG_PROC0_HALTED_Pos                  0
#define SYSCFG_PROC_CONFIG_PROC0_HALTED_Msk                  ((uint32_t)0x00000001)
#define SYSCFG_PROC_CONFIG_PROC0_HALTED                      SYSCFG_PROC_CONFIG_PROC0_HALTED_Msk
// SYSCFG->PROC_IN_SYNC_BYPASS
#define SYSCFG_PROC_IN_SYNC_BYPASS_PROC_IN_SYNC_BYPASS_Pos   0
#define SYSCFG_PROC_IN_SYNC_BYPASS_PROC_IN_SYNC_BYPASS_Msk   ((uint32_t)0x3FFFFFFF)
#define SYSCFG_PROC_IN_SYNC_BYPASS_PROC_IN_SYNC_BYPASS       SYSCFG_PROC_IN_SYNC_BYPASS_PROC_IN_SYNC_BYPASS_Msk
// SYSCFG->PROC_IN_SYNC_BYPASS_HI
#define SYSCFG_PROC_IN_SYNC_BYPASS_HI_PROC_IN_SYNC_BYPASS_HI_Pos 0
#define SYSCFG_PROC_IN_SYNC_BYPASS_HI_PROC_IN_SYNC_BYPASS_HI_Msk ((uint32_t)0x0000003F)
#define SYSCFG_PROC_IN_SYNC_BYPASS_HI_PROC_IN_SYNC_BYPASS_HI SYSCFG_PROC_IN_SYNC_BYPASS_HI_PROC_IN_SYNC_BYPASS_HI_Msk
// SYSCFG->DBGFORCE
#define SYSCFG_DBGFORCE_PROC1_ATTACH_Pos                     7
#define SYSCFG_DBGFORCE_PROC1_ATTACH_Msk                     ((uint32_t)0x00000080)
#define SYSCFG_DBGFORCE_PROC1_ATTACH                         SYSCFG_DBGFORCE_PROC1_ATTACH_Msk
#define SYSCFG_DBGFORCE_PROC1_SWCLK_Pos                      6
#define SYSCFG_DBGFORCE_PROC1_SWCLK_Msk                      ((uint32_t)0x00000040)
#define SYSCFG_DBGFORCE_PROC1_SWCLK                          SYSCFG_DBGFORCE_PROC1_SWCLK_Msk
#define SYSCFG_DBGFORCE_PROC1_SWDI_Pos                       5
#define SYSCFG_DBGFORCE_PROC1_SWDI_Msk                       ((uint32_t)0x00000020)
#define SYSCFG_DBGFORCE_PROC1_SWDI                           SYSCFG_DBGFORCE_PROC1_SWDI_Msk
#define SYSCFG_DBGFORCE_PROC1_SWDO_Pos                       4
#define SYSCFG_DBGFORCE_PROC1_SWDO_Msk                       ((uint32_t)0x00000010)
#define SYSCFG_DBGFORCE_PROC1_SWDO                           SYSCFG_DBGFORCE_PROC1_SWDO_Msk
#define SYSCFG_DBGFORCE_PROC0_ATTACH_Pos                     3
#define SYSCFG_DBGFORCE_PROC0_ATTACH_Msk                     ((uint32_t)0x00000008)
#define SYSCFG_DBGFORCE_PROC0_ATTACH                         SYSCFG_DBGFORCE_PROC0_ATTACH_Msk
#define SYSCFG_DBGFORCE_PROC0_SWCLK_Pos                      2
#define SYSCFG_DBGFORCE_PROC0_SWCLK_Msk                      ((uint32_t)0x00000004)
#define SYSCFG_DBGFORCE_PROC0_SWCLK                          SYSCFG_DBGFORCE_PROC0_SWCLK_Msk
#define SYSCFG_DBGFORCE_PROC0_SWDI_Pos                       1
#define SYSCFG_DBGFORCE_PROC0_SWDI_Msk                       ((uint32_t)0x00000002)
#define SYSCFG_DBGFORCE_PROC0_SWDI                           SYSCFG_DBGFORCE_PROC0_SWDI_Msk
#define SYSCFG_DBGFORCE_PROC0_SWDO_Pos                       0
#define SYSCFG_DBGFORCE_PROC0_SWDO_Msk                       ((uint32_t)0x00000001)
#define SYSCFG_DBGFORCE_PROC0_SWDO                           SYSCFG_DBGFORCE_PROC0_SWDO_Msk
// SYSCFG->MEMPOWERDOWN
#define SYSCFG_MEMPOWERDOWN_ROM_Pos                          7
#define SYSCFG_MEMPOWERDOWN_ROM_Msk                          ((uint32_t)0x00000080)
#define SYSCFG_MEMPOWERDOWN_ROM                              SYSCFG_MEMPOWERDOWN_ROM_Msk
#define SYSCFG_MEMPOWERDOWN_USB_Pos                          6
#define SYSCFG_MEMPOWERDOWN_USB_Msk                          ((uint32_t)0x00000040)
#define SYSCFG_MEMPOWERDOWN_USB                              SYSCFG_MEMPOWERDOWN_USB_Msk
#define SYSCFG_MEMPOWERDOWN_SRAM5_Pos                        5
#define SYSCFG_MEMPOWERDOWN_SRAM5_Msk                        ((uint32_t)0x00000020)
#define SYSCFG_MEMPOWERDOWN_SRAM5                            SYSCFG_MEMPOWERDOWN_SRAM5_Msk
#define SYSCFG_MEMPOWERDOWN_SRAM4_Pos                        4
#define SYSCFG_MEMPOWERDOWN_SRAM4_Msk                        ((uint32_t)0x00000010)
#define SYSCFG_MEMPOWERDOWN_SRAM4                            SYSCFG_MEMPOWERDOWN_SRAM4_Msk
#define SYSCFG_MEMPOWERDOWN_SRAM3_Pos                        3
#define SYSCFG_MEMPOWERDOWN_SRAM3_Msk                        ((uint32_t)0x00000008)
#define SYSCFG_MEMPOWERDOWN_SRAM3                            SYSCFG_MEMPOWERDOWN_SRAM3_Msk
#define SYSCFG_MEMPOWERDOWN_SRAM2_Pos                        2
#define SYSCFG_MEMPOWERDOWN_SRAM2_Msk                        ((uint32_t)0x00000004)
#define SYSCFG_MEMPOWERDOWN_SRAM2                            SYSCFG_MEMPOWERDOWN_SRAM2_Msk
#define SYSCFG_MEMPOWERDOWN_SRAM1_Pos                        1
#define SYSCFG_MEMPOWERDOWN_SRAM1_Msk                        ((uint32_t)0x00000002)
#define SYSCFG_MEMPOWERDOWN_SRAM1                            SYSCFG_MEMPOWERDOWN_SRAM1_Msk
#define SYSCFG_MEMPOWERDOWN_SRAM0_Pos                        0
#define SYSCFG_MEMPOWERDOWN_SRAM0_Msk                        ((uint32_t)0x00000001)
#define SYSCFG_MEMPOWERDOWN_SRAM0                            SYSCFG_MEMPOWERDOWN_SRAM0_Msk

// SYSINFO->CHIP_ID
#define SYSINFO_CHIP_ID_REVISION_Pos                         28
#define SYSINFO_CHIP_ID_REVISION_Msk                         ((uint32_t)0xF0000000)
#define SYSINFO_CHIP_ID_REVISION                             SYSINFO_CHIP_ID_REVISION_Msk
#define SYSINFO_CHIP_ID_PART_Pos                             12
#define SYSINFO_CHIP_ID_PART_Msk                             ((uint32_t)0x0FFFF000)
#define SYSINFO_CHIP_ID_PART                                 SYSINFO_CHIP_ID_PART_Msk
#define SYSINFO_CHIP_ID_MANUFACTURER_Pos                     0
#define SYSINFO_CHIP_ID_MANUFACTURER_Msk                     ((uint32_t)0x00000FFF)
#define SYSINFO_CHIP_ID_MANUFACTURER                         SYSINFO_CHIP_ID_MANUFACTURER_Msk
// SYSINFO->PLATFORM
#define SYSINFO_PLATFORM_ASIC_Pos                            1
#define SYSINFO_PLATFORM_ASIC_Msk                            ((uint32_t)0x00000002)
#define SYSINFO_PLATFORM_ASIC                                SYSINFO_PLATFORM_ASIC_Msk
#define SYSINFO_PLATFORM_FPGA_Pos                            0
#define SYSINFO_PLATFORM_FPGA_Msk                            ((uint32_t)0x00000001)
#define SYSINFO_PLATFORM_FPGA                                SYSINFO_PLATFORM_FPGA_Msk
// SYSINFO->GITREF_RP2040
#define SYSINFO_GITREF_RP2040_GITREF_RP2040_Pos              0
#define SYSINFO_GITREF_RP2040_GITREF_RP2040_Msk              ((uint32_t)0xFFFFFFFF)
#define SYSINFO_GITREF_RP2040_GITREF_RP2040                  SYSINFO_GITREF_RP2040_GITREF_RP2040_Msk

// TBMAN->PLATFORM
#define TBMAN_PLATFORM_FPGA_Pos                              1
#define TBMAN_PLATFORM_FPGA_Msk                              ((uint32_t)0x00000002)
#define TBMAN_PLATFORM_FPGA                                  TBMAN_PLATFORM_FPGA_Msk
#define TBMAN_PLATFORM_ASIC_Pos                              0
#define TBMAN_PLATFORM_ASIC_Msk                              ((uint32_t)0x00000001)
#define TBMAN_PLATFORM_ASIC                                  TBMAN_PLATFORM_ASIC_Msk

// TIMER->TIMEHW
#define TIMER_TIMEHW_TIMEHW_Pos                              0
#define TIMER_TIMEHW_TIMEHW_Msk                              ((uint32_t)0xFFFFFFFF)
#define TIMER_TIMEHW_TIMEHW                                  TIMER_TIMEHW_TIMEHW_Msk
// TIMER->TIMELW
#define TIMER_TIMELW_TIMELW_Pos                              0
#define TIMER_TIMELW_TIMELW_Msk                              ((uint32_t)0xFFFFFFFF)
#define TIMER_TIMELW_TIMELW                                  TIMER_TIMELW_TIMELW_Msk
// TIMER->TIMEHR
#define TIMER_TIMEHR_TIMEHR_Pos                              0
#define TIMER_TIMEHR_TIMEHR_Msk                              ((uint32_t)0xFFFFFFFF)
#define TIMER_TIMEHR_TIMEHR                                  TIMER_TIMEHR_TIMEHR_Msk
// TIMER->TIMELR
#define TIMER_TIMELR_TIMELR_Pos                              0
#define TIMER_TIMELR_TIMELR_Msk                              ((uint32_t)0xFFFFFFFF)
#define TIMER_TIMELR_TIMELR                                  TIMER_TIMELR_TIMELR_Msk
// TIMER->ALARM
#define TIMER_ALARM_ALARM0_Pos                               0
#define TIMER_ALARM_ALARM0_Msk                               ((uint32_t)0xFFFFFFFF)
#define TIMER_ALARM_ALARM0                                   TIMER_ALARM_ALARM0_Msk
// TIMER->ARMED
#define TIMER_ARMED_ARMED_Pos                                0
#define TIMER_ARMED_ARMED_Msk                                ((uint32_t)0x0000000F)
#define TIMER_ARMED_ARMED                                    TIMER_ARMED_ARMED_Msk
// TIMER->TIMERAWH
#define TIMER_TIMERAWH_TIMERAWH_Pos                          0
#define TIMER_TIMERAWH_TIMERAWH_Msk                          ((uint32_t)0xFFFFFFFF)
#define TIMER_TIMERAWH_TIMERAWH                              TIMER_TIMERAWH_TIMERAWH_Msk
// TIMER->TIMERAWL
#define TIMER_TIMERAWL_TIMERAWL_Pos                          0
#define TIMER_TIMERAWL_TIMERAWL_Msk                          ((uint32_t)0xFFFFFFFF)
#define TIMER_TIMERAWL_TIMERAWL                              TIMER_TIMERAWL_TIMERAWL_Msk
// TIMER->DBGPAUSE
#define TIMER_DBGPAUSE_DBG1_Pos                              2
#define TIMER_DBGPAUSE_DBG1_Msk                              ((uint32_t)0x00000004)
#define TIMER_DBGPAUSE_DBG1                                  TIMER_DBGPAUSE_DBG1_Msk
#define TIMER_DBGPAUSE_DBG0_Pos                              1
#define TIMER_DBGPAUSE_DBG0_Msk                              ((uint32_t)0x00000002)
#define TIMER_DBGPAUSE_DBG0                                  TIMER_DBGPAUSE_DBG0_Msk
// TIMER->PAUSE
#define TIMER_PAUSE_PAUSE_Pos                                0
#define TIMER_PAUSE_PAUSE_Msk                                ((uint32_t)0x00000001)
#define TIMER_PAUSE_PAUSE                                    TIMER_PAUSE_PAUSE_Msk
// TIMER->INTR
#define TIMER_INTR_ALARM_3_Pos                               3
#define TIMER_INTR_ALARM_3_Msk                               ((uint32_t)0x00000008)
#define TIMER_INTR_ALARM_3                                   TIMER_INTR_ALARM_3_Msk
#define TIMER_INTR_ALARM_2_Pos                               2
#define TIMER_INTR_ALARM_2_Msk                               ((uint32_t)0x00000004)
#define TIMER_INTR_ALARM_2                                   TIMER_INTR_ALARM_2_Msk
#define TIMER_INTR_ALARM_1_Pos                               1
#define TIMER_INTR_ALARM_1_Msk                               ((uint32_t)0x00000002)
#define TIMER_INTR_ALARM_1                                   TIMER_INTR_ALARM_1_Msk
#define TIMER_INTR_ALARM_0_Pos                               0
#define TIMER_INTR_ALARM_0_Msk                               ((uint32_t)0x00000001)
#define TIMER_INTR_ALARM_0                                   TIMER_INTR_ALARM_0_Msk
// TIMER->INTE
#define TIMER_INTE_ALARM_3_Pos                               3
#define TIMER_INTE_ALARM_3_Msk                               ((uint32_t)0x00000008)
#define TIMER_INTE_ALARM_3                                   TIMER_INTE_ALARM_3_Msk
#define TIMER_INTE_ALARM_2_Pos                               2
#define TIMER_INTE_ALARM_2_Msk                               ((uint32_t)0x00000004)
#define TIMER_INTE_ALARM_2                                   TIMER_INTE_ALARM_2_Msk
#define TIMER_INTE_ALARM_1_Pos                               1
#define TIMER_INTE_ALARM_1_Msk                               ((uint32_t)0x00000002)
#define TIMER_INTE_ALARM_1                                   TIMER_INTE_ALARM_1_Msk
#define TIMER_INTE_ALARM_0_Pos                               0
#define TIMER_INTE_ALARM_0_Msk                               ((uint32_t)0x00000001)
#define TIMER_INTE_ALARM_0                                   TIMER_INTE_ALARM_0_Msk
// TIMER->INTF
#define TIMER_INTF_ALARM_3_Pos                               3
#define TIMER_INTF_ALARM_3_Msk                               ((uint32_t)0x00000008)
#define TIMER_INTF_ALARM_3                                   TIMER_INTF_ALARM_3_Msk
#define TIMER_INTF_ALARM_2_Pos                               2
#define TIMER_INTF_ALARM_2_Msk                               ((uint32_t)0x00000004)
#define TIMER_INTF_ALARM_2                                   TIMER_INTF_ALARM_2_Msk
#define TIMER_INTF_ALARM_1_Pos                               1
#define TIMER_INTF_ALARM_1_Msk                               ((uint32_t)0x00000002)
#define TIMER_INTF_ALARM_1                                   TIMER_INTF_ALARM_1_Msk
#define TIMER_INTF_ALARM_0_Pos                               0
#define TIMER_INTF_ALARM_0_Msk                               ((uint32_t)0x00000001)
#define TIMER_INTF_ALARM_0                                   TIMER_INTF_ALARM_0_Msk
// TIMER->INTS
#define TIMER_INTS_ALARM_3_Pos                               3
#define TIMER_INTS_ALARM_3_Msk                               ((uint32_t)0x00000008)
#define TIMER_INTS_ALARM_3                                   TIMER_INTS_ALARM_3_Msk
#define TIMER_INTS_ALARM_2_Pos                               2
#define TIMER_INTS_ALARM_2_Msk                               ((uint32_t)0x00000004)
#define TIMER_INTS_ALARM_2                                   TIMER_INTS_ALARM_2_Msk
#define TIMER_INTS_ALARM_1_Pos                               1
#define TIMER_INTS_ALARM_1_Msk                               ((uint32_t)0x00000002)
#define TIMER_INTS_ALARM_1                                   TIMER_INTS_ALARM_1_Msk
#define TIMER_INTS_ALARM_0_Pos                               0
#define TIMER_INTS_ALARM_0_Msk                               ((uint32_t)0x00000001)
#define TIMER_INTS_ALARM_0                                   TIMER_INTS_ALARM_0_Msk

// UART->UARTDR
#define UART_UARTDR_OE_Pos                                   11
#define UART_UARTDR_OE_Msk                                   ((uint32_t)0x00000800)
#define UART_UARTDR_OE                                       UART_UARTDR_OE_Msk
#define UART_UARTDR_BE_Pos                                   10
#define UART_UARTDR_BE_Msk                                   ((uint32_t)0x00000400)
#define UART_UARTDR_BE                                       UART_UARTDR_BE_Msk
#define UART_UARTDR_PE_Pos                                   9
#define UART_UARTDR_PE_Msk                                   ((uint32_t)0x00000200)
#define UART_UARTDR_PE                                       UART_UARTDR_PE_Msk
#define UART_UARTDR_FE_Pos                                   8
#define UART_UARTDR_FE_Msk                                   ((uint32_t)0x00000100)
#define UART_UARTDR_FE                                       UART_UARTDR_FE_Msk
#define UART_UARTDR_DATA_Pos                                 0
#define UART_UARTDR_DATA_Msk                                 ((uint32_t)0x000000FF)
#define UART_UARTDR_DATA                                     UART_UARTDR_DATA_Msk
// UART->UARTRSR
#define UART_UARTRSR_OE_Pos                                  3
#define UART_UARTRSR_OE_Msk                                  ((uint32_t)0x00000008)
#define UART_UARTRSR_OE                                      UART_UARTRSR_OE_Msk
#define UART_UARTRSR_BE_Pos                                  2
#define UART_UARTRSR_BE_Msk                                  ((uint32_t)0x00000004)
#define UART_UARTRSR_BE                                      UART_UARTRSR_BE_Msk
#define UART_UARTRSR_PE_Pos                                  1
#define UART_UARTRSR_PE_Msk                                  ((uint32_t)0x00000002)
#define UART_UARTRSR_PE                                      UART_UARTRSR_PE_Msk
#define UART_UARTRSR_FE_Pos                                  0
#define UART_UARTRSR_FE_Msk                                  ((uint32_t)0x00000001)
#define UART_UARTRSR_FE                                      UART_UARTRSR_FE_Msk
// UART->UARTFR
#define UART_UARTFR_RI_Pos                                   8
#define UART_UARTFR_RI_Msk                                   ((uint32_t)0x00000100)
#define UART_UARTFR_RI                                       UART_UARTFR_RI_Msk
#define UART_UARTFR_TXFE_Pos                                 7
#define UART_UARTFR_TXFE_Msk                                 ((uint32_t)0x00000080)
#define UART_UARTFR_TXFE                                     UART_UARTFR_TXFE_Msk
#define UART_UARTFR_RXFF_Pos                                 6
#define UART_UARTFR_RXFF_Msk                                 ((uint32_t)0x00000040)
#define UART_UARTFR_RXFF                                     UART_UARTFR_RXFF_Msk
#define UART_UARTFR_TXFF_Pos                                 5
#define UART_UARTFR_TXFF_Msk                                 ((uint32_t)0x00000020)
#define UART_UARTFR_TXFF                                     UART_UARTFR_TXFF_Msk
#define UART_UARTFR_RXFE_Pos                                 4
#define UART_UARTFR_RXFE_Msk                                 ((uint32_t)0x00000010)
#define UART_UARTFR_RXFE                                     UART_UARTFR_RXFE_Msk
#define UART_UARTFR_BUSY_Pos                                 3
#define UART_UARTFR_BUSY_Msk                                 ((uint32_t)0x00000008)
#define UART_UARTFR_BUSY                                     UART_UARTFR_BUSY_Msk
#define UART_UARTFR_DCD_Pos                                  2
#define UART_UARTFR_DCD_Msk                                  ((uint32_t)0x00000004)
#define UART_UARTFR_DCD                                      UART_UARTFR_DCD_Msk
#define UART_UARTFR_DSR_Pos                                  1
#define UART_UARTFR_DSR_Msk                                  ((uint32_t)0x00000002)
#define UART_UARTFR_DSR                                      UART_UARTFR_DSR_Msk
#define UART_UARTFR_CTS_Pos                                  0
#define UART_UARTFR_CTS_Msk                                  ((uint32_t)0x00000001)
#define UART_UARTFR_CTS                                      UART_UARTFR_CTS_Msk
// UART->UARTILPR
#define UART_UARTILPR_ILPDVSR_Pos                            0
#define UART_UARTILPR_ILPDVSR_Msk                            ((uint32_t)0x000000FF)
#define UART_UARTILPR_ILPDVSR                                UART_UARTILPR_ILPDVSR_Msk
// UART->UARTIBRD
#define UART_UARTIBRD_BAUD_DIVINT_Pos                        0
#define UART_UARTIBRD_BAUD_DIVINT_Msk                        ((uint32_t)0x0000FFFF)
#define UART_UARTIBRD_BAUD_DIVINT                            UART_UARTIBRD_BAUD_DIVINT_Msk
// UART->UARTFBRD
#define UART_UARTFBRD_BAUD_DIVFRAC_Pos                       0
#define UART_UARTFBRD_BAUD_DIVFRAC_Msk                       ((uint32_t)0x0000003F)
#define UART_UARTFBRD_BAUD_DIVFRAC                           UART_UARTFBRD_BAUD_DIVFRAC_Msk
// UART->UARTLCR_H
#define UART_UARTLCR_H_SPS_Pos                               7
#define UART_UARTLCR_H_SPS_Msk                               ((uint32_t)0x00000080)
#define UART_UARTLCR_H_SPS                                   UART_UARTLCR_H_SPS_Msk
#define UART_UARTLCR_H_WLEN_Pos                              5
#define UART_UARTLCR_H_WLEN_Msk                              ((uint32_t)0x00000060)
#define UART_UARTLCR_H_WLEN                                  UART_UARTLCR_H_WLEN_Msk
#define UART_UARTLCR_H_FEN_Pos                               4
#define UART_UARTLCR_H_FEN_Msk                               ((uint32_t)0x00000010)
#define UART_UARTLCR_H_FEN                                   UART_UARTLCR_H_FEN_Msk
#define UART_UARTLCR_H_STP2_Pos                              3
#define UART_UARTLCR_H_STP2_Msk                              ((uint32_t)0x00000008)
#define UART_UARTLCR_H_STP2                                  UART_UARTLCR_H_STP2_Msk
#define UART_UARTLCR_H_EPS_Pos                               2
#define UART_UARTLCR_H_EPS_Msk                               ((uint32_t)0x00000004)
#define UART_UARTLCR_H_EPS                                   UART_UARTLCR_H_EPS_Msk
#define UART_UARTLCR_H_PEN_Pos                               1
#define UART_UARTLCR_H_PEN_Msk                               ((uint32_t)0x00000002)
#define UART_UARTLCR_H_PEN                                   UART_UARTLCR_H_PEN_Msk
#define UART_UARTLCR_H_BRK_Pos                               0
#define UART_UARTLCR_H_BRK_Msk                               ((uint32_t)0x00000001)
#define UART_UARTLCR_H_BRK                                   UART_UARTLCR_H_BRK_Msk
// UART->UARTCR
#define UART_UARTCR_CTSEN_Pos                                15
#define UART_UARTCR_CTSEN_Msk                                ((uint32_t)0x00008000)
#define UART_UARTCR_CTSEN                                    UART_UARTCR_CTSEN_Msk
#define UART_UARTCR_RTSEN_Pos                                14
#define UART_UARTCR_RTSEN_Msk                                ((uint32_t)0x00004000)
#define UART_UARTCR_RTSEN                                    UART_UARTCR_RTSEN_Msk
#define UART_UARTCR_OUT2_Pos                                 13
#define UART_UARTCR_OUT2_Msk                                 ((uint32_t)0x00002000)
#define UART_UARTCR_OUT2                                     UART_UARTCR_OUT2_Msk
#define UART_UARTCR_OUT1_Pos                                 12
#define UART_UARTCR_OUT1_Msk                                 ((uint32_t)0x00001000)
#define UART_UARTCR_OUT1                                     UART_UARTCR_OUT1_Msk
#define UART_UARTCR_RTS_Pos                                  11
#define UART_UARTCR_RTS_Msk                                  ((uint32_t)0x00000800)
#define UART_UARTCR_RTS                                      UART_UARTCR_RTS_Msk
#define UART_UARTCR_DTR_Pos                                  10
#define UART_UARTCR_DTR_Msk                                  ((uint32_t)0x00000400)
#define UART_UARTCR_DTR                                      UART_UARTCR_DTR_Msk
#define UART_UARTCR_RXE_Pos                                  9
#define UART_UARTCR_RXE_Msk                                  ((uint32_t)0x00000200)
#define UART_UARTCR_RXE                                      UART_UARTCR_RXE_Msk
#define UART_UARTCR_TXE_Pos                                  8
#define UART_UARTCR_TXE_Msk                                  ((uint32_t)0x00000100)
#define UART_UARTCR_TXE                                      UART_UARTCR_TXE_Msk
#define UART_UARTCR_LBE_Pos                                  7
#define UART_UARTCR_LBE_Msk                                  ((uint32_t)0x00000080)
#define UART_UARTCR_LBE                                      UART_UARTCR_LBE_Msk
#define UART_UARTCR_SIRLP_Pos                                2
#define UART_UARTCR_SIRLP_Msk                                ((uint32_t)0x00000004)
#define UART_UARTCR_SIRLP                                    UART_UARTCR_SIRLP_Msk
#define UART_UARTCR_SIREN_Pos                                1
#define UART_UARTCR_SIREN_Msk                                ((uint32_t)0x00000002)
#define UART_UARTCR_SIREN                                    UART_UARTCR_SIREN_Msk
#define UART_UARTCR_UARTEN_Pos                               0
#define UART_UARTCR_UARTEN_Msk                               ((uint32_t)0x00000001)
#define UART_UARTCR_UARTEN                                   UART_UARTCR_UARTEN_Msk
// UART->UARTIFLS
#define UART_UARTIFLS_RXIFLSEL_Pos                           3
#define UART_UARTIFLS_RXIFLSEL_Msk                           ((uint32_t)0x00000038)
#define UART_UARTIFLS_RXIFLSEL                               UART_UARTIFLS_RXIFLSEL_Msk
#define UART_UARTIFLS_TXIFLSEL_Pos                           0
#define UART_UARTIFLS_TXIFLSEL_Msk                           ((uint32_t)0x00000007)
#define UART_UARTIFLS_TXIFLSEL                               UART_UARTIFLS_TXIFLSEL_Msk
// UART->UARTIMSC
#define UART_UARTIMSC_OEIM_Pos                               10
#define UART_UARTIMSC_OEIM_Msk                               ((uint32_t)0x00000400)
#define UART_UARTIMSC_OEIM                                   UART_UARTIMSC_OEIM_Msk
#define UART_UARTIMSC_BEIM_Pos                               9
#define UART_UARTIMSC_BEIM_Msk                               ((uint32_t)0x00000200)
#define UART_UARTIMSC_BEIM                                   UART_UARTIMSC_BEIM_Msk
#define UART_UARTIMSC_PEIM_Pos                               8
#define UART_UARTIMSC_PEIM_Msk                               ((uint32_t)0x00000100)
#define UART_UARTIMSC_PEIM                                   UART_UARTIMSC_PEIM_Msk
#define UART_UARTIMSC_FEIM_Pos                               7
#define UART_UARTIMSC_FEIM_Msk                               ((uint32_t)0x00000080)
#define UART_UARTIMSC_FEIM                                   UART_UARTIMSC_FEIM_Msk
#define UART_UARTIMSC_RTIM_Pos                               6
#define UART_UARTIMSC_RTIM_Msk                               ((uint32_t)0x00000040)
#define UART_UARTIMSC_RTIM                                   UART_UARTIMSC_RTIM_Msk
#define UART_UARTIMSC_TXIM_Pos                               5
#define UART_UARTIMSC_TXIM_Msk                               ((uint32_t)0x00000020)
#define UART_UARTIMSC_TXIM                                   UART_UARTIMSC_TXIM_Msk
#define UART_UARTIMSC_RXIM_Pos                               4
#define UART_UARTIMSC_RXIM_Msk                               ((uint32_t)0x00000010)
#define UART_UARTIMSC_RXIM                                   UART_UARTIMSC_RXIM_Msk
#define UART_UARTIMSC_DSRMIM_Pos                             3
#define UART_UARTIMSC_DSRMIM_Msk                             ((uint32_t)0x00000008)
#define UART_UARTIMSC_DSRMIM                                 UART_UARTIMSC_DSRMIM_Msk
#define UART_UARTIMSC_DCDMIM_Pos                             2
#define UART_UARTIMSC_DCDMIM_Msk                             ((uint32_t)0x00000004)
#define UART_UARTIMSC_DCDMIM                                 UART_UARTIMSC_DCDMIM_Msk
#define UART_UARTIMSC_CTSMIM_Pos                             1
#define UART_UARTIMSC_CTSMIM_Msk                             ((uint32_t)0x00000002)
#define UART_UARTIMSC_CTSMIM                                 UART_UARTIMSC_CTSMIM_Msk
#define UART_UARTIMSC_RIMIM_Pos                              0
#define UART_UARTIMSC_RIMIM_Msk                              ((uint32_t)0x00000001)
#define UART_UARTIMSC_RIMIM                                  UART_UARTIMSC_RIMIM_Msk
// UART->UARTRIS
#define UART_UARTRIS_OERIS_Pos                               10
#define UART_UARTRIS_OERIS_Msk                               ((uint32_t)0x00000400)
#define UART_UARTRIS_OERIS                                   UART_UARTRIS_OERIS_Msk
#define UART_UARTRIS_BERIS_Pos                               9
#define UART_UARTRIS_BERIS_Msk                               ((uint32_t)0x00000200)
#define UART_UARTRIS_BERIS                                   UART_UARTRIS_BERIS_Msk
#define UART_UARTRIS_PERIS_Pos                               8
#define UART_UARTRIS_PERIS_Msk                               ((uint32_t)0x00000100)
#define UART_UARTRIS_PERIS                                   UART_UARTRIS_PERIS_Msk
#define UART_UARTRIS_FERIS_Pos                               7
#define UART_UARTRIS_FERIS_Msk                               ((uint32_t)0x00000080)
#define UART_UARTRIS_FERIS                                   UART_UARTRIS_FERIS_Msk
#define UART_UARTRIS_RTRIS_Pos                               6
#define UART_UARTRIS_RTRIS_Msk                               ((uint32_t)0x00000040)
#define UART_UARTRIS_RTRIS                                   UART_UARTRIS_RTRIS_Msk
#define UART_UARTRIS_TXRIS_Pos                               5
#define UART_UARTRIS_TXRIS_Msk                               ((uint32_t)0x00000020)
#define UART_UARTRIS_TXRIS                                   UART_UARTRIS_TXRIS_Msk
#define UART_UARTRIS_RXRIS_Pos                               4
#define UART_UARTRIS_RXRIS_Msk                               ((uint32_t)0x00000010)
#define UART_UARTRIS_RXRIS                                   UART_UARTRIS_RXRIS_Msk
#define UART_UARTRIS_DSRRMIS_Pos                             3
#define UART_UARTRIS_DSRRMIS_Msk                             ((uint32_t)0x00000008)
#define UART_UARTRIS_DSRRMIS                                 UART_UARTRIS_DSRRMIS_Msk
#define UART_UARTRIS_DCDRMIS_Pos                             2
#define UART_UARTRIS_DCDRMIS_Msk                             ((uint32_t)0x00000004)
#define UART_UARTRIS_DCDRMIS                                 UART_UARTRIS_DCDRMIS_Msk
#define UART_UARTRIS_CTSRMIS_Pos                             1
#define UART_UARTRIS_CTSRMIS_Msk                             ((uint32_t)0x00000002)
#define UART_UARTRIS_CTSRMIS                                 UART_UARTRIS_CTSRMIS_Msk
#define UART_UARTRIS_RIRMIS_Pos                              0
#define UART_UARTRIS_RIRMIS_Msk                              ((uint32_t)0x00000001)
#define UART_UARTRIS_RIRMIS                                  UART_UARTRIS_RIRMIS_Msk
// UART->UARTMIS
#define UART_UARTMIS_OEMIS_Pos                               10
#define UART_UARTMIS_OEMIS_Msk                               ((uint32_t)0x00000400)
#define UART_UARTMIS_OEMIS                                   UART_UARTMIS_OEMIS_Msk
#define UART_UARTMIS_BEMIS_Pos                               9
#define UART_UARTMIS_BEMIS_Msk                               ((uint32_t)0x00000200)
#define UART_UARTMIS_BEMIS                                   UART_UARTMIS_BEMIS_Msk
#define UART_UARTMIS_PEMIS_Pos                               8
#define UART_UARTMIS_PEMIS_Msk                               ((uint32_t)0x00000100)
#define UART_UARTMIS_PEMIS                                   UART_UARTMIS_PEMIS_Msk
#define UART_UARTMIS_FEMIS_Pos                               7
#define UART_UARTMIS_FEMIS_Msk                               ((uint32_t)0x00000080)
#define UART_UARTMIS_FEMIS                                   UART_UARTMIS_FEMIS_Msk
#define UART_UARTMIS_RTMIS_Pos                               6
#define UART_UARTMIS_RTMIS_Msk                               ((uint32_t)0x00000040)
#define UART_UARTMIS_RTMIS                                   UART_UARTMIS_RTMIS_Msk
#define UART_UARTMIS_TXMIS_Pos                               5
#define UART_UARTMIS_TXMIS_Msk                               ((uint32_t)0x00000020)
#define UART_UARTMIS_TXMIS                                   UART_UARTMIS_TXMIS_Msk
#define UART_UARTMIS_RXMIS_Pos                               4
#define UART_UARTMIS_RXMIS_Msk                               ((uint32_t)0x00000010)
#define UART_UARTMIS_RXMIS                                   UART_UARTMIS_RXMIS_Msk
#define UART_UARTMIS_DSRMMIS_Pos                             3
#define UART_UARTMIS_DSRMMIS_Msk                             ((uint32_t)0x00000008)
#define UART_UARTMIS_DSRMMIS                                 UART_UARTMIS_DSRMMIS_Msk
#define UART_UARTMIS_DCDMMIS_Pos                             2
#define UART_UARTMIS_DCDMMIS_Msk                             ((uint32_t)0x00000004)
#define UART_UARTMIS_DCDMMIS                                 UART_UARTMIS_DCDMMIS_Msk
#define UART_UARTMIS_CTSMMIS_Pos                             1
#define UART_UARTMIS_CTSMMIS_Msk                             ((uint32_t)0x00000002)
#define UART_UARTMIS_CTSMMIS                                 UART_UARTMIS_CTSMMIS_Msk
#define UART_UARTMIS_RIMMIS_Pos                              0
#define UART_UARTMIS_RIMMIS_Msk                              ((uint32_t)0x00000001)
#define UART_UARTMIS_RIMMIS                                  UART_UARTMIS_RIMMIS_Msk
// UART->UARTICR
#define UART_UARTICR_OEIC_Pos                                10
#define UART_UARTICR_OEIC_Msk                                ((uint32_t)0x00000400)
#define UART_UARTICR_OEIC                                    UART_UARTICR_OEIC_Msk
#define UART_UARTICR_BEIC_Pos                                9
#define UART_UARTICR_BEIC_Msk                                ((uint32_t)0x00000200)
#define UART_UARTICR_BEIC                                    UART_UARTICR_BEIC_Msk
#define UART_UARTICR_PEIC_Pos                                8
#define UART_UARTICR_PEIC_Msk                                ((uint32_t)0x00000100)
#define UART_UARTICR_PEIC                                    UART_UARTICR_PEIC_Msk
#define UART_UARTICR_FEIC_Pos                                7
#define UART_UARTICR_FEIC_Msk                                ((uint32_t)0x00000080)
#define UART_UARTICR_FEIC                                    UART_UARTICR_FEIC_Msk
#define UART_UARTICR_RTIC_Pos                                6
#define UART_UARTICR_RTIC_Msk                                ((uint32_t)0x00000040)
#define UART_UARTICR_RTIC                                    UART_UARTICR_RTIC_Msk
#define UART_UARTICR_TXIC_Pos                                5
#define UART_UARTICR_TXIC_Msk                                ((uint32_t)0x00000020)
#define UART_UARTICR_TXIC                                    UART_UARTICR_TXIC_Msk
#define UART_UARTICR_RXIC_Pos                                4
#define UART_UARTICR_RXIC_Msk                                ((uint32_t)0x00000010)
#define UART_UARTICR_RXIC                                    UART_UARTICR_RXIC_Msk
#define UART_UARTICR_DSRMIC_Pos                              3
#define UART_UARTICR_DSRMIC_Msk                              ((uint32_t)0x00000008)
#define UART_UARTICR_DSRMIC                                  UART_UARTICR_DSRMIC_Msk
#define UART_UARTICR_DCDMIC_Pos                              2
#define UART_UARTICR_DCDMIC_Msk                              ((uint32_t)0x00000004)
#define UART_UARTICR_DCDMIC                                  UART_UARTICR_DCDMIC_Msk
#define UART_UARTICR_CTSMIC_Pos                              1
#define UART_UARTICR_CTSMIC_Msk                              ((uint32_t)0x00000002)
#define UART_UARTICR_CTSMIC                                  UART_UARTICR_CTSMIC_Msk
#define UART_UARTICR_RIMIC_Pos                               0
#define UART_UARTICR_RIMIC_Msk                               ((uint32_t)0x00000001)
#define UART_UARTICR_RIMIC                                   UART_UARTICR_RIMIC_Msk
// UART->UARTDMACR
#define UART_UARTDMACR_DMAONERR_Pos                          2
#define UART_UARTDMACR_DMAONERR_Msk                          ((uint32_t)0x00000004)
#define UART_UARTDMACR_DMAONERR                              UART_UARTDMACR_DMAONERR_Msk
#define UART_UARTDMACR_TXDMAE_Pos                            1
#define UART_UARTDMACR_TXDMAE_Msk                            ((uint32_t)0x00000002)
#define UART_UARTDMACR_TXDMAE                                UART_UARTDMACR_TXDMAE_Msk
#define UART_UARTDMACR_RXDMAE_Pos                            0
#define UART_UARTDMACR_RXDMAE_Msk                            ((uint32_t)0x00000001)
#define UART_UARTDMACR_RXDMAE                                UART_UARTDMACR_RXDMAE_Msk
// UART->UARTPERIPHID0
#define UART_UARTPERIPHID0_PARTNUMBER0_Pos                   0
#define UART_UARTPERIPHID0_PARTNUMBER0_Msk                   ((uint32_t)0x000000FF)
#define UART_UARTPERIPHID0_PARTNUMBER0                       UART_UARTPERIPHID0_PARTNUMBER0_Msk
// UART->UARTPERIPHID1
#define UART_UARTPERIPHID1_DESIGNER0_Pos                     4
#define UART_UARTPERIPHID1_DESIGNER0_Msk                     ((uint32_t)0x000000F0)
#define UART_UARTPERIPHID1_DESIGNER0                         UART_UARTPERIPHID1_DESIGNER0_Msk
#define UART_UARTPERIPHID1_PARTNUMBER1_Pos                   0
#define UART_UARTPERIPHID1_PARTNUMBER1_Msk                   ((uint32_t)0x0000000F)
#define UART_UARTPERIPHID1_PARTNUMBER1                       UART_UARTPERIPHID1_PARTNUMBER1_Msk
// UART->UARTPERIPHID2
#define UART_UARTPERIPHID2_REVISION_Pos                      4
#define UART_UARTPERIPHID2_REVISION_Msk                      ((uint32_t)0x000000F0)
#define UART_UARTPERIPHID2_REVISION                          UART_UARTPERIPHID2_REVISION_Msk
#define UART_UARTPERIPHID2_DESIGNER1_Pos                     0
#define UART_UARTPERIPHID2_DESIGNER1_Msk                     ((uint32_t)0x0000000F)
#define UART_UARTPERIPHID2_DESIGNER1                         UART_UARTPERIPHID2_DESIGNER1_Msk
// UART->UARTPERIPHID3
#define UART_UARTPERIPHID3_CONFIGURATION_Pos                 0
#define UART_UARTPERIPHID3_CONFIGURATION_Msk                 ((uint32_t)0x000000FF)
#define UART_UARTPERIPHID3_CONFIGURATION                     UART_UARTPERIPHID3_CONFIGURATION_Msk
// UART->UARTPCELLID
#define UART_UARTPCELLID_UARTPCELLID0_Pos                    0
#define UART_UARTPCELLID_UARTPCELLID0_Msk                    ((uint32_t)0x000000FF)
#define UART_UARTPCELLID_UARTPCELLID0                        UART_UARTPCELLID_UARTPCELLID0_Msk

// USB->ADDR_ENDP
#define USB_ADDR_ENDP_ENDPOINT_Pos                           16
#define USB_ADDR_ENDP_ENDPOINT_Msk                           ((uint32_t)0x000F0000)
#define USB_ADDR_ENDP_ENDPOINT                               USB_ADDR_ENDP_ENDPOINT_Msk
#define USB_ADDR_ENDP_ADDRESS_Pos                            0
#define USB_ADDR_ENDP_ADDRESS_Msk                            ((uint32_t)0x0000007F)
#define USB_ADDR_ENDP_ADDRESS                                USB_ADDR_ENDP_ADDRESS_Msk
// USB->ADDR_ENDP1
#define USB_ADDR_ENDP1_INTEP_PREAMBLE_Pos                    26
#define USB_ADDR_ENDP1_INTEP_PREAMBLE_Msk                    ((uint32_t)0x04000000)
#define USB_ADDR_ENDP1_INTEP_PREAMBLE                        USB_ADDR_ENDP1_INTEP_PREAMBLE_Msk
#define USB_ADDR_ENDP1_INTEP_DIR_Pos                         25
#define USB_ADDR_ENDP1_INTEP_DIR_Msk                         ((uint32_t)0x02000000)
#define USB_ADDR_ENDP1_INTEP_DIR                             USB_ADDR_ENDP1_INTEP_DIR_Msk
#define USB_ADDR_ENDP1_ENDPOINT_Pos                          16
#define USB_ADDR_ENDP1_ENDPOINT_Msk                          ((uint32_t)0x000F0000)
#define USB_ADDR_ENDP1_ENDPOINT                              USB_ADDR_ENDP1_ENDPOINT_Msk
#define USB_ADDR_ENDP1_ADDRESS_Pos                           0
#define USB_ADDR_ENDP1_ADDRESS_Msk                           ((uint32_t)0x0000007F)
#define USB_ADDR_ENDP1_ADDRESS                               USB_ADDR_ENDP1_ADDRESS_Msk
// USB->ADDR_ENDP2
#define USB_ADDR_ENDP2_INTEP_PREAMBLE_Pos                    26
#define USB_ADDR_ENDP2_INTEP_PREAMBLE_Msk                    ((uint32_t)0x04000000)
#define USB_ADDR_ENDP2_INTEP_PREAMBLE                        USB_ADDR_ENDP2_INTEP_PREAMBLE_Msk
#define USB_ADDR_ENDP2_INTEP_DIR_Pos                         25
#define USB_ADDR_ENDP2_INTEP_DIR_Msk                         ((uint32_t)0x02000000)
#define USB_ADDR_ENDP2_INTEP_DIR                             USB_ADDR_ENDP2_INTEP_DIR_Msk
#define USB_ADDR_ENDP2_ENDPOINT_Pos                          16
#define USB_ADDR_ENDP2_ENDPOINT_Msk                          ((uint32_t)0x000F0000)
#define USB_ADDR_ENDP2_ENDPOINT                              USB_ADDR_ENDP2_ENDPOINT_Msk
#define USB_ADDR_ENDP2_ADDRESS_Pos                           0
#define USB_ADDR_ENDP2_ADDRESS_Msk                           ((uint32_t)0x0000007F)
#define USB_ADDR_ENDP2_ADDRESS                               USB_ADDR_ENDP2_ADDRESS_Msk
// USB->ADDR_ENDP3
#define USB_ADDR_ENDP3_INTEP_PREAMBLE_Pos                    26
#define USB_ADDR_ENDP3_INTEP_PREAMBLE_Msk                    ((uint32_t)0x04000000)
#define USB_ADDR_ENDP3_INTEP_PREAMBLE                        USB_ADDR_ENDP3_INTEP_PREAMBLE_Msk
#define USB_ADDR_ENDP3_INTEP_DIR_Pos                         25
#define USB_ADDR_ENDP3_INTEP_DIR_Msk                         ((uint32_t)0x02000000)
#define USB_ADDR_ENDP3_INTEP_DIR                             USB_ADDR_ENDP3_INTEP_DIR_Msk
#define USB_ADDR_ENDP3_ENDPOINT_Pos                          16
#define USB_ADDR_ENDP3_ENDPOINT_Msk                          ((uint32_t)0x000F0000)
#define USB_ADDR_ENDP3_ENDPOINT                              USB_ADDR_ENDP3_ENDPOINT_Msk
#define USB_ADDR_ENDP3_ADDRESS_Pos                           0
#define USB_ADDR_ENDP3_ADDRESS_Msk                           ((uint32_t)0x0000007F)
#define USB_ADDR_ENDP3_ADDRESS                               USB_ADDR_ENDP3_ADDRESS_Msk
// USB->ADDR_ENDP4
#define USB_ADDR_ENDP4_INTEP_PREAMBLE_Pos                    26
#define USB_ADDR_ENDP4_INTEP_PREAMBLE_Msk                    ((uint32_t)0x04000000)
#define USB_ADDR_ENDP4_INTEP_PREAMBLE                        USB_ADDR_ENDP4_INTEP_PREAMBLE_Msk
#define USB_ADDR_ENDP4_INTEP_DIR_Pos                         25
#define USB_ADDR_ENDP4_INTEP_DIR_Msk                         ((uint32_t)0x02000000)
#define USB_ADDR_ENDP4_INTEP_DIR                             USB_ADDR_ENDP4_INTEP_DIR_Msk
#define USB_ADDR_ENDP4_ENDPOINT_Pos                          16
#define USB_ADDR_ENDP4_ENDPOINT_Msk                          ((uint32_t)0x000F0000)
#define USB_ADDR_ENDP4_ENDPOINT                              USB_ADDR_ENDP4_ENDPOINT_Msk
#define USB_ADDR_ENDP4_ADDRESS_Pos                           0
#define USB_ADDR_ENDP4_ADDRESS_Msk                           ((uint32_t)0x0000007F)
#define USB_ADDR_ENDP4_ADDRESS                               USB_ADDR_ENDP4_ADDRESS_Msk
// USB->ADDR_ENDP5
#define USB_ADDR_ENDP5_INTEP_PREAMBLE_Pos                    26
#define USB_ADDR_ENDP5_INTEP_PREAMBLE_Msk                    ((uint32_t)0x04000000)
#define USB_ADDR_ENDP5_INTEP_PREAMBLE                        USB_ADDR_ENDP5_INTEP_PREAMBLE_Msk
#define USB_ADDR_ENDP5_INTEP_DIR_Pos                         25
#define USB_ADDR_ENDP5_INTEP_DIR_Msk                         ((uint32_t)0x02000000)
#define USB_ADDR_ENDP5_INTEP_DIR                             USB_ADDR_ENDP5_INTEP_DIR_Msk
#define USB_ADDR_ENDP5_ENDPOINT_Pos                          16
#define USB_ADDR_ENDP5_ENDPOINT_Msk                          ((uint32_t)0x000F0000)
#define USB_ADDR_ENDP5_ENDPOINT                              USB_ADDR_ENDP5_ENDPOINT_Msk
#define USB_ADDR_ENDP5_ADDRESS_Pos                           0
#define USB_ADDR_ENDP5_ADDRESS_Msk                           ((uint32_t)0x0000007F)
#define USB_ADDR_ENDP5_ADDRESS                               USB_ADDR_ENDP5_ADDRESS_Msk
// USB->ADDR_ENDP6
#define USB_ADDR_ENDP6_INTEP_PREAMBLE_Pos                    26
#define USB_ADDR_ENDP6_INTEP_PREAMBLE_Msk                    ((uint32_t)0x04000000)
#define USB_ADDR_ENDP6_INTEP_PREAMBLE                        USB_ADDR_ENDP6_INTEP_PREAMBLE_Msk
#define USB_ADDR_ENDP6_INTEP_DIR_Pos                         25
#define USB_ADDR_ENDP6_INTEP_DIR_Msk                         ((uint32_t)0x02000000)
#define USB_ADDR_ENDP6_INTEP_DIR                             USB_ADDR_ENDP6_INTEP_DIR_Msk
#define USB_ADDR_ENDP6_ENDPOINT_Pos                          16
#define USB_ADDR_ENDP6_ENDPOINT_Msk                          ((uint32_t)0x000F0000)
#define USB_ADDR_ENDP6_ENDPOINT                              USB_ADDR_ENDP6_ENDPOINT_Msk
#define USB_ADDR_ENDP6_ADDRESS_Pos                           0
#define USB_ADDR_ENDP6_ADDRESS_Msk                           ((uint32_t)0x0000007F)
#define USB_ADDR_ENDP6_ADDRESS                               USB_ADDR_ENDP6_ADDRESS_Msk
// USB->ADDR_ENDP7
#define USB_ADDR_ENDP7_INTEP_PREAMBLE_Pos                    26
#define USB_ADDR_ENDP7_INTEP_PREAMBLE_Msk                    ((uint32_t)0x04000000)
#define USB_ADDR_ENDP7_INTEP_PREAMBLE                        USB_ADDR_ENDP7_INTEP_PREAMBLE_Msk
#define USB_ADDR_ENDP7_INTEP_DIR_Pos                         25
#define USB_ADDR_ENDP7_INTEP_DIR_Msk                         ((uint32_t)0x02000000)
#define USB_ADDR_ENDP7_INTEP_DIR                             USB_ADDR_ENDP7_INTEP_DIR_Msk
#define USB_ADDR_ENDP7_ENDPOINT_Pos                          16
#define USB_ADDR_ENDP7_ENDPOINT_Msk                          ((uint32_t)0x000F0000)
#define USB_ADDR_ENDP7_ENDPOINT                              USB_ADDR_ENDP7_ENDPOINT_Msk
#define USB_ADDR_ENDP7_ADDRESS_Pos                           0
#define USB_ADDR_ENDP7_ADDRESS_Msk                           ((uint32_t)0x0000007F)
#define USB_ADDR_ENDP7_ADDRESS                               USB_ADDR_ENDP7_ADDRESS_Msk
// USB->ADDR_ENDP8
#define USB_ADDR_ENDP8_INTEP_PREAMBLE_Pos                    26
#define USB_ADDR_ENDP8_INTEP_PREAMBLE_Msk                    ((uint32_t)0x04000000)
#define USB_ADDR_ENDP8_INTEP_PREAMBLE                        USB_ADDR_ENDP8_INTEP_PREAMBLE_Msk
#define USB_ADDR_ENDP8_INTEP_DIR_Pos                         25
#define USB_ADDR_ENDP8_INTEP_DIR_Msk                         ((uint32_t)0x02000000)
#define USB_ADDR_ENDP8_INTEP_DIR                             USB_ADDR_ENDP8_INTEP_DIR_Msk
#define USB_ADDR_ENDP8_ENDPOINT_Pos                          16
#define USB_ADDR_ENDP8_ENDPOINT_Msk                          ((uint32_t)0x000F0000)
#define USB_ADDR_ENDP8_ENDPOINT                              USB_ADDR_ENDP8_ENDPOINT_Msk
#define USB_ADDR_ENDP8_ADDRESS_Pos                           0
#define USB_ADDR_ENDP8_ADDRESS_Msk                           ((uint32_t)0x0000007F)
#define USB_ADDR_ENDP8_ADDRESS                               USB_ADDR_ENDP8_ADDRESS_Msk
// USB->ADDR_ENDP9
#define USB_ADDR_ENDP9_INTEP_PREAMBLE_Pos                    26
#define USB_ADDR_ENDP9_INTEP_PREAMBLE_Msk                    ((uint32_t)0x04000000)
#define USB_ADDR_ENDP9_INTEP_PREAMBLE                        USB_ADDR_ENDP9_INTEP_PREAMBLE_Msk
#define USB_ADDR_ENDP9_INTEP_DIR_Pos                         25
#define USB_ADDR_ENDP9_INTEP_DIR_Msk                         ((uint32_t)0x02000000)
#define USB_ADDR_ENDP9_INTEP_DIR                             USB_ADDR_ENDP9_INTEP_DIR_Msk
#define USB_ADDR_ENDP9_ENDPOINT_Pos                          16
#define USB_ADDR_ENDP9_ENDPOINT_Msk                          ((uint32_t)0x000F0000)
#define USB_ADDR_ENDP9_ENDPOINT                              USB_ADDR_ENDP9_ENDPOINT_Msk
#define USB_ADDR_ENDP9_ADDRESS_Pos                           0
#define USB_ADDR_ENDP9_ADDRESS_Msk                           ((uint32_t)0x0000007F)
#define USB_ADDR_ENDP9_ADDRESS                               USB_ADDR_ENDP9_ADDRESS_Msk
// USB->ADDR_ENDP10
#define USB_ADDR_ENDP10_INTEP_PREAMBLE_Pos                   26
#define USB_ADDR_ENDP10_INTEP_PREAMBLE_Msk                   ((uint32_t)0x04000000)
#define USB_ADDR_ENDP10_INTEP_PREAMBLE                       USB_ADDR_ENDP10_INTEP_PREAMBLE_Msk
#define USB_ADDR_ENDP10_INTEP_DIR_Pos                        25
#define USB_ADDR_ENDP10_INTEP_DIR_Msk                        ((uint32_t)0x02000000)
#define USB_ADDR_ENDP10_INTEP_DIR                            USB_ADDR_ENDP10_INTEP_DIR_Msk
#define USB_ADDR_ENDP10_ENDPOINT_Pos                         16
#define USB_ADDR_ENDP10_ENDPOINT_Msk                         ((uint32_t)0x000F0000)
#define USB_ADDR_ENDP10_ENDPOINT                             USB_ADDR_ENDP10_ENDPOINT_Msk
#define USB_ADDR_ENDP10_ADDRESS_Pos                          0
#define USB_ADDR_ENDP10_ADDRESS_Msk                          ((uint32_t)0x0000007F)
#define USB_ADDR_ENDP10_ADDRESS                              USB_ADDR_ENDP10_ADDRESS_Msk
// USB->ADDR_ENDP11
#define USB_ADDR_ENDP11_INTEP_PREAMBLE_Pos                   26
#define USB_ADDR_ENDP11_INTEP_PREAMBLE_Msk                   ((uint32_t)0x04000000)
#define USB_ADDR_ENDP11_INTEP_PREAMBLE                       USB_ADDR_ENDP11_INTEP_PREAMBLE_Msk
#define USB_ADDR_ENDP11_INTEP_DIR_Pos                        25
#define USB_ADDR_ENDP11_INTEP_DIR_Msk                        ((uint32_t)0x02000000)
#define USB_ADDR_ENDP11_INTEP_DIR                            USB_ADDR_ENDP11_INTEP_DIR_Msk
#define USB_ADDR_ENDP11_ENDPOINT_Pos                         16
#define USB_ADDR_ENDP11_ENDPOINT_Msk                         ((uint32_t)0x000F0000)
#define USB_ADDR_ENDP11_ENDPOINT                             USB_ADDR_ENDP11_ENDPOINT_Msk
#define USB_ADDR_ENDP11_ADDRESS_Pos                          0
#define USB_ADDR_ENDP11_ADDRESS_Msk                          ((uint32_t)0x0000007F)
#define USB_ADDR_ENDP11_ADDRESS                              USB_ADDR_ENDP11_ADDRESS_Msk
// USB->ADDR_ENDP12
#define USB_ADDR_ENDP12_INTEP_PREAMBLE_Pos                   26
#define USB_ADDR_ENDP12_INTEP_PREAMBLE_Msk                   ((uint32_t)0x04000000)
#define USB_ADDR_ENDP12_INTEP_PREAMBLE                       USB_ADDR_ENDP12_INTEP_PREAMBLE_Msk
#define USB_ADDR_ENDP12_INTEP_DIR_Pos                        25
#define USB_ADDR_ENDP12_INTEP_DIR_Msk                        ((uint32_t)0x02000000)
#define USB_ADDR_ENDP12_INTEP_DIR                            USB_ADDR_ENDP12_INTEP_DIR_Msk
#define USB_ADDR_ENDP12_ENDPOINT_Pos                         16
#define USB_ADDR_ENDP12_ENDPOINT_Msk                         ((uint32_t)0x000F0000)
#define USB_ADDR_ENDP12_ENDPOINT                             USB_ADDR_ENDP12_ENDPOINT_Msk
#define USB_ADDR_ENDP12_ADDRESS_Pos                          0
#define USB_ADDR_ENDP12_ADDRESS_Msk                          ((uint32_t)0x0000007F)
#define USB_ADDR_ENDP12_ADDRESS                              USB_ADDR_ENDP12_ADDRESS_Msk
// USB->ADDR_ENDP13
#define USB_ADDR_ENDP13_INTEP_PREAMBLE_Pos                   26
#define USB_ADDR_ENDP13_INTEP_PREAMBLE_Msk                   ((uint32_t)0x04000000)
#define USB_ADDR_ENDP13_INTEP_PREAMBLE                       USB_ADDR_ENDP13_INTEP_PREAMBLE_Msk
#define USB_ADDR_ENDP13_INTEP_DIR_Pos                        25
#define USB_ADDR_ENDP13_INTEP_DIR_Msk                        ((uint32_t)0x02000000)
#define USB_ADDR_ENDP13_INTEP_DIR                            USB_ADDR_ENDP13_INTEP_DIR_Msk
#define USB_ADDR_ENDP13_ENDPOINT_Pos                         16
#define USB_ADDR_ENDP13_ENDPOINT_Msk                         ((uint32_t)0x000F0000)
#define USB_ADDR_ENDP13_ENDPOINT                             USB_ADDR_ENDP13_ENDPOINT_Msk
#define USB_ADDR_ENDP13_ADDRESS_Pos                          0
#define USB_ADDR_ENDP13_ADDRESS_Msk                          ((uint32_t)0x0000007F)
#define USB_ADDR_ENDP13_ADDRESS                              USB_ADDR_ENDP13_ADDRESS_Msk
// USB->ADDR_ENDP14
#define USB_ADDR_ENDP14_INTEP_PREAMBLE_Pos                   26
#define USB_ADDR_ENDP14_INTEP_PREAMBLE_Msk                   ((uint32_t)0x04000000)
#define USB_ADDR_ENDP14_INTEP_PREAMBLE                       USB_ADDR_ENDP14_INTEP_PREAMBLE_Msk
#define USB_ADDR_ENDP14_INTEP_DIR_Pos                        25
#define USB_ADDR_ENDP14_INTEP_DIR_Msk                        ((uint32_t)0x02000000)
#define USB_ADDR_ENDP14_INTEP_DIR                            USB_ADDR_ENDP14_INTEP_DIR_Msk
#define USB_ADDR_ENDP14_ENDPOINT_Pos                         16
#define USB_ADDR_ENDP14_ENDPOINT_Msk                         ((uint32_t)0x000F0000)
#define USB_ADDR_ENDP14_ENDPOINT                             USB_ADDR_ENDP14_ENDPOINT_Msk
#define USB_ADDR_ENDP14_ADDRESS_Pos                          0
#define USB_ADDR_ENDP14_ADDRESS_Msk                          ((uint32_t)0x0000007F)
#define USB_ADDR_ENDP14_ADDRESS                              USB_ADDR_ENDP14_ADDRESS_Msk
// USB->ADDR_ENDP15
#define USB_ADDR_ENDP15_INTEP_PREAMBLE_Pos                   26
#define USB_ADDR_ENDP15_INTEP_PREAMBLE_Msk                   ((uint32_t)0x04000000)
#define USB_ADDR_ENDP15_INTEP_PREAMBLE                       USB_ADDR_ENDP15_INTEP_PREAMBLE_Msk
#define USB_ADDR_ENDP15_INTEP_DIR_Pos                        25
#define USB_ADDR_ENDP15_INTEP_DIR_Msk                        ((uint32_t)0x02000000)
#define USB_ADDR_ENDP15_INTEP_DIR                            USB_ADDR_ENDP15_INTEP_DIR_Msk
#define USB_ADDR_ENDP15_ENDPOINT_Pos                         16
#define USB_ADDR_ENDP15_ENDPOINT_Msk                         ((uint32_t)0x000F0000)
#define USB_ADDR_ENDP15_ENDPOINT                             USB_ADDR_ENDP15_ENDPOINT_Msk
#define USB_ADDR_ENDP15_ADDRESS_Pos                          0
#define USB_ADDR_ENDP15_ADDRESS_Msk                          ((uint32_t)0x0000007F)
#define USB_ADDR_ENDP15_ADDRESS                              USB_ADDR_ENDP15_ADDRESS_Msk
// USB->MAIN_CTRL
#define USB_MAIN_CTRL_SIM_TIMING_Pos                         31
#define USB_MAIN_CTRL_SIM_TIMING_Msk                         ((uint32_t)0x80000000)
#define USB_MAIN_CTRL_SIM_TIMING                             USB_MAIN_CTRL_SIM_TIMING_Msk
#define USB_MAIN_CTRL_HOST_NDEVICE_Pos                       1
#define USB_MAIN_CTRL_HOST_NDEVICE_Msk                       ((uint32_t)0x00000002)
#define USB_MAIN_CTRL_HOST_NDEVICE                           USB_MAIN_CTRL_HOST_NDEVICE_Msk
#define USB_MAIN_CTRL_CONTROLLER_EN_Pos                      0
#define USB_MAIN_CTRL_CONTROLLER_EN_Msk                      ((uint32_t)0x00000001)
#define USB_MAIN_CTRL_CONTROLLER_EN                          USB_MAIN_CTRL_CONTROLLER_EN_Msk
// USB->SOF_WR
#define USB_SOF_WR_COUNT_Pos                                 0
#define USB_SOF_WR_COUNT_Msk                                 ((uint32_t)0x000007FF)
#define USB_SOF_WR_COUNT                                     USB_SOF_WR_COUNT_Msk
// USB->SOF_RD
#define USB_SOF_RD_COUNT_Pos                                 0
#define USB_SOF_RD_COUNT_Msk                                 ((uint32_t)0x000007FF)
#define USB_SOF_RD_COUNT                                     USB_SOF_RD_COUNT_Msk
// USB->SIE_CTRL
#define USB_SIE_CTRL_EP0_INT_STALL_Pos                       31
#define USB_SIE_CTRL_EP0_INT_STALL_Msk                       ((uint32_t)0x80000000)
#define USB_SIE_CTRL_EP0_INT_STALL                           USB_SIE_CTRL_EP0_INT_STALL_Msk
#define USB_SIE_CTRL_EP0_DOUBLE_BUF_Pos                      30
#define USB_SIE_CTRL_EP0_DOUBLE_BUF_Msk                      ((uint32_t)0x40000000)
#define USB_SIE_CTRL_EP0_DOUBLE_BUF                          USB_SIE_CTRL_EP0_DOUBLE_BUF_Msk
#define USB_SIE_CTRL_EP0_INT_1BUF_Pos                        29
#define USB_SIE_CTRL_EP0_INT_1BUF_Msk                        ((uint32_t)0x20000000)
#define USB_SIE_CTRL_EP0_INT_1BUF                            USB_SIE_CTRL_EP0_INT_1BUF_Msk
#define USB_SIE_CTRL_EP0_INT_2BUF_Pos                        28
#define USB_SIE_CTRL_EP0_INT_2BUF_Msk                        ((uint32_t)0x10000000)
#define USB_SIE_CTRL_EP0_INT_2BUF                            USB_SIE_CTRL_EP0_INT_2BUF_Msk
#define USB_SIE_CTRL_EP0_INT_NAK_Pos                         27
#define USB_SIE_CTRL_EP0_INT_NAK_Msk                         ((uint32_t)0x08000000)
#define USB_SIE_CTRL_EP0_INT_NAK                             USB_SIE_CTRL_EP0_INT_NAK_Msk
#define USB_SIE_CTRL_DIRECT_EN_Pos                           26
#define USB_SIE_CTRL_DIRECT_EN_Msk                           ((uint32_t)0x04000000)
#define USB_SIE_CTRL_DIRECT_EN                               USB_SIE_CTRL_DIRECT_EN_Msk
#define USB_SIE_CTRL_DIRECT_DP_Pos                           25
#define USB_SIE_CTRL_DIRECT_DP_Msk                           ((uint32_t)0x02000000)
#define USB_SIE_CTRL_DIRECT_DP                               USB_SIE_CTRL_DIRECT_DP_Msk
#define USB_SIE_CTRL_DIRECT_DM_Pos                           24
#define USB_SIE_CTRL_DIRECT_DM_Msk                           ((uint32_t)0x01000000)
#define USB_SIE_CTRL_DIRECT_DM                               USB_SIE_CTRL_DIRECT_DM_Msk
#define USB_SIE_CTRL_TRANSCEIVER_PD_Pos                      18
#define USB_SIE_CTRL_TRANSCEIVER_PD_Msk                      ((uint32_t)0x00040000)
#define USB_SIE_CTRL_TRANSCEIVER_PD                          USB_SIE_CTRL_TRANSCEIVER_PD_Msk
#define USB_SIE_CTRL_RPU_OPT_Pos                             17
#define USB_SIE_CTRL_RPU_OPT_Msk                             ((uint32_t)0x00020000)
#define USB_SIE_CTRL_RPU_OPT                                 USB_SIE_CTRL_RPU_OPT_Msk
#define USB_SIE_CTRL_PULLUP_EN_Pos                           16
#define USB_SIE_CTRL_PULLUP_EN_Msk                           ((uint32_t)0x00010000)
#define USB_SIE_CTRL_PULLUP_EN                               USB_SIE_CTRL_PULLUP_EN_Msk
#define USB_SIE_CTRL_PULLDOWN_EN_Pos                         15
#define USB_SIE_CTRL_PULLDOWN_EN_Msk                         ((uint32_t)0x00008000)
#define USB_SIE_CTRL_PULLDOWN_EN                             USB_SIE_CTRL_PULLDOWN_EN_Msk
#define USB_SIE_CTRL_RESET_BUS_Pos                           13
#define USB_SIE_CTRL_RESET_BUS_Msk                           ((uint32_t)0x00002000)
#define USB_SIE_CTRL_RESET_BUS                               USB_SIE_CTRL_RESET_BUS_Msk
#define USB_SIE_CTRL_RESUME_Pos                              12
#define USB_SIE_CTRL_RESUME_Msk                              ((uint32_t)0x00001000)
#define USB_SIE_CTRL_RESUME                                  USB_SIE_CTRL_RESUME_Msk
#define USB_SIE_CTRL_VBUS_EN_Pos                             11
#define USB_SIE_CTRL_VBUS_EN_Msk                             ((uint32_t)0x00000800)
#define USB_SIE_CTRL_VBUS_EN                                 USB_SIE_CTRL_VBUS_EN_Msk
#define USB_SIE_CTRL_KEEP_ALIVE_EN_Pos                       10
#define USB_SIE_CTRL_KEEP_ALIVE_EN_Msk                       ((uint32_t)0x00000400)
#define USB_SIE_CTRL_KEEP_ALIVE_EN                           USB_SIE_CTRL_KEEP_ALIVE_EN_Msk
#define USB_SIE_CTRL_SOF_EN_Pos                              9
#define USB_SIE_CTRL_SOF_EN_Msk                              ((uint32_t)0x00000200)
#define USB_SIE_CTRL_SOF_EN                                  USB_SIE_CTRL_SOF_EN_Msk
#define USB_SIE_CTRL_SOF_SYNC_Pos                            8
#define USB_SIE_CTRL_SOF_SYNC_Msk                            ((uint32_t)0x00000100)
#define USB_SIE_CTRL_SOF_SYNC                                USB_SIE_CTRL_SOF_SYNC_Msk
#define USB_SIE_CTRL_PREAMBLE_EN_Pos                         6
#define USB_SIE_CTRL_PREAMBLE_EN_Msk                         ((uint32_t)0x00000040)
#define USB_SIE_CTRL_PREAMBLE_EN                             USB_SIE_CTRL_PREAMBLE_EN_Msk
#define USB_SIE_CTRL_STOP_TRANS_Pos                          4
#define USB_SIE_CTRL_STOP_TRANS_Msk                          ((uint32_t)0x00000010)
#define USB_SIE_CTRL_STOP_TRANS                              USB_SIE_CTRL_STOP_TRANS_Msk
#define USB_SIE_CTRL_RECEIVE_DATA_Pos                        3
#define USB_SIE_CTRL_RECEIVE_DATA_Msk                        ((uint32_t)0x00000008)
#define USB_SIE_CTRL_RECEIVE_DATA                            USB_SIE_CTRL_RECEIVE_DATA_Msk
#define USB_SIE_CTRL_SEND_DATA_Pos                           2
#define USB_SIE_CTRL_SEND_DATA_Msk                           ((uint32_t)0x00000004)
#define USB_SIE_CTRL_SEND_DATA                               USB_SIE_CTRL_SEND_DATA_Msk
#define USB_SIE_CTRL_SEND_SETUP_Pos                          1
#define USB_SIE_CTRL_SEND_SETUP_Msk                          ((uint32_t)0x00000002)
#define USB_SIE_CTRL_SEND_SETUP                              USB_SIE_CTRL_SEND_SETUP_Msk
#define USB_SIE_CTRL_START_TRANS_Pos                         0
#define USB_SIE_CTRL_START_TRANS_Msk                         ((uint32_t)0x00000001)
#define USB_SIE_CTRL_START_TRANS                             USB_SIE_CTRL_START_TRANS_Msk
// USB->SIE_STATUS
#define USB_SIE_STATUS_DATA_SEQ_ERROR_Pos                    31
#define USB_SIE_STATUS_DATA_SEQ_ERROR_Msk                    ((uint32_t)0x80000000)
#define USB_SIE_STATUS_DATA_SEQ_ERROR                        USB_SIE_STATUS_DATA_SEQ_ERROR_Msk
#define USB_SIE_STATUS_ACK_REC_Pos                           30
#define USB_SIE_STATUS_ACK_REC_Msk                           ((uint32_t)0x40000000)
#define USB_SIE_STATUS_ACK_REC                               USB_SIE_STATUS_ACK_REC_Msk
#define USB_SIE_STATUS_STALL_REC_Pos                         29
#define USB_SIE_STATUS_STALL_REC_Msk                         ((uint32_t)0x20000000)
#define USB_SIE_STATUS_STALL_REC                             USB_SIE_STATUS_STALL_REC_Msk
#define USB_SIE_STATUS_NAK_REC_Pos                           28
#define USB_SIE_STATUS_NAK_REC_Msk                           ((uint32_t)0x10000000)
#define USB_SIE_STATUS_NAK_REC                               USB_SIE_STATUS_NAK_REC_Msk
#define USB_SIE_STATUS_RX_TIMEOUT_Pos                        27
#define USB_SIE_STATUS_RX_TIMEOUT_Msk                        ((uint32_t)0x08000000)
#define USB_SIE_STATUS_RX_TIMEOUT                            USB_SIE_STATUS_RX_TIMEOUT_Msk
#define USB_SIE_STATUS_RX_OVERFLOW_Pos                       26
#define USB_SIE_STATUS_RX_OVERFLOW_Msk                       ((uint32_t)0x04000000)
#define USB_SIE_STATUS_RX_OVERFLOW                           USB_SIE_STATUS_RX_OVERFLOW_Msk
#define USB_SIE_STATUS_BIT_STUFF_ERROR_Pos                   25
#define USB_SIE_STATUS_BIT_STUFF_ERROR_Msk                   ((uint32_t)0x02000000)
#define USB_SIE_STATUS_BIT_STUFF_ERROR                       USB_SIE_STATUS_BIT_STUFF_ERROR_Msk
#define USB_SIE_STATUS_CRC_ERROR_Pos                         24
#define USB_SIE_STATUS_CRC_ERROR_Msk                         ((uint32_t)0x01000000)
#define USB_SIE_STATUS_CRC_ERROR                             USB_SIE_STATUS_CRC_ERROR_Msk
#define USB_SIE_STATUS_BUS_RESET_Pos                         19
#define USB_SIE_STATUS_BUS_RESET_Msk                         ((uint32_t)0x00080000)
#define USB_SIE_STATUS_BUS_RESET                             USB_SIE_STATUS_BUS_RESET_Msk
#define USB_SIE_STATUS_TRANS_COMPLETE_Pos                    18
#define USB_SIE_STATUS_TRANS_COMPLETE_Msk                    ((uint32_t)0x00040000)
#define USB_SIE_STATUS_TRANS_COMPLETE                        USB_SIE_STATUS_TRANS_COMPLETE_Msk
#define USB_SIE_STATUS_SETUP_REC_Pos                         17
#define USB_SIE_STATUS_SETUP_REC_Msk                         ((uint32_t)0x00020000)
#define USB_SIE_STATUS_SETUP_REC                             USB_SIE_STATUS_SETUP_REC_Msk
#define USB_SIE_STATUS_CONNECTED_Pos                         16
#define USB_SIE_STATUS_CONNECTED_Msk                         ((uint32_t)0x00010000)
#define USB_SIE_STATUS_CONNECTED                             USB_SIE_STATUS_CONNECTED_Msk
#define USB_SIE_STATUS_RESUME_Pos                            11
#define USB_SIE_STATUS_RESUME_Msk                            ((uint32_t)0x00000800)
#define USB_SIE_STATUS_RESUME                                USB_SIE_STATUS_RESUME_Msk
#define USB_SIE_STATUS_VBUS_OVER_CURR_Pos                    10
#define USB_SIE_STATUS_VBUS_OVER_CURR_Msk                    ((uint32_t)0x00000400)
#define USB_SIE_STATUS_VBUS_OVER_CURR                        USB_SIE_STATUS_VBUS_OVER_CURR_Msk
#define USB_SIE_STATUS_SPEED_Pos                             8
#define USB_SIE_STATUS_SPEED_Msk                             ((uint32_t)0x00000300)
#define USB_SIE_STATUS_SPEED                                 USB_SIE_STATUS_SPEED_Msk
#define USB_SIE_STATUS_SUSPENDED_Pos                         4
#define USB_SIE_STATUS_SUSPENDED_Msk                         ((uint32_t)0x00000010)
#define USB_SIE_STATUS_SUSPENDED                             USB_SIE_STATUS_SUSPENDED_Msk
#define USB_SIE_STATUS_LINE_STATE_Pos                        2
#define USB_SIE_STATUS_LINE_STATE_Msk                        ((uint32_t)0x0000000C)
#define USB_SIE_STATUS_LINE_STATE                            USB_SIE_STATUS_LINE_STATE_Msk
#define USB_SIE_STATUS_VBUS_DETECTED_Pos                     0
#define USB_SIE_STATUS_VBUS_DETECTED_Msk                     ((uint32_t)0x00000001)
#define USB_SIE_STATUS_VBUS_DETECTED                         USB_SIE_STATUS_VBUS_DETECTED_Msk
// USB->INT_EP_CTRL
#define USB_INT_EP_CTRL_INT_EP_ACTIVE_Pos                    1
#define USB_INT_EP_CTRL_INT_EP_ACTIVE_Msk                    ((uint32_t)0x0000FFFE)
#define USB_INT_EP_CTRL_INT_EP_ACTIVE                        USB_INT_EP_CTRL_INT_EP_ACTIVE_Msk
// USB->BUFF_STATUS
#define USB_BUFF_STATUS_EP15_OUT_Pos                         31
#define USB_BUFF_STATUS_EP15_OUT_Msk                         ((uint32_t)0x80000000)
#define USB_BUFF_STATUS_EP15_OUT                             USB_BUFF_STATUS_EP15_OUT_Msk
#define USB_BUFF_STATUS_EP15_IN_Pos                          30
#define USB_BUFF_STATUS_EP15_IN_Msk                          ((uint32_t)0x40000000)
#define USB_BUFF_STATUS_EP15_IN                              USB_BUFF_STATUS_EP15_IN_Msk
#define USB_BUFF_STATUS_EP14_OUT_Pos                         29
#define USB_BUFF_STATUS_EP14_OUT_Msk                         ((uint32_t)0x20000000)
#define USB_BUFF_STATUS_EP14_OUT                             USB_BUFF_STATUS_EP14_OUT_Msk
#define USB_BUFF_STATUS_EP14_IN_Pos                          28
#define USB_BUFF_STATUS_EP14_IN_Msk                          ((uint32_t)0x10000000)
#define USB_BUFF_STATUS_EP14_IN                              USB_BUFF_STATUS_EP14_IN_Msk
#define USB_BUFF_STATUS_EP13_OUT_Pos                         27
#define USB_BUFF_STATUS_EP13_OUT_Msk                         ((uint32_t)0x08000000)
#define USB_BUFF_STATUS_EP13_OUT                             USB_BUFF_STATUS_EP13_OUT_Msk
#define USB_BUFF_STATUS_EP13_IN_Pos                          26
#define USB_BUFF_STATUS_EP13_IN_Msk                          ((uint32_t)0x04000000)
#define USB_BUFF_STATUS_EP13_IN                              USB_BUFF_STATUS_EP13_IN_Msk
#define USB_BUFF_STATUS_EP12_OUT_Pos                         25
#define USB_BUFF_STATUS_EP12_OUT_Msk                         ((uint32_t)0x02000000)
#define USB_BUFF_STATUS_EP12_OUT                             USB_BUFF_STATUS_EP12_OUT_Msk
#define USB_BUFF_STATUS_EP12_IN_Pos                          24
#define USB_BUFF_STATUS_EP12_IN_Msk                          ((uint32_t)0x01000000)
#define USB_BUFF_STATUS_EP12_IN                              USB_BUFF_STATUS_EP12_IN_Msk
#define USB_BUFF_STATUS_EP11_OUT_Pos                         23
#define USB_BUFF_STATUS_EP11_OUT_Msk                         ((uint32_t)0x00800000)
#define USB_BUFF_STATUS_EP11_OUT                             USB_BUFF_STATUS_EP11_OUT_Msk
#define USB_BUFF_STATUS_EP11_IN_Pos                          22
#define USB_BUFF_STATUS_EP11_IN_Msk                          ((uint32_t)0x00400000)
#define USB_BUFF_STATUS_EP11_IN                              USB_BUFF_STATUS_EP11_IN_Msk
#define USB_BUFF_STATUS_EP10_OUT_Pos                         21
#define USB_BUFF_STATUS_EP10_OUT_Msk                         ((uint32_t)0x00200000)
#define USB_BUFF_STATUS_EP10_OUT                             USB_BUFF_STATUS_EP10_OUT_Msk
#define USB_BUFF_STATUS_EP10_IN_Pos                          20
#define USB_BUFF_STATUS_EP10_IN_Msk                          ((uint32_t)0x00100000)
#define USB_BUFF_STATUS_EP10_IN                              USB_BUFF_STATUS_EP10_IN_Msk
#define USB_BUFF_STATUS_EP9_OUT_Pos                          19
#define USB_BUFF_STATUS_EP9_OUT_Msk                          ((uint32_t)0x00080000)
#define USB_BUFF_STATUS_EP9_OUT                              USB_BUFF_STATUS_EP9_OUT_Msk
#define USB_BUFF_STATUS_EP9_IN_Pos                           18
#define USB_BUFF_STATUS_EP9_IN_Msk                           ((uint32_t)0x00040000)
#define USB_BUFF_STATUS_EP9_IN                               USB_BUFF_STATUS_EP9_IN_Msk
#define USB_BUFF_STATUS_EP8_OUT_Pos                          17
#define USB_BUFF_STATUS_EP8_OUT_Msk                          ((uint32_t)0x00020000)
#define USB_BUFF_STATUS_EP8_OUT                              USB_BUFF_STATUS_EP8_OUT_Msk
#define USB_BUFF_STATUS_EP8_IN_Pos                           16
#define USB_BUFF_STATUS_EP8_IN_Msk                           ((uint32_t)0x00010000)
#define USB_BUFF_STATUS_EP8_IN                               USB_BUFF_STATUS_EP8_IN_Msk
#define USB_BUFF_STATUS_EP7_OUT_Pos                          15
#define USB_BUFF_STATUS_EP7_OUT_Msk                          ((uint32_t)0x00008000)
#define USB_BUFF_STATUS_EP7_OUT                              USB_BUFF_STATUS_EP7_OUT_Msk
#define USB_BUFF_STATUS_EP7_IN_Pos                           14
#define USB_BUFF_STATUS_EP7_IN_Msk                           ((uint32_t)0x00004000)
#define USB_BUFF_STATUS_EP7_IN                               USB_BUFF_STATUS_EP7_IN_Msk
#define USB_BUFF_STATUS_EP6_OUT_Pos                          13
#define USB_BUFF_STATUS_EP6_OUT_Msk                          ((uint32_t)0x00002000)
#define USB_BUFF_STATUS_EP6_OUT                              USB_BUFF_STATUS_EP6_OUT_Msk
#define USB_BUFF_STATUS_EP6_IN_Pos                           12
#define USB_BUFF_STATUS_EP6_IN_Msk                           ((uint32_t)0x00001000)
#define USB_BUFF_STATUS_EP6_IN                               USB_BUFF_STATUS_EP6_IN_Msk
#define USB_BUFF_STATUS_EP5_OUT_Pos                          11
#define USB_BUFF_STATUS_EP5_OUT_Msk                          ((uint32_t)0x00000800)
#define USB_BUFF_STATUS_EP5_OUT                              USB_BUFF_STATUS_EP5_OUT_Msk
#define USB_BUFF_STATUS_EP5_IN_Pos                           10
#define USB_BUFF_STATUS_EP5_IN_Msk                           ((uint32_t)0x00000400)
#define USB_BUFF_STATUS_EP5_IN                               USB_BUFF_STATUS_EP5_IN_Msk
#define USB_BUFF_STATUS_EP4_OUT_Pos                          9
#define USB_BUFF_STATUS_EP4_OUT_Msk                          ((uint32_t)0x00000200)
#define USB_BUFF_STATUS_EP4_OUT                              USB_BUFF_STATUS_EP4_OUT_Msk
#define USB_BUFF_STATUS_EP4_IN_Pos                           8
#define USB_BUFF_STATUS_EP4_IN_Msk                           ((uint32_t)0x00000100)
#define USB_BUFF_STATUS_EP4_IN                               USB_BUFF_STATUS_EP4_IN_Msk
#define USB_BUFF_STATUS_EP3_OUT_Pos                          7
#define USB_BUFF_STATUS_EP3_OUT_Msk                          ((uint32_t)0x00000080)
#define USB_BUFF_STATUS_EP3_OUT                              USB_BUFF_STATUS_EP3_OUT_Msk
#define USB_BUFF_STATUS_EP3_IN_Pos                           6
#define USB_BUFF_STATUS_EP3_IN_Msk                           ((uint32_t)0x00000040)
#define USB_BUFF_STATUS_EP3_IN                               USB_BUFF_STATUS_EP3_IN_Msk
#define USB_BUFF_STATUS_EP2_OUT_Pos                          5
#define USB_BUFF_STATUS_EP2_OUT_Msk                          ((uint32_t)0x00000020)
#define USB_BUFF_STATUS_EP2_OUT                              USB_BUFF_STATUS_EP2_OUT_Msk
#define USB_BUFF_STATUS_EP2_IN_Pos                           4
#define USB_BUFF_STATUS_EP2_IN_Msk                           ((uint32_t)0x00000010)
#define USB_BUFF_STATUS_EP2_IN                               USB_BUFF_STATUS_EP2_IN_Msk
#define USB_BUFF_STATUS_EP1_OUT_Pos                          3
#define USB_BUFF_STATUS_EP1_OUT_Msk                          ((uint32_t)0x00000008)
#define USB_BUFF_STATUS_EP1_OUT                              USB_BUFF_STATUS_EP1_OUT_Msk
#define USB_BUFF_STATUS_EP1_IN_Pos                           2
#define USB_BUFF_STATUS_EP1_IN_Msk                           ((uint32_t)0x00000004)
#define USB_BUFF_STATUS_EP1_IN                               USB_BUFF_STATUS_EP1_IN_Msk
#define USB_BUFF_STATUS_EP0_OUT_Pos                          1
#define USB_BUFF_STATUS_EP0_OUT_Msk                          ((uint32_t)0x00000002)
#define USB_BUFF_STATUS_EP0_OUT                              USB_BUFF_STATUS_EP0_OUT_Msk
#define USB_BUFF_STATUS_EP0_IN_Pos                           0
#define USB_BUFF_STATUS_EP0_IN_Msk                           ((uint32_t)0x00000001)
#define USB_BUFF_STATUS_EP0_IN                               USB_BUFF_STATUS_EP0_IN_Msk
// USB->BUFF_CPU_SHOULD_HANDLE
#define USB_BUFF_CPU_SHOULD_HANDLE_EP15_OUT_Pos              31
#define USB_BUFF_CPU_SHOULD_HANDLE_EP15_OUT_Msk              ((uint32_t)0x80000000)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP15_OUT                  USB_BUFF_CPU_SHOULD_HANDLE_EP15_OUT_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP15_IN_Pos               30
#define USB_BUFF_CPU_SHOULD_HANDLE_EP15_IN_Msk               ((uint32_t)0x40000000)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP15_IN                   USB_BUFF_CPU_SHOULD_HANDLE_EP15_IN_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP14_OUT_Pos              29
#define USB_BUFF_CPU_SHOULD_HANDLE_EP14_OUT_Msk              ((uint32_t)0x20000000)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP14_OUT                  USB_BUFF_CPU_SHOULD_HANDLE_EP14_OUT_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP14_IN_Pos               28
#define USB_BUFF_CPU_SHOULD_HANDLE_EP14_IN_Msk               ((uint32_t)0x10000000)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP14_IN                   USB_BUFF_CPU_SHOULD_HANDLE_EP14_IN_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP13_OUT_Pos              27
#define USB_BUFF_CPU_SHOULD_HANDLE_EP13_OUT_Msk              ((uint32_t)0x08000000)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP13_OUT                  USB_BUFF_CPU_SHOULD_HANDLE_EP13_OUT_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP13_IN_Pos               26
#define USB_BUFF_CPU_SHOULD_HANDLE_EP13_IN_Msk               ((uint32_t)0x04000000)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP13_IN                   USB_BUFF_CPU_SHOULD_HANDLE_EP13_IN_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP12_OUT_Pos              25
#define USB_BUFF_CPU_SHOULD_HANDLE_EP12_OUT_Msk              ((uint32_t)0x02000000)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP12_OUT                  USB_BUFF_CPU_SHOULD_HANDLE_EP12_OUT_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP12_IN_Pos               24
#define USB_BUFF_CPU_SHOULD_HANDLE_EP12_IN_Msk               ((uint32_t)0x01000000)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP12_IN                   USB_BUFF_CPU_SHOULD_HANDLE_EP12_IN_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP11_OUT_Pos              23
#define USB_BUFF_CPU_SHOULD_HANDLE_EP11_OUT_Msk              ((uint32_t)0x00800000)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP11_OUT                  USB_BUFF_CPU_SHOULD_HANDLE_EP11_OUT_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP11_IN_Pos               22
#define USB_BUFF_CPU_SHOULD_HANDLE_EP11_IN_Msk               ((uint32_t)0x00400000)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP11_IN                   USB_BUFF_CPU_SHOULD_HANDLE_EP11_IN_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP10_OUT_Pos              21
#define USB_BUFF_CPU_SHOULD_HANDLE_EP10_OUT_Msk              ((uint32_t)0x00200000)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP10_OUT                  USB_BUFF_CPU_SHOULD_HANDLE_EP10_OUT_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP10_IN_Pos               20
#define USB_BUFF_CPU_SHOULD_HANDLE_EP10_IN_Msk               ((uint32_t)0x00100000)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP10_IN                   USB_BUFF_CPU_SHOULD_HANDLE_EP10_IN_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP9_OUT_Pos               19
#define USB_BUFF_CPU_SHOULD_HANDLE_EP9_OUT_Msk               ((uint32_t)0x00080000)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP9_OUT                   USB_BUFF_CPU_SHOULD_HANDLE_EP9_OUT_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP9_IN_Pos                18
#define USB_BUFF_CPU_SHOULD_HANDLE_EP9_IN_Msk                ((uint32_t)0x00040000)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP9_IN                    USB_BUFF_CPU_SHOULD_HANDLE_EP9_IN_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP8_OUT_Pos               17
#define USB_BUFF_CPU_SHOULD_HANDLE_EP8_OUT_Msk               ((uint32_t)0x00020000)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP8_OUT                   USB_BUFF_CPU_SHOULD_HANDLE_EP8_OUT_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP8_IN_Pos                16
#define USB_BUFF_CPU_SHOULD_HANDLE_EP8_IN_Msk                ((uint32_t)0x00010000)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP8_IN                    USB_BUFF_CPU_SHOULD_HANDLE_EP8_IN_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP7_OUT_Pos               15
#define USB_BUFF_CPU_SHOULD_HANDLE_EP7_OUT_Msk               ((uint32_t)0x00008000)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP7_OUT                   USB_BUFF_CPU_SHOULD_HANDLE_EP7_OUT_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP7_IN_Pos                14
#define USB_BUFF_CPU_SHOULD_HANDLE_EP7_IN_Msk                ((uint32_t)0x00004000)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP7_IN                    USB_BUFF_CPU_SHOULD_HANDLE_EP7_IN_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP6_OUT_Pos               13
#define USB_BUFF_CPU_SHOULD_HANDLE_EP6_OUT_Msk               ((uint32_t)0x00002000)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP6_OUT                   USB_BUFF_CPU_SHOULD_HANDLE_EP6_OUT_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP6_IN_Pos                12
#define USB_BUFF_CPU_SHOULD_HANDLE_EP6_IN_Msk                ((uint32_t)0x00001000)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP6_IN                    USB_BUFF_CPU_SHOULD_HANDLE_EP6_IN_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP5_OUT_Pos               11
#define USB_BUFF_CPU_SHOULD_HANDLE_EP5_OUT_Msk               ((uint32_t)0x00000800)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP5_OUT                   USB_BUFF_CPU_SHOULD_HANDLE_EP5_OUT_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP5_IN_Pos                10
#define USB_BUFF_CPU_SHOULD_HANDLE_EP5_IN_Msk                ((uint32_t)0x00000400)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP5_IN                    USB_BUFF_CPU_SHOULD_HANDLE_EP5_IN_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP4_OUT_Pos               9
#define USB_BUFF_CPU_SHOULD_HANDLE_EP4_OUT_Msk               ((uint32_t)0x00000200)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP4_OUT                   USB_BUFF_CPU_SHOULD_HANDLE_EP4_OUT_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP4_IN_Pos                8
#define USB_BUFF_CPU_SHOULD_HANDLE_EP4_IN_Msk                ((uint32_t)0x00000100)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP4_IN                    USB_BUFF_CPU_SHOULD_HANDLE_EP4_IN_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP3_OUT_Pos               7
#define USB_BUFF_CPU_SHOULD_HANDLE_EP3_OUT_Msk               ((uint32_t)0x00000080)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP3_OUT                   USB_BUFF_CPU_SHOULD_HANDLE_EP3_OUT_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP3_IN_Pos                6
#define USB_BUFF_CPU_SHOULD_HANDLE_EP3_IN_Msk                ((uint32_t)0x00000040)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP3_IN                    USB_BUFF_CPU_SHOULD_HANDLE_EP3_IN_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP2_OUT_Pos               5
#define USB_BUFF_CPU_SHOULD_HANDLE_EP2_OUT_Msk               ((uint32_t)0x00000020)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP2_OUT                   USB_BUFF_CPU_SHOULD_HANDLE_EP2_OUT_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP2_IN_Pos                4
#define USB_BUFF_CPU_SHOULD_HANDLE_EP2_IN_Msk                ((uint32_t)0x00000010)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP2_IN                    USB_BUFF_CPU_SHOULD_HANDLE_EP2_IN_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP1_OUT_Pos               3
#define USB_BUFF_CPU_SHOULD_HANDLE_EP1_OUT_Msk               ((uint32_t)0x00000008)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP1_OUT                   USB_BUFF_CPU_SHOULD_HANDLE_EP1_OUT_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP1_IN_Pos                2
#define USB_BUFF_CPU_SHOULD_HANDLE_EP1_IN_Msk                ((uint32_t)0x00000004)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP1_IN                    USB_BUFF_CPU_SHOULD_HANDLE_EP1_IN_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP0_OUT_Pos               1
#define USB_BUFF_CPU_SHOULD_HANDLE_EP0_OUT_Msk               ((uint32_t)0x00000002)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP0_OUT                   USB_BUFF_CPU_SHOULD_HANDLE_EP0_OUT_Msk
#define USB_BUFF_CPU_SHOULD_HANDLE_EP0_IN_Pos                0
#define USB_BUFF_CPU_SHOULD_HANDLE_EP0_IN_Msk                ((uint32_t)0x00000001)
#define USB_BUFF_CPU_SHOULD_HANDLE_EP0_IN                    USB_BUFF_CPU_SHOULD_HANDLE_EP0_IN_Msk
// USB->EP_ABORT
#define USB_EP_ABORT_EP15_OUT_Pos                            31
#define USB_EP_ABORT_EP15_OUT_Msk                            ((uint32_t)0x80000000)
#define USB_EP_ABORT_EP15_OUT                                USB_EP_ABORT_EP15_OUT_Msk
#define USB_EP_ABORT_EP15_IN_Pos                             30
#define USB_EP_ABORT_EP15_IN_Msk                             ((uint32_t)0x40000000)
#define USB_EP_ABORT_EP15_IN                                 USB_EP_ABORT_EP15_IN_Msk
#define USB_EP_ABORT_EP14_OUT_Pos                            29
#define USB_EP_ABORT_EP14_OUT_Msk                            ((uint32_t)0x20000000)
#define USB_EP_ABORT_EP14_OUT                                USB_EP_ABORT_EP14_OUT_Msk
#define USB_EP_ABORT_EP14_IN_Pos                             28
#define USB_EP_ABORT_EP14_IN_Msk                             ((uint32_t)0x10000000)
#define USB_EP_ABORT_EP14_IN                                 USB_EP_ABORT_EP14_IN_Msk
#define USB_EP_ABORT_EP13_OUT_Pos                            27
#define USB_EP_ABORT_EP13_OUT_Msk                            ((uint32_t)0x08000000)
#define USB_EP_ABORT_EP13_OUT                                USB_EP_ABORT_EP13_OUT_Msk
#define USB_EP_ABORT_EP13_IN_Pos                             26
#define USB_EP_ABORT_EP13_IN_Msk                             ((uint32_t)0x04000000)
#define USB_EP_ABORT_EP13_IN                                 USB_EP_ABORT_EP13_IN_Msk
#define USB_EP_ABORT_EP12_OUT_Pos                            25
#define USB_EP_ABORT_EP12_OUT_Msk                            ((uint32_t)0x02000000)
#define USB_EP_ABORT_EP12_OUT                                USB_EP_ABORT_EP12_OUT_Msk
#define USB_EP_ABORT_EP12_IN_Pos                             24
#define USB_EP_ABORT_EP12_IN_Msk                             ((uint32_t)0x01000000)
#define USB_EP_ABORT_EP12_IN                                 USB_EP_ABORT_EP12_IN_Msk
#define USB_EP_ABORT_EP11_OUT_Pos                            23
#define USB_EP_ABORT_EP11_OUT_Msk                            ((uint32_t)0x00800000)
#define USB_EP_ABORT_EP11_OUT                                USB_EP_ABORT_EP11_OUT_Msk
#define USB_EP_ABORT_EP11_IN_Pos                             22
#define USB_EP_ABORT_EP11_IN_Msk                             ((uint32_t)0x00400000)
#define USB_EP_ABORT_EP11_IN                                 USB_EP_ABORT_EP11_IN_Msk
#define USB_EP_ABORT_EP10_OUT_Pos                            21
#define USB_EP_ABORT_EP10_OUT_Msk                            ((uint32_t)0x00200000)
#define USB_EP_ABORT_EP10_OUT                                USB_EP_ABORT_EP10_OUT_Msk
#define USB_EP_ABORT_EP10_IN_Pos                             20
#define USB_EP_ABORT_EP10_IN_Msk                             ((uint32_t)0x00100000)
#define USB_EP_ABORT_EP10_IN                                 USB_EP_ABORT_EP10_IN_Msk
#define USB_EP_ABORT_EP9_OUT_Pos                             19
#define USB_EP_ABORT_EP9_OUT_Msk                             ((uint32_t)0x00080000)
#define USB_EP_ABORT_EP9_OUT                                 USB_EP_ABORT_EP9_OUT_Msk
#define USB_EP_ABORT_EP9_IN_Pos                              18
#define USB_EP_ABORT_EP9_IN_Msk                              ((uint32_t)0x00040000)
#define USB_EP_ABORT_EP9_IN                                  USB_EP_ABORT_EP9_IN_Msk
#define USB_EP_ABORT_EP8_OUT_Pos                             17
#define USB_EP_ABORT_EP8_OUT_Msk                             ((uint32_t)0x00020000)
#define USB_EP_ABORT_EP8_OUT                                 USB_EP_ABORT_EP8_OUT_Msk
#define USB_EP_ABORT_EP8_IN_Pos                              16
#define USB_EP_ABORT_EP8_IN_Msk                              ((uint32_t)0x00010000)
#define USB_EP_ABORT_EP8_IN                                  USB_EP_ABORT_EP8_IN_Msk
#define USB_EP_ABORT_EP7_OUT_Pos                             15
#define USB_EP_ABORT_EP7_OUT_Msk                             ((uint32_t)0x00008000)
#define USB_EP_ABORT_EP7_OUT                                 USB_EP_ABORT_EP7_OUT_Msk
#define USB_EP_ABORT_EP7_IN_Pos                              14
#define USB_EP_ABORT_EP7_IN_Msk                              ((uint32_t)0x00004000)
#define USB_EP_ABORT_EP7_IN                                  USB_EP_ABORT_EP7_IN_Msk
#define USB_EP_ABORT_EP6_OUT_Pos                             13
#define USB_EP_ABORT_EP6_OUT_Msk                             ((uint32_t)0x00002000)
#define USB_EP_ABORT_EP6_OUT                                 USB_EP_ABORT_EP6_OUT_Msk
#define USB_EP_ABORT_EP6_IN_Pos                              12
#define USB_EP_ABORT_EP6_IN_Msk                              ((uint32_t)0x00001000)
#define USB_EP_ABORT_EP6_IN                                  USB_EP_ABORT_EP6_IN_Msk
#define USB_EP_ABORT_EP5_OUT_Pos                             11
#define USB_EP_ABORT_EP5_OUT_Msk                             ((uint32_t)0x00000800)
#define USB_EP_ABORT_EP5_OUT                                 USB_EP_ABORT_EP5_OUT_Msk
#define USB_EP_ABORT_EP5_IN_Pos                              10
#define USB_EP_ABORT_EP5_IN_Msk                              ((uint32_t)0x00000400)
#define USB_EP_ABORT_EP5_IN                                  USB_EP_ABORT_EP5_IN_Msk
#define USB_EP_ABORT_EP4_OUT_Pos                             9
#define USB_EP_ABORT_EP4_OUT_Msk                             ((uint32_t)0x00000200)
#define USB_EP_ABORT_EP4_OUT                                 USB_EP_ABORT_EP4_OUT_Msk
#define USB_EP_ABORT_EP4_IN_Pos                              8
#define USB_EP_ABORT_EP4_IN_Msk                              ((uint32_t)0x00000100)
#define USB_EP_ABORT_EP4_IN                                  USB_EP_ABORT_EP4_IN_Msk
#define USB_EP_ABORT_EP3_OUT_Pos                             7
#define USB_EP_ABORT_EP3_OUT_Msk                             ((uint32_t)0x00000080)
#define USB_EP_ABORT_EP3_OUT                                 USB_EP_ABORT_EP3_OUT_Msk
#define USB_EP_ABORT_EP3_IN_Pos                              6
#define USB_EP_ABORT_EP3_IN_Msk                              ((uint32_t)0x00000040)
#define USB_EP_ABORT_EP3_IN                                  USB_EP_ABORT_EP3_IN_Msk
#define USB_EP_ABORT_EP2_OUT_Pos                             5
#define USB_EP_ABORT_EP2_OUT_Msk                             ((uint32_t)0x00000020)
#define USB_EP_ABORT_EP2_OUT                                 USB_EP_ABORT_EP2_OUT_Msk
#define USB_EP_ABORT_EP2_IN_Pos                              4
#define USB_EP_ABORT_EP2_IN_Msk                              ((uint32_t)0x00000010)
#define USB_EP_ABORT_EP2_IN                                  USB_EP_ABORT_EP2_IN_Msk
#define USB_EP_ABORT_EP1_OUT_Pos                             3
#define USB_EP_ABORT_EP1_OUT_Msk                             ((uint32_t)0x00000008)
#define USB_EP_ABORT_EP1_OUT                                 USB_EP_ABORT_EP1_OUT_Msk
#define USB_EP_ABORT_EP1_IN_Pos                              2
#define USB_EP_ABORT_EP1_IN_Msk                              ((uint32_t)0x00000004)
#define USB_EP_ABORT_EP1_IN                                  USB_EP_ABORT_EP1_IN_Msk
#define USB_EP_ABORT_EP0_OUT_Pos                             1
#define USB_EP_ABORT_EP0_OUT_Msk                             ((uint32_t)0x00000002)
#define USB_EP_ABORT_EP0_OUT                                 USB_EP_ABORT_EP0_OUT_Msk
#define USB_EP_ABORT_EP0_IN_Pos                              0
#define USB_EP_ABORT_EP0_IN_Msk                              ((uint32_t)0x00000001)
#define USB_EP_ABORT_EP0_IN                                  USB_EP_ABORT_EP0_IN_Msk
// USB->EP_ABORT_DONE
#define USB_EP_ABORT_DONE_EP15_OUT_Pos                       31
#define USB_EP_ABORT_DONE_EP15_OUT_Msk                       ((uint32_t)0x80000000)
#define USB_EP_ABORT_DONE_EP15_OUT                           USB_EP_ABORT_DONE_EP15_OUT_Msk
#define USB_EP_ABORT_DONE_EP15_IN_Pos                        30
#define USB_EP_ABORT_DONE_EP15_IN_Msk                        ((uint32_t)0x40000000)
#define USB_EP_ABORT_DONE_EP15_IN                            USB_EP_ABORT_DONE_EP15_IN_Msk
#define USB_EP_ABORT_DONE_EP14_OUT_Pos                       29
#define USB_EP_ABORT_DONE_EP14_OUT_Msk                       ((uint32_t)0x20000000)
#define USB_EP_ABORT_DONE_EP14_OUT                           USB_EP_ABORT_DONE_EP14_OUT_Msk
#define USB_EP_ABORT_DONE_EP14_IN_Pos                        28
#define USB_EP_ABORT_DONE_EP14_IN_Msk                        ((uint32_t)0x10000000)
#define USB_EP_ABORT_DONE_EP14_IN                            USB_EP_ABORT_DONE_EP14_IN_Msk
#define USB_EP_ABORT_DONE_EP13_OUT_Pos                       27
#define USB_EP_ABORT_DONE_EP13_OUT_Msk                       ((uint32_t)0x08000000)
#define USB_EP_ABORT_DONE_EP13_OUT                           USB_EP_ABORT_DONE_EP13_OUT_Msk
#define USB_EP_ABORT_DONE_EP13_IN_Pos                        26
#define USB_EP_ABORT_DONE_EP13_IN_Msk                        ((uint32_t)0x04000000)
#define USB_EP_ABORT_DONE_EP13_IN                            USB_EP_ABORT_DONE_EP13_IN_Msk
#define USB_EP_ABORT_DONE_EP12_OUT_Pos                       25
#define USB_EP_ABORT_DONE_EP12_OUT_Msk                       ((uint32_t)0x02000000)
#define USB_EP_ABORT_DONE_EP12_OUT                           USB_EP_ABORT_DONE_EP12_OUT_Msk
#define USB_EP_ABORT_DONE_EP12_IN_Pos                        24
#define USB_EP_ABORT_DONE_EP12_IN_Msk                        ((uint32_t)0x01000000)
#define USB_EP_ABORT_DONE_EP12_IN                            USB_EP_ABORT_DONE_EP12_IN_Msk
#define USB_EP_ABORT_DONE_EP11_OUT_Pos                       23
#define USB_EP_ABORT_DONE_EP11_OUT_Msk                       ((uint32_t)0x00800000)
#define USB_EP_ABORT_DONE_EP11_OUT                           USB_EP_ABORT_DONE_EP11_OUT_Msk
#define USB_EP_ABORT_DONE_EP11_IN_Pos                        22
#define USB_EP_ABORT_DONE_EP11_IN_Msk                        ((uint32_t)0x00400000)
#define USB_EP_ABORT_DONE_EP11_IN                            USB_EP_ABORT_DONE_EP11_IN_Msk
#define USB_EP_ABORT_DONE_EP10_OUT_Pos                       21
#define USB_EP_ABORT_DONE_EP10_OUT_Msk                       ((uint32_t)0x00200000)
#define USB_EP_ABORT_DONE_EP10_OUT                           USB_EP_ABORT_DONE_EP10_OUT_Msk
#define USB_EP_ABORT_DONE_EP10_IN_Pos                        20
#define USB_EP_ABORT_DONE_EP10_IN_Msk                        ((uint32_t)0x00100000)
#define USB_EP_ABORT_DONE_EP10_IN                            USB_EP_ABORT_DONE_EP10_IN_Msk
#define USB_EP_ABORT_DONE_EP9_OUT_Pos                        19
#define USB_EP_ABORT_DONE_EP9_OUT_Msk                        ((uint32_t)0x00080000)
#define USB_EP_ABORT_DONE_EP9_OUT                            USB_EP_ABORT_DONE_EP9_OUT_Msk
#define USB_EP_ABORT_DONE_EP9_IN_Pos                         18
#define USB_EP_ABORT_DONE_EP9_IN_Msk                         ((uint32_t)0x00040000)
#define USB_EP_ABORT_DONE_EP9_IN                             USB_EP_ABORT_DONE_EP9_IN_Msk
#define USB_EP_ABORT_DONE_EP8_OUT_Pos                        17
#define USB_EP_ABORT_DONE_EP8_OUT_Msk                        ((uint32_t)0x00020000)
#define USB_EP_ABORT_DONE_EP8_OUT                            USB_EP_ABORT_DONE_EP8_OUT_Msk
#define USB_EP_ABORT_DONE_EP8_IN_Pos                         16
#define USB_EP_ABORT_DONE_EP8_IN_Msk                         ((uint32_t)0x00010000)
#define USB_EP_ABORT_DONE_EP8_IN                             USB_EP_ABORT_DONE_EP8_IN_Msk
#define USB_EP_ABORT_DONE_EP7_OUT_Pos                        15
#define USB_EP_ABORT_DONE_EP7_OUT_Msk                        ((uint32_t)0x00008000)
#define USB_EP_ABORT_DONE_EP7_OUT                            USB_EP_ABORT_DONE_EP7_OUT_Msk
#define USB_EP_ABORT_DONE_EP7_IN_Pos                         14
#define USB_EP_ABORT_DONE_EP7_IN_Msk                         ((uint32_t)0x00004000)
#define USB_EP_ABORT_DONE_EP7_IN                             USB_EP_ABORT_DONE_EP7_IN_Msk
#define USB_EP_ABORT_DONE_EP6_OUT_Pos                        13
#define USB_EP_ABORT_DONE_EP6_OUT_Msk                        ((uint32_t)0x00002000)
#define USB_EP_ABORT_DONE_EP6_OUT                            USB_EP_ABORT_DONE_EP6_OUT_Msk
#define USB_EP_ABORT_DONE_EP6_IN_Pos                         12
#define USB_EP_ABORT_DONE_EP6_IN_Msk                         ((uint32_t)0x00001000)
#define USB_EP_ABORT_DONE_EP6_IN                             USB_EP_ABORT_DONE_EP6_IN_Msk
#define USB_EP_ABORT_DONE_EP5_OUT_Pos                        11
#define USB_EP_ABORT_DONE_EP5_OUT_Msk                        ((uint32_t)0x00000800)
#define USB_EP_ABORT_DONE_EP5_OUT                            USB_EP_ABORT_DONE_EP5_OUT_Msk
#define USB_EP_ABORT_DONE_EP5_IN_Pos                         10
#define USB_EP_ABORT_DONE_EP5_IN_Msk                         ((uint32_t)0x00000400)
#define USB_EP_ABORT_DONE_EP5_IN                             USB_EP_ABORT_DONE_EP5_IN_Msk
#define USB_EP_ABORT_DONE_EP4_OUT_Pos                        9
#define USB_EP_ABORT_DONE_EP4_OUT_Msk                        ((uint32_t)0x00000200)
#define USB_EP_ABORT_DONE_EP4_OUT                            USB_EP_ABORT_DONE_EP4_OUT_Msk
#define USB_EP_ABORT_DONE_EP4_IN_Pos                         8
#define USB_EP_ABORT_DONE_EP4_IN_Msk                         ((uint32_t)0x00000100)
#define USB_EP_ABORT_DONE_EP4_IN                             USB_EP_ABORT_DONE_EP4_IN_Msk
#define USB_EP_ABORT_DONE_EP3_OUT_Pos                        7
#define USB_EP_ABORT_DONE_EP3_OUT_Msk                        ((uint32_t)0x00000080)
#define USB_EP_ABORT_DONE_EP3_OUT                            USB_EP_ABORT_DONE_EP3_OUT_Msk
#define USB_EP_ABORT_DONE_EP3_IN_Pos                         6
#define USB_EP_ABORT_DONE_EP3_IN_Msk                         ((uint32_t)0x00000040)
#define USB_EP_ABORT_DONE_EP3_IN                             USB_EP_ABORT_DONE_EP3_IN_Msk
#define USB_EP_ABORT_DONE_EP2_OUT_Pos                        5
#define USB_EP_ABORT_DONE_EP2_OUT_Msk                        ((uint32_t)0x00000020)
#define USB_EP_ABORT_DONE_EP2_OUT                            USB_EP_ABORT_DONE_EP2_OUT_Msk
#define USB_EP_ABORT_DONE_EP2_IN_Pos                         4
#define USB_EP_ABORT_DONE_EP2_IN_Msk                         ((uint32_t)0x00000010)
#define USB_EP_ABORT_DONE_EP2_IN                             USB_EP_ABORT_DONE_EP2_IN_Msk
#define USB_EP_ABORT_DONE_EP1_OUT_Pos                        3
#define USB_EP_ABORT_DONE_EP1_OUT_Msk                        ((uint32_t)0x00000008)
#define USB_EP_ABORT_DONE_EP1_OUT                            USB_EP_ABORT_DONE_EP1_OUT_Msk
#define USB_EP_ABORT_DONE_EP1_IN_Pos                         2
#define USB_EP_ABORT_DONE_EP1_IN_Msk                         ((uint32_t)0x00000004)
#define USB_EP_ABORT_DONE_EP1_IN                             USB_EP_ABORT_DONE_EP1_IN_Msk
#define USB_EP_ABORT_DONE_EP0_OUT_Pos                        1
#define USB_EP_ABORT_DONE_EP0_OUT_Msk                        ((uint32_t)0x00000002)
#define USB_EP_ABORT_DONE_EP0_OUT                            USB_EP_ABORT_DONE_EP0_OUT_Msk
#define USB_EP_ABORT_DONE_EP0_IN_Pos                         0
#define USB_EP_ABORT_DONE_EP0_IN_Msk                         ((uint32_t)0x00000001)
#define USB_EP_ABORT_DONE_EP0_IN                             USB_EP_ABORT_DONE_EP0_IN_Msk
// USB->EP_STALL_ARM
#define USB_EP_STALL_ARM_EP0_OUT_Pos                         1
#define USB_EP_STALL_ARM_EP0_OUT_Msk                         ((uint32_t)0x00000002)
#define USB_EP_STALL_ARM_EP0_OUT                             USB_EP_STALL_ARM_EP0_OUT_Msk
#define USB_EP_STALL_ARM_EP0_IN_Pos                          0
#define USB_EP_STALL_ARM_EP0_IN_Msk                          ((uint32_t)0x00000001)
#define USB_EP_STALL_ARM_EP0_IN                              USB_EP_STALL_ARM_EP0_IN_Msk
// USB->NAK_POLL
#define USB_NAK_POLL_DELAY_FS_Pos                            16
#define USB_NAK_POLL_DELAY_FS_Msk                            ((uint32_t)0x03FF0000)
#define USB_NAK_POLL_DELAY_FS                                USB_NAK_POLL_DELAY_FS_Msk
#define USB_NAK_POLL_DELAY_LS_Pos                            0
#define USB_NAK_POLL_DELAY_LS_Msk                            ((uint32_t)0x000003FF)
#define USB_NAK_POLL_DELAY_LS                                USB_NAK_POLL_DELAY_LS_Msk
// USB->EP_STATUS_STALL_NAK
#define USB_EP_STATUS_STALL_NAK_EP15_OUT_Pos                 31
#define USB_EP_STATUS_STALL_NAK_EP15_OUT_Msk                 ((uint32_t)0x80000000)
#define USB_EP_STATUS_STALL_NAK_EP15_OUT                     USB_EP_STATUS_STALL_NAK_EP15_OUT_Msk
#define USB_EP_STATUS_STALL_NAK_EP15_IN_Pos                  30
#define USB_EP_STATUS_STALL_NAK_EP15_IN_Msk                  ((uint32_t)0x40000000)
#define USB_EP_STATUS_STALL_NAK_EP15_IN                      USB_EP_STATUS_STALL_NAK_EP15_IN_Msk
#define USB_EP_STATUS_STALL_NAK_EP14_OUT_Pos                 29
#define USB_EP_STATUS_STALL_NAK_EP14_OUT_Msk                 ((uint32_t)0x20000000)
#define USB_EP_STATUS_STALL_NAK_EP14_OUT                     USB_EP_STATUS_STALL_NAK_EP14_OUT_Msk
#define USB_EP_STATUS_STALL_NAK_EP14_IN_Pos                  28
#define USB_EP_STATUS_STALL_NAK_EP14_IN_Msk                  ((uint32_t)0x10000000)
#define USB_EP_STATUS_STALL_NAK_EP14_IN                      USB_EP_STATUS_STALL_NAK_EP14_IN_Msk
#define USB_EP_STATUS_STALL_NAK_EP13_OUT_Pos                 27
#define USB_EP_STATUS_STALL_NAK_EP13_OUT_Msk                 ((uint32_t)0x08000000)
#define USB_EP_STATUS_STALL_NAK_EP13_OUT                     USB_EP_STATUS_STALL_NAK_EP13_OUT_Msk
#define USB_EP_STATUS_STALL_NAK_EP13_IN_Pos                  26
#define USB_EP_STATUS_STALL_NAK_EP13_IN_Msk                  ((uint32_t)0x04000000)
#define USB_EP_STATUS_STALL_NAK_EP13_IN                      USB_EP_STATUS_STALL_NAK_EP13_IN_Msk
#define USB_EP_STATUS_STALL_NAK_EP12_OUT_Pos                 25
#define USB_EP_STATUS_STALL_NAK_EP12_OUT_Msk                 ((uint32_t)0x02000000)
#define USB_EP_STATUS_STALL_NAK_EP12_OUT                     USB_EP_STATUS_STALL_NAK_EP12_OUT_Msk
#define USB_EP_STATUS_STALL_NAK_EP12_IN_Pos                  24
#define USB_EP_STATUS_STALL_NAK_EP12_IN_Msk                  ((uint32_t)0x01000000)
#define USB_EP_STATUS_STALL_NAK_EP12_IN                      USB_EP_STATUS_STALL_NAK_EP12_IN_Msk
#define USB_EP_STATUS_STALL_NAK_EP11_OUT_Pos                 23
#define USB_EP_STATUS_STALL_NAK_EP11_OUT_Msk                 ((uint32_t)0x00800000)
#define USB_EP_STATUS_STALL_NAK_EP11_OUT                     USB_EP_STATUS_STALL_NAK_EP11_OUT_Msk
#define USB_EP_STATUS_STALL_NAK_EP11_IN_Pos                  22
#define USB_EP_STATUS_STALL_NAK_EP11_IN_Msk                  ((uint32_t)0x00400000)
#define USB_EP_STATUS_STALL_NAK_EP11_IN                      USB_EP_STATUS_STALL_NAK_EP11_IN_Msk
#define USB_EP_STATUS_STALL_NAK_EP10_OUT_Pos                 21
#define USB_EP_STATUS_STALL_NAK_EP10_OUT_Msk                 ((uint32_t)0x00200000)
#define USB_EP_STATUS_STALL_NAK_EP10_OUT                     USB_EP_STATUS_STALL_NAK_EP10_OUT_Msk
#define USB_EP_STATUS_STALL_NAK_EP10_IN_Pos                  20
#define USB_EP_STATUS_STALL_NAK_EP10_IN_Msk                  ((uint32_t)0x00100000)
#define USB_EP_STATUS_STALL_NAK_EP10_IN                      USB_EP_STATUS_STALL_NAK_EP10_IN_Msk
#define USB_EP_STATUS_STALL_NAK_EP9_OUT_Pos                  19
#define USB_EP_STATUS_STALL_NAK_EP9_OUT_Msk                  ((uint32_t)0x00080000)
#define USB_EP_STATUS_STALL_NAK_EP9_OUT                      USB_EP_STATUS_STALL_NAK_EP9_OUT_Msk
#define USB_EP_STATUS_STALL_NAK_EP9_IN_Pos                   18
#define USB_EP_STATUS_STALL_NAK_EP9_IN_Msk                   ((uint32_t)0x00040000)
#define USB_EP_STATUS_STALL_NAK_EP9_IN                       USB_EP_STATUS_STALL_NAK_EP9_IN_Msk
#define USB_EP_STATUS_STALL_NAK_EP8_OUT_Pos                  17
#define USB_EP_STATUS_STALL_NAK_EP8_OUT_Msk                  ((uint32_t)0x00020000)
#define USB_EP_STATUS_STALL_NAK_EP8_OUT                      USB_EP_STATUS_STALL_NAK_EP8_OUT_Msk
#define USB_EP_STATUS_STALL_NAK_EP8_IN_Pos                   16
#define USB_EP_STATUS_STALL_NAK_EP8_IN_Msk                   ((uint32_t)0x00010000)
#define USB_EP_STATUS_STALL_NAK_EP8_IN                       USB_EP_STATUS_STALL_NAK_EP8_IN_Msk
#define USB_EP_STATUS_STALL_NAK_EP7_OUT_Pos                  15
#define USB_EP_STATUS_STALL_NAK_EP7_OUT_Msk                  ((uint32_t)0x00008000)
#define USB_EP_STATUS_STALL_NAK_EP7_OUT                      USB_EP_STATUS_STALL_NAK_EP7_OUT_Msk
#define USB_EP_STATUS_STALL_NAK_EP7_IN_Pos                   14
#define USB_EP_STATUS_STALL_NAK_EP7_IN_Msk                   ((uint32_t)0x00004000)
#define USB_EP_STATUS_STALL_NAK_EP7_IN                       USB_EP_STATUS_STALL_NAK_EP7_IN_Msk
#define USB_EP_STATUS_STALL_NAK_EP6_OUT_Pos                  13
#define USB_EP_STATUS_STALL_NAK_EP6_OUT_Msk                  ((uint32_t)0x00002000)
#define USB_EP_STATUS_STALL_NAK_EP6_OUT                      USB_EP_STATUS_STALL_NAK_EP6_OUT_Msk
#define USB_EP_STATUS_STALL_NAK_EP6_IN_Pos                   12
#define USB_EP_STATUS_STALL_NAK_EP6_IN_Msk                   ((uint32_t)0x00001000)
#define USB_EP_STATUS_STALL_NAK_EP6_IN                       USB_EP_STATUS_STALL_NAK_EP6_IN_Msk
#define USB_EP_STATUS_STALL_NAK_EP5_OUT_Pos                  11
#define USB_EP_STATUS_STALL_NAK_EP5_OUT_Msk                  ((uint32_t)0x00000800)
#define USB_EP_STATUS_STALL_NAK_EP5_OUT                      USB_EP_STATUS_STALL_NAK_EP5_OUT_Msk
#define USB_EP_STATUS_STALL_NAK_EP5_IN_Pos                   10
#define USB_EP_STATUS_STALL_NAK_EP5_IN_Msk                   ((uint32_t)0x00000400)
#define USB_EP_STATUS_STALL_NAK_EP5_IN                       USB_EP_STATUS_STALL_NAK_EP5_IN_Msk
#define USB_EP_STATUS_STALL_NAK_EP4_OUT_Pos                  9
#define USB_EP_STATUS_STALL_NAK_EP4_OUT_Msk                  ((uint32_t)0x00000200)
#define USB_EP_STATUS_STALL_NAK_EP4_OUT                      USB_EP_STATUS_STALL_NAK_EP4_OUT_Msk
#define USB_EP_STATUS_STALL_NAK_EP4_IN_Pos                   8
#define USB_EP_STATUS_STALL_NAK_EP4_IN_Msk                   ((uint32_t)0x00000100)
#define USB_EP_STATUS_STALL_NAK_EP4_IN                       USB_EP_STATUS_STALL_NAK_EP4_IN_Msk
#define USB_EP_STATUS_STALL_NAK_EP3_OUT_Pos                  7
#define USB_EP_STATUS_STALL_NAK_EP3_OUT_Msk                  ((uint32_t)0x00000080)
#define USB_EP_STATUS_STALL_NAK_EP3_OUT                      USB_EP_STATUS_STALL_NAK_EP3_OUT_Msk
#define USB_EP_STATUS_STALL_NAK_EP3_IN_Pos                   6
#define USB_EP_STATUS_STALL_NAK_EP3_IN_Msk                   ((uint32_t)0x00000040)
#define USB_EP_STATUS_STALL_NAK_EP3_IN                       USB_EP_STATUS_STALL_NAK_EP3_IN_Msk
#define USB_EP_STATUS_STALL_NAK_EP2_OUT_Pos                  5
#define USB_EP_STATUS_STALL_NAK_EP2_OUT_Msk                  ((uint32_t)0x00000020)
#define USB_EP_STATUS_STALL_NAK_EP2_OUT                      USB_EP_STATUS_STALL_NAK_EP2_OUT_Msk
#define USB_EP_STATUS_STALL_NAK_EP2_IN_Pos                   4
#define USB_EP_STATUS_STALL_NAK_EP2_IN_Msk                   ((uint32_t)0x00000010)
#define USB_EP_STATUS_STALL_NAK_EP2_IN                       USB_EP_STATUS_STALL_NAK_EP2_IN_Msk
#define USB_EP_STATUS_STALL_NAK_EP1_OUT_Pos                  3
#define USB_EP_STATUS_STALL_NAK_EP1_OUT_Msk                  ((uint32_t)0x00000008)
#define USB_EP_STATUS_STALL_NAK_EP1_OUT                      USB_EP_STATUS_STALL_NAK_EP1_OUT_Msk
#define USB_EP_STATUS_STALL_NAK_EP1_IN_Pos                   2
#define USB_EP_STATUS_STALL_NAK_EP1_IN_Msk                   ((uint32_t)0x00000004)
#define USB_EP_STATUS_STALL_NAK_EP1_IN                       USB_EP_STATUS_STALL_NAK_EP1_IN_Msk
#define USB_EP_STATUS_STALL_NAK_EP0_OUT_Pos                  1
#define USB_EP_STATUS_STALL_NAK_EP0_OUT_Msk                  ((uint32_t)0x00000002)
#define USB_EP_STATUS_STALL_NAK_EP0_OUT                      USB_EP_STATUS_STALL_NAK_EP0_OUT_Msk
#define USB_EP_STATUS_STALL_NAK_EP0_IN_Pos                   0
#define USB_EP_STATUS_STALL_NAK_EP0_IN_Msk                   ((uint32_t)0x00000001)
#define USB_EP_STATUS_STALL_NAK_EP0_IN                       USB_EP_STATUS_STALL_NAK_EP0_IN_Msk
// USB->USB_MUXING
#define USB_USB_MUXING_SOFTCON_Pos                           3
#define USB_USB_MUXING_SOFTCON_Msk                           ((uint32_t)0x00000008)
#define USB_USB_MUXING_SOFTCON                               USB_USB_MUXING_SOFTCON_Msk
#define USB_USB_MUXING_TO_DIGITAL_PAD_Pos                    2
#define USB_USB_MUXING_TO_DIGITAL_PAD_Msk                    ((uint32_t)0x00000004)
#define USB_USB_MUXING_TO_DIGITAL_PAD                        USB_USB_MUXING_TO_DIGITAL_PAD_Msk
#define USB_USB_MUXING_TO_EXTPHY_Pos                         1
#define USB_USB_MUXING_TO_EXTPHY_Msk                         ((uint32_t)0x00000002)
#define USB_USB_MUXING_TO_EXTPHY                             USB_USB_MUXING_TO_EXTPHY_Msk
#define USB_USB_MUXING_TO_PHY_Pos                            0
#define USB_USB_MUXING_TO_PHY_Msk                            ((uint32_t)0x00000001)
#define USB_USB_MUXING_TO_PHY                                USB_USB_MUXING_TO_PHY_Msk
// USB->USB_PWR
#define USB_USB_PWR_OVERCURR_DETECT_EN_Pos                   5
#define USB_USB_PWR_OVERCURR_DETECT_EN_Msk                   ((uint32_t)0x00000020)
#define USB_USB_PWR_OVERCURR_DETECT_EN                       USB_USB_PWR_OVERCURR_DETECT_EN_Msk
#define USB_USB_PWR_OVERCURR_DETECT_Pos                      4
#define USB_USB_PWR_OVERCURR_DETECT_Msk                      ((uint32_t)0x00000010)
#define USB_USB_PWR_OVERCURR_DETECT                          USB_USB_PWR_OVERCURR_DETECT_Msk
#define USB_USB_PWR_VBUS_DETECT_OVERRIDE_EN_Pos              3
#define USB_USB_PWR_VBUS_DETECT_OVERRIDE_EN_Msk              ((uint32_t)0x00000008)
#define USB_USB_PWR_VBUS_DETECT_OVERRIDE_EN                  USB_USB_PWR_VBUS_DETECT_OVERRIDE_EN_Msk
#define USB_USB_PWR_VBUS_DETECT_Pos                          2
#define USB_USB_PWR_VBUS_DETECT_Msk                          ((uint32_t)0x00000004)
#define USB_USB_PWR_VBUS_DETECT                              USB_USB_PWR_VBUS_DETECT_Msk
#define USB_USB_PWR_VBUS_EN_OVERRIDE_EN_Pos                  1
#define USB_USB_PWR_VBUS_EN_OVERRIDE_EN_Msk                  ((uint32_t)0x00000002)
#define USB_USB_PWR_VBUS_EN_OVERRIDE_EN                      USB_USB_PWR_VBUS_EN_OVERRIDE_EN_Msk
#define USB_USB_PWR_VBUS_EN_Pos                              0
#define USB_USB_PWR_VBUS_EN_Msk                              ((uint32_t)0x00000001)
#define USB_USB_PWR_VBUS_EN                                  USB_USB_PWR_VBUS_EN_Msk
// USB->USBPHY_DIRECT
#define USB_USBPHY_DIRECT_DM_OVV_Pos                         22
#define USB_USBPHY_DIRECT_DM_OVV_Msk                         ((uint32_t)0x00400000)
#define USB_USBPHY_DIRECT_DM_OVV                             USB_USBPHY_DIRECT_DM_OVV_Msk
#define USB_USBPHY_DIRECT_DP_OVV_Pos                         21
#define USB_USBPHY_DIRECT_DP_OVV_Msk                         ((uint32_t)0x00200000)
#define USB_USBPHY_DIRECT_DP_OVV                             USB_USBPHY_DIRECT_DP_OVV_Msk
#define USB_USBPHY_DIRECT_DM_OVCN_Pos                        20
#define USB_USBPHY_DIRECT_DM_OVCN_Msk                        ((uint32_t)0x00100000)
#define USB_USBPHY_DIRECT_DM_OVCN                            USB_USBPHY_DIRECT_DM_OVCN_Msk
#define USB_USBPHY_DIRECT_DP_OVCN_Pos                        19
#define USB_USBPHY_DIRECT_DP_OVCN_Msk                        ((uint32_t)0x00080000)
#define USB_USBPHY_DIRECT_DP_OVCN                            USB_USBPHY_DIRECT_DP_OVCN_Msk
#define USB_USBPHY_DIRECT_RX_DM_Pos                          18
#define USB_USBPHY_DIRECT_RX_DM_Msk                          ((uint32_t)0x00040000)
#define USB_USBPHY_DIRECT_RX_DM                              USB_USBPHY_DIRECT_RX_DM_Msk
#define USB_USBPHY_DIRECT_RX_DP_Pos                          17
#define USB_USBPHY_DIRECT_RX_DP_Msk                          ((uint32_t)0x00020000)
#define USB_USBPHY_DIRECT_RX_DP                              USB_USBPHY_DIRECT_RX_DP_Msk
#define USB_USBPHY_DIRECT_RX_DD_Pos                          16
#define USB_USBPHY_DIRECT_RX_DD_Msk                          ((uint32_t)0x00010000)
#define USB_USBPHY_DIRECT_RX_DD                              USB_USBPHY_DIRECT_RX_DD_Msk
#define USB_USBPHY_DIRECT_TX_DIFFMODE_Pos                    15
#define USB_USBPHY_DIRECT_TX_DIFFMODE_Msk                    ((uint32_t)0x00008000)
#define USB_USBPHY_DIRECT_TX_DIFFMODE                        USB_USBPHY_DIRECT_TX_DIFFMODE_Msk
#define USB_USBPHY_DIRECT_TX_FSSLEW_Pos                      14
#define USB_USBPHY_DIRECT_TX_FSSLEW_Msk                      ((uint32_t)0x00004000)
#define USB_USBPHY_DIRECT_TX_FSSLEW                          USB_USBPHY_DIRECT_TX_FSSLEW_Msk
#define USB_USBPHY_DIRECT_TX_PD_Pos                          13
#define USB_USBPHY_DIRECT_TX_PD_Msk                          ((uint32_t)0x00002000)
#define USB_USBPHY_DIRECT_TX_PD                              USB_USBPHY_DIRECT_TX_PD_Msk
#define USB_USBPHY_DIRECT_RX_PD_Pos                          12
#define USB_USBPHY_DIRECT_RX_PD_Msk                          ((uint32_t)0x00001000)
#define USB_USBPHY_DIRECT_RX_PD                              USB_USBPHY_DIRECT_RX_PD_Msk
#define USB_USBPHY_DIRECT_TX_DM_Pos                          11
#define USB_USBPHY_DIRECT_TX_DM_Msk                          ((uint32_t)0x00000800)
#define USB_USBPHY_DIRECT_TX_DM                              USB_USBPHY_DIRECT_TX_DM_Msk
#define USB_USBPHY_DIRECT_TX_DP_Pos                          10
#define USB_USBPHY_DIRECT_TX_DP_Msk                          ((uint32_t)0x00000400)
#define USB_USBPHY_DIRECT_TX_DP                              USB_USBPHY_DIRECT_TX_DP_Msk
#define USB_USBPHY_DIRECT_TX_DM_OE_Pos                       9
#define USB_USBPHY_DIRECT_TX_DM_OE_Msk                       ((uint32_t)0x00000200)
#define USB_USBPHY_DIRECT_TX_DM_OE                           USB_USBPHY_DIRECT_TX_DM_OE_Msk
#define USB_USBPHY_DIRECT_TX_DP_OE_Pos                       8
#define USB_USBPHY_DIRECT_TX_DP_OE_Msk                       ((uint32_t)0x00000100)
#define USB_USBPHY_DIRECT_TX_DP_OE                           USB_USBPHY_DIRECT_TX_DP_OE_Msk
#define USB_USBPHY_DIRECT_DM_PULLDN_EN_Pos                   6
#define USB_USBPHY_DIRECT_DM_PULLDN_EN_Msk                   ((uint32_t)0x00000040)
#define USB_USBPHY_DIRECT_DM_PULLDN_EN                       USB_USBPHY_DIRECT_DM_PULLDN_EN_Msk
#define USB_USBPHY_DIRECT_DM_PULLUP_EN_Pos                   5
#define USB_USBPHY_DIRECT_DM_PULLUP_EN_Msk                   ((uint32_t)0x00000020)
#define USB_USBPHY_DIRECT_DM_PULLUP_EN                       USB_USBPHY_DIRECT_DM_PULLUP_EN_Msk
#define USB_USBPHY_DIRECT_DM_PULLUP_HISEL_Pos                4
#define USB_USBPHY_DIRECT_DM_PULLUP_HISEL_Msk                ((uint32_t)0x00000010)
#define USB_USBPHY_DIRECT_DM_PULLUP_HISEL                    USB_USBPHY_DIRECT_DM_PULLUP_HISEL_Msk
#define USB_USBPHY_DIRECT_DP_PULLDN_EN_Pos                   2
#define USB_USBPHY_DIRECT_DP_PULLDN_EN_Msk                   ((uint32_t)0x00000004)
#define USB_USBPHY_DIRECT_DP_PULLDN_EN                       USB_USBPHY_DIRECT_DP_PULLDN_EN_Msk
#define USB_USBPHY_DIRECT_DP_PULLUP_EN_Pos                   1
#define USB_USBPHY_DIRECT_DP_PULLUP_EN_Msk                   ((uint32_t)0x00000002)
#define USB_USBPHY_DIRECT_DP_PULLUP_EN                       USB_USBPHY_DIRECT_DP_PULLUP_EN_Msk
#define USB_USBPHY_DIRECT_DP_PULLUP_HISEL_Pos                0
#define USB_USBPHY_DIRECT_DP_PULLUP_HISEL_Msk                ((uint32_t)0x00000001)
#define USB_USBPHY_DIRECT_DP_PULLUP_HISEL                    USB_USBPHY_DIRECT_DP_PULLUP_HISEL_Msk
// USB->USBPHY_DIRECT_OVERRIDE
#define USB_USBPHY_DIRECT_OVERRIDE_TX_DIFFMODE_OVERRIDE_EN_Pos 15
#define USB_USBPHY_DIRECT_OVERRIDE_TX_DIFFMODE_OVERRIDE_EN_Msk ((uint32_t)0x00008000)
#define USB_USBPHY_DIRECT_OVERRIDE_TX_DIFFMODE_OVERRIDE_EN   USB_USBPHY_DIRECT_OVERRIDE_TX_DIFFMODE_OVERRIDE_EN_Msk
#define USB_USBPHY_DIRECT_OVERRIDE_DM_PULLUP_OVERRIDE_EN_Pos 12
#define USB_USBPHY_DIRECT_OVERRIDE_DM_PULLUP_OVERRIDE_EN_Msk ((uint32_t)0x00001000)
#define USB_USBPHY_DIRECT_OVERRIDE_DM_PULLUP_OVERRIDE_EN     USB_USBPHY_DIRECT_OVERRIDE_DM_PULLUP_OVERRIDE_EN_Msk
#define USB_USBPHY_DIRECT_OVERRIDE_TX_FSSLEW_OVERRIDE_EN_Pos 11
#define USB_USBPHY_DIRECT_OVERRIDE_TX_FSSLEW_OVERRIDE_EN_Msk ((uint32_t)0x00000800)
#define USB_USBPHY_DIRECT_OVERRIDE_TX_FSSLEW_OVERRIDE_EN     USB_USBPHY_DIRECT_OVERRIDE_TX_FSSLEW_OVERRIDE_EN_Msk
#define USB_USBPHY_DIRECT_OVERRIDE_TX_PD_OVERRIDE_EN_Pos     10
#define USB_USBPHY_DIRECT_OVERRIDE_TX_PD_OVERRIDE_EN_Msk     ((uint32_t)0x00000400)
#define USB_USBPHY_DIRECT_OVERRIDE_TX_PD_OVERRIDE_EN         USB_USBPHY_DIRECT_OVERRIDE_TX_PD_OVERRIDE_EN_Msk
#define USB_USBPHY_DIRECT_OVERRIDE_RX_PD_OVERRIDE_EN_Pos     9
#define USB_USBPHY_DIRECT_OVERRIDE_RX_PD_OVERRIDE_EN_Msk     ((uint32_t)0x00000200)
#define USB_USBPHY_DIRECT_OVERRIDE_RX_PD_OVERRIDE_EN         USB_USBPHY_DIRECT_OVERRIDE_RX_PD_OVERRIDE_EN_Msk
#define USB_USBPHY_DIRECT_OVERRIDE_TX_DM_OVERRIDE_EN_Pos     8
#define USB_USBPHY_DIRECT_OVERRIDE_TX_DM_OVERRIDE_EN_Msk     ((uint32_t)0x00000100)
#define USB_USBPHY_DIRECT_OVERRIDE_TX_DM_OVERRIDE_EN         USB_USBPHY_DIRECT_OVERRIDE_TX_DM_OVERRIDE_EN_Msk
#define USB_USBPHY_DIRECT_OVERRIDE_TX_DP_OVERRIDE_EN_Pos     7
#define USB_USBPHY_DIRECT_OVERRIDE_TX_DP_OVERRIDE_EN_Msk     ((uint32_t)0x00000080)
#define USB_USBPHY_DIRECT_OVERRIDE_TX_DP_OVERRIDE_EN         USB_USBPHY_DIRECT_OVERRIDE_TX_DP_OVERRIDE_EN_Msk
#define USB_USBPHY_DIRECT_OVERRIDE_TX_DM_OE_OVERRIDE_EN_Pos  6
#define USB_USBPHY_DIRECT_OVERRIDE_TX_DM_OE_OVERRIDE_EN_Msk  ((uint32_t)0x00000040)
#define USB_USBPHY_DIRECT_OVERRIDE_TX_DM_OE_OVERRIDE_EN      USB_USBPHY_DIRECT_OVERRIDE_TX_DM_OE_OVERRIDE_EN_Msk
#define USB_USBPHY_DIRECT_OVERRIDE_TX_DP_OE_OVERRIDE_EN_Pos  5
#define USB_USBPHY_DIRECT_OVERRIDE_TX_DP_OE_OVERRIDE_EN_Msk  ((uint32_t)0x00000020)
#define USB_USBPHY_DIRECT_OVERRIDE_TX_DP_OE_OVERRIDE_EN      USB_USBPHY_DIRECT_OVERRIDE_TX_DP_OE_OVERRIDE_EN_Msk
#define USB_USBPHY_DIRECT_OVERRIDE_DM_PULLDN_EN_OVERRIDE_EN_Pos 4
#define USB_USBPHY_DIRECT_OVERRIDE_DM_PULLDN_EN_OVERRIDE_EN_Msk ((uint32_t)0x00000010)
#define USB_USBPHY_DIRECT_OVERRIDE_DM_PULLDN_EN_OVERRIDE_EN  USB_USBPHY_DIRECT_OVERRIDE_DM_PULLDN_EN_OVERRIDE_EN_Msk
#define USB_USBPHY_DIRECT_OVERRIDE_DP_PULLDN_EN_OVERRIDE_EN_Pos 3
#define USB_USBPHY_DIRECT_OVERRIDE_DP_PULLDN_EN_OVERRIDE_EN_Msk ((uint32_t)0x00000008)
#define USB_USBPHY_DIRECT_OVERRIDE_DP_PULLDN_EN_OVERRIDE_EN  USB_USBPHY_DIRECT_OVERRIDE_DP_PULLDN_EN_OVERRIDE_EN_Msk
#define USB_USBPHY_DIRECT_OVERRIDE_DP_PULLUP_EN_OVERRIDE_EN_Pos 2
#define USB_USBPHY_DIRECT_OVERRIDE_DP_PULLUP_EN_OVERRIDE_EN_Msk ((uint32_t)0x00000004)
#define USB_USBPHY_DIRECT_OVERRIDE_DP_PULLUP_EN_OVERRIDE_EN  USB_USBPHY_DIRECT_OVERRIDE_DP_PULLUP_EN_OVERRIDE_EN_Msk
#define USB_USBPHY_DIRECT_OVERRIDE_DM_PULLUP_HISEL_OVERRIDE_EN_Pos 1
#define USB_USBPHY_DIRECT_OVERRIDE_DM_PULLUP_HISEL_OVERRIDE_EN_Msk ((uint32_t)0x00000002)
#define USB_USBPHY_DIRECT_OVERRIDE_DM_PULLUP_HISEL_OVERRIDE_EN USB_USBPHY_DIRECT_OVERRIDE_DM_PULLUP_HISEL_OVERRIDE_EN_Msk
#define USB_USBPHY_DIRECT_OVERRIDE_DP_PULLUP_HISEL_OVERRIDE_EN_Pos 0
#define USB_USBPHY_DIRECT_OVERRIDE_DP_PULLUP_HISEL_OVERRIDE_EN_Msk ((uint32_t)0x00000001)
#define USB_USBPHY_DIRECT_OVERRIDE_DP_PULLUP_HISEL_OVERRIDE_EN USB_USBPHY_DIRECT_OVERRIDE_DP_PULLUP_HISEL_OVERRIDE_EN_Msk
// USB->USBPHY_TRIM
#define USB_USBPHY_TRIM_DM_PULLDN_TRIM_Pos                   8
#define USB_USBPHY_TRIM_DM_PULLDN_TRIM_Msk                   ((uint32_t)0x00001F00)
#define USB_USBPHY_TRIM_DM_PULLDN_TRIM                       USB_USBPHY_TRIM_DM_PULLDN_TRIM_Msk
#define USB_USBPHY_TRIM_DP_PULLDN_TRIM_Pos                   0
#define USB_USBPHY_TRIM_DP_PULLDN_TRIM_Msk                   ((uint32_t)0x0000001F)
#define USB_USBPHY_TRIM_DP_PULLDN_TRIM                       USB_USBPHY_TRIM_DP_PULLDN_TRIM_Msk
// USB->INTR
#define USB_INTR_EP_STALL_NAK_Pos                            19
#define USB_INTR_EP_STALL_NAK_Msk                            ((uint32_t)0x00080000)
#define USB_INTR_EP_STALL_NAK                                USB_INTR_EP_STALL_NAK_Msk
#define USB_INTR_ABORT_DONE_Pos                              18
#define USB_INTR_ABORT_DONE_Msk                              ((uint32_t)0x00040000)
#define USB_INTR_ABORT_DONE                                  USB_INTR_ABORT_DONE_Msk
#define USB_INTR_DEV_SOF_Pos                                 17
#define USB_INTR_DEV_SOF_Msk                                 ((uint32_t)0x00020000)
#define USB_INTR_DEV_SOF                                     USB_INTR_DEV_SOF_Msk
#define USB_INTR_SETUP_REQ_Pos                               16
#define USB_INTR_SETUP_REQ_Msk                               ((uint32_t)0x00010000)
#define USB_INTR_SETUP_REQ                                   USB_INTR_SETUP_REQ_Msk
#define USB_INTR_DEV_RESUME_FROM_HOST_Pos                    15
#define USB_INTR_DEV_RESUME_FROM_HOST_Msk                    ((uint32_t)0x00008000)
#define USB_INTR_DEV_RESUME_FROM_HOST                        USB_INTR_DEV_RESUME_FROM_HOST_Msk
#define USB_INTR_DEV_SUSPEND_Pos                             14
#define USB_INTR_DEV_SUSPEND_Msk                             ((uint32_t)0x00004000)
#define USB_INTR_DEV_SUSPEND                                 USB_INTR_DEV_SUSPEND_Msk
#define USB_INTR_DEV_CONN_DIS_Pos                            13
#define USB_INTR_DEV_CONN_DIS_Msk                            ((uint32_t)0x00002000)
#define USB_INTR_DEV_CONN_DIS                                USB_INTR_DEV_CONN_DIS_Msk
#define USB_INTR_BUS_RESET_Pos                               12
#define USB_INTR_BUS_RESET_Msk                               ((uint32_t)0x00001000)
#define USB_INTR_BUS_RESET                                   USB_INTR_BUS_RESET_Msk
#define USB_INTR_VBUS_DETECT_Pos                             11
#define USB_INTR_VBUS_DETECT_Msk                             ((uint32_t)0x00000800)
#define USB_INTR_VBUS_DETECT                                 USB_INTR_VBUS_DETECT_Msk
#define USB_INTR_STALL_Pos                                   10
#define USB_INTR_STALL_Msk                                   ((uint32_t)0x00000400)
#define USB_INTR_STALL                                       USB_INTR_STALL_Msk
#define USB_INTR_ERROR_CRC_Pos                               9
#define USB_INTR_ERROR_CRC_Msk                               ((uint32_t)0x00000200)
#define USB_INTR_ERROR_CRC                                   USB_INTR_ERROR_CRC_Msk
#define USB_INTR_ERROR_BIT_STUFF_Pos                         8
#define USB_INTR_ERROR_BIT_STUFF_Msk                         ((uint32_t)0x00000100)
#define USB_INTR_ERROR_BIT_STUFF                             USB_INTR_ERROR_BIT_STUFF_Msk
#define USB_INTR_ERROR_RX_OVERFLOW_Pos                       7
#define USB_INTR_ERROR_RX_OVERFLOW_Msk                       ((uint32_t)0x00000080)
#define USB_INTR_ERROR_RX_OVERFLOW                           USB_INTR_ERROR_RX_OVERFLOW_Msk
#define USB_INTR_ERROR_RX_TIMEOUT_Pos                        6
#define USB_INTR_ERROR_RX_TIMEOUT_Msk                        ((uint32_t)0x00000040)
#define USB_INTR_ERROR_RX_TIMEOUT                            USB_INTR_ERROR_RX_TIMEOUT_Msk
#define USB_INTR_ERROR_DATA_SEQ_Pos                          5
#define USB_INTR_ERROR_DATA_SEQ_Msk                          ((uint32_t)0x00000020)
#define USB_INTR_ERROR_DATA_SEQ                              USB_INTR_ERROR_DATA_SEQ_Msk
#define USB_INTR_BUFF_STATUS_Pos                             4
#define USB_INTR_BUFF_STATUS_Msk                             ((uint32_t)0x00000010)
#define USB_INTR_BUFF_STATUS                                 USB_INTR_BUFF_STATUS_Msk
#define USB_INTR_TRANS_COMPLETE_Pos                          3
#define USB_INTR_TRANS_COMPLETE_Msk                          ((uint32_t)0x00000008)
#define USB_INTR_TRANS_COMPLETE                              USB_INTR_TRANS_COMPLETE_Msk
#define USB_INTR_HOST_SOF_Pos                                2
#define USB_INTR_HOST_SOF_Msk                                ((uint32_t)0x00000004)
#define USB_INTR_HOST_SOF                                    USB_INTR_HOST_SOF_Msk
#define USB_INTR_HOST_RESUME_Pos                             1
#define USB_INTR_HOST_RESUME_Msk                             ((uint32_t)0x00000002)
#define USB_INTR_HOST_RESUME                                 USB_INTR_HOST_RESUME_Msk
#define USB_INTR_HOST_CONN_DIS_Pos                           0
#define USB_INTR_HOST_CONN_DIS_Msk                           ((uint32_t)0x00000001)
#define USB_INTR_HOST_CONN_DIS                               USB_INTR_HOST_CONN_DIS_Msk
// USB->INTE
#define USB_INTE_EP_STALL_NAK_Pos                            19
#define USB_INTE_EP_STALL_NAK_Msk                            ((uint32_t)0x00080000)
#define USB_INTE_EP_STALL_NAK                                USB_INTE_EP_STALL_NAK_Msk
#define USB_INTE_ABORT_DONE_Pos                              18
#define USB_INTE_ABORT_DONE_Msk                              ((uint32_t)0x00040000)
#define USB_INTE_ABORT_DONE                                  USB_INTE_ABORT_DONE_Msk
#define USB_INTE_DEV_SOF_Pos                                 17
#define USB_INTE_DEV_SOF_Msk                                 ((uint32_t)0x00020000)
#define USB_INTE_DEV_SOF                                     USB_INTE_DEV_SOF_Msk
#define USB_INTE_SETUP_REQ_Pos                               16
#define USB_INTE_SETUP_REQ_Msk                               ((uint32_t)0x00010000)
#define USB_INTE_SETUP_REQ                                   USB_INTE_SETUP_REQ_Msk
#define USB_INTE_DEV_RESUME_FROM_HOST_Pos                    15
#define USB_INTE_DEV_RESUME_FROM_HOST_Msk                    ((uint32_t)0x00008000)
#define USB_INTE_DEV_RESUME_FROM_HOST                        USB_INTE_DEV_RESUME_FROM_HOST_Msk
#define USB_INTE_DEV_SUSPEND_Pos                             14
#define USB_INTE_DEV_SUSPEND_Msk                             ((uint32_t)0x00004000)
#define USB_INTE_DEV_SUSPEND                                 USB_INTE_DEV_SUSPEND_Msk
#define USB_INTE_DEV_CONN_DIS_Pos                            13
#define USB_INTE_DEV_CONN_DIS_Msk                            ((uint32_t)0x00002000)
#define USB_INTE_DEV_CONN_DIS                                USB_INTE_DEV_CONN_DIS_Msk
#define USB_INTE_BUS_RESET_Pos                               12
#define USB_INTE_BUS_RESET_Msk                               ((uint32_t)0x00001000)
#define USB_INTE_BUS_RESET                                   USB_INTE_BUS_RESET_Msk
#define USB_INTE_VBUS_DETECT_Pos                             11
#define USB_INTE_VBUS_DETECT_Msk                             ((uint32_t)0x00000800)
#define USB_INTE_VBUS_DETECT                                 USB_INTE_VBUS_DETECT_Msk
#define USB_INTE_STALL_Pos                                   10
#define USB_INTE_STALL_Msk                                   ((uint32_t)0x00000400)
#define USB_INTE_STALL                                       USB_INTE_STALL_Msk
#define USB_INTE_ERROR_CRC_Pos                               9
#define USB_INTE_ERROR_CRC_Msk                               ((uint32_t)0x00000200)
#define USB_INTE_ERROR_CRC                                   USB_INTE_ERROR_CRC_Msk
#define USB_INTE_ERROR_BIT_STUFF_Pos                         8
#define USB_INTE_ERROR_BIT_STUFF_Msk                         ((uint32_t)0x00000100)
#define USB_INTE_ERROR_BIT_STUFF                             USB_INTE_ERROR_BIT_STUFF_Msk
#define USB_INTE_ERROR_RX_OVERFLOW_Pos                       7
#define USB_INTE_ERROR_RX_OVERFLOW_Msk                       ((uint32_t)0x00000080)
#define USB_INTE_ERROR_RX_OVERFLOW                           USB_INTE_ERROR_RX_OVERFLOW_Msk
#define USB_INTE_ERROR_RX_TIMEOUT_Pos                        6
#define USB_INTE_ERROR_RX_TIMEOUT_Msk                        ((uint32_t)0x00000040)
#define USB_INTE_ERROR_RX_TIMEOUT                            USB_INTE_ERROR_RX_TIMEOUT_Msk
#define USB_INTE_ERROR_DATA_SEQ_Pos                          5
#define USB_INTE_ERROR_DATA_SEQ_Msk                          ((uint32_t)0x00000020)
#define USB_INTE_ERROR_DATA_SEQ                              USB_INTE_ERROR_DATA_SEQ_Msk
#define USB_INTE_BUFF_STATUS_Pos                             4
#define USB_INTE_BUFF_STATUS_Msk                             ((uint32_t)0x00000010)
#define USB_INTE_BUFF_STATUS                                 USB_INTE_BUFF_STATUS_Msk
#define USB_INTE_TRANS_COMPLETE_Pos                          3
#define USB_INTE_TRANS_COMPLETE_Msk                          ((uint32_t)0x00000008)
#define USB_INTE_TRANS_COMPLETE                              USB_INTE_TRANS_COMPLETE_Msk
#define USB_INTE_HOST_SOF_Pos                                2
#define USB_INTE_HOST_SOF_Msk                                ((uint32_t)0x00000004)
#define USB_INTE_HOST_SOF                                    USB_INTE_HOST_SOF_Msk
#define USB_INTE_HOST_RESUME_Pos                             1
#define USB_INTE_HOST_RESUME_Msk                             ((uint32_t)0x00000002)
#define USB_INTE_HOST_RESUME                                 USB_INTE_HOST_RESUME_Msk
#define USB_INTE_HOST_CONN_DIS_Pos                           0
#define USB_INTE_HOST_CONN_DIS_Msk                           ((uint32_t)0x00000001)
#define USB_INTE_HOST_CONN_DIS                               USB_INTE_HOST_CONN_DIS_Msk
// USB->INTF
#define USB_INTF_EP_STALL_NAK_Pos                            19
#define USB_INTF_EP_STALL_NAK_Msk                            ((uint32_t)0x00080000)
#define USB_INTF_EP_STALL_NAK                                USB_INTF_EP_STALL_NAK_Msk
#define USB_INTF_ABORT_DONE_Pos                              18
#define USB_INTF_ABORT_DONE_Msk                              ((uint32_t)0x00040000)
#define USB_INTF_ABORT_DONE                                  USB_INTF_ABORT_DONE_Msk
#define USB_INTF_DEV_SOF_Pos                                 17
#define USB_INTF_DEV_SOF_Msk                                 ((uint32_t)0x00020000)
#define USB_INTF_DEV_SOF                                     USB_INTF_DEV_SOF_Msk
#define USB_INTF_SETUP_REQ_Pos                               16
#define USB_INTF_SETUP_REQ_Msk                               ((uint32_t)0x00010000)
#define USB_INTF_SETUP_REQ                                   USB_INTF_SETUP_REQ_Msk
#define USB_INTF_DEV_RESUME_FROM_HOST_Pos                    15
#define USB_INTF_DEV_RESUME_FROM_HOST_Msk                    ((uint32_t)0x00008000)
#define USB_INTF_DEV_RESUME_FROM_HOST                        USB_INTF_DEV_RESUME_FROM_HOST_Msk
#define USB_INTF_DEV_SUSPEND_Pos                             14
#define USB_INTF_DEV_SUSPEND_Msk                             ((uint32_t)0x00004000)
#define USB_INTF_DEV_SUSPEND                                 USB_INTF_DEV_SUSPEND_Msk
#define USB_INTF_DEV_CONN_DIS_Pos                            13
#define USB_INTF_DEV_CONN_DIS_Msk                            ((uint32_t)0x00002000)
#define USB_INTF_DEV_CONN_DIS                                USB_INTF_DEV_CONN_DIS_Msk
#define USB_INTF_BUS_RESET_Pos                               12
#define USB_INTF_BUS_RESET_Msk                               ((uint32_t)0x00001000)
#define USB_INTF_BUS_RESET                                   USB_INTF_BUS_RESET_Msk
#define USB_INTF_VBUS_DETECT_Pos                             11
#define USB_INTF_VBUS_DETECT_Msk                             ((uint32_t)0x00000800)
#define USB_INTF_VBUS_DETECT                                 USB_INTF_VBUS_DETECT_Msk
#define USB_INTF_STALL_Pos                                   10
#define USB_INTF_STALL_Msk                                   ((uint32_t)0x00000400)
#define USB_INTF_STALL                                       USB_INTF_STALL_Msk
#define USB_INTF_ERROR_CRC_Pos                               9
#define USB_INTF_ERROR_CRC_Msk                               ((uint32_t)0x00000200)
#define USB_INTF_ERROR_CRC                                   USB_INTF_ERROR_CRC_Msk
#define USB_INTF_ERROR_BIT_STUFF_Pos                         8
#define USB_INTF_ERROR_BIT_STUFF_Msk                         ((uint32_t)0x00000100)
#define USB_INTF_ERROR_BIT_STUFF                             USB_INTF_ERROR_BIT_STUFF_Msk
#define USB_INTF_ERROR_RX_OVERFLOW_Pos                       7
#define USB_INTF_ERROR_RX_OVERFLOW_Msk                       ((uint32_t)0x00000080)
#define USB_INTF_ERROR_RX_OVERFLOW                           USB_INTF_ERROR_RX_OVERFLOW_Msk
#define USB_INTF_ERROR_RX_TIMEOUT_Pos                        6
#define USB_INTF_ERROR_RX_TIMEOUT_Msk                        ((uint32_t)0x00000040)
#define USB_INTF_ERROR_RX_TIMEOUT                            USB_INTF_ERROR_RX_TIMEOUT_Msk
#define USB_INTF_ERROR_DATA_SEQ_Pos                          5
#define USB_INTF_ERROR_DATA_SEQ_Msk                          ((uint32_t)0x00000020)
#define USB_INTF_ERROR_DATA_SEQ                              USB_INTF_ERROR_DATA_SEQ_Msk
#define USB_INTF_BUFF_STATUS_Pos                             4
#define USB_INTF_BUFF_STATUS_Msk                             ((uint32_t)0x00000010)
#define USB_INTF_BUFF_STATUS                                 USB_INTF_BUFF_STATUS_Msk
#define USB_INTF_TRANS_COMPLETE_Pos                          3
#define USB_INTF_TRANS_COMPLETE_Msk                          ((uint32_t)0x00000008)
#define USB_INTF_TRANS_COMPLETE                              USB_INTF_TRANS_COMPLETE_Msk
#define USB_INTF_HOST_SOF_Pos                                2
#define USB_INTF_HOST_SOF_Msk                                ((uint32_t)0x00000004)
#define USB_INTF_HOST_SOF                                    USB_INTF_HOST_SOF_Msk
#define USB_INTF_HOST_RESUME_Pos                             1
#define USB_INTF_HOST_RESUME_Msk                             ((uint32_t)0x00000002)
#define USB_INTF_HOST_RESUME                                 USB_INTF_HOST_RESUME_Msk
#define USB_INTF_HOST_CONN_DIS_Pos                           0
#define USB_INTF_HOST_CONN_DIS_Msk                           ((uint32_t)0x00000001)
#define USB_INTF_HOST_CONN_DIS                               USB_INTF_HOST_CONN_DIS_Msk
// USB->INTS
#define USB_INTS_EP_STALL_NAK_Pos                            19
#define USB_INTS_EP_STALL_NAK_Msk                            ((uint32_t)0x00080000)
#define USB_INTS_EP_STALL_NAK                                USB_INTS_EP_STALL_NAK_Msk
#define USB_INTS_ABORT_DONE_Pos                              18
#define USB_INTS_ABORT_DONE_Msk                              ((uint32_t)0x00040000)
#define USB_INTS_ABORT_DONE                                  USB_INTS_ABORT_DONE_Msk
#define USB_INTS_DEV_SOF_Pos                                 17
#define USB_INTS_DEV_SOF_Msk                                 ((uint32_t)0x00020000)
#define USB_INTS_DEV_SOF                                     USB_INTS_DEV_SOF_Msk
#define USB_INTS_SETUP_REQ_Pos                               16
#define USB_INTS_SETUP_REQ_Msk                               ((uint32_t)0x00010000)
#define USB_INTS_SETUP_REQ                                   USB_INTS_SETUP_REQ_Msk
#define USB_INTS_DEV_RESUME_FROM_HOST_Pos                    15
#define USB_INTS_DEV_RESUME_FROM_HOST_Msk                    ((uint32_t)0x00008000)
#define USB_INTS_DEV_RESUME_FROM_HOST                        USB_INTS_DEV_RESUME_FROM_HOST_Msk
#define USB_INTS_DEV_SUSPEND_Pos                             14
#define USB_INTS_DEV_SUSPEND_Msk                             ((uint32_t)0x00004000)
#define USB_INTS_DEV_SUSPEND                                 USB_INTS_DEV_SUSPEND_Msk
#define USB_INTS_DEV_CONN_DIS_Pos                            13
#define USB_INTS_DEV_CONN_DIS_Msk                            ((uint32_t)0x00002000)
#define USB_INTS_DEV_CONN_DIS                                USB_INTS_DEV_CONN_DIS_Msk
#define USB_INTS_BUS_RESET_Pos                               12
#define USB_INTS_BUS_RESET_Msk                               ((uint32_t)0x00001000)
#define USB_INTS_BUS_RESET                                   USB_INTS_BUS_RESET_Msk
#define USB_INTS_VBUS_DETECT_Pos                             11
#define USB_INTS_VBUS_DETECT_Msk                             ((uint32_t)0x00000800)
#define USB_INTS_VBUS_DETECT                                 USB_INTS_VBUS_DETECT_Msk
#define USB_INTS_STALL_Pos                                   10
#define USB_INTS_STALL_Msk                                   ((uint32_t)0x00000400)
#define USB_INTS_STALL                                       USB_INTS_STALL_Msk
#define USB_INTS_ERROR_CRC_Pos                               9
#define USB_INTS_ERROR_CRC_Msk                               ((uint32_t)0x00000200)
#define USB_INTS_ERROR_CRC                                   USB_INTS_ERROR_CRC_Msk
#define USB_INTS_ERROR_BIT_STUFF_Pos                         8
#define USB_INTS_ERROR_BIT_STUFF_Msk                         ((uint32_t)0x00000100)
#define USB_INTS_ERROR_BIT_STUFF                             USB_INTS_ERROR_BIT_STUFF_Msk
#define USB_INTS_ERROR_RX_OVERFLOW_Pos                       7
#define USB_INTS_ERROR_RX_OVERFLOW_Msk                       ((uint32_t)0x00000080)
#define USB_INTS_ERROR_RX_OVERFLOW                           USB_INTS_ERROR_RX_OVERFLOW_Msk
#define USB_INTS_ERROR_RX_TIMEOUT_Pos                        6
#define USB_INTS_ERROR_RX_TIMEOUT_Msk                        ((uint32_t)0x00000040)
#define USB_INTS_ERROR_RX_TIMEOUT                            USB_INTS_ERROR_RX_TIMEOUT_Msk
#define USB_INTS_ERROR_DATA_SEQ_Pos                          5
#define USB_INTS_ERROR_DATA_SEQ_Msk                          ((uint32_t)0x00000020)
#define USB_INTS_ERROR_DATA_SEQ                              USB_INTS_ERROR_DATA_SEQ_Msk
#define USB_INTS_BUFF_STATUS_Pos                             4
#define USB_INTS_BUFF_STATUS_Msk                             ((uint32_t)0x00000010)
#define USB_INTS_BUFF_STATUS                                 USB_INTS_BUFF_STATUS_Msk
#define USB_INTS_TRANS_COMPLETE_Pos                          3
#define USB_INTS_TRANS_COMPLETE_Msk                          ((uint32_t)0x00000008)
#define USB_INTS_TRANS_COMPLETE                              USB_INTS_TRANS_COMPLETE_Msk
#define USB_INTS_HOST_SOF_Pos                                2
#define USB_INTS_HOST_SOF_Msk                                ((uint32_t)0x00000004)
#define USB_INTS_HOST_SOF                                    USB_INTS_HOST_SOF_Msk
#define USB_INTS_HOST_RESUME_Pos                             1
#define USB_INTS_HOST_RESUME_Msk                             ((uint32_t)0x00000002)
#define USB_INTS_HOST_RESUME                                 USB_INTS_HOST_RESUME_Msk
#define USB_INTS_HOST_CONN_DIS_Pos                           0
#define USB_INTS_HOST_CONN_DIS_Msk                           ((uint32_t)0x00000001)
#define USB_INTS_HOST_CONN_DIS                               USB_INTS_HOST_CONN_DIS_Msk

// VREG_AND_CHIP_RESET->VREG
#define VREG_AND_CHIP_RESET_VREG_ROK_Pos                     12
#define VREG_AND_CHIP_RESET_VREG_ROK_Msk                     ((uint32_t)0x00001000)
#define VREG_AND_CHIP_RESET_VREG_ROK                         VREG_AND_CHIP_RESET_VREG_ROK_Msk
#define VREG_AND_CHIP_RESET_VREG_VSEL_Pos                    4
#define VREG_AND_CHIP_RESET_VREG_VSEL_Msk                    ((uint32_t)0x000000F0)
#define VREG_AND_CHIP_RESET_VREG_VSEL                        VREG_AND_CHIP_RESET_VREG_VSEL_Msk
#define VREG_AND_CHIP_RESET_VREG_HIZ_Pos                     1
#define VREG_AND_CHIP_RESET_VREG_HIZ_Msk                     ((uint32_t)0x00000002)
#define VREG_AND_CHIP_RESET_VREG_HIZ                         VREG_AND_CHIP_RESET_VREG_HIZ_Msk
#define VREG_AND_CHIP_RESET_VREG_EN_Pos                      0
#define VREG_AND_CHIP_RESET_VREG_EN_Msk                      ((uint32_t)0x00000001)
#define VREG_AND_CHIP_RESET_VREG_EN                          VREG_AND_CHIP_RESET_VREG_EN_Msk
// VREG_AND_CHIP_RESET->BOD
#define VREG_AND_CHIP_RESET_BOD_VSEL_Pos                     4
#define VREG_AND_CHIP_RESET_BOD_VSEL_Msk                     ((uint32_t)0x000000F0)
#define VREG_AND_CHIP_RESET_BOD_VSEL                         VREG_AND_CHIP_RESET_BOD_VSEL_Msk
#define VREG_AND_CHIP_RESET_BOD_EN_Pos                       0
#define VREG_AND_CHIP_RESET_BOD_EN_Msk                       ((uint32_t)0x00000001)
#define VREG_AND_CHIP_RESET_BOD_EN                           VREG_AND_CHIP_RESET_BOD_EN_Msk
// VREG_AND_CHIP_RESET->CHIP_RESET
#define VREG_AND_CHIP_RESET_CHIP_RESET_PSM_RESTART_FLAG_Pos  24
#define VREG_AND_CHIP_RESET_CHIP_RESET_PSM_RESTART_FLAG_Msk  ((uint32_t)0x01000000)
#define VREG_AND_CHIP_RESET_CHIP_RESET_PSM_RESTART_FLAG      VREG_AND_CHIP_RESET_CHIP_RESET_PSM_RESTART_FLAG_Msk
#define VREG_AND_CHIP_RESET_CHIP_RESET_HAD_PSM_RESTART_Pos   20
#define VREG_AND_CHIP_RESET_CHIP_RESET_HAD_PSM_RESTART_Msk   ((uint32_t)0x00100000)
#define VREG_AND_CHIP_RESET_CHIP_RESET_HAD_PSM_RESTART       VREG_AND_CHIP_RESET_CHIP_RESET_HAD_PSM_RESTART_Msk
#define VREG_AND_CHIP_RESET_CHIP_RESET_HAD_RUN_Pos           16
#define VREG_AND_CHIP_RESET_CHIP_RESET_HAD_RUN_Msk           ((uint32_t)0x00010000)
#define VREG_AND_CHIP_RESET_CHIP_RESET_HAD_RUN               VREG_AND_CHIP_RESET_CHIP_RESET_HAD_RUN_Msk
#define VREG_AND_CHIP_RESET_CHIP_RESET_HAD_POR_Pos           8
#define VREG_AND_CHIP_RESET_CHIP_RESET_HAD_POR_Msk           ((uint32_t)0x00000100)
#define VREG_AND_CHIP_RESET_CHIP_RESET_HAD_POR               VREG_AND_CHIP_RESET_CHIP_RESET_HAD_POR_Msk

// WATCHDOG->CTRL
#define WATCHDOG_CTRL_TRIGGER_Pos                            31
#define WATCHDOG_CTRL_TRIGGER_Msk                            ((uint32_t)0x80000000)
#define WATCHDOG_CTRL_TRIGGER                                WATCHDOG_CTRL_TRIGGER_Msk
#define WATCHDOG_CTRL_ENABLE_Pos                             30
#define WATCHDOG_CTRL_ENABLE_Msk                             ((uint32_t)0x40000000)
#define WATCHDOG_CTRL_ENABLE                                 WATCHDOG_CTRL_ENABLE_Msk
#define WATCHDOG_CTRL_PAUSE_DBG1_Pos                         26
#define WATCHDOG_CTRL_PAUSE_DBG1_Msk                         ((uint32_t)0x04000000)
#define WATCHDOG_CTRL_PAUSE_DBG1                             WATCHDOG_CTRL_PAUSE_DBG1_Msk
#define WATCHDOG_CTRL_PAUSE_DBG0_Pos                         25
#define WATCHDOG_CTRL_PAUSE_DBG0_Msk                         ((uint32_t)0x02000000)
#define WATCHDOG_CTRL_PAUSE_DBG0                             WATCHDOG_CTRL_PAUSE_DBG0_Msk
#define WATCHDOG_CTRL_PAUSE_JTAG_Pos                         24
#define WATCHDOG_CTRL_PAUSE_JTAG_Msk                         ((uint32_t)0x01000000)
#define WATCHDOG_CTRL_PAUSE_JTAG                             WATCHDOG_CTRL_PAUSE_JTAG_Msk
#define WATCHDOG_CTRL_TIME_Pos                               0
#define WATCHDOG_CTRL_TIME_Msk                               ((uint32_t)0x00FFFFFF)
#define WATCHDOG_CTRL_TIME                                   WATCHDOG_CTRL_TIME_Msk
// WATCHDOG->LOAD
#define WATCHDOG_LOAD_LOAD_Pos                               0
#define WATCHDOG_LOAD_LOAD_Msk                               ((uint32_t)0x00FFFFFF)
#define WATCHDOG_LOAD_LOAD                                   WATCHDOG_LOAD_LOAD_Msk
// WATCHDOG->REASON
#define WATCHDOG_REASON_FORCE_Pos                            1
#define WATCHDOG_REASON_FORCE_Msk                            ((uint32_t)0x00000002)
#define WATCHDOG_REASON_FORCE                                WATCHDOG_REASON_FORCE_Msk
#define WATCHDOG_REASON_TIMER_Pos                            0
#define WATCHDOG_REASON_TIMER_Msk                            ((uint32_t)0x00000001)
#define WATCHDOG_REASON_TIMER                                WATCHDOG_REASON_TIMER_Msk
// WATCHDOG->SCRATCH
#define WATCHDOG_SCRATCH_SCRATCH0_Pos                        0
#define WATCHDOG_SCRATCH_SCRATCH0_Msk                        ((uint32_t)0xFFFFFFFF)
#define WATCHDOG_SCRATCH_SCRATCH0                            WATCHDOG_SCRATCH_SCRATCH0_Msk
// WATCHDOG->TICK
#define WATCHDOG_TICK_COUNT_Pos                              11
#define WATCHDOG_TICK_COUNT_Msk                              ((uint32_t)0x000FF800)
#define WATCHDOG_TICK_COUNT                                  WATCHDOG_TICK_COUNT_Msk
#define WATCHDOG_TICK_RUNNING_Pos                            10
#define WATCHDOG_TICK_RUNNING_Msk                            ((uint32_t)0x00000400)
#define WATCHDOG_TICK_RUNNING                                WATCHDOG_TICK_RUNNING_Msk
#define WATCHDOG_TICK_ENABLE_Pos                             9
#define WATCHDOG_TICK_ENABLE_Msk                             ((uint32_t)0x00000200)
#define WATCHDOG_TICK_ENABLE                                 WATCHDOG_TICK_ENABLE_Msk
#define WATCHDOG_TICK_CYCLES_Pos                             0
#define WATCHDOG_TICK_CYCLES_Msk                             ((uint32_t)0x000001FF)
#define WATCHDOG_TICK_CYCLES                                 WATCHDOG_TICK_CYCLES_Msk

// XIP_CTRL->CTRL
#define XIP_CTRL_CTRL_POWER_DOWN_Pos                         3
#define XIP_CTRL_CTRL_POWER_DOWN_Msk                         ((uint32_t)0x00000008)
#define XIP_CTRL_CTRL_POWER_DOWN                             XIP_CTRL_CTRL_POWER_DOWN_Msk
#define XIP_CTRL_CTRL_ERR_BADWRITE_Pos                       1
#define XIP_CTRL_CTRL_ERR_BADWRITE_Msk                       ((uint32_t)0x00000002)
#define XIP_CTRL_CTRL_ERR_BADWRITE                           XIP_CTRL_CTRL_ERR_BADWRITE_Msk
#define XIP_CTRL_CTRL_EN_Pos                                 0
#define XIP_CTRL_CTRL_EN_Msk                                 ((uint32_t)0x00000001)
#define XIP_CTRL_CTRL_EN                                     XIP_CTRL_CTRL_EN_Msk
// XIP_CTRL->FLUSH
#define XIP_CTRL_FLUSH_FLUSH_Pos                             0
#define XIP_CTRL_FLUSH_FLUSH_Msk                             ((uint32_t)0x00000001)
#define XIP_CTRL_FLUSH_FLUSH                                 XIP_CTRL_FLUSH_FLUSH_Msk
// XIP_CTRL->STAT
#define XIP_CTRL_STAT_FIFO_FULL_Pos                          2
#define XIP_CTRL_STAT_FIFO_FULL_Msk                          ((uint32_t)0x00000004)
#define XIP_CTRL_STAT_FIFO_FULL                              XIP_CTRL_STAT_FIFO_FULL_Msk
#define XIP_CTRL_STAT_FIFO_EMPTY_Pos                         1
#define XIP_CTRL_STAT_FIFO_EMPTY_Msk                         ((uint32_t)0x00000002)
#define XIP_CTRL_STAT_FIFO_EMPTY                             XIP_CTRL_STAT_FIFO_EMPTY_Msk
#define XIP_CTRL_STAT_FLUSH_READY_Pos                        0
#define XIP_CTRL_STAT_FLUSH_READY_Msk                        ((uint32_t)0x00000001)
#define XIP_CTRL_STAT_FLUSH_READY                            XIP_CTRL_STAT_FLUSH_READY_Msk
// XIP_CTRL->CTR_HIT
#define XIP_CTRL_CTR_HIT_CTR_HIT_Pos                         0
#define XIP_CTRL_CTR_HIT_CTR_HIT_Msk                         ((uint32_t)0xFFFFFFFF)
#define XIP_CTRL_CTR_HIT_CTR_HIT                             XIP_CTRL_CTR_HIT_CTR_HIT_Msk
// XIP_CTRL->CTR_ACC
#define XIP_CTRL_CTR_ACC_CTR_ACC_Pos                         0
#define XIP_CTRL_CTR_ACC_CTR_ACC_Msk                         ((uint32_t)0xFFFFFFFF)
#define XIP_CTRL_CTR_ACC_CTR_ACC                             XIP_CTRL_CTR_ACC_CTR_ACC_Msk
// XIP_CTRL->STREAM_ADDR
#define XIP_CTRL_STREAM_ADDR_STREAM_ADDR_Pos                 2
#define XIP_CTRL_STREAM_ADDR_STREAM_ADDR_Msk                 ((uint32_t)0xFFFFFFFC)
#define XIP_CTRL_STREAM_ADDR_STREAM_ADDR                     XIP_CTRL_STREAM_ADDR_STREAM_ADDR_Msk
// XIP_CTRL->STREAM_CTR
#define XIP_CTRL_STREAM_CTR_STREAM_CTR_Pos                   0
#define XIP_CTRL_STREAM_CTR_STREAM_CTR_Msk                   ((uint32_t)0x003FFFFF)
#define XIP_CTRL_STREAM_CTR_STREAM_CTR                       XIP_CTRL_STREAM_CTR_STREAM_CTR_Msk
// XIP_CTRL->STREAM_FIFO
#define XIP_CTRL_STREAM_FIFO_STREAM_FIFO_Pos                 0
#define XIP_CTRL_STREAM_FIFO_STREAM_FIFO_Msk                 ((uint32_t)0xFFFFFFFF)
#define XIP_CTRL_STREAM_FIFO_STREAM_FIFO                     XIP_CTRL_STREAM_FIFO_STREAM_FIFO_Msk

// XOSC->CTRL
#define XOSC_CTRL_ENABLE_Pos                                 12
#define XOSC_CTRL_ENABLE_Msk                                 ((uint32_t)0x00FFF000)
#define XOSC_CTRL_ENABLE                                     XOSC_CTRL_ENABLE_Msk
#define XOSC_CTRL_ENABLE_DISABLE                             ((uint32_t)0x00D1E000)
#define XOSC_CTRL_ENABLE_ENABLE                              ((uint32_t)0x00FAB000)
#define XOSC_CTRL_FREQ_RANGE_Pos                             0
#define XOSC_CTRL_FREQ_RANGE_Msk                             ((uint32_t)0x00000FFF)
#define XOSC_CTRL_FREQ_RANGE                                 XOSC_CTRL_FREQ_RANGE_Msk
#define XOSC_CTRL_FREQ_RANGE_1_15MHZ                         ((uint32_t)0x00000AA0)
#define XOSC_CTRL_FREQ_RANGE_RESERVED_1                      ((uint32_t)0x00000AA1)
#define XOSC_CTRL_FREQ_RANGE_RESERVED_2                      ((uint32_t)0x00000AA2)
#define XOSC_CTRL_FREQ_RANGE_RESERVED_3                      ((uint32_t)0x00000AA3)
// XOSC->STATUS
#define XOSC_STATUS_STABLE_Pos                               31
#define XOSC_STATUS_STABLE_Msk                               ((uint32_t)0x80000000)
#define XOSC_STATUS_STABLE                                   XOSC_STATUS_STABLE_Msk
#define XOSC_STATUS_BADWRITE_Pos                             24
#define XOSC_STATUS_BADWRITE_Msk                             ((uint32_t)0x01000000)
#define XOSC_STATUS_BADWRITE                                 XOSC_STATUS_BADWRITE_Msk
#define XOSC_STATUS_ENABLED_Pos                              12
#define XOSC_STATUS_ENABLED_Msk                              ((uint32_t)0x00001000)
#define XOSC_STATUS_ENABLED                                  XOSC_STATUS_ENABLED_Msk
#define XOSC_STATUS_FREQ_RANGE_Pos                           0
#define XOSC_STATUS_FREQ_RANGE_Msk                           ((uint32_t)0x00000003)
#define XOSC_STATUS_FREQ_RANGE                               XOSC_STATUS_FREQ_RANGE_Msk
#define XOSC_STATUS_FREQ_RANGE_1_15MHZ                       ((uint32_t)0x00000000)
#define XOSC_STATUS_FREQ_RANGE_RESERVED_1                    ((uint32_t)0x00000001)
#define XOSC_STATUS_FREQ_RANGE_RESERVED_2                    ((uint32_t)0x00000002)
#define XOSC_STATUS_FREQ_RANGE_RESERVED_3                    ((uint32_t)0x00000003)
// XOSC->DORMANT
#define XOSC_DORMANT_DORMANT_Pos                             0
#define XOSC_DORMANT_DORMANT_Msk                             ((uint32_t)0xFFFFFFFF)
#define XOSC_DORMANT_DORMANT                                 XOSC_DORMANT_DORMANT_Msk
#define XOSC_DORMANT_DORMANT_DORMANT                         ((uint32_t)0x636F6D61)
#define XOSC_DORMANT_DORMANT_WAKE                            ((uint32_t)0x77616B65)
// XOSC->STARTUP
#define XOSC_STARTUP_X4_Pos                                  20
#define XOSC_STARTUP_X4_Msk                                  ((uint32_t)0x00100000)
#define XOSC_STARTUP_X4                                      XOSC_STARTUP_X4_Msk
#define XOSC_STARTUP_DELAY_Pos                               0
#define XOSC_STARTUP_DELAY_Msk                               ((uint32_t)0x00003FFF)
#define XOSC_STARTUP_DELAY                                   XOSC_STARTUP_DELAY_Msk
// XOSC->COUNT
#define XOSC_COUNT_COUNT_Pos                                 0
#define XOSC_COUNT_COUNT_Msk                                 ((uint32_t)0x000000FF)
#define XOSC_COUNT_COUNT                                     XOSC_COUNT_COUNT_Msk


#ifdef __cplusplus
}
#endif
