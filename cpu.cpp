/******************************************************************************
  GrooveStomp's GB Emulator
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: cpu.cpp
  Created: 2019-08-29
  Updated: 2019-09-13
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)

  This program comes with ABSOLUTELY NO WARRANTY. This is free software, and
  you are welcome to redistribute it under certain conditions; See LICENSE for
  details.
 ******************************************************************************/

//! \file cpu.cpp

#include <string>
#include <map>
#include <cassert>

#include "cpu.hpp"
#include "bus.hpp"
#include "operand.hpp"

#include "cpu_impl.cpp"

namespace gs {

struct instruction {
        std::string name;
        void (cpu::impl::*op)();
        unsigned int cycles;
};

cpu::cpu(bus *messageBus) {
        BC = 0;
        DE = 0;
        HL = 0;
        AF = 0;
        R = 0;
        PC = 0x100;
        SP = 0;
        I = 0;
        opcode = 0x0;
        msgBus = messageBus;

        implementation = new impl(this, messageBus);
}

cpu::~cpu() {
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
                F |= bitFlag;
        } else {
                F &= (~bitFlag);
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
        uint8_t bit = F & (1 << bitToCheck);
        return bit >> bitToCheck;
}

cpu::impl::impl(cpu *cpuIn, bus *busIn) {
        mCpu = cpuIn;
        mBus = busIn;
}

cpu::impl::~impl() {
}

uint8_t TwosComplement(uint8_t num) {
        return (~num) + 1;
}

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

//-----------------------------------------------------------------------------
// Opcode Implementations
//-----------------------------------------------------------------------------

//-- 8-Bit Load Opcodes --------------------------------------------------------

// These operations are of the form LD R,n where R is a register and n is an
// 8-bit immediate value that is loaded into R.

void cpu::impl::Op_0006() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_value>(mBus->Read(mCpu->PC++));
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_000E() {
        auto op1 = std::make_shared<operand_reference>(mCpu->C);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_value>(mBus->Read(mCpu->PC++));
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0016() {
        auto op1 = std::make_shared<operand_reference>(mCpu->D);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_value>(mBus->Read(mCpu->PC++));
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_001E() {
        auto op1 = std::make_shared<operand_reference>(mCpu->E);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_value>(mBus->Read(mCpu->PC++));
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0026() {
        auto op1 = std::make_shared<operand_reference>(mCpu->H);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_value>(mBus->Read(mCpu->PC++));
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_002E() {
        auto op1 = std::make_shared<operand_reference>(mCpu->L);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_value>(mBus->Read(mCpu->PC++));
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

// These instructions are of the form LD r1,r2 where the value of register r2 is
// loaded into the register r1.

void cpu::impl::Op_007F() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->A);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0078() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->B);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0079() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->C);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_007A() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->D);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_007B() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->E);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_007C() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->H);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_007D() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->L);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_007E() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0040() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->B);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0041() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->C);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0042() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->D);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0043() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->E);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0044() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->H);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0045() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->L);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0046() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0048() {
        auto op1 = std::make_shared<operand_reference>(mCpu->C);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->B);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0049() {
        auto op1 = std::make_shared<operand_reference>(mCpu->C);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->C);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_004A() {
        auto op1 = std::make_shared<operand_reference>(mCpu->C);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->D);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_004B() {
        auto op1 = std::make_shared<operand_reference>(mCpu->C);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->E);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_004C() {
        auto op1 = std::make_shared<operand_reference>(mCpu->C);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->H);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_004D() {
        auto op1 = std::make_shared<operand_reference>(mCpu->C);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->L);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_004E() {
        auto op1 = std::make_shared<operand_reference>(mCpu->C);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0050() {
        auto op1 = std::make_shared<operand_reference>(mCpu->D);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->B);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0051() {
        auto op1 = std::make_shared<operand_reference>(mCpu->D);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->C);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0052() {
        auto op1 = std::make_shared<operand_reference>(mCpu->D);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->D);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0053() {
        auto op1 = std::make_shared<operand_reference>(mCpu->D);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->E);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0054() {
        auto op1 = std::make_shared<operand_reference>(mCpu->D);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->H);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0055() {
        auto op1 = std::make_shared<operand_reference>(mCpu->D);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->L);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0056() {
        auto op1 = std::make_shared<operand_reference>(mCpu->D);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0058() {
        auto op1 = std::make_shared<operand_reference>(mCpu->E);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->B);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0059() {
        auto op1 = std::make_shared<operand_reference>(mCpu->E);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->C);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_005A() {
        auto op1 = std::make_shared<operand_reference>(mCpu->E);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->D);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_005B() {
        auto op1 = std::make_shared<operand_reference>(mCpu->E);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->E);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_005C() {
        auto op1 = std::make_shared<operand_reference>(mCpu->E);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->H);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_005D() {
        auto op1 = std::make_shared<operand_reference>(mCpu->E);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->L);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_005E() {
        auto op1 = std::make_shared<operand_reference>(mCpu->E);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0060() {
        auto op1 = std::make_shared<operand_reference>(mCpu->H);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->B);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0061() {
        auto op1 = std::make_shared<operand_reference>(mCpu->H);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->C);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0062() {
        auto op1 = std::make_shared<operand_reference>(mCpu->H);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->D);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0063() {
        auto op1 = std::make_shared<operand_reference>(mCpu->H);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->E);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0064() {
        auto op1 = std::make_shared<operand_reference>(mCpu->H);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->H);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0065() {
        auto op1 = std::make_shared<operand_reference>(mCpu->H);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->L);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0066() {
        auto op1 = std::make_shared<operand_reference>(mCpu->H);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0068() {
        auto op1 = std::make_shared<operand_reference>(mCpu->L);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->B);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0069() {
        auto op1 = std::make_shared<operand_reference>(mCpu->L);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->C);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_006A() {
        auto op1 = std::make_shared<operand_reference>(mCpu->L);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->D);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_006B() {
        auto op1 = std::make_shared<operand_reference>(mCpu->L);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->E);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_006C() {
        auto op1 = std::make_shared<operand_reference>(mCpu->L);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->H);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_006D() {
        auto op1 = std::make_shared<operand_reference>(mCpu->L);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->L);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_006E() {
        auto op1 = std::make_shared<operand_reference>(mCpu->L);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0070() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->B);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0071() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->C);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0072() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->D);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0073() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->E);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0074() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->H);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0075() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->L);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0036() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_value>(mBus->Read(mCpu->PC++));
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

// These instructions are of the form LD A,n where A is the accumulator and n is
// a register, immediate value or indirect value.

void cpu::impl::Op_000A() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_address>(mCpu->BC, mBus);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_001A() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_address>(mCpu->DE, mBus);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_00FA() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        uint8_t lsb = mBus->Read(mCpu->PC++);
        uint8_t msb = mBus->Read(mCpu->PC++);
        uint16_t address = (msb << 8) | lsb;

        auto op2 = std::make_shared<operand_address>(address, mBus);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_003E() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_value>(mBus->Read(mCpu->PC++));
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

// These instructions are of the form LD n,A where n is a register or indirect
// address and the contents of the accumulator are copied into that.

void cpu::impl::Op_0047() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->A);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_004F() {
        auto op1 = std::make_shared<operand_reference>(mCpu->C);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->A);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0057() {
        auto op1 = std::make_shared<operand_reference>(mCpu->D);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->A);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_005F() {
        auto op1 = std::make_shared<operand_reference>(mCpu->E);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->A);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0067() {
        auto op1 = std::make_shared<operand_reference>(mCpu->H);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->A);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_006F() {
        auto op1 = std::make_shared<operand_reference>(mCpu->L);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->A);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0002() {
        auto op1 = std::make_shared<operand_address>(mCpu->BC, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->A);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0012() {
        auto op1 = std::make_shared<operand_address>(mCpu->DE, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->A);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_0077() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->A);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

void cpu::impl::Op_00EA() {
        uint8_t lsb = mBus->Read(mCpu->PC++);
        uint8_t msb = mBus->Read(mCpu->PC++);
        uint16_t address = (msb << 8) | lsb;
        auto op1 = std::make_shared<operand_address>(address, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->A);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

//! \brief LD A,(C)
//!
//! Put value at address $FF00 + register C into A. Same as: LD A,($FF00+C)
void cpu::impl::Op_00F2() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        uint16_t address = static_cast<uint16_t>(mCpu->C) + 0xFF00;
        auto op2 = std::make_shared<operand_address>(address, mBus);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

//! \brief LD (C),A
//!
//! Put A into address $FF00 + register C.
void cpu::impl::Op_00E2() {
        uint16_t address = static_cast<uint16_t>(mCpu->C) + 0xFF00;
        auto op1 = std::make_shared<operand_address>(address, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->A);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

//! \brief LDD A,(HL)
//!
//! Put value at address HL into A. Decrement HL. Same as: LD A,(HL) - DEC HL
void cpu::impl::Op_003A() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();

        mCpu->HL--;
}

//! \brief LDD (HL),A
//!
//! Put A into memory address HL. Decrement HL. Same as: LD (HL),A - DEC HL
void cpu::impl::Op_0032() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->A);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();

        mCpu->HL--;
}

//! \brief LDI A,(HL)
//!
//! Put value at address HL into A. Increment HL. Same as: LD A,(HL) - INC HL
void cpu::impl::Op_002A() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();

        mCpu->HL++;
}

//! \brief LDI (HL),A
//!
//! Put A into memory address HL. Increment HL. Same as: LD (HL),A - INC HL
void cpu::impl::Op_0022() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->A);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();

        mCpu->HL++;
}

//! \brief LDH (n),A
//!
//! Put A into memory address $FF00+n
void cpu::impl::Op_00E0() {
        uint16_t address = mBus->Read(mCpu->PC++) + 0xFF00;
        auto op1 = std::make_shared<operand_address>(address, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->A);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

//! \brief LDH A,(n)
//!
//! Put memory address $FF00+n into A.
void cpu::impl::Op_00F0() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        uint16_t address = mBus->Read(mCpu->PC++) + 0xFF00;
        auto op2 = std::make_shared<operand_address>(address, mBus);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

//-- 16-Bit Load Opcodes -------------------------------------------------------

//! \brief LD BC,##
//!
//! Put 16-bit value ## into register pair BC.
void cpu::impl::Op_0001() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->BC);
        operand1 = std::static_pointer_cast<operand>(op1);

        uint16_t lo = mBus->Read(mCpu->PC++);
        uint16_t hi = mBus->Read(mCpu->PC++);
        auto op2 = std::make_shared<operand_value>((hi << 8) | lo);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

//! \brief LD DE,##
//!
//! Put 16-bit value ## into register pair BC.
void cpu::impl::Op_0011() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->DE);
        operand1 = std::static_pointer_cast<operand>(op1);

        uint16_t lo = mBus->Read(mCpu->PC++);
        uint16_t hi = mBus->Read(mCpu->PC++);
        auto op2 = std::make_shared<operand_value>((hi << 8) | lo);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

//! \brief LD HL,##
//!
//! Put 16-bit value ## into register pair BC.
void cpu::impl::Op_0021() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->HL);
        operand1 = std::static_pointer_cast<operand>(op1);

        uint16_t lo = mBus->Read(mCpu->PC++);
        uint16_t hi = mBus->Read(mCpu->PC++);
        auto op2 = std::make_shared<operand_value>((hi << 8) | lo);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

//! \brief LD SP,##
//!
//! Put 16-bit value ## into Stack Pointer (SP).
void cpu::impl::Op_0031() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->SP);
        operand1 = std::static_pointer_cast<operand>(op1);

        uint16_t lo = mBus->Read(mCpu->PC++);
        uint16_t hi = mBus->Read(mCpu->PC++);
        auto op2 = std::make_shared<operand_value>((hi << 8) | lo);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

//! \brief LD SP,HL
//!
//! Put HL into Stack Pointer (SP).
void cpu::impl::Op_00F9() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->SP);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_pair_reference>(mCpu->HL);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

//! \brief LDHL SP,n
//!
//! Put SP + n effective address into HL.
void cpu::impl::Op_00F8() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->HL);
        operand1 = std::static_pointer_cast<operand>(op1);

        uint16_t value = mCpu->SP;
        uint16_t byte = mBus->Read(mCpu->PC++);
        auto op2 = std::make_shared<operand_value>(value + byte);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();

        bool halfCarry = ((value & 0xFFF) + (byte & 0xFFF)) & 0x1000;
        bool carry = ((uint32_t)value + (uint32_t)byte) & 0x10000;

        mCpu->FlagSet('z', 0);
        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('h', halfCarry);
        mCpu->FlagSet('c', carry);
}

//! \brief LD (##),SP
//!
//! Put Stack Pointer (SP) at address ##.
void cpu::impl::Op_0008() {
        uint16_t lo = mBus->Read(mCpu->PC++);
        uint16_t hi = mBus->Read(mCpu->PC++);
        auto op1 = std::make_shared<operand_address>((hi << 8) + lo, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_pair_reference>(mCpu->SP);
        operand2 = std::static_pointer_cast<operand>(op2);

        LD();
}

//! \brief PUSH AF
//!
//! Push register pair AF onto stack.  Decrement Stack Pointer (SP) twice.
void cpu::impl::Op_00F5() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->AF);
        operand1 = std::static_pointer_cast<operand>(op1);

        PUSH();
}

//! \brief PUSH BC
//!
//! Push register pair BC onto stack.  Decrement Stack Pointer (SP) twice.
void cpu::impl::Op_00C5() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->BC);
        operand1 = std::static_pointer_cast<operand>(op1);

        PUSH();
}

