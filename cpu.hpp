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

#ifndef CPU_VERSION
#define CPU_VERSION "0.1.0" //!< include guard

#include <memory>
#include <cstdint>

//! \file cpu.hpp
//! \see http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf
//! \see http://z80.info/zip/z80cpu_um.pdf

enum reg_pair { AF, BC, DE, HL };
enum reg { A, F, B, C, D, E, H, L };

class bus;
class cpu_priv;

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

        std::shared_ptr<bus> msgBus;

        uint16_t opcode;

        uint16_t operand;
        int operandNumBytes;

public:
        cpu(bus *messageBus);
        ~cpu();

        void InstructionFetch(uint8_t memory[]);
        void InstructionDecode();
        void InstructionExecute();

        // Addressing Modes
        void IMM();
        void IME();
        void MPZ();
        void REL();
        void EXT();
        void IDX();
        void REG();
        void IMP();
        void IND();
        void BTA();

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
        cpu_priv *priv;
};

#endif // CPU_VERSION
