/******************************************************************************
  GrooveStomp's GB Emulator
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: cpu.hpp
  Created: 2019-08-29
  Updated: 2019-08-29
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)

  This program comes with ABSOLUTELY NO WARRANTY. This is free software, and
  you are welcome to redistribute it under certain conditions; See LICENSE for
  details.
 ******************************************************************************/
#ifndef CPU_VERSION
#define CPU_VERSION "0.1.0" //!< include guard

#include <cstdint>

//! \file cpu.hpp
//! \see http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf
//! \see http://z80.info/zip/z80cpu_um.pdf

class Cpu {
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

public:
        void InstructionFetch(uint8_t memory[]);
        void InstructionDecode();
        void InstructionExecute();
};

#endif // CPU_VERSION
