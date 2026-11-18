#!/usr/bin/env python3
# ===================================================================================
# Project:   pioc_asm - Assembler for the PIOC (RISC8B eMCU) of the CH32X035
# Version:   v1.0
# Year:      2026
# Author:    Stefan Wagner
# Github:    https://github.com/wagiminator
# License:   MIT License
# ===================================================================================
#
# Description:
# ------------
# Simple two-pass assembler for the programmable protocol I/O controller (PIOC) of
# the CH32X035/X034/X033. It understands the syntax of the WCH assembler WASM53B
# (instructions, aliases, EQU, ORG, DW, DB, INCLUDE, END) and creates a C header
# file with the machine code as a byte array, which can be loaded into the PIOC
# with PIOC_load() of the pioc library. Optionally a binary file (identical to the
# .BIN file of WASM53B) and a listing can be created.
#
# Syntax notes:
# -------------
# - Labels start in the first column (colon optional), instructions must be indented.
# - Comments start with ';'. Mnemonics, labels and symbols are case-insensitive.
# - Numbers: 123, 0x7B, H'7B', 7Bh, 0b01111011, B'01111011', D'123', 'c'.
# - Expressions with + - * / & | ^ ~ << >> and brackets are supported.
# - The include file PIOC_INC.ASM is built in and does not have to exist.
#
# Usage:
# ------
# python3 pioc_asm.py program.asm                -> creates program.h (C array)
# python3 pioc_asm.py program.asm -o prog.h      -> name of the output file
# python3 pioc_asm.py program.asm -n my_prog     -> name of the C array
# python3 pioc_asm.py program.asm -b -l          -> additionally .bin and .lst
#
# The C array is called PIOC_<NAME> (uppercase file name) by default, e.g.
# 'const uint8_t PIOC_BLINK[]' for blink.asm.

import os
import re
import sys
import argparse

