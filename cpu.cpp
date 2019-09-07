/******************************************************************************
  GrooveStomp's GB Emulator
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: cpu.cpp
  Created: 2019-08-29
  Updated: 2019-09-03
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)

  This program comes with ABSOLUTELY NO WARRANTY. This is free software, and
  you are welcome to redistribute it under certain conditions; See LICENSE for
  details.
 ******************************************************************************/

#include <string>
#include <map>
#include <cassert>

#include "cpu.hpp"
#include "bus.hpp"
#include "operand.hpp"

namespace gs {

struct instruction {
        std::string name;
        void (cpu::*operation)();
        std::shared_ptr<operand> (cpu::*getOperand1)();
        std::shared_ptr<operand> (cpu::*getOperand2)();
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
        { 0x00F1, { "POP AF", &cpu::POP, &cpu::IMP, &cpu::RPR, 12 } },
        { 0x00C1, { "POP BC", &cpu::POP, &cpu::IMP, &cpu::RPR, 12 } },
        { 0x00D1, { "POP DE", &cpu::POP, &cpu::IMP, &cpu::RPR, 12 } },
        { 0x00E1, { "POP HL", &cpu::POP, &cpu::IMP, &cpu::RPR, 12 } },

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
        { 0x0009, { "ADD HL,BC", &cpu::ADD, &cpu::RPR, &cpu::RPR, 8 } },
        { 0x0019, { "ADD HL,DE", &cpu::ADD, &cpu::RPR, &cpu::RPR, 8 } },
        { 0x0029, { "ADD HL,HL", &cpu::ADD, &cpu::RPR, &cpu::RPR, 8 } },
        { 0x0039, { "ADD HL,SP", &cpu::ADD, &cpu::RPR, &cpu::RPR, 8 } },
        { 0x00E8, { "ADD SP,n",  &cpu::ADD, &cpu::RPR, &cpu::IMM, 16 } },

        { 0x000B, { "DEC BC", &cpu::DEC, &cpu::IMP, &cpu::RPR, 8 } },
        { 0x001B, { "DEC DE", &cpu::DEC, &cpu::IMP, &cpu::RPR, 8 } },
        { 0x002B, { "DEC HL", &cpu::DEC, &cpu::IMP, &cpu::RPR, 8 } },
        { 0x003B, { "DEC SP", &cpu::DEC, &cpu::IMP, &cpu::IMP, 8 } },

        // Miscellaneous
        { 0x0037, { "SWAP A",    &cpu::SWAP, &cpu::IMP, &cpu::REG, 8 } },
        { 0x0030, { "SWAP B",    &cpu::SWAP, &cpu::IMP, &cpu::REG, 8 } },
        { 0x0031, { "SWAP C",    &cpu::SWAP, &cpu::IMP, &cpu::REG, 8 } },
        { 0x0032, { "SWAP D",    &cpu::SWAP, &cpu::IMP, &cpu::REG, 8 } },
        { 0x0033, { "SWAP E",    &cpu::SWAP, &cpu::IMP, &cpu::REG, 8 } },
        { 0x0034, { "SWAP H",    &cpu::SWAP, &cpu::IMP, &cpu::REG, 8 } },
        { 0x0035, { "SWAP L",    &cpu::SWAP, &cpu::IMP, &cpu::REG, 8 } },
        { 0x0036, { "SWAP (HL)", &cpu::SWAP, &cpu::IMP, &cpu::IND, 16 } },

        { 0x0027, { "DAA",  &cpu::DAA, &cpu::IMP, &cpu::IMP, 4 } },
        { 0x002F, { "CPL",  &cpu::CPL, &cpu::IMP, &cpu::IMP, 4 } },
        { 0x003F, { "CCF",  &cpu::CCF, &cpu::IMP, &cpu::IMP, 4 } },
        { 0x0037, { "SCF",  &cpu::SCF, &cpu::IMP, &cpu::IMP, 4 } },
        { 0x0000, { "NOP",  &cpu::NOP, &cpu::IMP, &cpu::IMP, 4 } },
        { 0x0076, { "HALT", &cpu::HALT, &cpu::IMP, &cpu::IMP, 4 } },
        { 0x1000, { "STOP", &cpu::STOP, &cpu::IMP, &cpu::IMP, 4 } },
        { 0x00F3, { "DI",   &cpu::DI, &cpu::IMP, &cpu::IMP, 4 } },
        { 0x00FB, { "EI",   &cpu::EI, &cpu::IMP, &cpu::IMP, 4 } },

        // Rotates and Shifts
        { 0x0007, { "RLCA", &cpu::RLCA, &cpu::IMP, &cpu::IMP, 4 } },
        { 0x0017, { "RLA",  &cpu::RLA,  &cpu::IMP, &cpu::IMP, 4 } },
        { 0x000F, { "RRCA", &cpu::RRCA, &cpu::IMP, &cpu::IMP, 4 } },
        { 0x001F, { "RRA",  &cpu::RRA,  &cpu::IMP, &cpu::IMP, 4 } },

