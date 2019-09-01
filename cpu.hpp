/******************************************************************************
  GrooveStomp's GB Emulator
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: cpu.hpp
  Created: 2019-08-29
  Updated: 2019-08-30
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
*/

#ifndef CPU_VERSION
#define CPU_VERSION "0.1.0" //!< include guard

#include <vector>
#include <memory>
#include <cstdint>

namespace gs {

//! \file cpu.hpp
//! \see http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf
//! \see http://z80.info/zip/z80cpu_um.pdf

enum reg_pair { AF, BC, DE, HL };
enum reg { B = 0, C, D, E, H, L, A = 7, F = 99 };

class bus;
class operand;

struct instruction;

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

        // Variables and functions to assist in emulation
        uint16_t opcode = 0x0;

        reg operandReg;
        reg_pair operandRegPair;
        std::vector<uint16_t> stack;
        std::shared_ptr<operand> operand1;
        std::shared_ptr<operand> operand2;
        std::shared_ptr<gs::instruction> instruction = nullptr;

        void FlagSet(uint8_t, uint8_t);
        void FlagSet(char, uint8_t);
        uint8_t FlagGet(char);
        uint8_t FlagGet(uint8_t);
        std::shared_ptr<uint8_t> GetRegister(gs::reg);
        uint8_t MaskForBitAt(uint8_t);
};

} // namespace gs

#endif // CPU_VERSION