# ===================================================================================
# Built-in Include File (PIOC_INC.ASM)
# ===================================================================================
PIOC_SYMBOLS = {
  # SFR registers
  'SFR_INDIR_PORT': 0x00, 'SFR_INDIR_PORT2': 0x01, 'SFR_PRG_COUNT': 0x02,
  'SFR_STATUS_REG': 0x03, 'SFR_INDIR_ADDR': 0x04, 'SFR_TMR0_COUNT': 0x05,
  'SFR_TIMER_CTRL': 0x06, 'SFR_TMR0_INIT': 0x07, 'SFR_BIT_CYCLE': 0x08,
  'SFR_INDIR_ADDR2': 0x09, 'SFR_PORT_DIR': 0x0A, 'SFR_PORT_IO': 0x0B,
  'SFR_BIT_CONFIG': 0x0C, 'SFR_SYS_CFG': 0x1C, 'SFR_CTRL_RD': 0x1D,
  'SFR_CTRL_WR': 0x1E, 'SFR_DATA_EXCH': 0x1F,
  # SFR_STATUS_REG
  'SB_EN_TOUT_RST': 5, 'SB_STACK_USED': 4, 'SB_GP_BIT_Y': 3, 'SB_FLAG_Z': 2,
  'SB_GP_BIT_X': 1, 'SB_FLAG_C': 0,
  # SFR_TIMER_CTRL
  'SB_EN_LEVEL1': 7, 'SB_EN_LEVEL0': 6, 'SB_TMR0_ENABLE': 5, 'SB_TMR0_OUT_EN': 4,
  'SB_TMR0_MODE': 3, 'SB_TMR0_FREQ2': 2, 'SB_TMR0_FREQ1': 1, 'SB_TMR0_FREQ0': 0,
  # SFR_BIT_CYCLE
  'SB_BIT_TX_O0': 7, 'SB_BIT_CYCLE_6': 6, 'SB_BIT_CYCLE_5': 5, 'SB_BIT_CYCLE_4': 4,
  'SB_BIT_CYCLE_3': 3, 'SB_BIT_CYCLE_2': 2, 'SB_BIT_CYCLE_1': 1, 'SB_BIT_CYCLE_0': 0,
  # SFR_PORT_DIR
  'SB_PORT_MOD3': 7, 'SB_PORT_MOD2': 6, 'SB_PORT_MOD1': 5, 'SB_PORT_MOD0': 4,
  'SB_PORT_PU1': 3, 'SB_PORT_PU0': 2, 'SB_PORT_DIR1': 1, 'SB_PORT_DIR0': 0,
  # SFR_PORT_IO
  'SB_PORT_IN_XOR': 7, 'SB_BIT_RX_I0': 6, 'SB_PORT_IN1': 5, 'SB_PORT_IN0': 4,
  'SB_PORT_XOR1': 3, 'SB_PORT_XOR0': 2, 'SB_PORT_OUT1': 1, 'SB_PORT_OUT0': 0,
  # SFR_BIT_CONFIG
  'SB_BIT_TX_EN': 7, 'SB_BIT_CODE_MOD': 6, 'SB_PORT_IN_EDGE': 5, 'SB_BIT_CYC_TAIL': 4,
  'SB_BIT_CYC_CNT6': 3, 'SB_BIT_CYC_CNT5': 2, 'SB_BIT_CYC_CNT4': 1, 'SB_BIT_CYC_CNT3': 0,
  # SFR_SYS_CFG
  'SB_INT_REQ': 7, 'SB_DATA_SW_MR': 6, 'SB_DATA_MW_SR': 5, 'SB_MST_CFG_B4': 4,
  'SB_MST_IO_EN1': 3, 'SB_MST_IO_EN0': 2, 'SB_MST_RESET': 1, 'SB_MST_CLK_GATE': 0,
  # BCTC, BP1F/BP2F/BG1F/BG2F
  'BI_C_XOR_IN0': 0, 'BIO_FLAG_C': 0,
  'BI_BIT_RX_I0': 1, 'BI_PORT_IN0': 2, 'BI_PORT_IN1': 3,
  'BO_BIT_TX_O0': 1, 'BO_PORT_OUT0': 2, 'BO_PORT_OUT1': 3,
  # WAITB
  'WB_DATA_SW_MR_0': 0, 'WB_BIT_CYC_TAIL_1': 1, 'WB_PORT_I0_FALL': 2,
  'WB_PORT_I0_RISE': 3, 'WB_DATA_MW_SR_1': 4, 'WB_PORT_XOR1_1': 5,
  'WB_PORT_XOR0_0': 6, 'WB_PORT_XOR0_1': 7,
}
for _i in range(32):
  PIOC_SYMBOLS['SFR_DATA_REG%d' % _i] = 0x20 + _i

PIOC_CODE_WORDS = 2048            # code space in words (4KB SRAM)