        { 0xCB07, { "RLC A",    &cpu::RLC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB00, { "RLC B",    &cpu::RLC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB01, { "RLC C",    &cpu::RLC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB02, { "RLC D",    &cpu::RLC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB03, { "RLC E",    &cpu::RLC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB04, { "RLC H",    &cpu::RLC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB05, { "RLC L",    &cpu::RLC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB06, { "RLC (HL)", &cpu::RLC, &cpu::IMP, &cpu::IND, 16 } },

        { 0xCB17, { "RL A",    &cpu::RL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB10, { "RL B",    &cpu::RL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB11, { "RL C",    &cpu::RL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB12, { "RL D",    &cpu::RL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB13, { "RL E",    &cpu::RL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB14, { "RL H",    &cpu::RL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB15, { "RL L",    &cpu::RL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB16, { "RL (HL)", &cpu::RL, &cpu::IMP, &cpu::IND, 16 } },

        { 0xCB0F, { "RRC A", &cpu::RRC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB08, { "RRC B", &cpu::RRC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB09, { "RRC C", &cpu::RRC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB0A, { "RRC D", &cpu::RRC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB0B, { "RRC E", &cpu::RRC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB0C, { "RRC H", &cpu::RRC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB0D, { "RRC L", &cpu::RRC, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB0E, { "RRC (HL)", &cpu::RRC, &cpu::IMP, &cpu::IND, 16 } },

        { 0xCB1F, { "RR A", &cpu::RR, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB18, { "RR B", &cpu::RR, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB19, { "RR C", &cpu::RR, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB1A, { "RR D", &cpu::RR, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB1B, { "RR E", &cpu::RR, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB1C, { "RR H", &cpu::RR, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB1D, { "RR L", &cpu::RR, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB1E, { "RR (HL)", &cpu::RR, &cpu::IMP, &cpu::IND, 16 } },

        { 0xCB27, { "SLA A", &cpu::SLA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB20, { "SLA B", &cpu::SLA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB21, { "SLA C", &cpu::SLA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB22, { "SLA D", &cpu::SLA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB23, { "SLA E", &cpu::SLA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB24, { "SLA H", &cpu::SLA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB25, { "SLA L", &cpu::SLA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB26, { "SLA (HL)", &cpu::SLA, &cpu::IMP, &cpu::IND, 16 } },

        { 0xCB2F, { "SRA A", &cpu::SRA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB28, { "SRA B", &cpu::SRA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB29, { "SRA C", &cpu::SRA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB2A, { "SRA D", &cpu::SRA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB2B, { "SRA E", &cpu::SRA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB2C, { "SRA H", &cpu::SRA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB2D, { "SRA L", &cpu::SRA, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB2E, { "SRA (HL)", &cpu::SRA, &cpu::IMP, &cpu::IND, 16 } },

        { 0xCB3F, { "SRL A", &cpu::SRL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB38, { "SRL B", &cpu::SRL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB39, { "SRL C", &cpu::SRL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB3A, { "SRL D", &cpu::SRL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB3B, { "SRL E", &cpu::SRL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB3C, { "SRL H", &cpu::SRL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB3D, { "SRL L", &cpu::SRL, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB3E, { "SRL (HL)", &cpu::SRL, &cpu::IMP, &cpu::IND, 16 } },

        // Bit Operations
        { 0xCB47, { "BIT b,A",    &cpu::BIT, &cpu::IMM, &cpu::BTA, 8 } },
        { 0xCB40, { "BIT b,B",    &cpu::BIT, &cpu::IMM, &cpu::BTA, 8 } },
        { 0xCB41, { "BIT b,C",    &cpu::BIT, &cpu::IMM, &cpu::BTA, 8 } },
        { 0xCB42, { "BIT b,D",    &cpu::BIT, &cpu::IMM, &cpu::BTA, 8 } },
        { 0xCB43, { "BIT b,E",    &cpu::BIT, &cpu::IMM, &cpu::BTA, 8 } },
        { 0xCB44, { "BIT b,H",    &cpu::BIT, &cpu::IMM, &cpu::BTA, 8 } },
        { 0xCB45, { "BIT b,L",    &cpu::BIT, &cpu::IMM, &cpu::BTA, 8 } },
        { 0xCB46, { "BIT b,(HL)", &cpu::BIT, &cpu::IMM, &cpu::BTI, 16 } },

        { 0xCBC7, { "SET b,A",    &cpu::SET, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCBC0, { "SET b,B",    &cpu::SET, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCBC1, { "SET b,C",    &cpu::SET, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCBC2, { "SET b,D",    &cpu::SET, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCBC3, { "SET b,E",    &cpu::SET, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCBC4, { "SET b,H",    &cpu::SET, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCBC5, { "SET b,L",    &cpu::SET, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCBC6, { "SET b,(HL)", &cpu::SET, &cpu::IMP, &cpu::IND, 16 } },

        { 0xCBC8, { "RES b,A",    &cpu::RES, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB80, { "RES b,B",    &cpu::RES, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB81, { "RES b,C",    &cpu::RES, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB82, { "RES b,D",    &cpu::RES, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB83, { "RES b,E",    &cpu::RES, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB84, { "RES b,H",    &cpu::RES, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB85, { "RES b,L",    &cpu::RES, &cpu::IMP, &cpu::REG, 8 } },
        { 0xCB86, { "RES b,(HL)", &cpu::RES, &cpu::IMP, &cpu::IND, 16 } },

        // Jumps
        { 0x00C3, { "JP nn",    &cpu::JP, &cpu::IMP, &cpu::IME, 12 } },
        { 0x00C2, { "JP NZ,nn", &cpu::JP, &cpu::IMP, &cpu::IME, 12 } },
        { 0x00CA, { "JP Z,nn",  &cpu::JP, &cpu::IMP, &cpu::IME, 12 } },
        { 0x00D2, { "JP NC,nn", &cpu::JP, &cpu::IMP, &cpu::IME, 12 } },
        { 0x00DA, { "JP C,nn",  &cpu::JP, &cpu::IMP, &cpu::IME, 12 } },
        { 0x00E9, { "JP (HL)",  &cpu::JP, &cpu::IMP, &cpu::IND, 4 } },
        { 0x0018, { "JR n",     &cpu::JP, &cpu::IMP, &cpu::IMM, 8 } },
        { 0x0020, { "JR NZ,n",  &cpu::JP, &cpu::IMP, &cpu::IMM, 8 } },
        { 0x0028, { "JR Z,n",   &cpu::JP, &cpu::IMP, &cpu::IMM, 8 } },
        { 0x0030, { "JR NC,n",  &cpu::JP, &cpu::IMP, &cpu::IMM, 8 } },
        { 0x0038, { "JR C,n" ,  &cpu::JP, &cpu::IMP, &cpu::IMM, 8 } },

        // Calls
        { 0x00CD, { "CALL nn",    &cpu::CALL, &cpu::IMP, &cpu::IME, 12 } },
        { 0x00C4, { "CALL NZ,nn", &cpu::CALL, &cpu::IMP, &cpu::IME, 12 } },
        { 0x00C4, { "CALL Z,nn",  &cpu::CALL, &cpu::IMP, &cpu::IME, 12 } },
        { 0x00C4, { "CALL NC,nn", &cpu::CALL, &cpu::IMP, &cpu::IME, 12 } },
        { 0x00C4, { "CALL C,nn",  &cpu::CALL, &cpu::IMP, &cpu::IME, 12 } },

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
        { 0x00C9, { "RET",    &cpu::RET, &cpu::REG, &cpu::IMP, 8 } },
        { 0x00C0, { "RET NZ", &cpu::RET, &cpu::REG, &cpu::IMP, 8 } },
        { 0x00C8, { "RET Z",  &cpu::RET, &cpu::REG, &cpu::IMP, 8 } },
        { 0x00D0, { "RET NC", &cpu::RET, &cpu::REG, &cpu::IMP, 8 } },
        { 0x00D8, { "RET C",  &cpu::RET, &cpu::REG, &cpu::IMP, 8 } },
        { 0x00D9, { "RETI",   &cpu::RETI,&cpu::REG, &cpu::IMP, 8 } },
};

//------------------------------------------------------------------------------
// Class stuff
//------------------------------------------------------------------------------

cpu::cpu(bus *messageBus) : msgBus(messageBus) {
        pc = 0x100;
        sp = 0;
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
std::shared_ptr<operand> cpu::IMM() {
        auto result = std::make_shared<operand_value>();
        result->value = 0 | msgBus->Read(pc++);
        return std::dynamic_pointer_cast<operand>(result);
}

//! \brief Immediate Mode Extended Addressing
//!
//! This mode is an extension of immediate addressing in that the two bytes
//! following the op codes are the operand.
//!
//! An example of this type of instruction is to load the HL register pair
//! (16-bit register) with 16 bits (two bytes) of data.
std::shared_ptr<operand> cpu::IME() {
        auto result = std::make_shared<operand_value>();
        result->value = 0;
        result->value |= msgBus->Read(pc++);
        result->value |= (msgBus->Read(pc++) << 8);
        return std::dynamic_pointer_cast<operand>(result);
}

//! \brief Modified Page Zero Addressing
//!
//! The Z80 contains a special single-byte CALL instruction to any of eight
//! locations in Page 0 of memory. This instruction, which is referred to as a
//! restart, sets the Program Counter to an effective address in Page 0. The
//! value of this instruction is that it allows a single byte to specify a
//! complete 16-bit address at which commonly-called subroutines are located,
//! thereby saving memory space.
std::shared_ptr<operand> cpu::MPZ() {
        // TODO
        return nullptr;
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
std::shared_ptr<operand> cpu::REL() {
        int8_t off = (int16_t)msgBus->Read(pc++);
        auto result = std::make_shared<operand_address>();
        result->address = (int16_t)pc + (int16_t)off;
        return std::dynamic_pointer_cast<operand>(result);
}

//! Extended Addressing
//!
//! Extended Addressing provides for two bytes (16 bits) of address to be
//! included in the instruction. This data can be an address to which a program
//! can jump or it can be an address at which an operand is located.
//!
//! Extended addressing is required for a program to jump from any location in
//! memory to any other location, or load and store data in any memory location.
std::shared_ptr<operand> cpu::EXT() {
        auto result = std::make_shared<operand_address>();
        result->address = 0;
        result->address |= msgBus->Read(pc++);
        result->address |= (msgBus->Read(pc++) << 8);
        return std::dynamic_pointer_cast<operand>(result);
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
std::shared_ptr<operand> cpu::IDX() {
        // nop
        return nullptr;
}

//------------------------------------------------------------------------------
// Other
//------------------------------------------------------------------------------

//! \brief Get the low or high nibble from byte
//! \param byte value to get nibble from
//! \param pos 0 for low nibble, 1 for high nibble.
//! \return high or low nibble of byte as specified by pos
uint8_t Nibble(uint8_t byte, uint8_t pos) {
        uint8_t result;

        if (pos) {
                result = (byte & 0xF0) >> 4;
        } else {
                result = byte & 0x0F;
        }

        return result;
}

//! \return 1 if parity is even, otherwise 0
//! \see https://stackoverflow.com/a/21618038
int Parity(uint8_t byte) {
        byte ^= byte >> 4;
        byte ^= byte >> 2;
        byte ^= byte >> 1;
        return (~byte) & 1;
}

//! Register Addressing
//!
//! Many of the Z80 op codes contain bits of information that specify which CPU
//! register is to be used for an operation. An example of register addressing
//! is to load the data in Register 6 into Register C.
std::shared_ptr<operand> cpu::REG() {
        auto result = std::make_shared<operand_reference>();
        result->ref = GetRegister(operandType.reg);
        return std::dynamic_pointer_cast<operand>(result);
}

//! Implied Addressing
//!
//! Implied addressing refers to operations in which the op code automatically
//! implies one or more CPU registers as containing the operands. An example is
//! the set of arithmetic opera-tions in which the Accumulator is always implied
//! to be the destination of the results.
//!
//! Generally, the register being used with IMP() is the Accumulator.
std::shared_ptr<operand> cpu::IMP() {
        // TODO: Treat 16-bit register specially?  Ie., DEC ss needs SP register...
        auto result = std::make_shared<operand_reference>();
        result->ref = GetRegister(reg8::A);
        return std::static_pointer_cast<operand>(result);
}

//! \brief Register Pair
std::shared_ptr<operand> cpu::RPR() {
        auto result = std::make_shared<operand_pair_reference>();

        switch (operandType.regPair) {
                case reg16::BC:
                        result->ref1 = std::make_shared<uint8_t>(regB);
                        result->ref2 = std::make_shared<uint8_t>(regC);
                        break;
                case reg16::DE:
                        result->ref1 = std::make_shared<uint8_t>(regD);
                        result->ref2 = std::make_shared<uint8_t>(regE);
                        break;
                case reg16::HL:
                        result->ref1 = std::make_shared<uint8_t>(regH);
                        result->ref2 = std::make_shared<uint8_t>(regL);
                        break;
                case reg16::AF:
                        result->ref1 = std::make_shared<uint8_t>(regA);
                        result->ref2 = std::make_shared<uint8_t>(regF);
                        break;
        }

        return std::dynamic_pointer_cast<operand>(result);
}

//! Register Indirect Addressing
//!
//! This type of addressing specifies a 16-bit CPU register pair (such as HL) to
//! be used as a pointer to any location in memory. This type of instruction is
//! powerful and it is used in a wide range of applications.
std::shared_ptr<operand> cpu::IND() {
        uint8_t byteLo, byteHi;

        switch (operandType.regPair) {
                case reg16::BC:
                        byteLo = regB;
                        byteHi = regC;
                        break;
                case reg16::DE:
                        byteLo = regD;
                        byteHi = regE;
                        break;
                case reg16::HL:
                        byteLo = regH;
                        byteHi = regL;
                        break;
                default:
                        byteLo = 0;
                        byteHi = 0;
                        break;
        }

        auto result = std::make_shared<operand_address>();
        result->address = (byteHi << 8) | byteLo;
        return std::dynamic_pointer_cast<operand>(result);
}

//! Bit Addressing
//!
//! The Z80 contains a large number of bit set, reset, and test
//! instructions. These instructions allow any memory location or CPU register
//! to be specified for a bit operation through one of three previous addressing
//! modes (register, Register Indirect, and indexed) while three bits in the op
//! code specify which of the eight bits is to be manipulated.
//!
//! NOTE: As a prerequisite, operand1 must have been fetched.
std::shared_ptr<operand> cpu::BTA() {
        uint8_t whichReg = operand1->Get() & 0x7; // bits 2,1,0

        auto result = std::make_shared<operand_reference>();
        result->ref = GetRegister(static_cast<reg8>(whichReg));
        return std::dynamic_pointer_cast<operand>(result);
}

//! Bit addressing, but indirect
std::shared_ptr<operand> cpu::BTI() {
        uint8_t whichReg = operand1->Get() & 0x7; // bits 2,1,0
        assert(0x6 == whichReg);

        auto result = std::make_shared<operand_address>();
        result->address = ((uint16_t)regH << 8) | regL;
        return std::dynamic_pointer_cast<operand>(result);
}

std::shared_ptr<uint8_t> cpu::GetRegister(reg8 reg) {
        switch (reg) {
                case reg8::B:
                        return std::make_shared<uint8_t>(regB);
                case reg8::C:
                        return std::make_shared<uint8_t>(regC);
                case reg8::D:
                        return std::make_shared<uint8_t>(regD);
                case reg8::E:
                        return std::make_shared<uint8_t>(regE);
                case reg8::H:
                        return std::make_shared<uint8_t>(regH);
                case reg8::L:
                        return std::make_shared<uint8_t>(regL);
                case reg8::A:
                        return std::make_shared<uint8_t>(regA);
                case reg8::F:
                        return std::make_shared<uint8_t>(regF);
                default:
                        return nullptr;
        }
        return nullptr;
}

uint8_t cpu::MaskForBitAt(uint8_t bitToSet) {
        if (bitToSet > 7)
                return 0;

        return (1 << bitToSet);
}

void cpu::FlagSet(char c, uint8_t onOrOff) {
        uint8_t bit = 0;
        switch (c) {
                case 's':
                case 'S':
                        bit = 0x7;
                        break;
                case 'z':
                case 'Z':
                        bit = 0x6;
                        break;
                case 'h':
                case'H':
                        bit = 0x4;
                        break;
                case 'p':
                case 'P':
                case 'v':
                case 'V':
                        bit = 0x2;
                        break;
                case 'n':
                case 'N':
                        bit = 0x1;
                        break;
                case 'c':
                case 'C':
                        bit = 0x0;
                        break;
        }
        FlagSet(bit, onOrOff);
}

void cpu::FlagSet(uint8_t bit, uint8_t onOrOff) {
        uint8_t bitFlag = 1 << bit;

        if (onOrOff) {
                regF |= bitFlag;
        } else {
                regF &= (~bitFlag);
        }
}

uint8_t cpu::FlagGet(char c) {
        uint8_t bit = 0;
        switch (c) {
                case 's':
                case 'S':
                        bit = 0x7;
                        break;
                case 'z':
                case 'Z':
                        bit = 0x6;
                        break;
                case 'h':
                case'H':
                        bit = 0x4;
                        break;
                case 'p':
                case 'P':
                case 'v':
                case 'V':
                        bit = 0x2;
                        break;
                case 'n':
                case 'N':
                        bit = 0x1;
                        break;
                case 'c':
                case 'C':
                        bit = 0x0;
                        break;
        }
        return FlagGet(bit);
}

uint8_t cpu::FlagGet(uint8_t bitToCheck) {
        uint8_t bit = regF & (1 << bitToCheck);
        return bit >> bitToCheck;
}

//------------------------------------------------------------------------------
// Operations
//------------------------------------------------------------------------------

void cpu::LD() {
        switch (opcode) {
                case 0x01:
                        break;
                case 0x11:
                        break;
                case 0x21:
                        break;
                case 0x31:
                        break;
        }
}

//! Push register pair nn onto stack. Decrement Stack Pointer (SP) twice.
void cpu::PUSH() {
        switch (opcode) {
                case 0xF5:
                        operandType.tag = reg_tag::reg16;
                        operandType.regPair = reg16::AF;
                        break;
                case 0xC5:
                        operandType.tag = reg_tag::reg16;
                        operandType.regPair = reg16::BC;
                        break;
                case 0xD5:
                        operandType.tag = reg_tag::reg16;
                        operandType.regPair = reg16::DE;
                        break;
                case 0xE5:
                        operandType.tag = reg_tag::reg16;
                        operandType.regPair = reg16::HL;
                        break;
        }

        operand2 = ((*this).*(instruction->getOperand2))();

        uint16_t word = operand2->Get();
        msgBus->Write(--sp, static_cast<uint8_t>(word | 0xFF));
        msgBus->Write(--sp, static_cast<uint8_t>(word >> 8));
}

//! Pop two bytes off stack into register pair nn. Increment Stack Pointer (SP)
//! twice.
void cpu::POP() {
        switch (opcode) {
                case 0xF1:
                        operandType.tag = reg_tag::reg16;
                        operandType.regPair = reg16::AF;
                        break;
                case 0xC1:
                        operandType.tag = reg_tag::reg16;
                        operandType.regPair = reg16::BC;
                        break;
                case 0xD1:
                        operandType.tag = reg_tag::reg16;
                        operandType.regPair = reg16::DE;
                        break;
                case 0xE1:
                        operandType.tag = reg_tag::reg16;
                        operandType.regPair = reg16::HL;
                        break;
        }
        operand2 = ((*this).*(instruction->getOperand2))();

        uint16_t word = 0;
        uint8_t byte = msgBus->Read(sp++);
        word = byte << 8;
        byte = msgBus->Read(sp++);
        word |= byte;

        operand2->Set(word);
}

void cpu::ADD() {
        switch (opcode) {
                case 0xE8: {
                        operandType.tag = reg_tag::regSP;
                        operandType.regSP = reg_sp::SP;
                        operand1 = ((*this).*(instruction->getOperand1))();
                        operand2 = ((*this).*(instruction->getOperand2))();
                        break;
                }
                case 0x09: {
                        operandType.tag = reg_tag::reg16;
                        operandType.regPair = reg16::HL;
                        operand1 = ((*this).*(instruction->getOperand1))();

                        operandType.regPair = reg16::BC;
                        operand2 = ((*this).*(instruction->getOperand2))();
                        break;
                }
                case 0x19: {
                        operandType.tag = reg_tag::reg16;
                        operandType.regPair = reg16::HL;
                        operand1 = ((*this).*(instruction->getOperand1))();

                        operandType.regPair = reg16::DE;
                        operand2 = ((*this).*(instruction->getOperand2))();
                        break;
                }
                case 0x29: {
                        operandType.tag = reg_tag::reg16;
                        operandType.regPair = reg16::HL;
                        operand1 = ((*this).*(instruction->getOperand1))();
                        operand2 = ((*this).*(instruction->getOperand2))();
                        break;
                }
                case 0x39: {
                        operandType.tag = reg_tag::reg16;
                        operandType.regPair = reg16::HL;
                        operand1 = ((*this).*(instruction->getOperand1))();

                        operandType.tag = reg_tag::regSP;
                        operandType.regSP = reg_sp::SP;
                        operand2 = ((*this).*(instruction->getOperand2))();
                        break;
                }
        }
        auto left = operand1->Get();
        auto right = operand2->Get();
        operand1->Set(left + right);

        FlagSet('z', 0);
        FlagSet('n', 0);
        // Set if carry from bit 11
        FlagSet('h', ((left >> 0x11) & 0x1) && ((right >> 0x11) & 0x1));
        // Set if carry from bit 15
        FlagSet('h', ((left >> 0x15) & 0x1) && ((right >> 0x15) & 0x1));
}

//! \brief Increment wide register
//!
//! The contents of register pair ss (any of register pairs BC, DE, HL, or SP)
//! are incremented.
void cpu::INC() {
        switch (opcode) {
                case 0x03:
                        operandType.tag = reg_tag::reg16;
                        operandType.regPair = reg16::BC;
                        break;
                case 0x13:
                        operandType.tag = reg_tag::reg16;
                        operandType.regPair = reg16::DE;
                        break;
                case 0x23:
                        operandType.tag = reg_tag::reg16;
                        operandType.regPair = reg16::HL;
                        break;
                case 0x33:
                        operandType.tag = reg_tag::regSP;
                        operandType.regSP = reg_sp::SP;
                        break;
        }
        operand2 = ((*this).*(instruction->getOperand2))();
        auto oldValue = operand2->Get();
        auto newValue = oldValue + 1;
        operand2->Set(newValue);
}

//! \brief Decrement wide register
//!
//! The contents of register pair ss (any of the register pairs BC, DE, HL, or
//! SP) are decremented.
void cpu::DEC() {
        switch (opcode) {
                case 0x0B:
                        operandType.tag = reg_tag::reg16;
                        operandType.regPair = reg16::BC;
                        break;
                case 0x1B:
                        operandType.tag = reg_tag::reg16;
                        operandType.regPair = reg16::DE;
                        break;
                case 0x2B:
                        operandType.tag = reg_tag::reg16;
                        operandType.regPair = reg16::HL;
                        break;
                case 0x3B:
                        operandType.tag = reg_tag::regSP;
                        operandType.regSP = reg_sp::SP;
                        break;
        }
        operand2 = ((*this).*(instruction->getOperand2))();
        auto oldValue = operand2->Get();
        auto newValue = oldValue - 1;
        operand2->Set(newValue);
}

//! \brief Swap upper & lower nibbles of operand.
void cpu::SWAP() {
        switch (opcode) {
                case 0xCB37:
                        operandType.reg = reg8::A;
                        break;
                case 0xCB30:
                        operandType.reg = reg8::B;
                        break;
                case 0xCB31:
                        operandType.reg = reg8::C;
                        break;
                case 0xCB32:
                        operandType.reg = reg8::D;
                        break;
                case 0xCB33:
                        operandType.reg = reg8::E;
                        break;
                case 0xCB34:
                        operandType.reg = reg8::H;
                        break;
                case 0xCB35:
                        operandType.reg = reg8::L;
                        break;
                case 0xCB36:
                        operandType.regPair = reg16::HL;
                        break;
        }
        operand2 = ((*this).*(instruction->getOperand2))();
        uint8_t oldValue = static_cast<uint8_t>(operand2->Get());
        uint8_t nibbleHi = Nibble(oldValue, 1);
        uint8_t nibbleLo = Nibble(oldValue, 0);
        uint8_t newValue = (nibbleLo << 4) | nibbleHi;
        operand2->Set(newValue);

        FlagSet('z', !newValue);
        FlagSet('n', 0);
        FlagSet('h', 0);
        FlagSet('c', 0);
}

//! \brief Decimal adjust register A.
//!
//! This instruction adjusts register A so that the correct representation of
//! Binary Coded Decimal (BCD) is obtained.
//!
//! DAA is intended to be run immediately after an addition or subtraction
//! operation, where the operands were BCD encoded. It then makes the
//! corrections described above, so the result (stored in the A register) is the
//! BCD encoded result of the previous operation.
//!
//! \see https://ehaskins.com/2018-01-30%20Z80%20DAA/
void cpu::DAA() {
        operand2 = ((*this).*(instruction->getOperand2))();
        uint8_t val = static_cast<uint8_t>(operand2->Get());

        uint8_t wasCarryHalf = FlagGet('h');
        uint8_t wasCarry = FlagGet('c');
        uint8_t wasSubtraction = FlagGet('n');

        uint8_t nibbleLo = Nibble(val, 0);
        uint8_t nibbleHi = Nibble(val, 1);

        if (wasSubtraction) {
                if (nibbleLo > 9 || wasCarryHalf) {
                        nibbleLo -= 6;
                }
                if (nibbleHi > 9 || wasCarry) {
                        nibbleHi -= 6;
                }
        } else {
                if (nibbleLo > 9 || wasCarryHalf) {
                        nibbleLo += 6;
                }
                if (nibbleHi > 9 || wasCarry) {
                        nibbleHi += 6;
                }
        }

        uint8_t newValue = (nibbleHi << 4) | nibbleLo;
        operand2->Set(newValue);

        FlagSet('z', !newValue);
        FlagSet('h', 0);
        FlagSet('p', Parity(newValue));
        FlagSet('c', newValue > 0x99); // TODO verify against table in Z80 manual
}

//! \brief The contents of the Accumulator (Register A) are inverted (one’s complement).
void cpu::CPL() {
        operandType.reg = reg8::A;
        operand2 = ((*this).*(instruction->getOperand2))();
        uint8_t val = static_cast<uint8_t>(operand2->Get());
        val = ~val;
        operand2->Set(val);

        FlagSet('h', 1);
        FlagSet('n', 1);
}

// TODO: Not used in GB?
//! \brief Perform two's complement negation on A.
//!
//! The contents of the Accumulator are negated (two’s complement). This method
//! is the same as subtracting the contents of the Accumulator from zero.
// void cpu::NEG() {
//         operand2 = ((*this).*(instruction->getOperand2))();
//         uint16_t val = static_cast<uint8_t>(operand2->Get());
//         uint16_t onesComplement = ~val;
//         uint16_t twosComplement = onesComplement + 1;
//         operand2->Set(static_cast<uint8_t>(twosComplement));

//         FlagSet('s', twosComplement & 0x7);
//         FlagSet('z', !twosComplement);
//         // TODO: set if borrow from bit 4. // FlagSet('h', (twosComplement >> 0x4)
//         FlagSet('p', 0x80 == val);
//         FlagSet('n', 0);
//         FlagSet('c', !(0x00 == val));
// }

//! \brief The carry flag is inverted in the F register.
void cpu::CCF() {
        FlagSet('h', 0);
        FlagSet('n', 0);
        FlagSet('c', !FlagGet('c'));
}

//! \brief the carry flag is set in the F register.
void cpu::SCF() {
        FlagSet('h', 0);
        FlagSet('n', 0);
        FlagSet('c', 1);
}

void cpu::NOP() {
}

//! \brief Power down CPU until an interrupt occurs.
//!
//! The HALT instruction suspends CPU operation until a subsequent interrupt or
//! reset is received. While in the HALT state, the processor executes NOPs to
//! maintain memory refresh logic.
void cpu::HALT() {
        halted = true;
        waitForButtonPress = false;
}

//! \brief Halt CPU & LCD display until button press.
void cpu::STOP() {
        halted = true;
        waitForButtonPress = true;
}

//! \brief Disable interrupts after next instruction.
//!
//! When the CPU executes the instruction DI the maskable interrupt is disabled
//! until it is subsequently re-enabled by an EI instruction. The CPU does not
//! respond to an Interrupt Request (INT) signal.
void cpu::DI() {
        interruptsDisabledRequested = true;
}

//! \brief Enable interrupts after next instruction.
void cpu::EI() {
        interruptsEnabledRequested = true;
}

//! \brief Rotate A left. Old bit 7 is copied to the carry flag.
//!
//! The contents of the Accumulator (Register A) are rotated left 1 bit
//! position. The sign bit (bit 7) is copied to the Carry flag and also to bit
//! 0. Bit 0 is the least-significant bit.
void cpu::RLCA() {
        operandType.reg = reg8::A;
        operand2 = ((*this).*(instruction->getOperand2))();
        uint16_t oldValue = operand2->Get();
        uint8_t carry = ((uint8_t)oldValue >> 0x7) & 0x1;
        uint8_t newValue = ((uint8_t)oldValue << 1) | carry;
        operand2->Set(newValue);

        FlagSet('z', !newValue);
        FlagSet('n', 0);
        FlagSet('h', 0);
        FlagSet('c', carry);
}

//! \brief Rotate A left through the carry flag.
//!
//! The contents of the Accumulator (Register A) are rotated left 1 bit position
//! through the Carry flag. The previous contents of the Carry flag are copied
//! to bit 0. Bit 0 is the least-significant bit.
void cpu::RLA() {
        operandType.reg = reg8::A;
        operand2 = ((*this).*(instruction->getOperand2))();
        uint16_t oldValue = operand2->Get();
        uint8_t carry = ((uint8_t)oldValue >> 0x7) & 0x1;
        uint8_t oldCarry = FlagGet('c');
        uint8_t newValue = ((uint8_t)oldValue << 1) | oldCarry;
        operand2->Set(newValue);

        FlagSet('z', !newValue);
        FlagSet('n', 0);
        FlagSet('h', 0);
        FlagSet('c', carry);
}

//! \brief Rotate A right. Old bit 0 is copied to the carry flag.
//!
//! The contents of the Accumulator (Register A) are rotated right 1 bit
//! position. Bit 0 is cop-ied to the Carry flag and also to bit 7. Bit 0 is the
//! least-significant bit.
void cpu::RRCA() {
        operandType.reg = reg8::A;
        operand2 = ((*this).*(instruction->getOperand2))();
        uint16_t oldValue = operand2->Get();
        uint8_t carry = (uint8_t)oldValue & 0x1;
        uint8_t newValue = ((uint8_t)oldValue >> 1) | (carry << 0x7);
        operand2->Set(newValue);

        FlagSet('z', !newValue);
        FlagSet('n', 0);
        FlagSet('h', 0);
        FlagSet('c', carry);
}

//! \brief Rotate A right through the carry flag.
//!
//! The contents of the Accumulator (Register A) are rotated right 1 bit
//! position through the Carry flag. The previous contents of the Carry flag are
//! copied to bit 7. Bit 0 is the least-significant bit.
void cpu::RRA() {
        operandType.reg = reg8::A;
        operand2 = ((*this).*(instruction->getOperand2))();
        uint16_t oldValue = operand2->Get();
        uint8_t carry = (uint8_t)oldValue & 0x1;
        uint8_t oldCarry = FlagGet('c');
        uint8_t newValue = ((uint8_t)oldValue >> 1) | (oldCarry << 0x7);
        operand2->Set(newValue);

        FlagSet('z', !newValue);
        FlagSet('n', 0);
        FlagSet('h', 0);
        FlagSet('c', carry);
}

//! \brief Rotate left. Old bit 7 is copied to the carry flag.
//!
//! The contents of register r are rotated left 1 bit position. The contents of
//! bit 7 are copied to the Carry flag and also to bit 0. In the assembled
//! object code, operand r is specified as fol-lows:
//! B: 000
//! C: 001
//! D: 010
//! E: 011
//! H: 100
//! L: 101
//! A: 111
void cpu::RLC() {
        switch (opcode) {
                case 0xCB07:
                        operandType.reg = reg8::A;
                        break;
                case 0xCB00:
                        operandType.reg = reg8::B;
                        break;
                case 0xCB01:
                        operandType.reg = reg8::C;
                        break;
                case 0xCB02:
                        operandType.reg = reg8::D;
                        break;
                case 0xCB03:
                        operandType.reg = reg8::E;
                        break;
                case 0xCB04:
                        operandType.reg = reg8::H;
                        break;
                case 0xCB05:
                        operandType.reg = reg8::L;
                        break;
                case 0xCB06:
                        operandType.regPair = reg16::HL;
                        break;
        }
        operand2 = ((*this).*(instruction->getOperand2))();
        uint16_t oldValue = operand2->Get();
        uint8_t carry = ((uint8_t)oldValue >> 0x7) & 0x1;
        uint8_t newValue = ((uint8_t)oldValue << 1) | carry;
        operand2->Set(newValue);

        FlagSet('s', newValue & (0x1 << 0x7));
        FlagSet('z', !newValue);
        FlagSet('h', 0);
        FlagSet('p', Parity(newValue));
        FlagSet('n', 0);
        FlagSet('c', carry);
}

//! \brief Rotate left through carry flag.
//!
//! The contents of the m operand are rotated left 1 bit position. The contents
//! of bit 7 are copied to the Carry flag, and the previous contents of the
//! Carry flag are copied to bit 0.
void cpu::RL() {
        switch (opcode) {
                case 0xCB17:
                        operandType.reg = reg8::A;
                        break;
                case 0xCB10:
                        operandType.reg = reg8::B;
                        break;
                case 0xCB11:
                        operandType.reg = reg8::C;
                        break;
                case 0xCB12:
                        operandType.reg = reg8::D;
                        break;
                case 0xCB13:
                        operandType.reg = reg8::E;
                        break;
                case 0xCB14:
                        operandType.reg = reg8::H;
                        break;
                case 0xCB15:
                        operandType.reg = reg8::L;
                        break;
                case 0xCB16:
                        operandType.regPair = reg16::HL;
                        break;
        }
        operand2 = ((*this).*(instruction->getOperand2))();
        uint16_t oldValue = operand2->Get();
        uint8_t carry = ((uint8_t)oldValue >> 0x7) & 0x1;
        uint8_t oldCarry = FlagGet('c');
        uint8_t newValue = ((uint8_t)oldValue << 1) | oldCarry;
        operand2->Set(newValue);

        FlagSet('s', newValue & (0x1 << 0x7));
        FlagSet('z', !newValue);
        FlagSet('h', 0);
        FlagSet('p', Parity(newValue));
        FlagSet('n', 0);
        FlagSet('c', carry);
}

//! \brief Rotate right. Old bit 0 is set to the carry flag.
//!
//! The contents of the m operand are rotated right 1 bit position. The contents
//! of bit 0 are copied to the Carry flag and also to bit 7. Bit 0 is the
//! least-significant bit.
void cpu::RRC() {
        switch (opcode) {
                case 0xCB0F:
                        operandType.reg = reg8::A;
                        break;
                case 0xCB08:
                        operandType.reg = reg8::B;
                        break;
                case 0xCB09:
                        operandType.reg = reg8::C;
                        break;
                case 0xCB0A:
                        operandType.reg = reg8::D;
                        break;
                case 0xCB0B:
                        operandType.reg = reg8::E;
                        break;
                case 0xCB0C:
                        operandType.reg = reg8::H;
                        break;
                case 0xCB0D:
                        operandType.reg = reg8::L;
                        break;
                case 0xCB0E:
                        operandType.regPair = reg16::HL;
                        break;
        }
        operand2 = ((*this).*(instruction->getOperand2))();
        uint16_t oldValue = operand2->Get();
        uint8_t carry = (uint8_t)oldValue & 0x1;
        uint8_t newValue = ((uint8_t)oldValue >> 1) | carry;
        operand2->Set(newValue);

        FlagSet('s', newValue & (0x1 << 0x7));
        FlagSet('z', !newValue);
        FlagSet('h', 0);
        FlagSet('p', Parity(newValue));
        FlagSet('n', 0);
        FlagSet('c', carry);
}

//! \brief Rotate n right through carry flag.
//!
//! The contents of operand m are rotated right 1 bit position through the carry
//! flag. The contents of bit 0 are copied to the carry flag and the previous
//! contents of the carry flag are copied to bit 7. Bit 0 is the
//! least-significant bit.
void cpu::RR() {
        switch (opcode) {
                case 0xCB0F:
                        operandType.reg = reg8::A;
                        break;
                case 0xCB08:
                        operandType.reg = reg8::B;
                        break;
                case 0xCB09:
                        operandType.reg = reg8::C;
                        break;
                case 0xCB0A:
                        operandType.reg = reg8::D;
                        break;
                case 0xCB0B:
                        operandType.reg = reg8::E;
                        break;
                case 0xCB0C:
                        operandType.reg = reg8::H;
                        break;
                case 0xCB0D:
                        operandType.reg = reg8::L;
                        break;
                case 0xCB0E:
                        operandType.regPair = reg16::HL;
                        break;
        }
        operand2 = ((*this).*(instruction->getOperand2))();
        uint16_t oldValue = operand2->Get();
        uint8_t carry = (uint8_t)oldValue & 0x1;
        uint8_t oldCarry = FlagGet('c');
        uint8_t newValue = ((uint8_t)oldValue >> 1) | oldCarry;
        operand2->Set(newValue);

        FlagSet('s', newValue & (0x1 << 0x7));
        FlagSet('z', !newValue);
        FlagSet('h', 0);
        FlagSet('p', Parity(newValue));
        FlagSet('n', 0);
        FlagSet('c', carry);
}

//! \brief Shift left into carry. LSB is set to 0.
//!
//! An arithmetic shift left 1 bit position is performed on the contents of
//! operand m. The contents of bit 7 are copied to the Carry flag. Bit 0 is the
//! least-significant bit.
void cpu::SLA() {
        switch (opcode) {
                case 0xCB27:
                        operandType.reg = reg8::A;
                        break;
                case 0xCB20:
                        operandType.reg = reg8::B;
                        break;
                case 0xCB21:
                        operandType.reg = reg8::C;
                        break;
                case 0xCB22:
                        operandType.reg = reg8::D;
                        break;
                case 0xCB23:
                        operandType.reg = reg8::E;
                        break;
                case 0xCB24:
                        operandType.reg = reg8::H;
                        break;
                case 0xCB25:
                        operandType.reg = reg8::L;
                        break;
                case 0xCB26:
                        operandType.regPair = reg16::HL;
                        break;
        }

        operand2 = ((*this).*(instruction->getOperand2))();
        uint8_t oldValue = (uint8_t)operand2->Get();
        uint8_t carry = oldValue & (0x1 << 0x7);
        uint8_t newValue = oldValue << 1;
        operand2->Set(newValue);

        FlagSet('s', (newValue >> 0x7) & 0x1);
        FlagSet('z', !newValue);
        FlagSet('h', 0);
        FlagSet('p', Parity(newValue));
        FlagSet('n', 0);
        FlagSet('c', carry);
}

//! \brief Shift right into Carry. MSB doesn't change.
//!
//! An arithmetic shift right 1 bit position is performed on the contents of
//! operand m. The contents of bit 0 are copied to the Carry flag and the
//! previous contents of bit 7 remain unchanged. Bit 0 is the least-significant
//! bit.
void cpu::SRA() {
        switch (opcode) {
                case 0xCB2F:
                        operandType.reg = reg8::A;
                        break;
                case 0xCB28:
                        operandType.reg = reg8::B;
                        break;
                case 0xCB29:
                        operandType.reg = reg8::C;
                        break;
                case 0xCB2A:
                        operandType.reg = reg8::D;
                        break;
                case 0xCB2B:
                        operandType.reg = reg8::E;
                        break;
                case 0xCB2C:
                        operandType.reg = reg8::H;
                        break;
                case 0xCB2D:
                        operandType.reg = reg8::L;
                        break;
                case 0xCB2E:
                        operandType.regPair = reg16::HL;
                        break;
        }

        operand2 = ((*this).*(instruction->getOperand2))();
        uint8_t oldValue = (uint8_t)operand2->Get();
        uint8_t msb = (oldValue & 0x7);
        uint8_t carry = oldValue & 0x1;
        uint8_t newValue = (oldValue >> 1) | msb;
        operand2->Set(newValue);

        FlagSet('z', !newValue);
        FlagSet('n', 0);
        FlagSet('h', 0);
        FlagSet('c', carry);
}

//! \brief Shift right into Carry. MSB is set to 0.
//!
//! The contents of operand m are shifted right 1 bit position. The contents of
//! bit 0 are copied to the Carry flag, and bit 7 is reset. Bit 0 is the
//! least-significant bit.
void cpu::SRL() {
        switch (opcode) {
                case 0xCB2F:
                        operandType.reg = reg8::A;
                        break;
                case 0xCB28:
                        operandType.reg = reg8::B;
                        break;
                case 0xCB29:
                        operandType.reg = reg8::C;
                        break;
                case 0xCB2A:
                        operandType.reg = reg8::D;
                        break;
                case 0xCB2B:
                        operandType.reg = reg8::E;
                        break;
                case 0xCB2C:
                        operandType.reg = reg8::H;
                        break;
                case 0xCB2D:
                        operandType.reg = reg8::L;
                        break;
                case 0xCB2E:
                        operandType.regPair = reg16::HL;
                        break;
        }

        operand2 = ((*this).*(instruction->getOperand2))();
        uint8_t oldValue = (uint8_t)operand2->Get();
        uint8_t carry = oldValue & 0x1;
        uint8_t newValue = oldValue >> 1;
        operand2->Set(newValue);

        FlagSet('z', !newValue);
        FlagSet('n', 0);
        FlagSet('h', 0);
        FlagSet('c', carry);
}

//! \brief Test bit in register
//!
//! This instruction tests bit b in register r and sets the Z flag
//! accordingly.
void cpu::BIT() {
        operand1 = ((*this).*(instruction->getOperand1))();
        operand2 = ((*this).*(instruction->getOperand2))();

        uint8_t bit = (operand1->Get() >> 3) & 0x7; // bits 3,4,5
        uint8_t mask = MaskForBitAt(bit);

        FlagSet('n', 0);
        FlagSet('h', 1);
        FlagSet('z', operand2->Get() & mask);
}

//! \brief Set bit in register
//!
//! Bit b in register r (any of registers B, C, D, E, H, L, or A) is set.
void cpu::SET() {
        operand1 = ((*this).*(instruction->getOperand1))();
        operand2 = ((*this).*(instruction->getOperand2))();

        uint8_t bit = (operand1->Get() >> 3) & 0x7; // bits 3,4,5
        uint8_t mask = MaskForBitAt(bit);

        operand2->Set(operand2->Get() | mask);
}

//! Reset bit in register.
void cpu::RES() {
        operand1 = ((*this).*(instruction->getOperand1))();
        operand2 = ((*this).*(instruction->getOperand2))();

        uint8_t bit = (operand1->Get() >> 3) & 0x7; // bits 3,4,5
        uint8_t mask = MaskForBitAt(bit);
        mask = ~mask;

        uint8_t current = operand2->Get();

        operand2->Set(current & mask);
}

//! Jump to the specified address
//! 0xC3: JP    nn
//! 0xC2: JP NZ,nn
//! 0xCA: JP  Z,nn
//! 0xD2: JP NC,nn
//! 0xDA: JP  C,nn
//! 0xE9: JP  (HL)
void cpu::JP() {
        operand1 = ((*this).*(instruction->getOperand1))();
        operand2 = ((*this).*(instruction->getOperand2))();

        auto jump = [this](uint16_t address) { pc = address; };

        uint16_t address = std::static_pointer_cast<operand_address>(operand2)->address;
        auto zero = FlagGet('z');
        auto carry = FlagGet('c');

        switch (opcode) {
                case 0xE9:
                case 0xC3:
                        jump(address);
                        break;
                case 0xC2:
                        if (0 == zero)
                                jump(address);
                        break;
                case 0xCA:
                        if (0 != zero)
                                jump(address);
                        break;
                case 0xD2:
                        if (0 == carry)
                                jump(address);
                        break;
                case 0xDA:
                        if (0 == carry)
                                jump(address);
                        break;
        }
}

//! \brief Add n to current address and jump to it
//!
//! This instruction provides for conditional and uncoditional branching to
//! other segments of a program depending on the results of a test on the Carry
//! Flag. If the flag = 1, the value of displace-ment e is added to the Program
//! Counter (PC) and the next instruction is fetched from the location
//! designated by the new contents of the PC. The jump is measured from the
//! address of the instruction op code and contains a range of –126 to +129
//! bytes. The assembler auto-matically adjusts for the twice incremented PC.
//!
//! If the flag = 0, the next instruction executed is taken from the location
//! following this instruction.
//! If condition is met: 3 cycles
//! If conditions is not met: 2 cycles
//!
//! 0x18: JR    n
//! 0x20: JR NZ,n
//! 0x28: JR  Z,n
//! 0x30: JR NC,n
//! 0x38: JR  C,n
void cpu::JR() {
        operand1 = ((*this).*(instruction->getOperand1))();
        operand2 = ((*this).*(instruction->getOperand2))();

        auto jump = [this](uint16_t address) { pc = address; };

        int16_t value = (int16_t)std::static_pointer_cast<operand_value>(operand2)->value;
        uint16_t address = pc + value;

        auto zero = FlagGet('z');
        auto carry = FlagGet('c');

        switch (opcode) {
                case 0x18:
                        jump(address);
                        break;
                case 0x20:
                        if (0 == zero)
                                jump(address);
                        break;
                case 0x28:
                        if (0 != zero)
                                jump(address);
                        break;
                case 0x30:
                        if (0 == carry)
                                jump(address);
                        break;
                case 0x38:
                        if (0 == carry)
                                jump(address);
                        break;
        }
}

//! \brief Push address of next instruction onto the stack and then jump to address nn
//!
//! If condition cc is true, this instruction pushes the current contents of the
//! Program Counter (PC) are pushed onto the top of the external memory
//! stack. The operands nn are then loaded to the PC to point to the address in
//! mem-ory at which the first op code of a subroutine is to be fetched. At the
//! end of the subroutine, a RETurn instruction can be used to return to the
//! original program flow by popping the top of the stack back to the PC.
//!
//! Condition cc is programmed as one of eight statuses that corresponds to
//! condition bits in the Flag Register (Register F). These eight statuses are
//! defined in the following table, which also specifies the corresponding cc
//! bit fields in the assembled object code.
//!
//! cc  | Condition         | Flag
//!-----+-------------------+----
//! 000 | Non-Zero (NZ)     | Z
//! 001 | Zero (Z)          | Z
//! 010 | Non-Carry (NC)    | C
//! 011 | Carry (C)         | C
//! 100 | Parity-Odd (PO)   | P/V
//! 101 | Parity-Even (PE)  | P/V
//! 110 | Sign-Positive (P) | S
//! 111 | Sign-Negative (M) | S
void cpu::CALL() {
        operandType.reg = reg8::F;
        operand1 = ((*this).*(instruction->getOperand1))();
        operand2 = ((*this).*(instruction->getOperand2))();

        auto jump = [this](uint16_t address) {
                            msgBus->Write(--sp, static_cast<uint8_t>(pc & 0xFF));
                            msgBus->Write(--sp, static_cast<uint8_t>(pc >> 8));
                            pc = address;
                    };

        uint16_t address = std::static_pointer_cast<operand_address>(operand2)->address;
        auto zero = FlagGet('z');
        auto carry = FlagGet('c');

        switch (opcode) {
                case 0xCD:
                        jump(address);
                        break;
                case 0xC4:
                        if (0 == zero)
                                jump(address);
                        break;
                case 0xCC:
                        if (0 != zero)
                                jump(address);
                        break;
                case 0xD4:
                        if (0 == carry)
                                jump(address);
                        break;
                case 0xDC:
                        if (0 == carry)
                                jump(address);
                        break;
        }
}

//! Push present address onto stack. Jump to address $0000 + n.
void cpu::RST() {
        uint16_t address = 0;
        switch (opcode) {
                case 0xC7:
                        address = 0x00;
                        break;
                case 0xCF:
                        address = 0x08;
                        break;
                case 0xD7:
                        address = 0x10;
                        break;
                case 0xDF:
                        address = 0x18;
                        break;
                case 0xE7:
                        address = 0x20;
                        break;
                case 0xEF:
                        address = 0x28;
                        break;
                case 0xF7:
                        address = 0x30;
                        break;
                case 0xFF:
                        address = 0x38;
                        break;
        }

        msgBus->Write(--sp, static_cast<uint8_t>(pc & 0xFF));
        msgBus->Write(--sp, static_cast<uint8_t>(pc >> 8));
        pc = address;
}

void cpu::RET() {
        auto jump = [this]() {
                            uint16_t address;
                            uint8_t byte = msgBus->Read(sp++);
                            address = byte << 8;
                            byte = msgBus->Read(sp++);
                            address = address | byte;
                            pc = address;
                    };

        auto zero = FlagGet('z');
        auto carry = FlagGet('c');

        switch (opcode) {
                case 0xC9:
                        jump();
                        break;
                case 0xC0:
                        if (0 == zero)
                                jump();
                        break;
                case 0xC8:
                        if (0 != zero)
                                jump();
                        break;
                case 0xD0:
                        if (0 == carry)
                                jump();
                        break;
                case 0xD8:
                        if (0 != carry)
                                jump();
                        break;
        }
}

//! This instruction is used at the end of a maskable interrupt service routine
//! to:
//!
//! • Restore the contents of the Program Counter (analogous to the RET
//!   instruction)
//!
//! • Signal an I/O device that the interrupt routine is
//!   completed. The RETI instruction also facilitates the nesting of interrupts,
//!   allowing higher priority devices to temporarily suspend service of lower
//!   priority service routines. However, this instruction does not enable
//!   interrupts that were disabled when the interrupt routine was entered. Before
//!   doing the RETI instruction, the enable interrupt instruction (EI) should be
//!   executed to allow recognition of interrupts after completion of the current
//!   service routine.
void cpu::RETI() {
        RET();
        interruptsDisabled = false;
        interruptsDisabledRequested = false;
        interruptsEnabledRequested = false;
}

//------------------------------------------------------------------------------
// Public interface
//------------------------------------------------------------------------------

void cpu::InstructionFetch(uint8_t memory[]) {
        uint8_t opcodeByte;
        opcode = 0;

        opcodeByte = msgBus->Read(pc++);

        // Some opcodes are two-bytes long; these are prefixed with the byte
        // 0x10 or 0xCB.
        // TODO: I'm not sure if these are ordered B1|B2 or B2|B1.
        //       I'm assuming B1|B2 in this implementation but may need to
        //       follow up on that.
        if (0x10 == opcodeByte || 0xCB == opcodeByte) {
                opcode = (opcodeByte << 8);
                opcodeByte = msgBus->Read(pc++);
        }

        opcode = (opcode | opcodeByte);
        instruction = std::make_shared<gs::instruction>(instructionMap[opcode]);
}

void cpu::InstructionDecode() {
        // nop
}

void cpu::InstructionExecute() {
        ((*this).*(instruction->operation))();
}

} // namespace gs