//! \brief PUSH DE
//!
//! Push register pair DE onto stack.  Decrement Stack Pointer (SP) twice.
void cpu::impl::Op_00D5() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->DE);
        operand1 = std::static_pointer_cast<operand>(op1);

        PUSH();
}

//! \brief PUSH HL
//!
//! Push register pair HL onto stack.  Decrement Stack Pointer (SP) twice.
void cpu::impl::Op_00E5() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->HL);
        operand1 = std::static_pointer_cast<operand>(op1);

        PUSH();
}

//! \brief POP AF
//!
//! Pop two bytes off of the stack into AF. Increment Stack Pointer (SP) twice.
void cpu::impl::Op_00F1() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->AF);
        operand1 = std::static_pointer_cast<operand>(op1);

        POP();
}

//! \brief POP BC
//!
//! Pop two bytes off of the stack into BC. Increment Stack Pointer (SP) twice.
void cpu::impl::Op_00C1() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->BC);
        operand1 = std::static_pointer_cast<operand>(op1);

        POP();
}

//! \brief POP DE
//!
//! Pop two bytes off of the stack into DE. Increment Stack Pointer (SP) twice.
void cpu::impl::Op_00D1() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->DE);
        operand1 = std::static_pointer_cast<operand>(op1);

        POP();
}

//! \brief POP HL
//!
//! Pop two bytes off of the stack into HL. Increment Stack Pointer (SP) twice.
void cpu::impl::Op_00E1() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->HL);
        operand1 = std::static_pointer_cast<operand>(op1);

        POP();
}

//-- 8-Bit ALU Opcodes ---------------------------------------------------------

//! \brief ADD A,A
void cpu::impl::Op_0087() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->A);
        operand2 = std::static_pointer_cast<operand>(op2);

        ADD8();
}

//! \brief ADD A,B
void cpu::impl::Op_0080() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->B);
        operand2 = std::static_pointer_cast<operand>(op2);

        ADD8();
}

//! \brief ADD A,C
void cpu::impl::Op_0081() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->C);
        operand2 = std::static_pointer_cast<operand>(op2);

        ADD8();
}

//! \brief ADD A,D
void cpu::impl::Op_0082() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->D);
        operand2 = std::static_pointer_cast<operand>(op2);

        ADD8();
}

//! \brief ADD A,E
void cpu::impl::Op_0083() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->E);
        operand2 = std::static_pointer_cast<operand>(op2);

        ADD8();
}

//! \brief ADD A,H
void cpu::impl::Op_0084() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->H);
        operand2 = std::static_pointer_cast<operand>(op2);

        ADD8();
}

//! \brief ADD A,L
void cpu::impl::Op_0085() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->L);
        operand2 = std::static_pointer_cast<operand>(op2);

        ADD8();
}

//! \brief ADD A,(HL)
void cpu::impl::Op_0086() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand2 = std::static_pointer_cast<operand>(op2);

        ADD8();
}

//! \brief ADD A,#
void cpu::impl::Op_00C6() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        uint8_t byte = mBus->Read(mCpu->PC++);
        auto op2 = std::make_shared<operand_value>(byte);
        operand2 = std::static_pointer_cast<operand>(op2);

        ADD8();
}

//! \brief ADC A,A
void cpu::impl::Op_008F() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->A);
        operand2 = std::static_pointer_cast<operand>(op2);

        ADC8();
}

//! \brief ADC A,B
void cpu::impl::Op_0088() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->B);
        operand2 = std::static_pointer_cast<operand>(op2);

        ADC8();
}

//! \brief ADC A,C
void cpu::impl::Op_0089() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->C);
        operand2 = std::static_pointer_cast<operand>(op2);

        ADC8();
}

//! \brief ADC A,D
void cpu::impl::Op_008A() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->D);
        operand2 = std::static_pointer_cast<operand>(op2);

        ADC8();
}

//! \brief ADC A,E
void cpu::impl::Op_008B() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->E);
        operand2 = std::static_pointer_cast<operand>(op2);

        ADC8();
}

//! \brief ADC A,H
void cpu::impl::Op_008C() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->H);
        operand2 = std::static_pointer_cast<operand>(op2);

        ADC8();
}

//! \brief ADC A,L
void cpu::impl::Op_008D() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->L);
        operand2 = std::static_pointer_cast<operand>(op2);

        ADC8();
}

//! \brief ADC A,(HL)
void cpu::impl::Op_008E() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand2 = std::static_pointer_cast<operand>(op2);

        ADC8();
}

//! \brief ADC A,#
void cpu::impl::Op_00CE() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        uint8_t byte = mBus->Read(mCpu->PC++);
        auto op2 = std::make_shared<operand_value>(byte);
        operand2 = std::static_pointer_cast<operand>(op2);

        ADC8();
}

//! \brief SUB A
void cpu::impl::Op_0097() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        SUB8();
}

//! \brief SUB B
void cpu::impl::Op_0090() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        SUB8();
}

//! \brief SUB C
void cpu::impl::Op_0091() {
        auto op1 = std::make_shared<operand_reference>(mCpu->C);
        operand1 = std::static_pointer_cast<operand>(op1);

        SUB8();
}

//! \brief SUB D
void cpu::impl::Op_0092() {
        auto op1 = std::make_shared<operand_reference>(mCpu->D);
        operand1 = std::static_pointer_cast<operand>(op1);

        SUB8();
}

//! \brief SUB E
void cpu::impl::Op_0093() {
        auto op1 = std::make_shared<operand_reference>(mCpu->E);
        operand1 = std::static_pointer_cast<operand>(op1);

        SUB8();
}

//! \brief SUB H
void cpu::impl::Op_0094() {
        auto op1 = std::make_shared<operand_reference>(mCpu->H);
        operand1 = std::static_pointer_cast<operand>(op1);

        SUB8();
}

//! \brief SUB L
void cpu::impl::Op_0095() {
        auto op1 = std::make_shared<operand_reference>(mCpu->L);
        operand1 = std::static_pointer_cast<operand>(op1);

        SUB8();
}

//! \brief SUB (HL)
void cpu::impl::Op_0096() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        SUB8();
}

//! \brief SUB #
void cpu::impl::Op_00D6() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        auto op1 = std::make_shared<operand_value>(byte);
        operand1 = std::static_pointer_cast<operand>(op1);

        SUB8();
}

//! \brief SBC A
void cpu::impl::Op_009F() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        SBC8();
}

//! \brief SBC B
void cpu::impl::Op_0098() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        SBC8();
}

//! \brief SBC C
void cpu::impl::Op_0099() {
        auto op1 = std::make_shared<operand_reference>(mCpu->C);
        operand1 = std::static_pointer_cast<operand>(op1);

        SBC8();
}

//! \brief SBC D
void cpu::impl::Op_009A() {
        auto op1 = std::make_shared<operand_reference>(mCpu->D);
        operand1 = std::static_pointer_cast<operand>(op1);

        SBC8();
}

//! \brief SBC E
void cpu::impl::Op_009B() {
        auto op1 = std::make_shared<operand_reference>(mCpu->E);
        operand1 = std::static_pointer_cast<operand>(op1);

        SBC8();
}

//! \brief SBC H
void cpu::impl::Op_009C() {
        auto op1 = std::make_shared<operand_reference>(mCpu->H);
        operand1 = std::static_pointer_cast<operand>(op1);

        SBC8();
}

//! \brief SBC L
void cpu::impl::Op_009D() {
        auto op1 = std::make_shared<operand_reference>(mCpu->L);
        operand1 = std::static_pointer_cast<operand>(op1);

        SBC8();
}

//! \brief SBC (HL)
void cpu::impl::Op_009E() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        SBC8();
}

//! \brief AND A
void cpu::impl::Op_00A7() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        AND();
}

//! \brief AND B
void cpu::impl::Op_00A0() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        AND();
}

//! \brief AND C
void cpu::impl::Op_00A1() {
        auto op1 = std::make_shared<operand_reference>(mCpu->C);
        operand1 = std::static_pointer_cast<operand>(op1);

        AND();
}

//! \brief AND D
void cpu::impl::Op_00A2() {
        auto op1 = std::make_shared<operand_reference>(mCpu->D);
        operand1 = std::static_pointer_cast<operand>(op1);

        AND();
}

//! \brief AND E
void cpu::impl::Op_00A3() {
        auto op1 = std::make_shared<operand_reference>(mCpu->E);
        operand1 = std::static_pointer_cast<operand>(op1);

        AND();
}

//! \brief AND H
void cpu::impl::Op_00A4() {
        auto op1 = std::make_shared<operand_reference>(mCpu->H);
        operand1 = std::static_pointer_cast<operand>(op1);

        AND();
}

//! \brief AND L
void cpu::impl::Op_00A5() {
        auto op1 = std::make_shared<operand_reference>(mCpu->L);
        operand1 = std::static_pointer_cast<operand>(op1);

        AND();
}

//! \brief AND (HL)
void cpu::impl::Op_00A6() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        AND();
}

//! \brief AND #
void cpu::impl::Op_00E6() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        auto op1 = std::make_shared<operand_value>(byte);
        operand1 = std::static_pointer_cast<operand>(op1);

        AND();
}

//! \brief OR A
void cpu::impl::Op_00B7() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        OR();
}

//! \brief OR B
void cpu::impl::Op_00B0() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        OR();
}

//! \brief OR C
void cpu::impl::Op_00B1() {
        auto op1 = std::make_shared<operand_reference>(mCpu->C);
        operand1 = std::static_pointer_cast<operand>(op1);

        OR();
}

//! \brief OR D
void cpu::impl::Op_00B2() {
        auto op1 = std::make_shared<operand_reference>(mCpu->D);
        operand1 = std::static_pointer_cast<operand>(op1);

        OR();
}

//! \brief OR E
void cpu::impl::Op_00B3() {
        auto op1 = std::make_shared<operand_reference>(mCpu->E);
        operand1 = std::static_pointer_cast<operand>(op1);

        OR();
}

//! \brief OR H
void cpu::impl::Op_00B4() {
        auto op1 = std::make_shared<operand_reference>(mCpu->H);
        operand1 = std::static_pointer_cast<operand>(op1);

        OR();
}

//! \brief OR L
void cpu::impl::Op_00B5() {
        auto op1 = std::make_shared<operand_reference>(mCpu->L);
        operand1 = std::static_pointer_cast<operand>(op1);

        OR();
}

//! \brief OR (HL)
void cpu::impl::Op_00B6() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        OR();
}

//! \brief OR #
void cpu::impl::Op_00F6() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        auto op1 = std::make_shared<operand_value>(byte);
        operand1 = std::static_pointer_cast<operand>(op1);

        OR();
}

//! \brief XOR A
void cpu::impl::Op_00AF() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        XOR();
}

//! \brief XOR B
void cpu::impl::Op_00A8() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        XOR();
}

//! \brief XOR C
void cpu::impl::Op_00A9() {
        auto op1 = std::make_shared<operand_reference>(mCpu->C);
        operand1 = std::static_pointer_cast<operand>(op1);

        XOR();
}

//! \brief XOR D
void cpu::impl::Op_00AA() {
        auto op1 = std::make_shared<operand_reference>(mCpu->D);
        operand1 = std::static_pointer_cast<operand>(op1);

        XOR();
}

//! \brief XOR E
void cpu::impl::Op_00AB() {
        auto op1 = std::make_shared<operand_reference>(mCpu->E);
        operand1 = std::static_pointer_cast<operand>(op1);

        XOR();
}

//! \brief XOR H
void cpu::impl::Op_00AC() {
        auto op1 = std::make_shared<operand_reference>(mCpu->H);
        operand1 = std::static_pointer_cast<operand>(op1);

        XOR();
}

//! \brief XOR L
void cpu::impl::Op_00AD() {
        auto op1 = std::make_shared<operand_reference>(mCpu->L);
        operand1 = std::static_pointer_cast<operand>(op1);

        XOR();
}

//! \brief XOR (HL)
void cpu::impl::Op_00AE() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        XOR();
}

//! \brief XOR #
void cpu::impl::Op_00EE() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        auto op1 = std::make_shared<operand_value>(byte);
        operand1 = std::static_pointer_cast<operand>(op1);

        XOR();
}

//! \brief CP A
void cpu::impl::Op_00BF() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        CP();
}

//! \brief CP B
void cpu::impl::Op_00B8() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        CP();
}

//! \brief CP C
void cpu::impl::Op_00B9() {
        auto op1 = std::make_shared<operand_reference>(mCpu->C);
        operand1 = std::static_pointer_cast<operand>(op1);

        CP();
}

//! \brief CP D
void cpu::impl::Op_00BA() {
        auto op1 = std::make_shared<operand_reference>(mCpu->D);
        operand1 = std::static_pointer_cast<operand>(op1);

        CP();
}

//! \brief CP E
void cpu::impl::Op_00BB() {
        auto op1 = std::make_shared<operand_reference>(mCpu->E);
        operand1 = std::static_pointer_cast<operand>(op1);

        CP();
}

//! \brief CP H
void cpu::impl::Op_00BC() {
        auto op1 = std::make_shared<operand_reference>(mCpu->H);
        operand1 = std::static_pointer_cast<operand>(op1);

        CP();
}

//! \brief CP L
void cpu::impl::Op_00BD() {
        auto op1 = std::make_shared<operand_reference>(mCpu->L);
        operand1 = std::static_pointer_cast<operand>(op1);

        CP();
}

//! \brief CP (HL)
void cpu::impl::Op_00BE() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        CP();
}

//! \brief CP #
void cpu::impl::Op_00FE() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        auto op1 = std::make_shared<operand_value>(byte);
        operand1 = std::static_pointer_cast<operand>(op1);

        CP();
}

//! \brief INC A
void cpu::impl::Op_003C() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        INC8();
}

