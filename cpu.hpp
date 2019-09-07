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

#include <memory> // TODO: Remove dependency
#include <cstdint>

namespace gs {

//! \file cpu.hpp
//! \see http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf
//! \see http://z80.info/zip/z80cpu_um.pdf

class bus;
class operand;

struct instruction;

enum class reg8 { A,B,C,D,E,F,H,L };
enum class reg16 { AF,BC,DE,HL };
enum class reg_sp { SP };
enum class reg_tag { reg8,reg16,regSP };
struct operand_type {
        reg_tag tag;
        union {
                reg8 reg;
                reg16 regPair;
                reg_sp regSP;
        };
};

class cpu {
        // General purpose registers
        uint8_t regB;
        uint8_t regC;
        uint8_t regD;
        uint8_t regE;
        uint8_t regH;
        uint8_t regL;
        uint8_t regBp;
        uint8_t regCp;
        uint8_t regDp;
        uint8_t regEp;
        uint8_t regHp;
        uint8_t regLp;

        // Accumulator and flag registers
        uint8_t regA;
        uint8_t regF;
        uint8_t regAp;
        uint8_t regFp;

        // Special purpose registers
        uint8_t r; //!< memory refresh
        uint16_t pc; //!< program counter
        uint16_t sp; //!< stack pointer
        uint16_t i; //!< interrupt vector

        std::shared_ptr<gs::bus> msgBus;

public:
        cpu(bus *messageBus);
        ~cpu();

        void InstructionFetch(uint8_t memory[]);
        void InstructionDecode();
        void InstructionExecute();

        // Addressing Modes
        std::shared_ptr<operand> IMM();
        std::shared_ptr<operand> IME();
        std::shared_ptr<operand> MPZ();
        std::shared_ptr<operand> REL();
        std::shared_ptr<operand> EXT();
        std::shared_ptr<operand> IDX();
        std::shared_ptr<operand> REG();
        std::shared_ptr<operand> IMP();
        std::shared_ptr<operand> RPR();
        std::shared_ptr<operand> IND();
        std::shared_ptr<operand> BTA();
        std::shared_ptr<operand> BTI();

        // Load operations
        void LD();
        void LDD();
        void LDI();
        void LDH();
        void LDHL();

        // Arithmetic/logic operations
        void PUSH();
        void POP();
        void ADD();
        void ADC();
        void SUB();
        void SBC();
        void AND();
        void OR();
        void XOR();
        void CP();
        void INC();
        void DEC();

        // Miscellaneous operations
        void SWAP();
        void DAA();
        void CPL();
        void CCF();
        void SCF();
        void NOP();
        void HALT();
        void STOP();
        void DI();
        void EI();

        // Rotate and shift operations
        void RLCA();
        void RLA();
        void RRCA();
        void RRA();
        void RLC();
        void RL();
        void RRC();
        void RR();
        void SLA();
        void SRA();
        void SRL();

        // Bit operations
        void BIT();
        void SET();
        void RES();

        void JP(); // Jump
        void JR(); // Jump
        void CALL(); // Call
        void RST(); // Restart
        void RET(); // Return
        void RETI(); // Return, enabling interrupts

private:
        bool halted = false;
        bool waitForButtonPress = false;
        bool interruptsDisabled = false;
        bool interruptsDisabledRequested = false;
        bool interruptsEnabledRequested = false;

        // Variables and functions to assist in emulation
        uint16_t opcode = 0x0;

        operand_type operandType;
        std::shared_ptr<operand> operand1;
        std::shared_ptr<operand> operand2;
        std::shared_ptr<gs::instruction> instruction = nullptr;

        void FlagSet(uint8_t, uint8_t);
        void FlagSet(char, uint8_t);
        uint8_t FlagGet(char);
        uint8_t FlagGet(uint8_t);
        std::shared_ptr<uint8_t> GetRegister(reg8);
        uint8_t MaskForBitAt(uint8_t);
};

} // namespace gs

#endif // CPU_VERSION
