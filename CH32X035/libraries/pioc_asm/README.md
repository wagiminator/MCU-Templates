# PIOC Assembler and Programs for CH32X035/X034/X033

## Overview
The CH32X035/X034/X033 contains a programmable protocol I/O controller (PIOC, also called eMCU). It is a small 8-bit RISC core (RISC8B) that runs at the system clock (F_CPU) independently of the main RISC-V core and controls two pins (IO0, IO1) with cycle-accurate timing. Typical applications are protocols that are not supported by the hardware peripherals (e.g. NeoPixels, 1-Wire, IR remote controls, additional UARTs or special bus protocols).

This folder contains:

| File | Description |
|:-|:-|
| pioc_asm.py | Assembler for PIOC programs (Python3, no further packages), creates a C header with the machine code |
| pioc_sim.py | Simple simulator for checking PIOC programs without hardware (timing, pins, host interface; approximate peripheral models) |
| neo_pioc.asm | Source of the PIOC program of the *neo_pioc* library (NeoPixels) |
| uart_pioc.asm | Source of the PIOC program of the *uart_pioc* library (UART transmitter) |

The PIOC programs of the libraries are already assembled and included in the respective *.c* file, the sources are only needed if you want to change them. More PIOC programs (*blink.asm*, *pwm.asm*) can be found in the examples.

## The PIOC
| Feature | Description |
|:-|:-|
| Core | RISC8B, 8-bit data, 16-bit instructions, single cycle (jumps, calls, returns and taken skips: 2 cycles) |
| Clock | System clock (F_CPU), i.e. the timing of a program depends on F_CPU |
| Program memory | 2048 instruction words in the upper 4KB of the SRAM (0x20004000), loaded by the host; word 0 is never executed, the program starts at address 1 |
| Stack | 6 levels (CALL, PUSHAS) |
| Pins | IO0 (PC18 or PC7), IO1 (PC19); direction, output, input, pull-up and level change detection; IO0 with edge detection |
| Timer 0 | 8-bit timer with prescaler (F_CPU/1 ... F_CPU/1024), timeout reset or PWM output on IO0 |
| Bit encoder/decoder | Hardware encoding/decoding of bits with PWM duty cycle (25%/75%) or Manchester modulation on IO0 |
| Data exchange | 32 data registers (DATA_REG0..31) and DATA_EXCH (read/write by both sides), CTRL_WR (host -> PIOC) and CTRL_RD (PIOC -> host) with status flags, interrupt request to the host |

### Data Exchange with the Host
| Register | PIOC | Host (pioc library) | Notes |
|:-|:-|:-|:-|
| DATA_REG0..31 | SFR_DATA_REG0..31 | `PIOC_setReg(n,v)`, `PIOC_getReg(n)`, `PIOC_setReg16(n,v)`, `PIOC_getReg16(n)` | Parameters and results; the host can only write them while the PIOC is running |
| DATA_EXCH | SFR_DATA_EXCH | `PIOC_setExchange(v)`, `PIOC_getExchange()` | Supports single-cycle bit transfers (BP2F/BG2F) |
| CTRL_WR | `WAITB WB_DATA_MW_SR_1` then `MOV SFR_CTRL_WR,A` | `PIOC_write(c)`, `PIOC_ready()` | Commands or data from the host; reading by the PIOC clears the flag |
| CTRL_RD | `MOVA SFR_CTRL_RD` | `PIOC_read()`, `PIOC_available()` | Results or data from the PIOC; reading by the host clears the flag |
| INT_REQ | `BS SFR_SYS_CFG,SB_INT_REQ` | `PIOC_INT_enable()`, `PIOC_ISR { }` | Interrupt request; cleared by the library before PIOC_ISR is called |
| Code RAM | `RDCODE` (word address: low byte in SFR_INDIR_ADDR, high bits in A) | `PIOC_writeRAM(offs,buf,len)`, `PIOC_readRAM(offs,buf,len)` | Large data blocks; the host pauses the PIOC while accessing the RAM |

A typical sequence on the host side:
```c
#include "pioc.h"
#include "my_prog.h"                      // created by: python3 pioc_asm.py my_prog.asm

PIOC_init(PIOC_IO0);                      // connect IO0 (PC18) to the PIOC
PIOC_load(PIOC_MY_PROG, sizeof(PIOC_MY_PROG)); // load program
PIOC_start();                             // start program
PIOC_setReg(0, 42);                       // set parameters (after PIOC_start!)
uint8_t result = PIOC_command(0x01);      // send command, wait for result
```

## Assembler
The assembler *pioc_asm.py* understands the syntax of the WCH assembler WASM53B, so the PIOC programs of the WCH examples can be assembled as well (the results were verified against the BIN files of all WCH PIOC examples). The include file PIOC_INC.ASM with all register and bit names is built in.