//! \brief INC B
void cpu::impl::Op_0004() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        INC8();
}

//! \brief INC C
void cpu::impl::Op_000C() {
        auto op1 = std::make_shared<operand_reference>(mCpu->C);
        operand1 = std::static_pointer_cast<operand>(op1);

        INC8();
}

//! \brief INC D
void cpu::impl::Op_0014() {
        auto op1 = std::make_shared<operand_reference>(mCpu->D);
        operand1 = std::static_pointer_cast<operand>(op1);

        INC8();
}

//! \brief INC E
void cpu::impl::Op_001C() {
        auto op1 = std::make_shared<operand_reference>(mCpu->E);
        operand1 = std::static_pointer_cast<operand>(op1);

        INC8();
}

//! \brief INC H
void cpu::impl::Op_0024() {
        auto op1 = std::make_shared<operand_reference>(mCpu->H);
        operand1 = std::static_pointer_cast<operand>(op1);

        INC8();
}

//! \brief INC L
void cpu::impl::Op_002C() {
        auto op1 = std::make_shared<operand_reference>(mCpu->L);
        operand1 = std::static_pointer_cast<operand>(op1);

        INC8();
}

//! \brief INC (HL)
void cpu::impl::Op_0034() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        INC8();
}

//! \brief DEC A
void cpu::impl::Op_003D() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        DEC8();
}

//! \brief DEC B
void cpu::impl::Op_0005() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        DEC8();
}

//! \brief DEC C
void cpu::impl::Op_000D() {
        auto op1 = std::make_shared<operand_reference>(mCpu->C);
        operand1 = std::static_pointer_cast<operand>(op1);

        DEC8();
}

//! \brief DEC D
void cpu::impl::Op_0015() {
        auto op1 = std::make_shared<operand_reference>(mCpu->D);
        operand1 = std::static_pointer_cast<operand>(op1);

        DEC8();
}

//! \brief DEC E
void cpu::impl::Op_001D() {
        auto op1 = std::make_shared<operand_reference>(mCpu->E);
        operand1 = std::static_pointer_cast<operand>(op1);

        DEC8();
}

//! \brief DEC H
void cpu::impl::Op_0025() {
        auto op1 = std::make_shared<operand_reference>(mCpu->H);
        operand1 = std::static_pointer_cast<operand>(op1);

        DEC8();
}

//! \brief DEC L
void cpu::impl::Op_002D() {
        auto op1 = std::make_shared<operand_reference>(mCpu->L);
        operand1 = std::static_pointer_cast<operand>(op1);

        DEC8();
}

//! \brief DEC (HL)
void cpu::impl::Op_0035() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        DEC8();
}

//-- 16-Bit ALU Opcodes --------------------------------------------------------

//! \brief ADD HL,BC
void cpu::impl::Op_0009() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->BC);
        operand1 = std::static_pointer_cast<operand>(op1);

        ADD16();
}

//! \brief ADD HL,DE
void cpu::impl::Op_0019() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->DE);
        operand1 = std::static_pointer_cast<operand>(op1);

        ADD16();
}

//! \brief ADD HL,HL
void cpu::impl::Op_0029() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->HL);
        operand1 = std::static_pointer_cast<operand>(op1);

        ADD16();
}

//! \brief ADD HL,SP
void cpu::impl::Op_0039() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->SP);
        operand1 = std::static_pointer_cast<operand>(op1);

        ADD16();
}

//! \brief ADD SP,#
void cpu::impl::Op_00E8() {
        auto op1 = std::make_shared<operand_value>(mBus->Read(mCpu->PC++));
        operand1 = std::static_pointer_cast<operand>(op1);

        ADD16();
}

//! \brief INC BC
void cpu::impl::Op_0003() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->BC);
        operand1 = std::static_pointer_cast<operand>(op1);

        INC16();
}

//! \brief INC DE
void cpu::impl::Op_0013() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->DE);
        operand1 = std::static_pointer_cast<operand>(op1);

        INC16();
}

//! \brief INC HL
void cpu::impl::Op_0023() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->HL);
        operand1 = std::static_pointer_cast<operand>(op1);

        INC16();
}

//! \brief INC SP
void cpu::impl::Op_0033() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->SP);
        operand1 = std::static_pointer_cast<operand>(op1);

        INC16();
}

//! \brief DEC BC
void cpu::impl::Op_000B() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->BC);
        operand1 = std::static_pointer_cast<operand>(op1);

        DEC16();
}

//! \brief DEC DE
void cpu::impl::Op_001B() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->DE);
        operand1 = std::static_pointer_cast<operand>(op1);

        DEC16();
}

//! \brief DEC HL
void cpu::impl::Op_002B() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->HL);
        operand1 = std::static_pointer_cast<operand>(op1);

        DEC16();
}

//! \brief DEC SP
void cpu::impl::Op_003B() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->SP);
        operand1 = std::static_pointer_cast<operand>(op1);

        DEC16();
}

//-- Miscellaneous Opcodes ----------------------------------------------------

//! \brief SWAP A
//!
//! Swap upper and lower nibbles of specified register.
void cpu::impl::Op_CB37() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        SWAP();
}

//! \brief SWAP B
//!
//! Swap upper and lower nibbles of specified register.
void cpu::impl::Op_CB30() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        SWAP();
}

//! \brief SWAP C
//!
//! Swap upper and lower nibbles of specified register.
void cpu::impl::Op_CB31() {
        auto op1 = std::make_shared<operand_reference>(mCpu->C);
        operand1 = std::static_pointer_cast<operand>(op1);

        SWAP();
}

//! \brief SWAP D
//!
//! Swap upper and lower nibbles of specified register.
void cpu::impl::Op_CB32() {
        auto op1 = std::make_shared<operand_reference>(mCpu->D);
        operand1 = std::static_pointer_cast<operand>(op1);

        SWAP();
}

//! \brief SWAP E
//!
//! Swap upper and lower nibbles of specified register.
void cpu::impl::Op_CB33() {
        auto op1 = std::make_shared<operand_reference>(mCpu->E);
        operand1 = std::static_pointer_cast<operand>(op1);

        SWAP();
}

//! \brief SWAP H
//!
//! Swap upper and lower nibbles of specified register.
void cpu::impl::Op_CB34() {
        auto op1 = std::make_shared<operand_reference>(mCpu->H);
        operand1 = std::static_pointer_cast<operand>(op1);

        SWAP();
}

//! \brief SWAP L
//!
//! Swap upper and lower nibbles of specified register.
void cpu::impl::Op_CB35() {
        auto op1 = std::make_shared<operand_reference>(mCpu->L);
        operand1 = std::static_pointer_cast<operand>(op1);

        SWAP();
}

//! \brief SWAP (HL)
//!
//! Swap upper and lower nibbles of value at address.
void cpu::impl::Op_CB36() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        SWAP();
}

//! \brief DAA
//!
//! Decimal adjust register A.
void cpu::impl::Op_0027() {
        DAA();
}

//! \brief CPL
//!
//! Complement register A.
void cpu::impl::Op_002F() {
        CPL();
}

//! \brief CCF
//!
//! Complement carry flag.
void cpu::impl::Op_003F() {
        CCF();
}

//! \brief SCF
//!
//! Set carry flag.
void cpu::impl::Op_0037() {
        SCF();
}

//! \brief NOP
void cpu::impl::Op_0000() {
        NOP();
}

//! \brief HALT
//!
//! Power down the CPU until an interrupt occurs.
void cpu::impl::Op_0076() {
        HALT();
}

//! \brief STOP
//!
//! Halt CPU & LCD display until a button is pressed.
void cpu::impl::Op_1000() {
        STOP();
}

//! \brief DI
//!
//! Disable interrupts.
void cpu::impl::Op_00F3() {
        DI();
}

//! \brief EI
//!
//! Enable interrupts.
void cpu::impl::Op_00FB() {
        EI();
}

//-- Rotate & Shift Opcodes ---------------------------------------------------

//! \brief RLCA
void cpu::impl::Op_0007() {
        RLCA();
}

//! \brief RLA
void cpu::impl::Op_0017() {
        RLA();
}

//! \brief RRCA
void cpu::impl::Op_000F() {
        RRCA();
}

//! \brief RRA
void cpu::impl::Op_001F() {
        RRA();
}

//! \brief RLC A
void cpu::impl::Op_CB07() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        RLC();
}

//! \brief RLC B
void cpu::impl::Op_CB00() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        RLC();
}

//! \brief RLC C
void cpu::impl::Op_CB01() {
        auto op1 = std::make_shared<operand_reference>(mCpu->C);
        operand1 = std::static_pointer_cast<operand>(op1);

        RLC();
}

//! \brief RLC D
void cpu::impl::Op_CB02() {
        auto op1 = std::make_shared<operand_reference>(mCpu->D);
        operand1 = std::static_pointer_cast<operand>(op1);

        RLC();
}

//! \brief RLC E
void cpu::impl::Op_CB03() {
        auto op1 = std::make_shared<operand_reference>(mCpu->E);
        operand1 = std::static_pointer_cast<operand>(op1);

        RLC();
}

//! \brief RLC H
void cpu::impl::Op_CB04() {
        auto op1 = std::make_shared<operand_reference>(mCpu->H);
        operand1 = std::static_pointer_cast<operand>(op1);

        RLC();
}

//! \brief RLC L
void cpu::impl::Op_CB05() {
        auto op1 = std::make_shared<operand_reference>(mCpu->L);
        operand1 = std::static_pointer_cast<operand>(op1);

        RLC();
}

//! \brief RLC (HL)
void cpu::impl::Op_CB06() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        RLC();
}

//! \brief RL A
void cpu::impl::Op_CB17() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        RL();
}

//! \brief RL B
void cpu::impl::Op_CB10() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        RL();
}

//! \brief RL C
void cpu::impl::Op_CB11() {
        auto op1 = std::make_shared<operand_reference>(mCpu->C);
        operand1 = std::static_pointer_cast<operand>(op1);

        RL();
}

//! \brief RL D
void cpu::impl::Op_CB12() {
        auto op1 = std::make_shared<operand_reference>(mCpu->D);
        operand1 = std::static_pointer_cast<operand>(op1);

        RL();
}

//! \brief RL E
void cpu::impl::Op_CB13() {
        auto op1 = std::make_shared<operand_reference>(mCpu->E);
        operand1 = std::static_pointer_cast<operand>(op1);

        RL();
}

//! \brief RL H
void cpu::impl::Op_CB14() {
        auto op1 = std::make_shared<operand_reference>(mCpu->H);
        operand1 = std::static_pointer_cast<operand>(op1);

        RL();
}

//! \brief RL L
void cpu::impl::Op_CB15() {
        auto op1 = std::make_shared<operand_reference>(mCpu->L);
        operand1 = std::static_pointer_cast<operand>(op1);

        RL();
}

//! \brief RL (HL)
void cpu::impl::Op_CB16() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        RL();
}

//! \brief RRC A
void cpu::impl::Op_CB0F() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        RRC();
}

//! \brief RRC B
void cpu::impl::Op_CB08() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        RRC();
}

//! \brief RRC C
void cpu::impl::Op_CB09() {
        auto op1 = std::make_shared<operand_reference>(mCpu->C);
        operand1 = std::static_pointer_cast<operand>(op1);

        RRC();
}

//! \brief RRC D
void cpu::impl::Op_CB0A() {
        auto op1 = std::make_shared<operand_reference>(mCpu->D);
        operand1 = std::static_pointer_cast<operand>(op1);

        RRC();
}

//! \brief RRC E
void cpu::impl::Op_CB0B() {
        auto op1 = std::make_shared<operand_reference>(mCpu->E);
        operand1 = std::static_pointer_cast<operand>(op1);

        RRC();
}

//! \brief RRC H
void cpu::impl::Op_CB0C() {
        auto op1 = std::make_shared<operand_reference>(mCpu->H);
        operand1 = std::static_pointer_cast<operand>(op1);

        RRC();
}

//! \brief RRC L
void cpu::impl::Op_CB0D() {
        auto op1 = std::make_shared<operand_reference>(mCpu->L);
        operand1 = std::static_pointer_cast<operand>(op1);

        RRC();
}

//! \brief RRC (HL)
void cpu::impl::Op_CB0E() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        RRC();
}

//! \brief RR A
void cpu::impl::Op_CB1F() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        RR();
}

//! \brief RR B
void cpu::impl::Op_CB18() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        RR();
}

//! \brief RR C
void cpu::impl::Op_CB19() {
        auto op1 = std::make_shared<operand_reference>(mCpu->C);
        operand1 = std::static_pointer_cast<operand>(op1);

        RR();
}

//! \brief RR D
void cpu::impl::Op_CB1A() {
        auto op1 = std::make_shared<operand_reference>(mCpu->D);
        operand1 = std::static_pointer_cast<operand>(op1);

        RR();
}

//! \brief RR E
void cpu::impl::Op_CB1B() {
        auto op1 = std::make_shared<operand_reference>(mCpu->E);
        operand1 = std::static_pointer_cast<operand>(op1);

        RR();
}

//! \brief RR H
void cpu::impl::Op_CB1C() {
        auto op1 = std::make_shared<operand_reference>(mCpu->H);
        operand1 = std::static_pointer_cast<operand>(op1);

        RR();
}

//! \brief RR L
void cpu::impl::Op_CB1D() {
        auto op1 = std::make_shared<operand_reference>(mCpu->L);
        operand1 = std::static_pointer_cast<operand>(op1);

        RR();
}

//! \brief RR (HL)
void cpu::impl::Op_CB1E() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        RR();
}

//! \brief SLA A
void cpu::impl::Op_CB27() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        SLA();
}

