/* ===================================================================================
 * Linker Script for RP2040 with 8MB  Flash                                 * v1.0 *
 * ===================================================================================
 *
 * Flash (XIP, 0x10000000): boot2 (256 bytes), vector table (0x10000100), code, data
 * SRAM (0x20000000): data, RAM functions, bss, stack (264KB incl. SRAM4 and SRAM5)
 *
 * 2026 by Stefan Wagner:   https://github.com/wagiminator
 * =================================================================================== */

ENTRY( Reset_Handler )

MEMORY
{
  FLASH (rx) : ORIGIN = 0x10000000, LENGTH = 8192K
  RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 264K
}

SECTIONS
{
  .boot2 :
  {
    __boot2_start__ = .;
    KEEP(*(.boot2))
    __boot2_end__ = .;
  } > FLASH

  ASSERT(__boot2_end__ - __boot2_start__ == 256, "ERROR: boot2 must be 256 bytes in size")

  .vectors :
  {
    . = ALIGN(256);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } > FLASH

  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    *(.glue_7)
    *(.glue_7t)
    *(.eh_frame)
    KEEP (*(.init))
    KEEP (*(.fini))
    . = ALIGN(4);
    _etext = .;
  } > FLASH

  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } > FLASH

  .ARM.extab :
  {
    . = ALIGN(4);
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    . = ALIGN(4);
  } > FLASH

  .ARM :
  {
    . = ALIGN(4);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
    . = ALIGN(4);
  } > FLASH

  .preinit_array :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
    . = ALIGN(4);
  } > FLASH

  .init_array :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
    . = ALIGN(4);
  } > FLASH

  .fini_array :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
    . = ALIGN(4);
  } > FLASH

  .ctors :
  {
    KEEP(*crtbegin.o(.ctors))
    KEEP(*crtbegin?.o(.ctors))
    KEEP(*(EXCLUDE_FILE(*crtend.o *crtend?.o) .ctors))
    KEEP(*(SORT(.ctors.*)))
    KEEP(*(.ctors))
  } > FLASH
  
  .dtors :
  {
    KEEP(*crtbegin.o(.dtors))
    KEEP(*crtbegin?.o(.dtors))
    KEEP(*(EXCLUDE_FILE(*crtend.o *crtend?.o) .dtors))
    KEEP(*(SORT(.dtors.*)))
    KEEP(*(.dtors))
  } > FLASH

  _sidata = LOADADDR(.data);

  .data :
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data)
    *(.data*)
    *(.RamFunc)
    *(.RamFunc*)
    *(.time_critical*)
    . = ALIGN(4);
    _edata = .;

  } > RAM AT> FLASH

  .bss :
  {
    . = ALIGN(4);
    _sbss = .;
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
    __bss_end__ = _ebss;
  } > RAM

  PROVIDE(_exit = _ebss);
  PROVIDE(_end = _ebss);
  PROVIDE(end = . );
  PROVIDE(_estack = ORIGIN(RAM) + LENGTH(RAM));
  PROVIDE(_flash_end = LOADADDR(.data) + SIZEOF(.data));	

  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
