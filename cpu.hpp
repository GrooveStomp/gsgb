/******************************************************************************
  GrooveStomp's GB Emulator
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: cpu.hpp
  Created: 2019-08-29
  Updated: 2019-09-10
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)

  This program comes with ABSOLUTELY NO WARRANTY. This is free software, and
  you are welcome to redistribute it under certain conditions; See LICENSE for
  details.
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
ADD instructions that generate a Carry, and for SUB instructions that generate a Borrow,
the Carry Flag is set. The Carry Flag is reset by an ADD instruction that does not generate
a Carry, and by a SUB instruction that does not generate a Borrow. This saved Carry facil-
itates software routines for extended precision arithmetic. Additionally, the DAA instruc-
tion sets the Carry Flag if the conditions for making the decimal adjustment are met.
For the RLA, RRA, RLS, and RRS instructions, the Carry bit is used as a link between the
least-significant byte (LSB) and the most-significant byte (MSB) for any register or mem-
ory location. During the RLCA, RLC, and SLA instructions, the Carry flag contains the
final value shifted out of bit 7 of any register or memory location. During the RRCA,
RRC, SRA, and SRL instructions, the Carry flag contains the final value shifted out of bit
0 of any register or memory location.
For the logical instructions AND, OR, and XOR, the Carry flag is reset.
The Carry flag can also be set by the Set Carry Flag (SCF) instruction and complemented
by the Compliment Carry Flag (CCF) instruction. (See p82 of Z80 CPU User Manual.)

The Half Carry Flag (H) is set (1) or cleared (0) depending on the Carry and Borrow status
between bits 3 and 4 of an 8-bit arithmetic operation. This flag is used by the Decimal
Adjust Accumulator (DAA) instruction to correct the result of a packed BCD add or sub-
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
//! \see http://z80.info/zip/z80cpu_um.pdf

class bus;
class instruction;

class cpu {
public:
        cpu(bus *messageBus);
        ~cpu();

        void InstructionFetch();
        void InstructionExecute();
        char *InstructionDesc();

        void FlagSet(uint8_t, uint8_t);
        void FlagSet(char, uint8_t);
        uint8_t FlagGet(char);
        uint8_t FlagGet(uint8_t);

        union {
                struct r8_type {
                        uint8_t B;
                        uint8_t C;
                        uint8_t D;
                        uint8_t E;
                        uint8_t H;
                        uint8_t L;

                        // Special purpose accumulator and flag.
                        uint8_t A;
                        uint8_t F;
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
        bus *msgBus;
private:
        friend class instruction;
        class impl;
        impl *implementation;
};

} // namespace gs

#endif // CPU_VERSION