//! \brief SLA B
void cpu::impl::Op_CB20() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        SLA();
}

//! \brief SLA C
void cpu::impl::Op_CB21() {
        auto op1 = std::make_shared<operand_reference>(mCpu->C);
        operand1 = std::static_pointer_cast<operand>(op1);

        SLA();
}

//! \brief SLA D
void cpu::impl::Op_CB22() {
        auto op1 = std::make_shared<operand_reference>(mCpu->D);
        operand1 = std::static_pointer_cast<operand>(op1);

        SLA();
}

//! \brief SLA E
void cpu::impl::Op_CB23() {
        auto op1 = std::make_shared<operand_reference>(mCpu->E);
        operand1 = std::static_pointer_cast<operand>(op1);

        SLA();
}

//! \brief SLA H
void cpu::impl::Op_CB24() {
        auto op1 = std::make_shared<operand_reference>(mCpu->H);
        operand1 = std::static_pointer_cast<operand>(op1);

        SLA();
}

//! \brief SLA L
void cpu::impl::Op_CB25() {
        auto op1 = std::make_shared<operand_reference>(mCpu->L);
        operand1 = std::static_pointer_cast<operand>(op1);

        SLA();
}

//! \brief SLA (HL)
void cpu::impl::Op_CB26() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        SLA();
}

//! \brief SRA A
void cpu::impl::Op_CB2F() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        SRA();
}

//! \brief SRA B
void cpu::impl::Op_CB28() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        SRA();
}

//! \brief SRA C
void cpu::impl::Op_CB29() {
        auto op1 = std::make_shared<operand_reference>(mCpu->C);
        operand1 = std::static_pointer_cast<operand>(op1);

        SRA();
}

//! \brief SRA D
void cpu::impl::Op_CB2A() {
        auto op1 = std::make_shared<operand_reference>(mCpu->D);
        operand1 = std::static_pointer_cast<operand>(op1);

        SRA();
}

//! \brief SRA E
void cpu::impl::Op_CB2B() {
        auto op1 = std::make_shared<operand_reference>(mCpu->E);
        operand1 = std::static_pointer_cast<operand>(op1);

        SRA();
}

//! \brief SRA H
void cpu::impl::Op_CB2C() {
        auto op1 = std::make_shared<operand_reference>(mCpu->H);
        operand1 = std::static_pointer_cast<operand>(op1);

        SRA();
}

//! \brief SRA L
void cpu::impl::Op_CB2D() {
        auto op1 = std::make_shared<operand_reference>(mCpu->L);
        operand1 = std::static_pointer_cast<operand>(op1);

        SRA();
}

//! \brief SRA (HL)
void cpu::impl::Op_CB2E() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        SRA();
}

//! \brief SRL A
void cpu::impl::Op_CB3F() {
        auto op1 = std::make_shared<operand_reference>(mCpu->A);
        operand1 = std::static_pointer_cast<operand>(op1);

        SRL();
}

//! \brief SRL B
void cpu::impl::Op_CB38() {
        auto op1 = std::make_shared<operand_reference>(mCpu->B);
        operand1 = std::static_pointer_cast<operand>(op1);

        SRL();
}

//! \brief SRL C
void cpu::impl::Op_CB39() {
        auto op1 = std::make_shared<operand_reference>(mCpu->C);
        operand1 = std::static_pointer_cast<operand>(op1);

        SRL();
}

//! \brief SRL D
void cpu::impl::Op_CB3A() {
        auto op1 = std::make_shared<operand_reference>(mCpu->D);
        operand1 = std::static_pointer_cast<operand>(op1);

        SRL();
}

//! \brief SRL E
void cpu::impl::Op_CB3B() {
        auto op1 = std::make_shared<operand_reference>(mCpu->E);
        operand1 = std::static_pointer_cast<operand>(op1);

        SRL();
}

//! \brief SRL H
void cpu::impl::Op_CB3C() {
        auto op1 = std::make_shared<operand_reference>(mCpu->H);
        operand1 = std::static_pointer_cast<operand>(op1);

        SRL();
}

//! \brief SRL L
void cpu::impl::Op_CB3D() {
        auto op1 = std::make_shared<operand_reference>(mCpu->L);
        operand1 = std::static_pointer_cast<operand>(op1);

        SRL();
}

//! \brief SRL (HL)
void cpu::impl::Op_CB3E() {
        auto op1 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand1 = std::static_pointer_cast<operand>(op1);

        SRL();
}

//-- Bit Opcodes ---------------------------------------------------------------

//! \brief BIT #,A
void cpu::impl::Op_CB47() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<operand_value>(bit);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->A);
        operand2 = std::static_pointer_cast<operand>(op2);

        BIT();
}

//! \brief BIT #,B
void cpu::impl::Op_CB40() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<operand_value>(bit);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->B);
        operand2 = std::static_pointer_cast<operand>(op2);

        BIT();
}

//! \brief BIT #,C
void cpu::impl::Op_CB41() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<operand_value>(bit);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->C);
        operand2 = std::static_pointer_cast<operand>(op2);

        BIT();
}

//! \brief BIT #,D
void cpu::impl::Op_CB42() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<operand_value>(bit);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->D);
        operand2 = std::static_pointer_cast<operand>(op2);

        BIT();
}

//! \brief BIT #,E
void cpu::impl::Op_CB43() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<operand_value>(bit);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->E);
        operand2 = std::static_pointer_cast<operand>(op2);

        BIT();
}

//! \brief BIT #,H
void cpu::impl::Op_CB44() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<operand_value>(bit);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->H);
        operand2 = std::static_pointer_cast<operand>(op2);

        BIT();
}

//! \brief BIT #,L
void cpu::impl::Op_CB45() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<operand_value>(bit);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->L);
        operand2 = std::static_pointer_cast<operand>(op2);

        BIT();
}

//! \brief BIT #,(HL)
void cpu::impl::Op_CB46() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<operand_value>(bit);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand2 = std::static_pointer_cast<operand>(op2);

        BIT();
}

//! \brief SET #,A
void cpu::impl::Op_CBC7() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<operand_value>(bit);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->A);
        operand2 = std::static_pointer_cast<operand>(op2);

        SET();
}

//! \brief SET #,B
void cpu::impl::Op_CBC0() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<operand_value>(bit);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->B);
        operand2 = std::static_pointer_cast<operand>(op2);

        SET();
}

//! \brief SET #,C
void cpu::impl::Op_CBC1() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<operand_value>(bit);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->C);
        operand2 = std::static_pointer_cast<operand>(op2);

        SET();
}

//! \brief SET #,D
void cpu::impl::Op_CBC2() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<operand_value>(bit);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->D);
        operand2 = std::static_pointer_cast<operand>(op2);

        SET();
}

//! \brief SET #,E
void cpu::impl::Op_CBC3() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<operand_value>(bit);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->E);
        operand2 = std::static_pointer_cast<operand>(op2);

        SET();
}

//! \brief SET #,H
void cpu::impl::Op_CBC4() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<operand_value>(bit);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->H);
        operand2 = std::static_pointer_cast<operand>(op2);

        SET();
}

//! \brief SET #,L
void cpu::impl::Op_CBC5() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<operand_value>(bit);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->L);
        operand2 = std::static_pointer_cast<operand>(op2);

        SET();
}

//! \brief SET #,(HL)
void cpu::impl::Op_CBC6() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<operand_value>(bit);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand2 = std::static_pointer_cast<operand>(op2);

        SET();
}

//! \brief RES #,A
void cpu::impl::Op_CB87() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<operand_value>(bit);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->A);
        operand2 = std::static_pointer_cast<operand>(op2);

        RES();
}

//! \brief RES #,B
void cpu::impl::Op_CB80() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<operand_value>(bit);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->B);
        operand2 = std::static_pointer_cast<operand>(op2);

        RES();
}

//! \brief RES #,C
void cpu::impl::Op_CB81() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<operand_value>(bit);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->C);
        operand2 = std::static_pointer_cast<operand>(op2);

        RES();
}

//! \brief RES #,D
void cpu::impl::Op_CB82() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<operand_value>(bit);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->D);
        operand2 = std::static_pointer_cast<operand>(op2);

        RES();
}

//! \brief RES #,E
void cpu::impl::Op_CB83() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<operand_value>(bit);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->E);
        operand2 = std::static_pointer_cast<operand>(op2);

        RES();
}

//! \brief RES #,H
void cpu::impl::Op_CB84() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<operand_value>(bit);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->H);
        operand2 = std::static_pointer_cast<operand>(op2);

        RES();
}

//! \brief RES #,L
void cpu::impl::Op_CB85() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<operand_value>(bit);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_reference>(mCpu->L);
        operand2 = std::static_pointer_cast<operand>(op2);

        RES();
}

//! \brief RES #,(HL)
void cpu::impl::Op_CB86() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<operand_value>(bit);
        operand1 = std::static_pointer_cast<operand>(op1);

        auto op2 = std::make_shared<operand_address>(mCpu->HL, mBus);
        operand2 = std::static_pointer_cast<operand>(op2);

        RES();
}

//-- Jump Opcodes --------------------------------------------------------------

//! \brief JP ##
void cpu::impl::Op_00C3() {
        uint8_t lo = mBus->Read(mCpu->PC++);
        uint8_t hi = mBus->Read(mCpu->PC++);
        auto op1 = std::make_shared<operand_value>((hi << 8) | lo);
        operand1 = std::static_pointer_cast<operand>(op1);

        JP();
}

//! \brief JP NZ,##
void cpu::impl::Op_00C2() {
        uint8_t lo = mBus->Read(mCpu->PC++);
        uint8_t hi = mBus->Read(mCpu->PC++);
        auto op1 = std::make_shared<operand_value>((hi << 8) | lo);
        operand1 = std::static_pointer_cast<operand>(op1);

        if (!mCpu->FlagGet('z')) {
                JP();
        }
}

//! \brief JP Z,##
void cpu::impl::Op_00CA() {
        uint8_t lo = mBus->Read(mCpu->PC++);
        uint8_t hi = mBus->Read(mCpu->PC++);
        auto op1 = std::make_shared<operand_value>((hi << 8) | lo);
        operand1 = std::static_pointer_cast<operand>(op1);

        if (mCpu->FlagGet('z')) {
                JP();
        }
}

//! \brief JP NC,##
void cpu::impl::Op_00D2() {
        uint8_t lo = mBus->Read(mCpu->PC++);
        uint8_t hi = mBus->Read(mCpu->PC++);
        auto op1 = std::make_shared<operand_value>((hi << 8) | lo);
        operand1 = std::static_pointer_cast<operand>(op1);

        if (!mCpu->FlagGet('c')) {
                JP();
        }
}

//! \brief JP C,##
void cpu::impl::Op_00DA() {
        uint8_t lo = mBus->Read(mCpu->PC++);
        uint8_t hi = mBus->Read(mCpu->PC++);
        auto op1 = std::make_shared<operand_value>((hi << 8) | lo);
        operand1 = std::static_pointer_cast<operand>(op1);

        if (mCpu->FlagGet('c')) {
                JP();
        }
}

//! \brief JP (HL)
void cpu::impl::Op_00E9() {
        auto op1 = std::make_shared<operand_pair_reference>(mCpu->HL);
        operand1 = std::static_pointer_cast<operand>(op1);

        JP();
}

//! \brief JR #
void cpu::impl::Op_0018() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        auto op1 = std::make_shared<operand_value>(byte);
        operand1 = std::static_pointer_cast<operand>(op1);

        JR();
}

//! \brief JR NZ,#
void cpu::impl::Op_0020() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        auto op1 = std::make_shared<operand_value>(byte);
        operand1 = std::static_pointer_cast<operand>(op1);

        if (!mCpu->FlagGet('z')) {
                JR();
        }
}

//! \brief JR Z,#
void cpu::impl::Op_0028() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        auto op1 = std::make_shared<operand_value>(byte);
        operand1 = std::static_pointer_cast<operand>(op1);

        if (mCpu->FlagGet('z')) {
                JR();
        }
}

//! \brief JR NC,#
void cpu::impl::Op_0030() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        auto op1 = std::make_shared<operand_value>(byte);
        operand1 = std::static_pointer_cast<operand>(op1);

        if (!mCpu->FlagGet('c')) {
                JR();
        }
}

//! \brief JR C,#
void cpu::impl::Op_0038() {
        uint8_t byte = mBus->Read(mCpu->PC++);
        auto op1 = std::make_shared<operand_value>(byte);
        operand1 = std::static_pointer_cast<operand>(op1);

        if (mCpu->FlagGet('c')) {
                JR();
        }
}

//-- Call Opcodes --------------------------------------------------------------

//! \brief CALL ##
void cpu::impl::Op_00CD() {
        uint8_t lo = mBus->Read(mCpu->PC++);
        uint8_t hi = mBus->Read(mCpu->PC++);
        auto op1 = std::make_shared<operand_value>((hi << 8) | lo);
        operand1 = std::static_pointer_cast<operand>(op1);

        CALL();
}

//! \brief CALL NZ,##
void cpu::impl::Op_00C4() {
        uint8_t lo = mBus->Read(mCpu->PC++);
        uint8_t hi = mBus->Read(mCpu->PC++);
        auto op1 = std::make_shared<operand_value>((hi << 8) | lo);
        operand1 = std::static_pointer_cast<operand>(op1);

        if (!mCpu->FlagGet('z')) {
                CALL();
        }
}

//! \brief CALL Z,##
void cpu::impl::Op_00CC() {
        uint8_t lo = mBus->Read(mCpu->PC++);
        uint8_t hi = mBus->Read(mCpu->PC++);
        auto op1 = std::make_shared<operand_value>((hi << 8) | lo);
        operand1 = std::static_pointer_cast<operand>(op1);

        if (mCpu->FlagGet('z')) {
                CALL();
        }
}