# ===================================================================================
# Instruction Set
# ===================================================================================
# operand types: '' none, 'k2', 'b3' (WAITB), 'a2' (BCTC), 'ab' (BPxF/BGxF a,b),
# 'f' (CLR), 'F' (MOVA, 9-bit), 'Fd' (MOV), 'fd' (byte ops), 'k8', 'k9', 'k10',
# 'fb' (bit ops), 'j12', 'j10', 'cmpz'
INSTR = {
  'NOP':    (0x0000, ''),     'CLRWDT': (0x0008, ''),     'CLRA':   (0x0004, ''),
  'SLEEP':  (0x000C, ''),     'SLEEPX': (0x000C, 'k2'),   'WAITB':  (0x0010, 'b3'),
  'WAITRD': (0x0010, ''),     'WAITWR': (0x0014, ''),     'WAITSPI':(0x0014, ''),
  'RDCODE': (0x0018, ''),     'RCODE':  (0x0018, 'k2'),   'WRCODE': (0x001C, ''),
  'EXEC':   (0x001C, 'k2'),   'BCTC':   (0x001C, 'a2'),
  'PUSHAS': (0x0020, ''),     'POPAS':  (0x0024, ''),     'PUSHA2': (0x0028, ''),
  'POPA2':  (0x002C, ''),     'RET':    (0x0030, ''),     'RETZ':   (0x0034, ''),
  'RETIE':  (0x0038, ''),
  'BP1F':   (0x0080, 'ab'),   'BP2F':   (0x00A0, 'ab'),   'BG1F':   (0x00C0, 'ab'),
  'BG2F':   (0x00E0, 'ab'),
  'CLR':    (0x0100, 'f'),    'MOVA':   (0x1000, 'F'),    'MOV':    (0x0200, 'Fd'),
  'INC':    (0x0400, 'fd'),   'DEC':    (0x0500, 'fd'),   'INCSZ':  (0x0600, 'fd'),
  'DECSZ':  (0x0700, 'fd'),   'SWAP':   (0x0800, 'fd'),   'AND':    (0x0900, 'fd'),
  'IOR':    (0x0A00, 'fd'),   'XOR':    (0x0B00, 'fd'),   'ADD':    (0x0C00, 'fd'),
  'SUB':    (0x0D00, 'fd'),   'RCL':    (0x0E00, 'fd'),   'RCR':    (0x0F00, 'fd'),
  'RETL':   (0x2000, 'k8'),   'RETLN':  (0x2100, 'k8'),   'MOVIP':  (0x2200, 'k9'),
  'MOVA1F': (0x2300, 'k8'),   'MOVIA':  (0x2400, 'k10'),  'MOVA2F': (0x2500, 'k8'),
  'MOVA2P': (0x2600, 'k8'),   'MOVA1P': (0x2700, 'k8'),   'MOVL':   (0x2800, 'k8'),
  'ANDL':   (0x2900, 'k8'),   'IORL':   (0x2A00, 'k8'),   'XORL':   (0x2B00, 'k8'),
  'ADDL':   (0x2C00, 'k8'),   'SUBL':   (0x2D00, 'k8'),   'CMPLN':  (0x2E00, 'k8'),
  'CMPL':   (0x2F00, 'k8'),
  'BC':     (0x4000, 'fb'),   'BS':     (0x4800, 'fb'),   'BTSC':   (0x5000, 'fb'),
  'BTSS':   (0x5800, 'fb'),
  'JMP':    (0x6000, 'j12'),  'CALL':   (0x7000, 'j12'),
  'JNZ':    (0x3000, 'j10'),  'JZ':     (0x3400, 'j10'),  'JNC':    (0x3800, 'j10'),
  'JC':     (0x3C00, 'j10'),  'CMPZ':   (0x8000, 'cmpz'),
}

ALIASES = {
  'WDT': 'CLRWDT', 'HALT': 'SLEEP', 'PUSH': 'PUSHAS', 'POP': 'POPAS',
  'RETURN': 'RET', 'RETOK': 'RETZ', 'RETI': 'RETIE', 'DB': 'RETL',
  'RETER': 'RETLN', 'GOTO': 'JMP',
  'MOVAF': 'MOVA', 'MOVF': 'MOV', 'CLRF': 'CLR',
  'RLF': 'RCL', 'RRF': 'RCR', 'RCLF': 'RCL', 'RCRF': 'RCR',
}
for _m in ('INC', 'DEC', 'SWAP', 'AND', 'IOR', 'XOR', 'ADD', 'SUB', 'BC', 'BS'):
  ALIASES[_m + 'F'] = _m
ALIASES['BTFSC'] = 'BTSC'
ALIASES['BTFSS'] = 'BTSS'

DIRECTIVES = ('EQU', 'ORG', 'DW', 'INCLUDE', 'END')


# ===================================================================================
# Assembler
# ===================================================================================
class AsmError(Exception):
  pass


