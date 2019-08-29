/******************************************************************************
  GrooveStomp's GB Emulator
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: cpu.cpp
  Created: 2019-08-29
  Updated: 2019-08-29
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)

  This program comes with ABSOLUTELY NO WARRANTY. This is free software, and
  you are welcome to redistribute it under certain conditions; See LICENSE for
  details.
 ******************************************************************************/

#include "cpu.hpp"

struct instruction {
        std::string name;
        void (Cpu::*operation)(void) = nullptr;
        void (Cpu::*addressMode)(void) = nullptr;
        uint8_t cycles = 0;
};

//------------------------------------------------------------------------------
// Addressing modes
//------------------------------------------------------------------------------

//! Immediate
void Cpu::IMM() { }

//! Immediate Extended
void Cpu::IME() { }

//! Modified Page Zero
void Cpu::MPZ() { }

//! Relative
void Cpu::REL() { }

//! Extended
void Cpu::EXT() { }

//! Indexed
void Cpu::IDX() { }

//! Register
void Cpu::REG() { }

//! Implied
void Cpu::IMP() { }

//! Register Indirect
void Cpu::IND() { }

//! Bit
void Cpu::BIT() { }

//------------------------------------------------------------------------------
// Operations
//------------------------------------------------------------------------------

void Cpu::LD() { }

void Cpu::

void Cpu::InstructionFetch(uint8_t memory[]) {
        uint8_t byte = memory[pc++];
        switch (byte) {
                case 0x7F:
                        LD('B','A')
        }
}