//! \brief CALL NC,##
void cpu::impl::Op_00D4() {
        uint8_t lo = mBus->Read(mCpu->PC++);
        uint8_t hi = mBus->Read(mCpu->PC++);
        auto op1 = std::make_shared<operand_value>((hi << 8) | lo);
        operand1 = std::static_pointer_cast<operand>(op1);

        if (!mCpu->FlagGet('c')) {
                CALL();
        }
}

//! \brief CALL C,##
void cpu::impl::Op_00DC() {
        uint8_t lo = mBus->Read(mCpu->PC++);
        uint8_t hi = mBus->Read(mCpu->PC++);
        auto op1 = std::make_shared<operand_value>((hi << 8) | lo);
        operand1 = std::static_pointer_cast<operand>(op1);

        if (mCpu->FlagGet('c')) {
                CALL();
        }
}

//-- Restart Opcodes -----------------------------------------------------------

// These instructions are of the form RST n where the current address is pushed
// onto the stack, then n specifies which address to jump to.

//! \brief RST 0x00
void cpu::impl::Op_00C7() {
        auto op = std::make_shared<operand_address>(0x00, mBus);
        operand1 = std::static_pointer_cast<operand>(op);

        RST();
}

//! \brief RST 0x08
void cpu::impl::Op_00CF() {
        auto op = std::make_shared<operand_address>(0x08, mBus);
        operand1 = std::static_pointer_cast<operand>(op);

        RST();
}

//! \brief RST 0x10
void cpu::impl::Op_00D7() {
        auto op = std::make_shared<operand_address>(0x10, mBus);
        operand1 = std::static_pointer_cast<operand>(op);

        RST();
}

//! \brief RST 0x18
void cpu::impl::Op_00DF() {
        auto op = std::make_shared<operand_address>(0x18, mBus);
        operand1 = std::static_pointer_cast<operand>(op);

        RST();
}

//! \brief RST 0x20
void cpu::impl::Op_00E7() {
        auto op = std::make_shared<operand_address>(0x20, mBus);
        operand1 = std::static_pointer_cast<operand>(op);

        RST();
}

//! \brief RST 0x28
void cpu::impl::Op_00EF() {
        auto op = std::make_shared<operand_address>(0x28, mBus);
        operand1 = std::static_pointer_cast<operand>(op);

        RST();
}

//! \brief RST 0x30
void cpu::impl::Op_00F7() {
        auto op = std::make_shared<operand_address>(0x30, mBus);
        operand1 = std::static_pointer_cast<operand>(op);

        RST();
}

//! \brief RST 0x38
void cpu::impl::Op_00FF() {
        auto op = std::make_shared<operand_address>(0x38, mBus);
        operand1 = std::static_pointer_cast<operand>(op);

        RST();
}

//-- Returns -------------------------------------------------------------------

//! \brief RET
void cpu::impl::Op_00C9() {
        RET();
}

//! \brief RET NZ
void cpu::impl::Op_00C0() {
        if (0 == mCpu->FlagGet('z')) {
                RET();
        }
}

//! \brief RET Z
void cpu::impl::Op_00C8() {
        if (0 != mCpu->FlagGet('z')) {
                RET();
        }
}

//! \brief RET NC
void cpu::impl::Op_00D0() {
        if (0 == mCpu->FlagGet('c')) {
                RET();
        }
}

//! \brief RET C
void cpu::impl::Op_00D8() {
        if (0 != mCpu->FlagGet('c')) {
                RET();
        }
}

//! \brief RETI
void cpu::impl::Op_00D9() {
        RETI();
}

//-----------------------------------------------------------------------------
// Opcode Function Mapping
//-----------------------------------------------------------------------------