```
python3 pioc_asm.py program.asm               -> program.h with 'static const uint8_t PIOC_PROGRAM[]'
python3 pioc_asm.py program.asm -n MY_CODE    -> name of the C array
python3 pioc_asm.py program.asm -o code.h     -> name of the output file
python3 pioc_asm.py program.asm -b -l         -> also create program.bin and listing program.lst
```

Syntax:
- Labels start in the first column (colon optional), instructions must be indented.
- Comments start with `;`. Mnemonics and symbols are not case-sensitive.
- Directives: `EQU`, `ORG`, `DW`, `DB`, `INCLUDE`, `END`.
- Numbers: `123`, `0x7B`, `7Bh`, `H'7B'`, `0b01111011`, `B'01111011'`, `D'123'`, `'c'`.
- Expressions with `+ - * / & | ^ ~ << >>` and brackets. WASM53B evaluates expressions from right to left, so use brackets to keep the sources compatible.

Program skeleton:
```
INCLUDE         PIOC_INC.ASM

                ORG   0x0000
                DW    0x0000                      ; word 0 is never executed
                JMP   START                       ; program starts at address 1

START:          BC    SFR_PORT_IO,SB_PORT_OUT0    ; IO0 low
                BS    SFR_PORT_DIR,SB_PORT_DIR0   ; IO0 output
LOOP:           WAITB WB_DATA_MW_SR_1             ; wait for command from host
                MOV   SFR_CTRL_WR,A               ; A = command (clears flag)
                ...
                MOVA  SFR_CTRL_RD                 ; result to host
                JMP   LOOP
                END
```

## Instruction Set (Overview)
`f` = register (0x00..0xFF), `d` = destination (`A` or `F`, default `F`), `b` = bit (0..7), `k` = constant.

| Group | Instructions |
|:-|:-|
| Byte operations | `MOV f,d`, `MOVA f` (A->f), `CLR f`, `CLRA`, `INC f,d`, `DEC f,d`, `INCSZ f,d`, `DECSZ f,d`, `SWAP f,d`, `AND f,d`, `IOR f,d`, `XOR f,d`, `ADD f,d`, `SUB f,d` (f-A), `RCL f,d`, `RCR f,d` |
| Constants | `MOVL k`, `ANDL k`, `IORL k`, `XORL k`, `ADDL k`, `SUBL k` (k-A), `CMPL k` (k-A, flags only), `CMPLN k` (k+A, flags only), `RETL k`, `RETLN k`, `MOVIP k`, `MOVIA k`, `MOVA1F k` (k->PORT_DIR), `MOVA2F k` (k->PORT_IO), `MOVA1P k`, `MOVA2P k` |
| Bit operations | `BC f,b`, `BS f,b`, `BTSC f,b` (skip if clear), `BTSS f,b` (skip if set), `BP1F a,b`/`BP2F a,b` (bit b of INDIR_ADDR/DATA_EXCH -> C, TX_O0, OUT0, OUT1), `BG1F a,b`/`BG2F a,b` (C, RX_I0, IN0, IN1 -> bit b of INDIR_ADDR/DATA_EXCH), `BCTC a` |
| Jumps | `JMP k`, `CALL k`, `RET`, `RETZ`, `JZ k`, `JNZ k`, `JC k`, `JNC k` (same 1K page), `CMPZ K,k` (jump if A==K, same 256-word page) |
| Others | `NOP`, `WAITB b` (wait for event), `RDCODE` (read code word), `SLEEP`, `CLRWDT`, `PUSHAS`, `POPAS`, `PUSHA2`, `POPA2` |

Pitfalls:
- For byte operations the destination is the register if it is omitted: `IOR CNT_H` changes CNT_H, `IOR CNT_H,A` only changes A.
- The carry flag after a subtraction or comparison is a borrow: `CMPL k` / `JNC` jumps if A <= k.
- `WAITB WB_PORT_I0_RISE/FALL` exit immediately if the edge has already been detected before; writing SFR_BIT_CYCLE clears the edge records.
- For exact timing, use instructions with constant run time (see the delay loops in the programs: 8 clock cycles per loop with a 16-bit counter).

## References
- [WCH: PIOC manual (PIOC-EN.pdf) and RISC8B instruction set (CHRISC8B-EN.pdf)](https://github.com/openwch/ch32x035/tree/main/EVT/EXAM/PIOC/Tool_Manual)
- [WCH: PIOC examples (1-Wire/NeoPixel, I2C, IR NEC, single wire, UART)](https://github.com/openwch/ch32x035/tree/main/EVT/EXAM/PIOC)