class Assembler:
  def __init__(self):
    self.symbols = dict(PIOC_SYMBOLS)
    self.code = {}                # address -> word
    self.listing = []             # (address, word or None, source line)

  # --- source handling --------------------------------------------------------------
  def read_lines(self, filename, depth=0):
    if depth > 8:
      raise AsmError('INCLUDE nesting too deep')
    with open(filename, 'rb') as f:
      raw = f.read()
    try:
      text = raw.decode('utf-8')
    except UnicodeDecodeError:
      text = raw.decode('gbk', errors='replace')
    lines = []
    basedir = os.path.dirname(os.path.abspath(filename))
    for num, line in enumerate(text.splitlines(), 1):
      code = self.strip_comment(line)
      tok = code.split()
      if len(tok) >= 2 and tok[0].upper() == 'INCLUDE':
        name = tok[1].strip('"\'<>')
        if name.upper() == 'PIOC_INC.ASM':
          continue                # built in
        lines += self.read_lines(os.path.join(basedir, name), depth + 1)
        continue
      lines.append((filename, num, line.rstrip()))
    return lines

  @staticmethod
  def strip_comment(line):
    out = ''
    quote = False
    for i, c in enumerate(line):
      if c == "'" and (i == 0 or line[i - 1] not in 'HhBbDd' or quote):
        quote = not quote
      if c == ';' and not quote:
        break
      out += c
    return out

  # --- expressions ------------------------------------------------------------------
  def value(self, expr, final):
    expr = expr.strip()
    if not expr:
      raise AsmError('missing operand')

    def num(m):
      t = m.group(0)
      u = t.upper()
      if u.startswith("H'"):
        return str(int(t[2:-1], 16))
      if u.startswith("B'"):
        return str(int(t[2:-1], 2))
      if u.startswith("D'"):
        return str(int(t[2:-1], 10))
      if u.startswith("O'"):
        return str(int(t[2:-1], 8))
      if t.startswith("'"):
        return str(ord(t[1]))
      if u.startswith('0X'):
        return str(int(t[2:], 16))
      if u.startswith('0B') and re.fullmatch(r'0B[01]+', u):
        return str(int(t[2:], 2))
      if u.startswith('0D') and re.fullmatch(r'0D[0-9]+', u):
        return str(int(t[2:], 10))
      if re.fullmatch(r'[0-9][0-9A-F]*H', u):
        return str(int(t[:-1], 16))
      if re.fullmatch(r'[0-9]+', u):
        return str(int(t, 10))
      key = u
      if key in self.symbols:
        return '(%d)' % self.symbols[key]
      if final:
        raise AsmError('undefined symbol: %s' % t)
      return '0'

    tokens = re.sub(r"[HBDOhbdo]'[0-9A-Fa-f]+'|'.'|[A-Za-z_0-9.$]+", num, expr)
    if not re.fullmatch(r"[0-9()+\-*/&|^~<> ]*", tokens):
      raise AsmError('invalid expression: %s' % expr)
    tokens = tokens.replace('/', '//')
    try:
      return int(eval(tokens, {'__builtins__': {}}, {}))
    except Exception:
      raise AsmError('invalid expression: %s' % expr)

  # --- operands ---------------------------------------------------------------------
  @staticmethod
  def split_ops(ops):
    if not ops.strip():
      return []
    parts, depth, cur, quote = [], 0, '', False
    for c in ops:
      if c == "'":
        quote = not quote
      if c == '(' and not quote:
        depth += 1
      if c == ')' and not quote:
        depth -= 1
      if c == ',' and depth == 0 and not quote:
        parts.append(cur.strip())
        cur = ''
      else:
        cur += c
    parts.append(cur.strip())
    return parts

  def dest(self, op):
    u = op.strip().upper()
    if u in ('A', 'W', '0'):
      return 0
    if u in ('F', '1'):
      return 1
    raise AsmError('invalid destination: %s' % op)

  @staticmethod
  def check(val, bits, what, signed_ok=False):
    if signed_ok and -(1 << (bits - 1)) <= val < 0:
      val &= (1 << bits) - 1
    if not 0 <= val < (1 << bits):
      raise AsmError('%s out of range: %d' % (what, val))
    return val

  def encode(self, mnem, ops, addr, final):
    base, typ = INSTR[mnem]
    o = self.split_ops(ops)
    v = lambda i: self.value(o[i], final)
    n = {'': 0, 'k2': 1, 'b3': 1, 'a2': 1, 'ab': 2, 'f': 1, 'F': 1, 'Fd': (1, 2),
         'fd': (1, 2), 'k8': 1, 'k9': 1, 'k10': 1, 'fb': 2, 'j12': 1, 'j10': 1,
         'cmpz': 2}[typ]
    if isinstance(n, tuple):
      if len(o) not in n:
        raise AsmError('%s: wrong number of operands' % mnem)
    elif len(o) != n:
      raise AsmError('%s: wrong number of operands' % mnem)
    if typ == '':
      return base
    if typ in ('k2', 'a2'):
      return base | self.check(v(0), 2, 'operand')
    if typ == 'b3':
      return base | self.check(v(0), 3, 'operand')
    if typ == 'ab':
      return base | self.check(v(0), 2, 'bit selection') << 3 | self.check(v(1), 3, 'bit')
    if typ == 'f':
      return base | self.check(v(0), 8, 'register')
    if typ == 'F':
      return base | self.check(v(0), 9, 'register')
    if typ == 'Fd':
      d = self.dest(o[1]) if len(o) > 1 else 1
      return base | d << 12 | self.check(v(0), 9, 'register')
    if typ == 'fd':
      d = self.dest(o[1]) if len(o) > 1 else 1
      return base | d << 12 | self.check(v(0), 8, 'register')
    if typ == 'k8':
      return base | self.check(v(0), 8, 'constant', True)
    if typ == 'k9':
      return base | self.check(v(0), 9, 'constant')
    if typ == 'k10':
      return base | self.check(v(0), 10, 'constant')
    if typ == 'fb':
      return base | self.check(v(1), 3, 'bit') << 8 | self.check(v(0), 8, 'register')
    if typ == 'j12':
      return base | self.check(v(0), 12, 'address')
    if typ == 'j10':
      t = v(0)
      if final and (t >> 10) != (addr >> 10):
        raise AsmError('%s target 0x%03X not in the same 1K page' % (mnem, t))
      return base | (t & 0x3FF)
    if typ == 'cmpz':
      t = v(1)
      if final and (t >> 8) != (addr >> 8):
        raise AsmError('CMPZ target 0x%03X not in the same 256-word page' % t)
      return base | self.check(v(0), 7, 'constant') << 8 | (t & 0xFF)
    raise AsmError('internal error')

  # --- passes -----------------------------------------------------------------------
  def parse(self, line):
    code = self.strip_comment(line)
    if not code.strip():
      return None, None, ''
    label = None
    if code[0] not in ' \t':
      m = re.match(r'([A-Za-z_.$][A-Za-z_0-9.$]*)\s*:?(.*)$', code)
      if not m:
        raise AsmError('invalid label')
      label, code = m.group(1), m.group(2)
    m = re.match(r'\s*([A-Za-z_][A-Za-z_0-9]*)?\s*(.*)$', code)
    mnem = (m.group(1) or '').upper()
    ops = m.group(2).strip()
    return label, mnem, ops

  def run_pass(self, lines, final):
    addr = 0
    self.code = {}
    self.listing = []
    for fname, num, line in lines:
      try:
        label, mnem, ops = self.parse(line)
        if label is None and not mnem:
          if final:
            self.listing.append((None, None, line))
          continue
        if mnem == 'END':
          if final:
            self.listing.append((None, None, line))
          break
        if mnem == 'EQU':
          if not label:
            raise AsmError('EQU without name')
          self.symbols[label.upper()] = self.value(ops, final)
          if final:
            self.listing.append((None, None, line))
          continue
        if label:
          key = label.upper()
          if not final and key in self.symbols and key not in PIOC_SYMBOLS \
             and self.symbols.get('__PASS1__' + key) is not None:
            raise AsmError('duplicate label: %s' % label)
          self.symbols[key] = addr
          if not final:
            self.symbols['__PASS1__' + key] = addr
        if not mnem:
          if final:
            self.listing.append((addr, None, line))
          continue
        if mnem == 'ORG':
          addr = self.value(ops, True)
          if final:
            self.listing.append((None, None, line))
          continue
        if mnem == 'DW':
          for op in self.split_ops(ops):
            w = self.check(self.value(op, final), 16, 'word', True)
            self.emit(addr, w, line if final else None)
            addr += 1
          continue
        mnem = ALIASES.get(mnem, mnem)
        if mnem not in INSTR:
          raise AsmError('unknown instruction: %s' % mnem)
        w = self.encode(mnem, ops, addr, final)
        self.emit(addr, w, line if final else None)
        addr += 1
      except AsmError as e:
        raise AsmError('%s:%d: %s' % (os.path.basename(fname), num, e))

  def emit(self, addr, word, line):
    if addr >= PIOC_CODE_WORDS:
      raise AsmError('code exceeds 2048 words')
    if addr in self.code:
      raise AsmError('address 0x%03X used twice' % addr)
    self.code[addr] = word
    if line is not None:
      self.listing.append((addr, word, line))

  def assemble(self, filename):
    lines = self.read_lines(filename)
    self.run_pass(lines, False)
    for k in [k for k in self.symbols if k.startswith('__PASS1__')]:
      del self.symbols[k]
    self.run_pass(lines, True)
    if not self.code:
      raise AsmError('no code')
    size = max(self.code) + 1
    words = [self.code.get(a, 0) for a in range(size)]
    return words