std::map<int,instruction> cpu::impl::instructionMap = {
        // 8-bit Load
        { 0x0006, { "LD B,n",    &cpu::impl::Op_0006, 8 } },
        { 0x000E, { "LD C,n",    &cpu::impl::Op_000E, 8 } },
        { 0x0016, { "LD D,n",    &cpu::impl::Op_0016, 8 } },
        { 0x001E, { "LD E,n",    &cpu::impl::Op_001E, 8 } },
        { 0x0026, { "LD H,n",    &cpu::impl::Op_0026, 8 } },
        { 0x002E, { "LD L,n",    &cpu::impl::Op_002E, 8 } },
        { 0x007F, { "LD A,A",    &cpu::impl::Op_007F, 4 } },
        { 0x0078, { "LD A,B",    &cpu::impl::Op_0078, 4 } },
        { 0x0079, { "LD A,C",    &cpu::impl::Op_0079, 4 } },
        { 0x007A, { "LD A,D",    &cpu::impl::Op_007A, 4 } },
        { 0x007B, { "LD A,E",    &cpu::impl::Op_007B, 4 } },
        { 0x007C, { "LD A,H",    &cpu::impl::Op_007C, 4 } },
        { 0x007D, { "LD A,L",    &cpu::impl::Op_007D, 4 } },
        { 0x007E, { "LD A,(HL)", &cpu::impl::Op_007E, 8 } },
        { 0x0040, { "LD B,B",    &cpu::impl::Op_0040, 4 } },
        { 0x0041, { "LD B,C",    &cpu::impl::Op_0041, 4 } },
        { 0x0042, { "LD B,D",    &cpu::impl::Op_0042, 4 } },
        { 0x0043, { "LD B,E",    &cpu::impl::Op_0043, 4 } },
        { 0x0044, { "LD B,H",    &cpu::impl::Op_0044, 4 } },
        { 0x0045, { "LD B,L",    &cpu::impl::Op_0045, 4 } },
        { 0x0046, { "LD B,(HL)", &cpu::impl::Op_0046, 8 } },
        { 0x0048, { "LD C,B",    &cpu::impl::Op_0048, 4 } },
        { 0x0049, { "LD C,C",    &cpu::impl::Op_0049, 4 } },
        { 0x004A, { "LD C,D",    &cpu::impl::Op_004A, 4 } },
        { 0x004B, { "LD C,E",    &cpu::impl::Op_004B, 4 } },
        { 0x004C, { "LD C,H",    &cpu::impl::Op_004C, 4 } },
        { 0x004D, { "LD C,L",    &cpu::impl::Op_004D, 4 } },
        { 0x004E, { "LD C,(HL)", &cpu::impl::Op_004E, 8 } },
        { 0x0050, { "LD D,B",    &cpu::impl::Op_0050, 4 } },
        { 0x0051, { "LD D,C",    &cpu::impl::Op_0051, 4 } },
        { 0x0052, { "LD D,D",    &cpu::impl::Op_0052, 4 } },
        { 0x0053, { "LD D,E",    &cpu::impl::Op_0053, 4 } },
        { 0x0054, { "LD D,H",    &cpu::impl::Op_0054, 4 } },
        { 0x0055, { "LD D,L",    &cpu::impl::Op_0055, 4 } },
        { 0x0056, { "LD D,(HL)", &cpu::impl::Op_0056, 8 } },
        { 0x0058, { "LD E,B",    &cpu::impl::Op_0058, 4 } },
        { 0x0059, { "LD E,C",    &cpu::impl::Op_0059, 4 } },
        { 0x005A, { "LD E,D",    &cpu::impl::Op_005A, 4 } },
        { 0x005B, { "LD E,E",    &cpu::impl::Op_005B, 4 } },
        { 0x005C, { "LD E,H",    &cpu::impl::Op_005C, 4 } },
        { 0x005D, { "LD E,L",    &cpu::impl::Op_005D, 4 } },
        { 0x005E, { "LD E,(HL)", &cpu::impl::Op_005E, 8 } },
        { 0x0060, { "LD H,B",    &cpu::impl::Op_0060, 4 } },
        { 0x0061, { "LD H,C",    &cpu::impl::Op_0061, 4 } },
        { 0x0062, { "LD H,D",    &cpu::impl::Op_0062, 4 } },
        { 0x0063, { "LD H,E",    &cpu::impl::Op_0063, 4 } },
        { 0x0064, { "LD H,H",    &cpu::impl::Op_0064, 4 } },
        { 0x0065, { "LD H,L",    &cpu::impl::Op_0065, 4 } },
        { 0x0066, { "LD H,(HL)", &cpu::impl::Op_0066, 8 } },
        { 0x0068, { "LD L,B",    &cpu::impl::Op_0068, 4 } },
        { 0x0069, { "LD L,C",    &cpu::impl::Op_0069, 4 } },
        { 0x006A, { "LD L,D",    &cpu::impl::Op_006A, 4 } },
        { 0x006B, { "LD L,E",    &cpu::impl::Op_006B, 4 } },
        { 0x006C, { "LD L,H",    &cpu::impl::Op_006C, 4 } },
        { 0x006D, { "LD L,L",    &cpu::impl::Op_006D, 4 } },
        { 0x006E, { "LD L,(HL)", &cpu::impl::Op_006E, 8 } },
        { 0x0070, { "LD (HL),B", &cpu::impl::Op_0070, 8 } },
        { 0x0071, { "LD (HL),C", &cpu::impl::Op_0071, 8 } },
        { 0x0072, { "LD (HL),D", &cpu::impl::Op_0072, 8 } },
        { 0x0073, { "LD (HL),E", &cpu::impl::Op_0073, 8 } },
        { 0x0074, { "LD (HL),H", &cpu::impl::Op_0074, 8 } },
        { 0x0075, { "LD (HL),L", &cpu::impl::Op_0075, 8 } },
        { 0x0036, { "LD (HL),n", &cpu::impl::Op_0036, 12 } },
        { 0x007F, { "LD A,A",    &cpu::impl::Op_007F, 4 } },
        { 0x000A, { "LD A,(BC)", &cpu::impl::Op_000A, 8 } },
        { 0x001A, { "LD A,(DE)", &cpu::impl::Op_001A, 8 } },
        { 0x00FA, { "LD A,(##)", &cpu::impl::Op_00FA, 16 } },
        { 0x003E, { "LD A,n",    &cpu::impl::Op_003E, 8 } },
        { 0x0047, { "LD A,B",    &cpu::impl::Op_0047, 4 } },
        { 0x004F, { "LD A,C",    &cpu::impl::Op_004F, 4 } },
        { 0x0057, { "LD A,D",    &cpu::impl::Op_0057, 4 } },
        { 0x005F, { "LD A,E",    &cpu::impl::Op_005F, 4 } },
        { 0x0067, { "LD A,H",    &cpu::impl::Op_0067, 4 } },
        { 0x006F, { "LD A,L",    &cpu::impl::Op_006F, 4 } },
        { 0x0002, { "LD (BC),A", &cpu::impl::Op_0002, 8 } },
        { 0x0012, { "LD (DE),A", &cpu::impl::Op_0002, 8 } },
        { 0x007E, { "LD (HL),A", &cpu::impl::Op_007E, 8 } },
        { 0x00FA, { "LD (##),A", &cpu::impl::Op_00FA, 16 } },
        { 0x00F2, { "LDH A,(0x00FF00+C)", &cpu::impl::Op_00F2, 8 } },
        { 0x00E2, { "LDH (0x00FF00+C),A", &cpu::impl::Op_00E2, 8 } },
        { 0x003A, { "LDD A,(HL)", &cpu::impl::Op_003A, 8 } },
        { 0x0032, { "LDD (HL),A", &cpu::impl::Op_0032, 8 } },
        { 0x002A, { "LDI A,(HL)", &cpu::impl::Op_002A, 8 } },
        { 0x0022, { "LDI (HL),A", &cpu::impl::Op_0022, 8 } },
        { 0x002A, { "LDH (0x00FF00+n),A", &cpu::impl::Op_002A, 12 } },
        { 0x0022, { "LDH A,(0x00FF00+n)", &cpu::impl::Op_0022, 12 } },

        // 16-bit Load
        { 0x0001, { "LD BC,##", &cpu::impl::Op_0001, 12 } },
        { 0x0011, { "LD DE,##", &cpu::impl::Op_0011, 12 } },
        { 0x0021, { "LD HL,##", &cpu::impl::Op_0021, 12 } },
        { 0x0031, { "LD SP,##", &cpu::impl::Op_0031, 12 } },
        { 0x00F9, { "LD SP,HL", &cpu::impl::Op_00F9, 8 } },
        { 0x00F8, { "LDHL SP,n", &cpu::impl::Op_00F8, 12 } },
        { 0x0008, { "LD (##),SP", &cpu::impl::Op_0008, 20 } },
        { 0x00F5, { "PUSH AF", &cpu::impl::Op_00F5, 16 } },
        { 0x00C5, { "PUSH BC", &cpu::impl::Op_00C5, 16 } },
        { 0x00D5, { "PUSH DE", &cpu::impl::Op_00D5, 16 } },
        { 0x00E5, { "PUSH HL", &cpu::impl::Op_00E5, 16 } },
        { 0x00F1, { "POP AF", &cpu::impl::Op_00F1, 12 } },
        { 0x00C1, { "POP BC", &cpu::impl::Op_00C1, 12 } },
        { 0x00D1, { "POP DE", &cpu::impl::Op_00D1, 12 } },
        { 0x00E1, { "POP HL", &cpu::impl::Op_00E1, 12 } },

        // 8-bit ALU
        { 0x0087, { "ADD A,A", &cpu::impl::Op_0087, 4 } },
        { 0x0080, { "ADD A,B", &cpu::impl::Op_0080, 4 } },
        { 0x0081, { "ADD A,C", &cpu::impl::Op_0081, 4 } },
        { 0x0082, { "ADD A,D", &cpu::impl::Op_0082, 4 } },
        { 0x0083, { "ADD A,E", &cpu::impl::Op_0083, 4 } },
        { 0x0084, { "ADD A,H", &cpu::impl::Op_0084, 4 } },
        { 0x0085, { "ADD A,L", &cpu::impl::Op_0085, 4 } },
        { 0x0086, { "ADD A,(HL)", &cpu::impl::Op_0086, 8 } },
        { 0x00C6, { "ADD A,##", &cpu::impl::Op_00C6, 8 } },

        { 0x008F, { "ADC A,A", &cpu::impl::Op_008F, 4 } },
        { 0x0088, { "ADC A,B", &cpu::impl::Op_0088, 4 } },
        { 0x0089, { "ADC A,C", &cpu::impl::Op_0089, 4 } },
        { 0x008A, { "ADC A,D", &cpu::impl::Op_008A, 4 } },
        { 0x008B, { "ADC A,E", &cpu::impl::Op_008B, 4 } },
        { 0x008C, { "ADC A,H", &cpu::impl::Op_008C, 4 } },
        { 0x008D, { "ADC A,L", &cpu::impl::Op_008D, 4 } },
        { 0x008E, { "ADC A,(HL)", &cpu::impl::Op_008E, 8 } },
        { 0x00CE, { "ADC A,##", &cpu::impl::Op_00CE, 8 } },

        { 0x0097, { "SUB A", &cpu::impl::Op_0097, 4 } },
        { 0x0090, { "SUB B", &cpu::impl::Op_0090, 4 } },
        { 0x0091, { "SUB C", &cpu::impl::Op_0091, 4 } },
        { 0x0092, { "SUB D", &cpu::impl::Op_0092, 4 } },
        { 0x0093, { "SUB E", &cpu::impl::Op_0093, 4 } },
        { 0x0094, { "SUB H", &cpu::impl::Op_0094, 4 } },
        { 0x0095, { "SUB L", &cpu::impl::Op_0095, 4 } },
        { 0x0096, { "SUB (HL)", &cpu::impl::Op_0096, 8 } },
        { 0x00D6, { "SUB ##", &cpu::impl::Op_00D6, 8 } },

        { 0x009F, { "SBC A,A", &cpu::impl::Op_009F, 4 } },
        { 0x0098, { "SBC A,B", &cpu::impl::Op_0098, 4 } },
        { 0x0099, { "SBC A,C", &cpu::impl::Op_0099, 4 } },
        { 0x009A, { "SBC A,D", &cpu::impl::Op_009A, 4 } },
        { 0x009B, { "SBC A,E", &cpu::impl::Op_009B, 4 } },
        { 0x009C, { "SBC A,H", &cpu::impl::Op_009C, 4 } },
        { 0x009D, { "SBC A,L", &cpu::impl::Op_009D, 4 } },
        { 0x009E, { "SBC A,(HL)", &cpu::impl::Op_009E, 8 } },

        { 0x00A7, { "AND A", &cpu::impl::Op_00A7, 4 } },
        { 0x00A0, { "AND B", &cpu::impl::Op_00A0, 4 } },
        { 0x00A1, { "AND C", &cpu::impl::Op_00A1, 4 } },
        { 0x00A2, { "AND D", &cpu::impl::Op_00A2, 4 } },
        { 0x00A3, { "AND E", &cpu::impl::Op_00A3, 4 } },
        { 0x00A4, { "AND H", &cpu::impl::Op_00A4, 4 } },
        { 0x00A5, { "AND L", &cpu::impl::Op_00A5, 4 } },
        { 0x00A6, { "AND (HL)", &cpu::impl::Op_00A6, 8 } },
        { 0x00E6, { "AND ##", &cpu::impl::Op_00E6, 8 } },

        { 0x00B7, { "OR A", &cpu::impl::Op_00B7, 4 } },
        { 0x00B0, { "OR B", &cpu::impl::Op_00B0, 4 } },
        { 0x00B1, { "OR C", &cpu::impl::Op_00B1, 4 } },
        { 0x00B2, { "OR D", &cpu::impl::Op_00B2, 4 } },
        { 0x00B3, { "OR E", &cpu::impl::Op_00B3, 4 } },
        { 0x00B4, { "OR H", &cpu::impl::Op_00B4, 4 } },
        { 0x00B5, { "OR L", &cpu::impl::Op_00B5, 4 } },
        { 0x00B6, { "OR (HL)", &cpu::impl::Op_00B6, 8 } },
        { 0x00F6, { "OR ##", &cpu::impl::Op_00F6, 8 } },

        { 0x00AF, { "XOR A", &cpu::impl::Op_00AF, 4 } },
        { 0x00A8, { "XOR B", &cpu::impl::Op_00A8, 4 } },
        { 0x00A9, { "XOR C", &cpu::impl::Op_00A9, 4 } },
        { 0x00AA, { "XOR D", &cpu::impl::Op_00AA, 4 } },
        { 0x00AB, { "XOR E", &cpu::impl::Op_00AB, 4 } },
        { 0x00AC, { "XOR H", &cpu::impl::Op_00AC, 4 } },
        { 0x00AD, { "XOR L", &cpu::impl::Op_00AD, 4 } },
        { 0x00AE, { "XOR (HL)", &cpu::impl::Op_00AE, 8 } },
        { 0x00EE, { "XOR ##", &cpu::impl::Op_00EE, 8 } },

        { 0x00BF, { "CP A", &cpu::impl::Op_00BF, 4 } },
        { 0x00B8, { "CP B", &cpu::impl::Op_00B8, 4 } },
        { 0x00B9, { "CP C", &cpu::impl::Op_00B9, 4 } },
        { 0x00BA, { "CP D", &cpu::impl::Op_00BA, 4 } },
        { 0x00BB, { "CP E", &cpu::impl::Op_00BB, 4 } },
        { 0x00BC, { "CP H", &cpu::impl::Op_00BC, 4 } },
        { 0x00BD, { "CP L", &cpu::impl::Op_00BD, 4 } },
        { 0x00BE, { "CP (HL)", &cpu::impl::Op_00BE, 8 } },
        { 0x00FE, { "CP ##", &cpu::impl::Op_00FE, 8 } },

        { 0x003C, { "INC A", &cpu::impl::Op_003C, 4 } },
        { 0x0004, { "INC B", &cpu::impl::Op_0004, 4 } },
        { 0x000C, { "INC C", &cpu::impl::Op_000C, 4 } },
        { 0x0014, { "INC D", &cpu::impl::Op_0014, 4 } },
        { 0x001C, { "INC E", &cpu::impl::Op_001C, 4 } },
        { 0x0024, { "INC H", &cpu::impl::Op_0024, 4 } },
        { 0x002C, { "INC L", &cpu::impl::Op_002C, 4 } },
        { 0x0034, { "INC (HL)", &cpu::impl::Op_0034, 12 } },

        { 0x003D, { "DEC A", &cpu::impl::Op_003D, 4 } },
        { 0x0005, { "DEC B", &cpu::impl::Op_0005, 4 } },
        { 0x000D, { "DEC C", &cpu::impl::Op_000D, 4 } },
        { 0x0015, { "DEC D", &cpu::impl::Op_0015, 4 } },
        { 0x001D, { "DEC E", &cpu::impl::Op_001D, 4 } },
        { 0x0025, { "DEC H", &cpu::impl::Op_0025, 4 } },
        { 0x002D, { "DEC L", &cpu::impl::Op_002D, 4 } },
        { 0x0035, { "DEC (HL)", &cpu::impl::Op_0035, 12 } },

        // 16-bit Arithmetic
        { 0x0009, { "ADD HL,BC", &cpu::impl::Op_0009, 8 } },
        { 0x0019, { "ADD HL,DE", &cpu::impl::Op_0019, 8 } },
        { 0x0029, { "ADD HL,HL", &cpu::impl::Op_0029, 8 } },
        { 0x0039, { "ADD HL,SP", &cpu::impl::Op_0039, 8 } },
        { 0x00E8, { "ADD SP,n",  &cpu::impl::Op_00E8, 16 } },

        { 0x000B, { "DEC BC", &cpu::impl::Op_000B, 8 } },
        { 0x001B, { "DEC DE", &cpu::impl::Op_001B, 8 } },
        { 0x002B, { "DEC HL", &cpu::impl::Op_002B, 8 } },
        { 0x003B, { "DEC SP", &cpu::impl::Op_003B, 8 } },

        // Miscellaneous
        { 0x0037, { "SWAP A",    &cpu::impl::Op_0037, 8 } },
        { 0x0030, { "SWAP B",    &cpu::impl::Op_0030, 8 } },
        { 0x0031, { "SWAP C",    &cpu::impl::Op_0031, 8 } },
        { 0x0032, { "SWAP D",    &cpu::impl::Op_0032, 8 } },
        { 0x0033, { "SWAP E",    &cpu::impl::Op_0033, 8 } },
        { 0x0034, { "SWAP H",    &cpu::impl::Op_0034, 8 } },
        { 0x0035, { "SWAP L",    &cpu::impl::Op_0035, 8 } },
        { 0x0036, { "SWAP (HL)", &cpu::impl::Op_0036, 16 } },

        { 0x0027, { "DAA",  &cpu::impl::Op_0027, 4 } },
        { 0x002F, { "CPL",  &cpu::impl::Op_002F, 4 } },
        { 0x003F, { "CCF",  &cpu::impl::Op_003F, 4 } },
        { 0x0037, { "SCF",  &cpu::impl::Op_0037, 4 } },
        { 0x0000, { "NOP",  &cpu::impl::Op_0000, 4 } },
        { 0x0076, { "HALT", &cpu::impl::Op_0076, 4 } },
        { 0x1000, { "STOP", &cpu::impl::Op_1000, 4 } },
        { 0x00F3, { "DI",   &cpu::impl::Op_00F3, 4 } },
        { 0x00FB, { "EI",   &cpu::impl::Op_00FB, 4 } },

        // Rotates and Shifts
        { 0x0007, { "RLCA", &cpu::impl::Op_0007, 4 } },
        { 0x0017, { "RLA",  &cpu::impl::Op_0017,  4 } },
        { 0x000F, { "RRCA", &cpu::impl::Op_000F, 4 } },
        { 0x001F, { "RRA",  &cpu::impl::Op_001F,  4 } },

        { 0xCB07, { "RLC A",    &cpu::impl::Op_CB07, 8 } },
        { 0xCB00, { "RLC B",    &cpu::impl::Op_CB00, 8 } },
        { 0xCB01, { "RLC C",    &cpu::impl::Op_CB01, 8 } },
        { 0xCB02, { "RLC D",    &cpu::impl::Op_CB02, 8 } },
        { 0xCB03, { "RLC E",    &cpu::impl::Op_CB03, 8 } },
        { 0xCB04, { "RLC H",    &cpu::impl::Op_CB04, 8 } },
        { 0xCB05, { "RLC L",    &cpu::impl::Op_CB05, 8 } },
        { 0xCB06, { "RLC (HL)", &cpu::impl::Op_CB06, 16 } },

        { 0xCB17, { "RL A",    &cpu::impl::Op_CB17, 8 } },
        { 0xCB10, { "RL B",    &cpu::impl::Op_CB10, 8 } },
        { 0xCB11, { "RL C",    &cpu::impl::Op_CB11, 8 } },
        { 0xCB12, { "RL D",    &cpu::impl::Op_CB12, 8 } },
        { 0xCB13, { "RL E",    &cpu::impl::Op_CB13, 8 } },
        { 0xCB14, { "RL H",    &cpu::impl::Op_CB14, 8 } },
        { 0xCB15, { "RL L",    &cpu::impl::Op_CB15, 8 } },
        { 0xCB16, { "RL (HL)", &cpu::impl::Op_CB16, 16 } },

        { 0xCB0F, { "RRC A", &cpu::impl::Op_CB0F, 8 } },
        { 0xCB08, { "RRC B", &cpu::impl::Op_CB08, 8 } },
        { 0xCB09, { "RRC C", &cpu::impl::Op_CB09, 8 } },
        { 0xCB0A, { "RRC D", &cpu::impl::Op_CB0A, 8 } },
        { 0xCB0B, { "RRC E", &cpu::impl::Op_CB0B, 8 } },
        { 0xCB0C, { "RRC H", &cpu::impl::Op_CB0C, 8 } },
        { 0xCB0D, { "RRC L", &cpu::impl::Op_CB0D, 8 } },
        { 0xCB0E, { "RRC (HL)", &cpu::impl::Op_CB0E, 16 } },

        { 0xCB1F, { "RR A", &cpu::impl::Op_CB1F, 8 } },
        { 0xCB18, { "RR B", &cpu::impl::Op_CB18, 8 } },
        { 0xCB19, { "RR C", &cpu::impl::Op_CB19, 8 } },
        { 0xCB1A, { "RR D", &cpu::impl::Op_CB1A, 8 } },
        { 0xCB1B, { "RR E", &cpu::impl::Op_CB1B, 8 } },
        { 0xCB1C, { "RR H", &cpu::impl::Op_CB1C, 8 } },
        { 0xCB1D, { "RR L", &cpu::impl::Op_CB1D, 8 } },
        { 0xCB1E, { "RR (HL)", &cpu::impl::Op_CB1E, 16 } },

        { 0xCB27, { "SLA A", &cpu::impl::Op_CB27, 8 } },
        { 0xCB20, { "SLA B", &cpu::impl::Op_CB20, 8 } },
        { 0xCB21, { "SLA C", &cpu::impl::Op_CB21, 8 } },
        { 0xCB22, { "SLA D", &cpu::impl::Op_CB22, 8 } },
        { 0xCB23, { "SLA E", &cpu::impl::Op_CB23, 8 } },
        { 0xCB24, { "SLA H", &cpu::impl::Op_CB24, 8 } },
        { 0xCB25, { "SLA L", &cpu::impl::Op_CB25, 8 } },
        { 0xCB26, { "SLA (HL)", &cpu::impl::Op_CB26, 16 } },

        { 0xCB2F, { "SRA A", &cpu::impl::Op_CB2F, 8 } },
        { 0xCB28, { "SRA B", &cpu::impl::Op_CB28, 8 } },
        { 0xCB29, { "SRA C", &cpu::impl::Op_CB29, 8 } },
        { 0xCB2A, { "SRA D", &cpu::impl::Op_CB2A, 8 } },
        { 0xCB2B, { "SRA E", &cpu::impl::Op_CB2B, 8 } },
        { 0xCB2C, { "SRA H", &cpu::impl::Op_CB2C, 8 } },
        { 0xCB2D, { "SRA L", &cpu::impl::Op_CB2D, 8 } },
        { 0xCB2E, { "SRA (HL)", &cpu::impl::Op_CB2E, 16 } },

        { 0xCB3F, { "SRL A", &cpu::impl::Op_CB3F, 8 } },
        { 0xCB38, { "SRL B", &cpu::impl::Op_CB38, 8 } },
        { 0xCB39, { "SRL C", &cpu::impl::Op_CB39, 8 } },
        { 0xCB3A, { "SRL D", &cpu::impl::Op_CB3A, 8 } },
        { 0xCB3B, { "SRL E", &cpu::impl::Op_CB3B, 8 } },
        { 0xCB3C, { "SRL H", &cpu::impl::Op_CB3C, 8 } },
        { 0xCB3D, { "SRL L", &cpu::impl::Op_CB3D, 8 } },
        { 0xCB3E, { "SRL (HL)", &cpu::impl::Op_CB3E, 16 } },

        // Bit Operations
        { 0xCB47, { "BIT b,A",    &cpu::impl::Op_CB47, 8 } },
        { 0xCB40, { "BIT b,B",    &cpu::impl::Op_CB40, 8 } },
        { 0xCB41, { "BIT b,C",    &cpu::impl::Op_CB41, 8 } },
        { 0xCB42, { "BIT b,D",    &cpu::impl::Op_CB42, 8 } },
        { 0xCB43, { "BIT b,E",    &cpu::impl::Op_CB43, 8 } },
        { 0xCB44, { "BIT b,H",    &cpu::impl::Op_CB44, 8 } },
        { 0xCB45, { "BIT b,L",    &cpu::impl::Op_CB45, 8 } },
        { 0xCB46, { "BIT b,(HL)", &cpu::impl::Op_CB46, 16 } },

        { 0xCBC7, { "SET b,A",    &cpu::impl::Op_CBC7, 8 } },
        { 0xCBC0, { "SET b,B",    &cpu::impl::Op_CBC0, 8 } },
        { 0xCBC1, { "SET b,C",    &cpu::impl::Op_CBC1, 8 } },
        { 0xCBC2, { "SET b,D",    &cpu::impl::Op_CBC2, 8 } },
        { 0xCBC3, { "SET b,E",    &cpu::impl::Op_CBC3, 8 } },
        { 0xCBC4, { "SET b,H",    &cpu::impl::Op_CBC4, 8 } },
        { 0xCBC5, { "SET b,L",    &cpu::impl::Op_CBC5, 8 } },
        { 0xCBC6, { "SET b,(HL)", &cpu::impl::Op_CBC6, 16 } },

        { 0xCBC8, { "RES b,A",    &cpu::impl::Op_CB87, 8 } },
        { 0xCB80, { "RES b,B",    &cpu::impl::Op_CB80, 8 } },
        { 0xCB81, { "RES b,C",    &cpu::impl::Op_CB81, 8 } },
        { 0xCB82, { "RES b,D",    &cpu::impl::Op_CB82, 8 } },
        { 0xCB83, { "RES b,E",    &cpu::impl::Op_CB83, 8 } },
        { 0xCB84, { "RES b,H",    &cpu::impl::Op_CB84, 8 } },
        { 0xCB85, { "RES b,L",    &cpu::impl::Op_CB85, 8 } },
        { 0xCB86, { "RES b,(HL)", &cpu::impl::Op_CB86, 16 } },

        // Jumps
        { 0x00C3, { "JP ##",    &cpu::impl::Op_00C3, 12 } },
        { 0x00C2, { "JP NZ,##", &cpu::impl::Op_00C2, 12 } },
        { 0x00CA, { "JP Z,##",  &cpu::impl::Op_00CA, 12 } },
        { 0x00D2, { "JP NC,##", &cpu::impl::Op_00D2, 12 } },
        { 0x00DA, { "JP C,##",  &cpu::impl::Op_00DA, 12 } },
        { 0x00E9, { "JP (HL)",  &cpu::impl::Op_00E9, 4 } },
        { 0x0018, { "JR #",     &cpu::impl::Op_0018, 8 } },
        { 0x0020, { "JR NZ,#",  &cpu::impl::Op_0020, 8 } },
        { 0x0028, { "JR Z,#",   &cpu::impl::Op_0028, 8 } },
        { 0x0030, { "JR NC,#",  &cpu::impl::Op_0030, 8 } },
        { 0x0038, { "JR C,#" ,  &cpu::impl::Op_0038, 8 } },

        // Calls
        { 0x00CD, { "CALL ##",    &cpu::impl::Op_00CD, 12 } },
        { 0x00C4, { "CALL NZ,##", &cpu::impl::Op_00C4, 12 } },
        { 0x00C4, { "CALL Z,##",  &cpu::impl::Op_00C4, 12 } },
        { 0x00C4, { "CALL NC,##", &cpu::impl::Op_00C4, 12 } },
        { 0x00C4, { "CALL C,##",  &cpu::impl::Op_00C4, 12 } },

        // Restarts
        { 0x00C7, { "RST 0x00", &cpu::impl::Op_00C7, 32 } },
        { 0x00CF, { "RST 0x08", &cpu::impl::Op_00CF, 32 } },
        { 0x00D7, { "RST 0x10", &cpu::impl::Op_00D7, 32 } },
        { 0x00DF, { "RST 0x18", &cpu::impl::Op_00DF, 32 } },
        { 0x00E7, { "RST 0x20", &cpu::impl::Op_00E7, 32 } },
        { 0x00EF, { "RST 0x28", &cpu::impl::Op_00EF, 32 } },
        { 0x00F7, { "RST 0x30", &cpu::impl::Op_00F7, 32 } },
        { 0x00FF, { "RST 0x38", &cpu::impl::Op_00FF, 32 } },

        // Returns
        { 0x00C9, { "RET",    &cpu::impl::Op_00C9, 8 } },
        { 0x00C0, { "RET NZ", &cpu::impl::Op_00C0, 8 } },
        { 0x00C8, { "RET Z",  &cpu::impl::Op_00C8, 8 } },
        { 0x00D0, { "RET NC", &cpu::impl::Op_00D0, 8 } },
        { 0x00D8, { "RET C",  &cpu::impl::Op_00D8, 8 } },
        { 0x00D9, { "RETI",   &cpu::impl::Op_00D9,8 } },
};

