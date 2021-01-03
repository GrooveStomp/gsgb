/******************************************************************************
 * File: cpu.hpp
 * Created: 2019-08-29
 * Updated: 2020-12-30
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2020, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/

/*
  Flags:
  C: Carry
  N: Add/Subtract
  P: Parity/Overflow
  V: Parity/Overflow
  H: Half Carry Flag
  Z: Zero Flag
  S: Sign Flag

The Carry Flag (C) is set or cleared depending on the operation being performed. For
ADD Instructions that generate a Carry, and for SUB Instructions that generate a Borrow,
the Carry Flag is set. The Carry Flag is reset by an ADD Instruction that does not generate
a Carry, and by a SUB Instruction that does not generate a Borrow. This saved Carry facil-
itates software routines for extended precision arithmetic. Additionally, the DAA instruc-
tion sets the Carry Flag if the conditions for making the decimal adjustment are met.
For the RLA, RRA, RLS, and RRS Instructions, the Carry bit is used as a link between the
least-significant byte (LSB) and the most-significant byte (MSB) for any register or mem-
ory location. During the RLCA, RLC, and SLA Instructions, the Carry flag contains the
final value shifted out of bit 7 of any register or memory location. During the RRCA,
RRC, SRA, and SRL Instructions, the Carry flag contains the final value shifted out of bit
0 of any register or memory location.
For the logical Instructions AND, OR, and XOR, the Carry flag is reset.
The Carry flag can also be set by the Set Carry Flag (SCF) Instruction and complemented
by the Compliment Carry Flag (CCF) Instruction. (See p82 of Z80 CPU User Manual.)

The Half Carry Flag (H) is set (1) or cleared (0) depending on the Carry and Borrow status
between bits 3 and 4 of an 8-bit arithmetic operation. This flag is used by the Decimal
Adjust Accumulator (DAA) Instruction to correct the result of a packed BCD add or sub-
tract operation. The H Flag is set (1) or cleared (0) as shown:

| H Flag | Add                                 | Subtract                    |
|--------+-------------------------------------+-----------------------------|
|      1 | A Carry occurs from bit 3 to bit 4  | A Borrow from bit 4 occurs  |
|      0 | No Carry occurs from bit 3 to bit 4 | No Borrow from bit 4 occurs |

(See p82 of Z80 CPU User Manual.)
*/

#ifndef CPU_VERSION
#define CPU_VERSION "0.1.0" //!< include guard

#include <cstdint>

namespace gs {

//! \file cpu.hpp
//! \see http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf
//! \see http://z80.info/zip/z80Cpu_um.pdf

class Bus;
class Instruction;

class Cpu {
public:
        Cpu();
        ~Cpu();

        void instructionFetch();
        void instructionExecute();
        char *instructionDesc();

        void flagSet(uint8_t, uint8_t);
        void flagSet(char, uint8_t);
        uint8_t flagGet(char);
        uint8_t flagGet(uint8_t);

        void attach(Bus *bus);
        void reset();

        void dumpState();

        union {
                struct r8_type {
                        uint8_t C;
                        uint8_t B;
                        uint8_t E;
                        uint8_t D;
                        uint8_t L;
                        uint8_t H;

                        // Special purpose accumulator and flag.
                        uint8_t F;
                        uint8_t A;
                } r8;
                struct r16_type {
                        uint16_t BC;
                        uint16_t DE;
                        uint16_t HL;
                        uint16_t AF; // Special purpose accumulator and flag.
                } r16;
        } registers;

        // Special purpose registers
        uint8_t R; //!< memory refresh
        uint16_t PC; //!< program counter
        uint16_t SP; //!< stack pointer
        uint16_t I; //!< interrupt vector

        uint16_t opcode = 0x0;

private:
        friend class Instruction;
        class Impl;
        Bus *bus;
        Impl *impl;
};

} // namespace gs

#endif // CPU_VERSION