# ===================================================================================
# Output
# ===================================================================================
def to_bytes(words):
  out = bytearray()
  for w in words:
    out += bytes((w & 0xFF, w >> 8))
  return bytes(out)


def write_header(fname, name, words, src):
  data = to_bytes(words)
  guard = name.upper()
  with open(fname, 'w') as f:
    f.write('// ===================================================================================\n')
    f.write('// PIOC program "%s" (generated by pioc_asm.py, do not edit)\n' % os.path.basename(src))
    f.write('// ===================================================================================\n')
    f.write('// Size: %d words (%d bytes)\n\n' % (len(words), len(data)))
    f.write('#pragma once\n#include <stdint.h>\n\n')
    f.write('#define %s_SIZE %d\n\n' % (guard, len(data)))
    f.write('static const uint8_t %s[%d] __attribute__((aligned(4))) = {\n' % (guard, len(data)))
    for i in range(0, len(data), 16):
      f.write('  ' + ', '.join('0x%02X' % b for b in data[i:i + 16]) + ',\n')
    f.write('};\n')


def write_listing(fname, asm):
  with open(fname, 'w') as f:
    for addr, word, line in asm.listing:
      a = '%03X' % addr if addr is not None else '   '
      w = '%04X' % word if word is not None else '    '
      f.write('%s  %s  %s\n' % (a, w, line))
    f.write('\nSymbols:\n')
    for k, v in sorted(asm.symbols.items()):
      if k not in PIOC_SYMBOLS:
        f.write('  %-24s 0x%03X\n' % (k, v))