//------------------------------------------------------------------------------
// Operations
//------------------------------------------------------------------------------

void cpu::impl::LD() {
        operand1->Set(operand2->Get());
}

//! Push register pair nn onto stack. Decrement Stack Pointer (SP) twice.
//! Write Little-Endian, so the LSB occurs first in memory.
void cpu::impl::PUSH() {
        uint16_t word = operand1->Get();
        mBus->Write(--mCpu->SP, static_cast<uint8_t>(word >> 8));
        mBus->Write(--mCpu->SP, static_cast<uint8_t>(word | 0xFF));
}

//! Pop two bytes off stack into register pair nn. Increment Stack Pointer (SP)
//! twice.
void cpu::impl::POP() {
        uint16_t word = 0;
        uint16_t byte = mBus->Read(mCpu->SP++);
        word = byte << 8;
        byte = mBus->Read(mCpu->SP++);
        word |= byte;

        operand1->Set(word);
}

void cpu::impl::ADD8() {
        uint8_t left = operand1->Get();
        uint8_t right = operand2->Get();

        uint8_t sum = left + right;
        bool halfCarry = ((left & 0xF) + (right & 0xF)) & 0x10;
        bool carry = ((uint16_t)left + (uint16_t)right) & 0x100;

        operand1->Set(sum);

        mCpu->FlagSet('z', 0 == sum);
        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('h', halfCarry);
        mCpu->FlagSet('c', carry);
}

void cpu::impl::ADD16() {
        uint16_t left = operand1->Get();
        uint16_t right = operand2->Get();

        uint16_t sum = left + right;
        bool halfCarry = ((left & 0xFFF) + (right & 0xFFF)) & 0x1000;
        bool carry = ((uint32_t)left + (uint32_t)right) & 0x10000;

        operand1->Set(sum);

        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('h', halfCarry);
        mCpu->FlagSet('c', carry);
}

//! The operand, along with the Carry Flag (C in the F Register) is added to the
//! contents of the Accumulator, and the result is stored in the Accumulator.
void cpu::impl::ADC8() {
        uint8_t left = operand1->Get();
        uint8_t right = operand2->Get();

        uint8_t prevCarry = mCpu->FlagGet('c') ? 1 : 0;
        uint8_t sum = left + right + prevCarry;
        bool halfCarry = ((left & 0xF) + (right & 0xF) + prevCarry) & 0x10;
        bool carry = ((uint16_t)left + (uint16_t)right + prevCarry) & 0x100;

        operand1->Set(sum);

        mCpu->FlagSet('z', 0 == sum);
        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('h', halfCarry);
        mCpu->FlagSet('c', carry);
}

void cpu::impl::SUB8() {
        int16_t minuend = mCpu->A;
        int16_t subtrahend = operand1->Get();

        int16_t difference = minuend - subtrahend;
        bool halfCarry = (minuend & 0xF) < (subtrahend & 0xF);
        bool carry = minuend < subtrahend;

        operand1->Set(static_cast<uint8_t>(difference));

        mCpu->FlagSet('z', 0 == difference);
        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('h', halfCarry); // TODO: invert logic?
        mCpu->FlagSet('c', carry); // TODO: invert logic?
}

void cpu::impl::SBC8() {
        int16_t minuend = mCpu->A;
        uint8_t prevCarry = mCpu->FlagGet('c') ? 1 : 0;
        int16_t subtrahend = operand1->Get() + prevCarry;

        uint16_t difference = minuend - subtrahend;
        bool halfCarry = (minuend & 0xF) < (subtrahend & 0xF);
        bool carry = minuend < subtrahend;

        operand1->Set(static_cast<uint8_t>(difference));

        mCpu->FlagSet('z', 0 == difference);
        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('h', halfCarry); // TODO: invert logic?
        mCpu->FlagSet('c', carry); // TODO: invert logic?
}

void cpu::impl::AND() {
        mCpu->A &= operand1->Get();

        mCpu->FlagSet('z', 0 == mCpu->A);
        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('h', 0);
        mCpu->FlagSet('c', 0);
}

void cpu::impl::OR() {
        mCpu->A |= operand1->Get();

        mCpu->FlagSet('z', 0 == mCpu->A);
        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('h', 0);
        mCpu->FlagSet('c', 0);
}

void cpu::impl::XOR() {
        mCpu->A ^= operand1->Get();

        mCpu->FlagSet('z', 0 == mCpu->A);
        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('h', 0);
        mCpu->FlagSet('c', 0);
}

void cpu::impl::CP() {
        int16_t minuend = mCpu->A;
        int16_t subtrahend = operand1->Get();
        int16_t result = minuend - subtrahend;
        bool halfCarry = (minuend & 0xF) < (subtrahend & 0xF);

        mCpu->FlagSet('z', 0 == result);
        mCpu->FlagSet('n', 1);
        mCpu->FlagSet('h', halfCarry);
        mCpu->FlagSet('c', minuend < subtrahend);
}

void cpu::impl::INC8() {
        auto val = operand1->Get();
        bool halfCarry = ((val & 0xF) + 1) & 0x10;
        operand1->Set(++val);

        mCpu->FlagSet('z', 0 == val);
        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('h', halfCarry);
}

void cpu::impl::DEC8() {
        auto val = operand1->Get();
        bool halfCarry = (val & 0xF) < 1;
        operand1->Set(--val);

        mCpu->FlagSet('z', 0 == val);
        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('h', halfCarry);
}

void cpu::impl::INC16() {
        auto value = operand1->Get();
        operand1->Set(value++);
}

void cpu::impl::DEC16() {
        auto value = operand1->Get();
        operand1->Set(value--);
}

