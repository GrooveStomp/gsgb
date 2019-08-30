/******************************************************************************
  GrooveStomp's GB Emulator
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: cpu.cpp
  Created: 2019-08-29
  Updated: 2019-08-30
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)

  This program comes with ABSOLUTELY NO WARRANTY. This is free software, and
  you are welcome to redistribute it under certain conditions; See LICENSE for
  details.
 ******************************************************************************/

#include <string>
#include <map>

#include "cpu.hpp"
#include "bus.hpp"

struct instruction {
        std::string name;
        void (cpu::*operation)();
        void (cpu::*addr1)();
        void (cpu::*addr2)();
        unsigned int cycles;
};

static std::map<int,instruction> instructionMap = {
        // 8-bit Load
        { 0x0006, { "LD B,n", &cpu::LD, &cpu::REG, &cpu::IMM, 8 } },
        { 0x000E, { "LD C,n", &cpu::LD, &cpu::REG, &cpu::IMM, 8 } },
        { 0x0016, { "LD D,n", &cpu::LD, &cpu::REG, &cpu::IMM, 8 } },
        { 0x001E, { "LD E,n", &cpu::LD, &cpu::REG, &cpu::IMM, 8 } },
        { 0x0026, { "LD H,n", &cpu::LD, &cpu::REG, &cpu::IMM, 8 } },
        { 0x002E, { "LD L,n", &cpu::LD, &cpu::REG, &cpu::IMM, 8 } },
        { 0x007F, { "LD A,A", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0078, { "LD A,B", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0079, { "LD A,C", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x007A, { "LD A,D", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x007B, { "LD A,E", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x007C, { "LD A,H", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x007D, { "LD A,L", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x007E, { "LD A,(HL)", &cpu::LD, &cpu::REG, &cpu::IND, 8 } },
        { 0x0040, { "LD B,B", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0041, { "LD B,C", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0042, { "LD B,D", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0043, { "LD B,E", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0044, { "LD B,H", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0045, { "LD B,L", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0046, { "LD B,(HL)", &cpu::LD, &cpu::REG, &cpu::IND, 8 } },
        { 0x0048, { "LD C,B", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0049, { "LD C,C", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x004A, { "LD C,D", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x004B, { "LD C,E", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x004C, { "LD C,H", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x004D, { "LD C,L", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x004E, { "LD C,(HL)", &cpu::LD, &cpu::REG, &cpu::IND, 8 } },
        { 0x0050, { "LD D,B", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0051, { "LD D,C", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0052, { "LD D,D", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0053, { "LD D,E", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0054, { "LD D,H", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0055, { "LD D,L", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0056, { "LD D,(HL)", &cpu::LD, &cpu::REG, &cpu::IND, 8 } },
        { 0x0058, { "LD E,B", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0059, { "LD E,C", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x005A, { "LD E,D", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x005B, { "LD E,E", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x005C, { "LD E,H", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x005D, { "LD E,L", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x005E, { "LD E,(HL)", &cpu::LD, &cpu::REG, &cpu::IND, 8 } },
        { 0x0060, { "LD H,B", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0061, { "LD H,C", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0062, { "LD H,D", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0063, { "LD H,E", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0064, { "LD H,H", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0065, { "LD H,L", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0066, { "LD H,(HL)", &cpu::LD, &cpu::REG, &cpu::IND, 8 } },
        { 0x0068, { "LD L,B", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0069, { "LD L,C", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x006A, { "LD L,D", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x006B, { "LD L,E", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x006C, { "LD L,H", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x006D, { "LD L,L", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x006E, { "LD L,(HL)", &cpu::LD, &cpu::REG, &cpu::IND, 8 } },
        { 0x0070, { "LD (HL),B", &cpu::LD, &cpu::IND, &cpu::REG, 8 } },
        { 0x0071, { "LD (HL),C", &cpu::LD, &cpu::IND, &cpu::REG, 8 } },
        { 0x0072, { "LD (HL),D", &cpu::LD, &cpu::IND, &cpu::REG, 8 } },
        { 0x0073, { "LD (HL),E", &cpu::LD, &cpu::IND, &cpu::REG, 8 } },
        { 0x0074, { "LD (HL),H", &cpu::LD, &cpu::IND, &cpu::REG, 8 } },
        { 0x0075, { "LD (HL),L", &cpu::LD, &cpu::IND, &cpu::REG, 8 } },
        { 0x0036, { "LD (HL),n", &cpu::LD, &cpu::IND, &cpu::IMM, 12 } },
        { 0x007F, { "LD A,A", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0078, { "LD A,B", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0079, { "LD A,C", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x007A, { "LD A,D", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x007B, { "LD A,E", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x007C, { "LD A,H", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x007D, { "LD A,L", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x000A, { "LD A,(BC)", &cpu::LD, &cpu::REG, &cpu::IND, 8 } },
        { 0x001A, { "LD A,(DE)", &cpu::LD, &cpu::REG, &cpu::IND, 8 } },
        { 0x007E, { "LD A,(HL)", &cpu::LD, &cpu::REG, &cpu::IND, 8 } },
        { 0x00FA, { "LD A,(nn)", &cpu::LD, &cpu::REG, &cpu::EXT, 16 } },
        { 0x003E, { "LD A,n", &cpu::LD, &cpu::REG, &cpu::IMM, 8 } },
        { 0x0047, { "LD A,B", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x004F, { "LD A,C", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0057, { "LD A,D", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x005F, { "LD A,E", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0067, { "LD A,H", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x006F, { "LD A,L", &cpu::LD, &cpu::REG, &cpu::REG, 4 } },
        { 0x000A, { "LD (BC),A", &cpu::LD, &cpu::IND, &cpu::REG, 8 } },
        { 0x001A, { "LD (DE),A", &cpu::LD, &cpu::IND, &cpu::REG, 8 } },
        { 0x007E, { "LD (HL),A", &cpu::LD, &cpu::IND, &cpu::REG, 8 } },
        { 0x00FA, { "LD (nn),A", &cpu::LD, &cpu::EXT, &cpu::REG, 16 } },
        { 0x00F2, { "LDH A,(0x00FF00+C)", &cpu::LDH, &cpu::REG, &cpu::IND, 8 } },
        { 0x00E2, { "LDH (0x00FF00+C),A", &cpu::LDH, &cpu::IND, &cpu::REG, 8 } },
        { 0x003A, { "LDD A,(HL)", &cpu::LDD, &cpu::REG, &cpu::IND, 8 } },
        { 0x0032, { "LDD (HL),A", &cpu::LDD, &cpu::IND, &cpu::REG, 8 } },
        { 0x002A, { "LDI A,(HL)", &cpu::LDI, &cpu::REG, &cpu::IND, 8 } },
        { 0x0022, { "LDI (HL),A", &cpu::LDI, &cpu::IND, &cpu::REG, 8 } },
        { 0x002A, { "LDH (0x00FF00+n),A", &cpu::LDH, &cpu::IND, &cpu::REG, 12 } },
        { 0x0022, { "LDH A,(0x00FF00+n)", &cpu::LDH, &cpu::REG, &cpu::IND, 12 } },

        // 16-bit Load
        { 0x0001, { "LD BC,nn", &cpu::LD, &cpu::REG, &cpu::EXT, 12 } },
        { 0x0011, { "LD DE,nn", &cpu::LD, &cpu::REG, &cpu::EXT, 12 } },
        { 0x0021, { "LD HL,nn", &cpu::LD, &cpu::REG, &cpu::EXT, 12 } },
        { 0x0031, { "LD SP,nn", &cpu::LD, &cpu::REG, &cpu::EXT, 12 } },
        { 0x00F9, { "LD SP,HL", &cpu::LD, &cpu::REG, &cpu::REG, 8 } },
        { 0x00F8, { "LDHL SP,n", &cpu::LDHL, &cpu::REG, &cpu::IMM, 12 } },
        { 0x0008, { "LD (nn),SP", &cpu::LD, &cpu::EXT, &cpu::REG, 20 } },
        { 0x00F5, { "PUSH AF", &cpu::PUSH, &cpu::IMP, &cpu::REG, 16 } },
        { 0x00C5, { "PUSH BC", &cpu::PUSH, &cpu::IMP, &cpu::REG, 16 } },
        { 0x00D5, { "PUSH DE", &cpu::PUSH, &cpu::IMP, &cpu::REG, 16 } },
        { 0x00E5, { "PUSH HL", &cpu::PUSH, &cpu::IMP, &cpu::REG, 16 } },
        { 0x00F1, { "POP AF", &cpu::POP, &cpu::IMP, &cpu::REG, 12 } },
        { 0x00C1, { "POP BC", &cpu::POP, &cpu::IMP, &cpu::REG, 12 } },
        { 0x00D1, { "POP DE", &cpu::POP, &cpu::IMP, &cpu::REG, 12 } },
        { 0x00E1, { "POP HL", &cpu::POP, &cpu::IMP, &cpu::REG, 12 } },

        // 8-bit ALU
        { 0x0087, { "ADD A,A", &cpu::ADD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0080, { "ADD A,B", &cpu::ADD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0081, { "ADD A,C", &cpu::ADD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0082, { "ADD A,D", &cpu::ADD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0083, { "ADD A,E", &cpu::ADD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0084, { "ADD A,H", &cpu::ADD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0085, { "ADD A,L", &cpu::ADD, &cpu::REG, &cpu::REG, 4 } },
        { 0x0086, { "ADD A,(HL)", &cpu::ADD, &cpu::REG, &cpu::IND, 8 } },
        { 0x00C6, { "ADD A,nn", &cpu::ADD, &cpu::REG, &cpu::IMM, 8 } },

        { 0x008F, { "ADC A,A", &cpu::ADC, &cpu::REG, &cpu::REG, 4 } },
        { 0x0088, { "ADC A,B", &cpu::ADC, &cpu::REG, &cpu::REG, 4 } },
        { 0x0089, { "ADC A,C", &cpu::ADC, &cpu::REG, &cpu::REG, 4 } },
        { 0x008A, { "ADC A,D", &cpu::ADC, &cpu::REG, &cpu::REG, 4 } },
        { 0x008B, { "ADC A,E", &cpu::ADC, &cpu::REG, &cpu::REG, 4 } },
        { 0x008C, { "ADC A,H", &cpu::ADC, &cpu::REG, &cpu::REG, 4 } },
        { 0x008D, { "ADC A,L", &cpu::ADC, &cpu::REG, &cpu::REG, 4 } },
        { 0x008E, { "ADC A,(HL)", &cpu::ADC, &cpu::REG, &cpu::REG, 8 } },
        { 0x00CE, { "ADC A,nn", &cpu::ADC, &cpu::REG, &cpu::IMM, 8 } },

        { 0x0097, { "SUB A", &cpu::SUB, &cpu::IMP, &cpu::REG, 4 } },
        { 0x0090, { "SUB B", &cpu::SUB, &cpu::IMP, &cpu::REG, 4 } },
        { 0x0091, { "SUB C", &cpu::SUB, &cpu::IMP, &cpu::REG, 4 } },
        { 0x0092, { "SUB D", &cpu::SUB, &cpu::IMP, &cpu::REG, 4 } },
        { 0x0093, { "SUB E", &cpu::SUB, &cpu::IMP, &cpu::REG, 4 } },
        { 0x0094, { "SUB H", &cpu::SUB, &cpu::IMP, &cpu::REG, 4 } },
        { 0x0095, { "SUB L", &cpu::SUB, &cpu::IMP, &cpu::REG, 4 } },
        { 0x0096, { "SUB (HL)", &cpu::SUB, &cpu::IMP, &cpu::REG, 8 } },
        { 0x00D6, { "SUB nn", &cpu::SUB, &cpu::IMP, &cpu::IMM, 8 } },

        { 0x009F, { "SBC A,A", &cpu::SBC, &cpu::REG, &cpu::REG, 4 } },
        { 0x0098, { "SBC A,B", &cpu::SBC, &cpu::REG, &cpu::REG, 4 } },
        { 0x0099, { "SBC A,C", &cpu::SBC, &cpu::REG, &cpu::REG, 4 } },
        { 0x009A, { "SBC A,D", &cpu::SBC, &cpu::REG, &cpu::REG, 4 } },
        { 0x009B, { "SBC A,E", &cpu::SBC, &cpu::REG, &cpu::REG, 4 } },
        { 0x009C, { "SBC A,H", &cpu::SBC, &cpu::REG, &cpu::REG, 4 } },
        { 0x009D, { "SBC A,L", &cpu::SBC, &cpu::REG, &cpu::REG, 4 } },
        { 0x009E, { "SBC A,(HL)", &cpu::SBC, &cpu::REG, &cpu::IND, 8 } },
//                { 0x00??, { "SBC A,nn", &cpu::SBC, &cpu::IMP, &cpu::IMM, ? } },

        { 0x00A7, { "AND A", &cpu::AND, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00A0, { "AND B", &cpu::AND, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00A1, { "AND C", &cpu::AND, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00A2, { "AND D", &cpu::AND, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00A3, { "AND E", &cpu::AND, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00A4, { "AND H", &cpu::AND, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00A5, { "AND L", &cpu::AND, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00A6, { "AND (HL)", &cpu::AND, &cpu::IMP, &cpu::IND, 8 } },
        { 0x00E6, { "AND nn", &cpu::AND, &cpu::IMP, &cpu::REG, 8 } },

        { 0x00B7, { "OR A", &cpu::OR, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00B0, { "OR B", &cpu::OR, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00B1, { "OR C", &cpu::OR, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00B2, { "OR D", &cpu::OR, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00B3, { "OR E", &cpu::OR, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00B4, { "OR H", &cpu::OR, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00B5, { "OR L", &cpu::OR, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00B6, { "OR (HL)", &cpu::OR, &cpu::IMP, &cpu::IND, 8 } },
        { 0x00F6, { "OR nn", &cpu::OR, &cpu::IMP, &cpu::REG, 8 } },

        { 0x00AF, { "XOR A", &cpu::XOR, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00A8, { "XOR B", &cpu::XOR, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00A9, { "XOR C", &cpu::XOR, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00AA, { "XOR D", &cpu::XOR, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00AB, { "XOR E", &cpu::XOR, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00AC, { "XOR H", &cpu::XOR, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00AD, { "XOR L", &cpu::XOR, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00AE, { "XOR (HL)", &cpu::XOR, &cpu::IMP, &cpu::IND, 8 } },
        { 0x00EE, { "XOR nn", &cpu::XOR, &cpu::IMP, &cpu::REG, 8 } },

        { 0x00BF, { "CP A", &cpu::CP, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00B8, { "CP B", &cpu::CP, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00B9, { "CP C", &cpu::CP, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00BA, { "CP D", &cpu::CP, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00BB, { "CP E", &cpu::CP, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00BC, { "CP H", &cpu::CP, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00BD, { "CP L", &cpu::CP, &cpu::IMP, &cpu::REG, 4 } },
        { 0x00BE, { "CP (HL)", &cpu::CP, &cpu::IMP, &cpu::IND, 8 } },
        { 0x00FE, { "CP nn", &cpu::CP, &cpu::IMP, &cpu::REG, 8 } },

        { 0x003C, { "INC A", &cpu::INC, &cpu::IMP, &cpu::REG, 4 } },
        { 0x0004, { "INC B", &cpu::INC, &cpu::IMP, &cpu::REG, 4 } },
        { 0x000C, { "INC C", &cpu::INC, &cpu::IMP, &cpu::REG, 4 } },
        { 0x0014, { "INC D", &cpu::INC, &cpu::IMP, &cpu::REG, 4 } },
        { 0x001C, { "INC E", &cpu::INC, &cpu::IMP, &cpu::REG, 4 } },
        { 0x0024, { "INC H", &cpu::INC, &cpu::IMP, &cpu::REG, 4 } },
        { 0x002C, { "INC L", &cpu::INC, &cpu::IMP, &cpu::REG, 4 } },
        { 0x0034, { "INC (HL)", &cpu::INC, &cpu::IMP, &cpu::IND, 12 } },

        { 0x003D, { "DEC A", &cpu::DEC, &cpu::IMP, &cpu::REG, 4 } },
        { 0x0005, { "DEC B", &cpu::DEC, &cpu::IMP, &cpu::REG, 4 } },
        { 0x000D, { "DEC C", &cpu::DEC, &cpu::IMP, &cpu::REG, 4 } },
        { 0x0015, { "DEC D", &cpu::DEC, &cpu::IMP, &cpu::REG, 4 } },
        { 0x001D, { "DEC E", &cpu::DEC, &cpu::IMP, &cpu::REG, 4 } },
        { 0x0025, { "DEC H", &cpu::DEC, &cpu::IMP, &cpu::REG, 4 } },
        { 0x002D, { "DEC L", &cpu::DEC, &cpu::IMP, &cpu::REG, 4 } },
        { 0x0035, { "DEC (HL)", &cpu::DEC, &cpu::IMP, &cpu::IND, 12 } },

        // 16-bit Arithmetic
        { 0x0009, { "ADD HL,BC", &cpu::ADD, &cpu::IND, &cpu::IMM, 8 } },
        { 0x0019, { "ADD HL,DE", &cpu::ADD, &cpu::IND, &cpu::IMM, 8 } },
        { 0x0029, { "ADD HL,HL", &cpu::ADD, &cpu::IND, &cpu::IMM, 8 } },
        { 0x0039, { "ADD HL,SP", &cpu::ADD, &cpu::IND, &cpu::IMM, 8 } },

        { 0x00E8, { "ADD SP,nn", &cpu::ADD, &cpu::IND, &cpu::IMM, 16 } },

        { 0x000B, { "DEC BC", &cpu::DEC, &cpu::IMP, &cpu::IND, 8 } },
        { 0x001B, { "DEC DE", &cpu::DEC, &cpu::IMP, &cpu::IND, 8 } },
        { 0x002B, { "DEC HL", &cpu::DEC, &cpu::IMP, &cpu::IND, 8 } },
        { 0x003B, { "DEC SP", &cpu::DEC, &cpu::IMP, &cpu::IND, 8 } },

        // Miscellaneous
        { 0x0037, { "SWAP A", &cpu::SWAP, &cpu::IMP, &cpu::REG, 8 } },
        { 0x0030, { "SWAP B", &cpu::SWAP, &cpu::IMP, &cpu::REG, 8 } },
        { 0x0031, { "SWAP C", &cpu::SWAP, &cpu::IMP, &cpu::REG, 8 } },
        { 0x0032, { "SWAP D", &cpu::SWAP, &cpu::IMP, &cpu::REG, 8 } },
        { 0x0033, { "SWAP E", &cpu::SWAP, &cpu::IMP, &cpu::REG, 8 } },
        { 0x0034, { "SWAP H", &cpu::SWAP, &cpu::IMP, &cpu::REG, 8 } },
        { 0x0035, { "SWAP L", &cpu::SWAP, &cpu::IMP, &cpu::REG, 8 } },
        { 0x0036, { "SWAP (HL)", &cpu::SWAP, &cpu::IMP, &cpu::IND, 16 } },

        { 0x0027, { "DAA", &cpu::DAA, &cpu::IMP, &cpu::IMP, 4 } },
        { 0x002F, { "CPL", &cpu::CPL, &cpu::IMP, &cpu::IMP, 4 } },
        { 0x003F, { "CCF", &cpu::CCF, &cpu::IMP, &cpu::IMP, 4 } },
        { 0x0037, { "SCF", &cpu::SCF, &cpu::IMP, &cpu::IMP, 4 } },
        { 0x0000, { "NOP", &cpu::NOP, &cpu::IMP, &cpu::IMP, 4 } },
        { 0x0076, { "HALT", &cpu::HALT, &cpu::IMP, &cpu::IMP, 4 } },
        { 0x1000, { "STOP", &cpu::STOP, &cpu::IMP, &cpu::IMP, 4 } },
        { 0x00F3, { "DI", &cpu::DI, &cpu::IMP, &cpu::IMP, 4 } },
        { 0x00FB, { "EI", &cpu::EI, &cpu::IMP, &cpu::IMP, 4 } },

        // Rotates and Shifts
        { 0x0007, { "RLCA", &cpu::RLCA, &cpu::IMP, &cpu::IMP, 4 } },
        { 0x0017, { "RLA", &cpu::RLA, &cpu::IMP, &cpu::IMP, 4 } },
        { 0x000F, { "RRCA", &cpu::RRCA, &cpu::IMP, &cpu::IMP, 4 } },
        { 0x001F, { "RRA", &cpu::RRA, &cpu::IMP, &cpu::IMP, 4 } },

        { 0xCB07, { "RLC A", &cpu::RLC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB00, { "RLC B", &cpu::RLC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB01, { "RLC C", &cpu::RLC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB02, { "RLC D", &cpu::RLC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB03, { "RLC E", &cpu::RLC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB04, { "RLC H", &cpu::RLC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB05, { "RLC L", &cpu::RLC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB06, { "RLC (HL)", &cpu::RLC, &cpu::IMP, &cpu::REG, 16 } },

        { 0xCB17, { "RL A", &cpu::RL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB10, { "RL B", &cpu::RL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB11, { "RL C", &cpu::RL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB12, { "RL D", &cpu::RL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB13, { "RL E", &cpu::RL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB14, { "RL H", &cpu::RL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB15, { "RL L", &cpu::RL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB16, { "RL (HL)", &cpu::RL, &cpu::IMP, &cpu::REG, 16 } },

        { 0xCB0F, { "RRC A", &cpu::RRC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB08, { "RRC B", &cpu::RRC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB09, { "RRC C", &cpu::RRC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB0A, { "RRC D", &cpu::RRC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB0B, { "RRC E", &cpu::RRC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB0C, { "RRC H", &cpu::RRC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB0D, { "RRC L", &cpu::RRC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB0E, { "RRC (HL)", &cpu::RRC, &cpu::IMP, &cpu::REG, 16 } },

        { 0xCB1F, { "RR A", &cpu::RR, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB18, { "RR B", &cpu::RR, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB19, { "RR C", &cpu::RR, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB1A, { "RR D", &cpu::RR, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB1B, { "RR E", &cpu::RR, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB1C, { "RR H", &cpu::RR, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB1D, { "RR L", &cpu::RR, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB1E, { "RR (HL)", &cpu::RR, &cpu::IMP, &cpu::REG, 16 } },

        { 0xCB27, { "SLA A", &cpu::SLA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB20, { "SLA B", &cpu::SLA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB21, { "SLA C", &cpu::SLA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB22, { "SLA D", &cpu::SLA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB23, { "SLA E", &cpu::SLA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB24, { "SLA H", &cpu::SLA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB25, { "SLA L", &cpu::SLA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB26, { "SLA (HL)", &cpu::SLA, &cpu::IMP, &cpu::REG, 16 } },

        { 0xCB2F, { "SRA A", &cpu::SRA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB28, { "SRA B", &cpu::SRA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB29, { "SRA C", &cpu::SRA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB2A, { "SRA D", &cpu::SRA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB2B, { "SRA E", &cpu::SRA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB2C, { "SRA H", &cpu::SRA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB2D, { "SRA L", &cpu::SRA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB2E, { "SRA (HL)", &cpu::SRA, &cpu::IMP, &cpu::REG, 16 } },

        { 0xCB3F, { "SRL A", &cpu::SRL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB38, { "SRL B", &cpu::SRL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB39, { "SRL C", &cpu::SRL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB3A, { "SRL D", &cpu::SRL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB3B, { "SRL E", &cpu::SRL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB3C, { "SRL H", &cpu::SRL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB3D, { "SRL L", &cpu::SRL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB3E, { "SRL (HL)", &cpu::SRL, &cpu::IMP, &cpu::REG, 16 } },

        // Bit Operations
        { 0xCB47, { "BIT b,A", &cpu::BIT, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB40, { "BIT b,B", &cpu::BIT, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB41, { "BIT b,C", &cpu::BIT, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB42, { "BIT b,D", &cpu::BIT, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB43, { "BIT b,E", &cpu::BIT, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB44, { "BIT b,H", &cpu::BIT, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB45, { "BIT b,L", &cpu::BIT, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB46, { "BIT b,(HL)", &cpu::BIT, &cpu::IMP, &cpu::REG, 16 } },

        { 0xCBC7, { "SET b,A", &cpu::SET, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCBC0, { "SET b,B", &cpu::SET, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCBC1, { "SET b,C", &cpu::SET, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCBC2, { "SET b,D", &cpu::SET, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCBC3, { "SET b,E", &cpu::SET, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCBC4, { "SET b,H", &cpu::SET, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCBC5, { "SET b,L", &cpu::SET, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCBC6, { "SET b,(HL)", &cpu::SET, &cpu::IMP, &cpu::REG, 16 } },

        { 0xCBC8, { "RES b,A", &cpu::RES, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB80, { "RES b,B", &cpu::RES, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB81, { "RES b,C", &cpu::RES, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB82, { "RES b,D", &cpu::RES, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB83, { "RES b,E", &cpu::RES, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB84, { "RES b,H", &cpu::RES, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB85, { "RES b,L", &cpu::RES, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB86, { "RES b,(HL)", &cpu::RES, &cpu::IMP, &cpu::REG, 16 } },

        // Jumps
        { 0x00C3, { "JP nn", &cpu::JP, &cpu::IMP, &cpu::IMM, 12 } },
        { 0x00C2, { "JP NZ,nn", &cpu::JP, &cpu::IMP, &cpu::IMM, 12 } },
        { 0x00CA, { "JP Z,nn", &cpu::JP, &cpu::IMP, &cpu::IMM, 12 } },
        { 0x00D2, { "JP NC,nn", &cpu::JP, &cpu::IMP, &cpu::IMM, 12 } },
        { 0x00DA, { "JP C,nn", &cpu::JP, &cpu::IMP, &cpu::IMM, 12 } },
        { 0x00E9, { "JP (HL)", &cpu::JP, &cpu::IMP, &cpu::IND, 4 } },
        { 0x0018, { "JR n", &cpu::JP, &cpu::IMP, &cpu::IMM, 8 } },
        { 0x0020, { "JR NZ,n", &cpu::JP, &cpu::IMP, &cpu::IMM, 8 } },
        { 0x0028, { "JR Z,n", &cpu::JP, &cpu::IMP, &cpu::IMM, 8 } },
        { 0x0030, { "JR NC,n", &cpu::JP, &cpu::IMP, &cpu::IMM, 8 } },
        { 0x0038, { "JR C,n", &cpu::JP, &cpu::IMP, &cpu::IMM, 8 } },

        // Calls
        { 0x00CD, { "CALL    nn", &cpu::CALL, &cpu::IMP, &cpu::IMM, 12 } },
        { 0x00C4, { "CALL NZ,nn", &cpu::CALL, &cpu::IMP, &cpu::IMM, 12 } },
        { 0x00C4, { "CALL  Z,nn", &cpu::CALL, &cpu::IMP, &cpu::IMM, 12 } },
        { 0x00C4, { "CALL NC,nn", &cpu::CALL, &cpu::IMP, &cpu::IMM, 12 } },
        { 0x00C4, { "CALL  C,nn", &cpu::CALL, &cpu::IMP, &cpu::IMM, 12 } },

        // Restarts
        { 0x00C7, { "RST 0x00", &cpu::RST, &cpu::IMP, &cpu::IMP, 32 } },
        { 0x00CF, { "RST 0x08", &cpu::RST, &cpu::IMP, &cpu::IMP, 32 } },
        { 0x00D7, { "RST 0x10", &cpu::RST, &cpu::IMP, &cpu::IMP, 32 } },
        { 0x00DF, { "RST 0x18", &cpu::RST, &cpu::IMP, &cpu::IMP, 32 } },
        { 0x00E7, { "RST 0x20", &cpu::RST, &cpu::IMP, &cpu::IMP, 32 } },
        { 0x00EF, { "RST 0x28", &cpu::RST, &cpu::IMP, &cpu::IMP, 32 } },
        { 0x00F7, { "RST 0x30", &cpu::RST, &cpu::IMP, &cpu::IMP, 32 } },
        { 0x00FF, { "RST 0x38", &cpu::RST, &cpu::IMP, &cpu::IMP, 32 } },

        // Returns
        { 0x00C9, { "RET",    &cpu::RET, &cpu::IMP, &cpu::IMP, 8 } },
        { 0x00C0, { "RET NZ", &cpu::RET, &cpu::IMP, &cpu::IMP, 8 } },
        { 0x00C8, { "RET  Z", &cpu::RET, &cpu::IMP, &cpu::IMP, 8 } },
        { 0x00D0, { "RET NC", &cpu::RET, &cpu::IMP, &cpu::IMP, 8 } },
        { 0x00D8, { "RET  C", &cpu::RET, &cpu::IMP, &cpu::IMP, 8 } },
        { 0x00D9, { "RETI",   &cpu::RETI,&cpu::IMP, &cpu::IMP, 8 } },
};

//------------------------------------------------------------------------------
// Class stuff
//------------------------------------------------------------------------------

cpu::cpu(bus *messageBus) : msgBus(messageBus) {
        pc = 0x100;
}

//------------------------------------------------------------------------------
// Addressing modes
//------------------------------------------------------------------------------

//! \brief Immediate Mode Addressing
//!
//! In the Immediate Addressing Mode, the byte following the op code in memory
//! contains the actual operand.
//!
//! An example of this type of instruction is to load the Accumulator with a
//! constant, in which the constant is the byte immediately following the op
//! code.
void cpu::IMM() {
        operand = 0 | msgBus->Read(pc++);
        operandNumBytes = 1;
}

//! \brief Immediate Mode Extended Addressing
//!
//! This mode is an extension of immediate addressing in that the two bytes
//! following the op codes are the operand.
//!
//! An example of this type of instruction is to load the HL register pair
//! (16-bit register) with 16 bits (two bytes) of data.
void cpu::IME() {
        operand = 0;
        operand |= msgBus->Read(pc++);
        operand |= (msgBus->Read(pc++) << 8);
        operandNumBytes = 2;
}

//! \brief Modified Page Zero Addressing
//!
//! The Z80 contains a special single-byte CALL instruction to any of eight
//! locations in Page 0 of memory. This instruction, which is referred to as a
//! restart, sets the Program Counter to an effective address in Page 0. The
//! value of this instruction is that it allows a single byte to specify a
//! complete 16-bit address at which commonly-called subroutines are located,
//! thereby saving memory space.
void cpu::MPZ() {
        // TODO
}

//! Relative Addressing
//!
//! Relative addressing uses one byte of data following the op code to specify a
//! displacement from the existing program to which a program jump can
//! occur. This displacement is a signed two’s complement number that is added
//! to the address of the op code of the follow-ing instruction.
//!
//! The value of relative addressing is that it allows jumps to nearby locations
//! while only requiring two bytes of memory space. For most programs, relative
//! jumps are by far the most prevalent type of jump due to the proximity of
//! related program segments. Therefore, these instructions can significantly
//! reduce memory space requirements. The signed dis-placement can range between
//! +127 and –128 from A+2. This range allows for a total dis-placement of +129
//! to –126 from the jump relative op code address. Another major advantage is
//! that it allows for relocatable code.
void cpu::REL() {
        operand = 0 | msgBus->Read(pc++);
        operandNumBytes = 1;
}

//! Extended Addressing
//!
//! Extended Addressing provides for two bytes (16 bits) of address to be
//! included in the instruction. This data can be an address to which a program
//! can jump or it can be an address at which an operand is located.
//!
//! Extended addressing is required for a program to jump from any location in
//! memory to any other location, or load and store data in any memory location.
void cpu::EXT() {
        operand = 0;
        operand |= msgBus->Read(pc++);
        operand |= (msgBus->Read(pc++) << 8);
        operandNumBytes = 2;
}

//! Indexed Addressing
//!
//! NOTE: Not implemented in the Sharp LR35902 used by the GameBoy.
//!
//! In the Indexed Addressing Mode, the byte of data following the op code
//! contains a dis-placement that is added to one of the two index registers
//! (the op code specifies which index register is used) to form a pointer to
//! memory. The contents of the index register are not altered by this
//! operation.
void cpu::IDX() {
        // nop
}

//! Register Addressing
//!
//! Many of the Z80 op codes contain bits of information that specify which CPU
//! register is to be used for an operation. An example of register addressing
//! is to load the data in Register 6 into Register C.
void cpu::REG() {
        operandNumBytes = 1;

        reg cpuReg = A; // TODO: determine which register is desired?!??! Might want to create a new tagged union parameter type...
        switch (cpuReg) {
                case A:
                        operand = 0 | regA;
                        break;
                case F:
                        operand = 0 | regF;
                        break;
                case B:
                        operand = 0 | regB;
                        break;
                case C:
                        operand = 0 | regC;
                        break;
                case D:
                        operand = 0 | regD;
                        break;
                case E:
                        operand = 0 | regE;
                        break;
                case H:
                        operand = 0 | regH;
                        break;
                case L:
                        operand = 0 | regL;
                        break;
        }
}

//! Implied Addressing
//!
//! Implied addressing refers to operations in which the op code automatically
//! implies one or more CPU registers as containing the operands. An example is
//! the set of arithmetic opera-tions in which the Accumulator is always implied
//! to be the destination of the results.
void cpu::IMP() {
        // nop
}

//! Register Indirect Addressing
//!
//! This type of addressing specifies a 16-bit CPU register pair (such as HL) to
//! be used as a pointer to any location in memory. This type of instruction is
//! powerful and it is used in a wide range of applications.
void cpu::IND() {
        uint8_t byteLo, byteHi;

        reg_pair regPair = HL; // TODO: determine which register is desired?!??! Might want to create a new tagged union parameter type...
        switch(regPair) {
                case BC:
                        byteLo = regB;
                        byteHi = regC;
                        break;
                case DE:
                        byteLo = regD;
                        byteHi = regE;
                        break;
                case HL:
                        byteLo = regH;
                        byteHi = regL;
                        break;
                default:
                        byteLo = 0;
                        byteHi = 0;
                        break;
        }

        operand = (byteHi << 8) | byteLo;
        operandNumBytes = 2;
}

//! Bit Addressing
//!
//! The Z80 contains a large number of bit set, reset, and test
//! instructions. These instructions allow any memory location or CPU register
//! to be specified for a bit operation through one of three previous addressing
//! modes (register, Register Indirect, and indexed) while three bits in the op
//! code specify which of the eight bits is to be manipulated.
void cpu::BTA() {
        // nop
}

//------------------------------------------------------------------------------
// Operations
//------------------------------------------------------------------------------

void cpu::InstructionFetch(uint8_t memory[]) {
        uint8_t opcodeByte;
        opcode = 0;

        opcodeByte = msgBus->Read(pc++);

        if (0x10 == opcodeByte || 0xCB == opcodeByte) {
                opcode = (opcodeByte << 8);
                opcodeByte = msgBus->Read(pc++);
        }

        opcode = (opcode | opcodeByte);
}

void cpu::InstructionDecode() {
        // nop
}

void cpu::InstructionExecute() {
        (this->*instructionMap[opcode].operation)();
        // // Get operands
        // // Something with addr1 and addr2.
        // auto fn = currentInstruction->operation;
        // //((*this).*fn)();
        // (this->*fn)();
}
