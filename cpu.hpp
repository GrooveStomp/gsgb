/******************************************************************************
  GrooveStomp's GB Emulator
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: cpu.hpp
  Created: 2019-08-29
  Updated: 2019-09-07
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)

  This program comes with ABSOLUTELY NO WARRANTY. This is free software, and
  you are welcome to redistribute it under certain conditions; See LICENSE for
  details.
 ******************************************************************************/

/*
  NOTE / TODO
  The current implementation of opcodes here tries to pre-allocated data statically and then use that data at runtime.
  I don't like this approach because the opcodes are not homogenous, so each instruction needs to include special conditions for each opcode.

  I think a better approach is to dynamically configure the following instruction.
  Here's an example in the current system:

      // statically allocate metadata:
      instructions = {
        ...,
        { 0x00F5, { "PUSH AF", PUSH, IMP, RPR, 16 } },
        ...
      }

      void cpu::PUSH() {
        switch (opcode) {
          ...
          case 0xF5:
            ...
            break;
          ...
        }

        // Do instruction here
      }

  And here's what I'm suggesting:

// statically allocate metadata:
opcodes = {
  ...
  { 0x00F5, { Opcode_0x00F5, "PUSH AF", 16 } }
  ...
}

void cpu::Execute() {
  ...
  opcodes[opcode]->fn();
  ...
}

void cpu::Opcode_0x00F5() {
  operand1 = std::make_shared<operand_sp_reference>();
  operand1->ref = regSP

  operand2 = std::make_shared<operand_pair_reference>();
  operand2->ref1 = A;
  operand2->ref2 = F;

  ADD();
}
 */

/*
  Flags:
  C: Carry
  N: Add/Subtract
  P: Parity/Overflow
  V: Parity/Overflow
  H: Half Carry Flag
  Z: Zero Flag
  S: Sign Flag
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

        // General purpose registers
        union {
                uint16_t BC;
                struct {
                        uint8_t B;
                        uint8_t C;
                };
        };
        union {
                uint16_t DE;
                struct {
                        uint8_t D;
                        uint8_t E;
                };
        };
        union {
                uint16_t HL;
                struct {
                        uint8_t H;
                        uint8_t L;
                };
        };

        // Accumulator and Flag registers
        union {
                uint16_t AF;
                struct {
                        uint8_t A;
                        uint8_t F;
                };
        };

        // Special purpose registers
        uint8_t r; //!< memory refresh
        uint16_t pc; //!< program counter
        uint16_t sp; //!< stack pointer
        uint16_t i; //!< interrupt vector

        uint16_t opcode = 0x0;
        bus *msgBus;
private:
        friend class instruction;
        class impl;
        impl *implementation;
};

} // namespace gs

#endif // CPU_VERSION