//! \brief Swap upper & lower nibbles of operand.
void cpu::impl::SWAP() {
        uint8_t oldValue = operand1->Get();
        uint8_t nibbleHi = Nibble(oldValue, 1);
        uint8_t nibbleLo = Nibble(oldValue, 0);
        uint8_t newValue = (nibbleLo << 4) | nibbleHi;
        operand2->Set(newValue);

        mCpu->FlagSet('z', !newValue);
        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('h', 0);
        mCpu->FlagSet('c', 0);
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
void cpu::impl::DAA() {
        uint8_t val = mCpu->A;

        uint8_t wasCarryHalf = mCpu->FlagGet('h');
        uint8_t wasCarry = mCpu->FlagGet('c');
        uint8_t wasSubtraction = mCpu->FlagGet('n');

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
        mCpu->A = newValue;

        mCpu->FlagSet('z', !newValue);
        mCpu->FlagSet('h', 0);
        mCpu->FlagSet('p', Parity(newValue));
        mCpu->FlagSet('c', newValue > 0x99); // TODO verify against table in Z80 manual
}

//! \brief The contents of the Accumulator (Register A) are inverted (one’s complement).
void cpu::impl::CPL() {
        uint8_t val = ~(mCpu->A);
        operand2->Set(val);

        mCpu->FlagSet('h', 1);
        mCpu->FlagSet('n', 1);
}

// // TODO: Not used in GB?
// //! \brief Perform two's complement negation on A.
// //!
// //! The contents of the Accumulator are negated (two’s complement). This method
// //! is the same as subtracting the contents of the Accumulator from zero.
// // void cpu::NEG() {
// //         operand2 = ((*this).*(instruction->getOperand2))();
// //         uint16_t val = static_cast<uint8_t>(operand2->Get());
// //         uint16_t onesComplement = ~val;
// //         uint16_t twosComplement = onesComplement + 1;
// //         operand2->Set(static_cast<uint8_t>(twosComplement));

// //         FlagSet('s', twosComplement & 0x7);
// //         FlagSet('z', !twosComplement);
// //         // TODO: set if borrow from bit 4. // FlagSet('h', (twosComplement >> 0x4)
// //         FlagSet('p', 0x80 == val);
// //         FlagSet('n', 0);
// //         FlagSet('c', !(0x00 == val));
// // }

//! \brief The carry flag is inverted in the F register.
void cpu::impl::CCF() {
        mCpu->FlagSet('h', 0);
        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('c', !mCpu->FlagGet('c'));
}

//! \brief the carry flag is set in the F register.
void cpu::impl::SCF() {
        mCpu->FlagSet('h', 0);
        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('c', 1);
}

void cpu::impl::NOP() {
}

//! \brief Power down CPU until an interrupt occurs.
//!
//! The HALT instruction suspends CPU operation until a subsequent interrupt or
//! reset is received. While in the HALT state, the processor executes NOPs to
//! maintain memory refresh logic.
void cpu::impl::HALT() {
        halted = true;
        waitForButtonPress = false;
}

//! \brief Halt CPU & LCD display until button press.
void cpu::impl::STOP() {
        halted = true;
        waitForButtonPress = true;
}

//! \brief Disable interrupts after next instruction.
//!
//! When the CPU executes the instruction DI the maskable interrupt is disabled
//! until it is subsequently re-enabled by an EI instruction. The CPU does not
//! respond to an Interrupt Request (INT) signal.
void cpu::impl::DI() {
        interruptsDisabledRequested = true;
}

//! \brief Enable interrupts after next instruction.
void cpu::impl::EI() {
        interruptsEnabledRequested = true;
}

//! \brief Rotate A left. Old bit 7 is copied to the carry flag.
//!
//! The contents of the Accumulator (Register A) are rotated left 1 bit
//! position. The sign bit (bit 7) is copied to the Carry flag and also to bit
//! 0. Bit 0 is the least-significant bit.
void cpu::impl::RLCA() {
        uint8_t oldValue = mCpu->A;
        uint8_t carry = ((uint8_t)oldValue >> 0x7) & 0x1;
        uint8_t newValue = ((uint8_t)oldValue << 1) | carry;
        mCpu->A = newValue;

        mCpu->FlagSet('z', !newValue);
        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('h', 0);
        mCpu->FlagSet('c', carry);
}

//! \brief Rotate A left through the carry flag.
//!
//! The contents of the Accumulator (Register A) are rotated left 1 bit position
//! through the Carry flag. The previous contents of the Carry flag are copied
//! to bit 0. Bit 0 is the least-significant bit.
void cpu::impl::RLA() {
        uint16_t oldValue = mCpu->A;
        uint8_t carry = ((uint8_t)oldValue >> 0x7) & 0x1;
        uint8_t oldCarry = mCpu->FlagGet('c');
        uint8_t newValue = ((uint8_t)oldValue << 1) | oldCarry;
        mCpu->A = newValue;

        mCpu->FlagSet('z', !newValue);
        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('h', 0);
        mCpu->FlagSet('c', carry);
}

//! \brief Rotate A right. Old bit 0 is copied to the carry flag.
//!
//! The contents of the Accumulator (Register A) are rotated right 1 bit
//! position. Bit 0 is cop-ied to the Carry flag and also to bit 7. Bit 0 is the
//! least-significant bit.
void cpu::impl::RRCA() {
        uint16_t oldValue = mCpu->A;
        uint8_t carry = (uint8_t)oldValue & 0x1;
        uint8_t newValue = ((uint8_t)oldValue >> 1) | (carry << 0x7);
        mCpu->A = newValue;

        mCpu->FlagSet('z', !newValue);
        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('h', 0);
        mCpu->FlagSet('c', carry);
}

//! \brief Rotate A right through the carry flag.
//!
//! The contents of the Accumulator (Register A) are rotated right 1 bit
//! position through the Carry flag. The previous contents of the Carry flag are
//! copied to bit 7. Bit 0 is the least-significant bit.
void cpu::impl::RRA() {
        uint16_t oldValue = mCpu->A;
        uint8_t carry = (uint8_t)oldValue & 0x1;
        uint8_t oldCarry = mCpu->FlagGet('c');
        uint8_t newValue = ((uint8_t)oldValue >> 1) | (oldCarry << 0x7);
        mCpu->A = newValue;

        mCpu->FlagSet('z', !newValue);
        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('h', 0);
        mCpu->FlagSet('c', carry);
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
void cpu::impl::RLC() {
        uint16_t oldValue = operand1->Get();
        uint8_t carry = ((uint8_t)oldValue >> 0x7) & 0x1;
        uint8_t newValue = ((uint8_t)oldValue << 1) | carry;
        operand1->Set(newValue);

        mCpu->FlagSet('s', newValue & (0x1 << 0x7));
        mCpu->FlagSet('z', !newValue);
        mCpu->FlagSet('h', 0);
        mCpu->FlagSet('p', Parity(newValue));
        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('c', carry);
}

//! \brief Rotate left through carry flag.
//!
//! The contents of the m operand are rotated left 1 bit position. The contents
//! of bit 7 are copied to the Carry flag, and the previous contents of the
//! Carry flag are copied to bit 0.
void cpu::impl::RL() {
        uint16_t oldValue = operand1->Get();
        uint8_t carry = ((uint8_t)oldValue >> 0x7) & 0x1;
        uint8_t oldCarry = mCpu->FlagGet('c');
        uint8_t newValue = ((uint8_t)oldValue << 1) | oldCarry;
        operand1->Set(newValue);

        mCpu->FlagSet('s', newValue & (0x1 << 0x7));
        mCpu->FlagSet('z', !newValue);
        mCpu->FlagSet('h', 0);
        mCpu->FlagSet('p', Parity(newValue));
        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('c', carry);
}

//! \brief Rotate right. Old bit 0 is set to the carry flag.
//!
//! The contents of the m operand are rotated right 1 bit position. The contents
//! of bit 0 are copied to the Carry flag and also to bit 7. Bit 0 is the
//! least-significant bit.
void cpu::impl::RRC() {
        uint16_t oldValue = operand1->Get();
        uint8_t carry = (uint8_t)oldValue & 0x1;
        uint8_t newValue = ((uint8_t)oldValue >> 1) | carry;
        operand1->Set(newValue);

        mCpu->FlagSet('s', newValue & (0x1 << 0x7));
        mCpu->FlagSet('z', !newValue);
        mCpu->FlagSet('h', 0);
        mCpu->FlagSet('p', Parity(newValue));
        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('c', carry);
}

//! \brief Rotate n right through carry flag.
//!
//! The contents of operand m are rotated right 1 bit position through the carry
//! flag. The contents of bit 0 are copied to the carry flag and the previous
//! contents of the carry flag are copied to bit 7. Bit 0 is the
//! least-significant bit.
void cpu::impl::RR() {
        uint16_t oldValue = operand1->Get();
        uint8_t carry = (uint8_t)oldValue & 0x1;
        uint8_t oldCarry = mCpu->FlagGet('c');
        uint8_t newValue = ((uint8_t)oldValue >> 1) | oldCarry;
        operand1->Set(newValue);

        mCpu->FlagSet('s', newValue & (0x1 << 0x7));
        mCpu->FlagSet('z', !newValue);
        mCpu->FlagSet('h', 0);
        mCpu->FlagSet('p', Parity(newValue));
        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('c', carry);
}

//! \brief Shift left into carry. LSB is set to 0.
//!
//! An arithmetic shift left 1 bit position is performed on the contents of
//! operand m. The contents of bit 7 are copied to the Carry flag. Bit 0 is the
//! least-significant bit.
void cpu::impl::SLA() {
        uint8_t oldValue = operand1->Get();
        uint8_t carry = oldValue & (0x1 << 0x7);
        uint8_t newValue = oldValue << 1;
        operand1->Set(newValue);

        mCpu->FlagSet('s', (newValue >> 0x7) & 0x1);
        mCpu->FlagSet('z', !newValue);
        mCpu->FlagSet('h', 0);
        mCpu->FlagSet('p', Parity(newValue));
        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('c', carry);
}

//! \brief Shift right into Carry. MSB doesn't change.
//!
//! An arithmetic shift right 1 bit position is performed on the contents of
//! operand m. The contents of bit 0 are copied to the Carry flag and the
//! previous contents of bit 7 remain unchanged. Bit 0 is the least-significant
//! bit.
void cpu::impl::SRA() {
        uint8_t oldValue = operand1->Get();
        uint8_t msb = (oldValue & 0x7);
        uint8_t carry = oldValue & 0x1;
        uint8_t newValue = (oldValue >> 1) | msb;
        operand1->Set(newValue);

        mCpu->FlagSet('z', !newValue);
        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('h', 0);
        mCpu->FlagSet('c', carry);
}

//! \brief Shift right into Carry. MSB is set to 0.
//!
//! The contents of operand m are shifted right 1 bit position. The contents of
//! bit 0 are copied to the Carry flag, and bit 7 is reset. Bit 0 is the
//! least-significant bit.
void cpu::impl::SRL() {
        uint8_t oldValue = operand1->Get();
        uint8_t carry = oldValue & 0x1;
        uint8_t newValue = oldValue >> 1;
        operand1->Set(newValue);

        mCpu->FlagSet('z', !newValue);
        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('h', 0);
        mCpu->FlagSet('c', carry);
}

//! \brief Test bit in register
//!
//! This instruction tests bit b in register r and sets the Z flag
//! accordingly.
void cpu::impl::BIT() {
        uint8_t bit = operand1->Get();
        uint8_t byte = operand2->Get();
        uint8_t test = 0x1 << bit;

        mCpu->FlagSet('z', !(byte & test));
        mCpu->FlagSet('n', 0);
        mCpu->FlagSet('h', 1);
}

//! \brief Set bit in register
//!
//! Bit b in register r (any of registers B, C, D, E, H, L, or A) is set.
void cpu::impl::SET() {
        uint8_t bit = operand1->Get();
        uint8_t byte = operand2->Get();
        uint8_t set = 0x1 << bit;
        operand2->Set(byte | set);
}

//! Reset bit in register.
void cpu::impl::RES() {
        uint8_t bit = operand1->Get();
        uint8_t byte = operand2->Get();
        uint8_t set = 0x1 << bit;
        set = ~set;
        operand2->Set(byte & set);
}

void cpu::impl::JP() {
        mCpu->PC = operand1->Get();
}

//! \brief Add n to current address and jump to it
void cpu::impl::JR() {
        uint16_t address = mCpu->PC + operand1->Get();
        mCpu->PC = address;
}

//! \brief Push address of next instruction onto the stack and then jump to address nn
void cpu::impl::CALL() {
        mBus->Write(--mCpu->SP, static_cast<uint8_t>(mCpu->PC & 0xFF));
        mBus->Write(--mCpu->SP, static_cast<uint8_t>(mCpu->PC >> 8));
        mCpu->PC = operand1->Get();
}

//! Push present address onto stack. Jump to address $0000 + n.
void cpu::impl::RST() {
        mBus->Write(--mCpu->SP, static_cast<uint8_t>(mCpu->PC & 0xFF));
        mBus->Write(--mCpu->SP, static_cast<uint8_t>(mCpu->PC >> 8));
        mCpu->PC = operand1->Get();
}

void cpu::impl::RET() {
        uint16_t address;
        uint8_t byte = mBus->Read(mCpu->SP++);
        address = byte << 8;
        byte = mBus->Read(mCpu->SP++);
        address = address | byte;
        mCpu->PC = address;
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
void cpu::impl::RETI() {
        RET();
        interruptsDisabled = false;
        interruptsDisabledRequested = false;
        interruptsEnabledRequested = false;
}

//------------------------------------------------------------------------------
// Public interface
//------------------------------------------------------------------------------

void cpu::InstructionFetch() {
        uint8_t opcodeByte;
        opcode = 0;

        opcodeByte = msgBus->Read(PC++);

        // Some opcodes are two-bytes long; these are prefixed with the byte
        // 0x10 or 0xCB.
        // TODO: I'm not sure if these are ordered B1|B2 or B2|B1.
        //       I'm assuming B1|B2 in this implementationementation but may need to
        //       follow up on that.
        if (0x10 == opcodeByte || 0xCB == opcodeByte) {
                opcode = (opcodeByte << 8);
                opcodeByte = msgBus->Read(PC++);
        }

        opcode = (opcode | opcodeByte);
        implementation->mInstruction = std::make_shared<instruction>(implementation->instructionMap[opcode]);
}

void cpu::InstructionExecute() {
        ((*implementation).*(implementation->mInstruction->op))();
}

} // namespace gs