def main():
  p = argparse.ArgumentParser(description='Assembler for the CH32X035 PIOC (RISC8B eMCU)')
  p.add_argument('source', help='assembler source file (.asm)')
  p.add_argument('-o', '--output', help='output C header file (default: <source>.h)')
  p.add_argument('-n', '--name', help='name of the C array (default: PIOC_<SOURCE>)')
  p.add_argument('-b', '--bin', action='store_true', help='also write binary file (.bin)')
  p.add_argument('-l', '--lst', action='store_true', help='also write listing file (.lst)')
  args = p.parse_args()

  base = os.path.splitext(args.source)[0]
  name = args.name or 'PIOC_' + re.sub(r'\W', '_', os.path.basename(base)).upper()
  asm = Assembler()
  try:
    words = asm.assemble(args.source)
  except (AsmError, OSError) as e:
    print('ERROR: %s' % e, file=sys.stderr)
    sys.exit(1)

  write_header(args.output or base + '.h', name, words, args.source)
  if args.bin:
    with open(base + '.bin', 'wb') as f:
      f.write(to_bytes(words))
  if args.lst:
    write_listing(base + '.lst', asm)
  print('%s: %d words (%d bytes) of 2048 words' % (os.path.basename(args.source),
        len(words), 2 * len(words)))


if __name__ == '__main__':
  main()
