/******************************************************************************
 * File: Cpu.cpp
 * Created: 2019-08-29
 * Updated: 2021-01-02
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2020, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
//! \file cpu.cpp

#include <string>
#include <map>
#include <cassert>
#include <iostream>
#include <iomanip>

#include "cpu.hpp"
#include "bus.hpp"
#include "operand.hpp"

#include "cpu_impl.cpp"

namespace gs {

struct Instruction {
        std::string name;
        void (Cpu::Impl::*op)();
        unsigned int cycles;

        friend std::ostream& operator<<(std::ostream& out, Instruction i) {
                using namespace std;
                ostream fmt(NULL);
                fmt.copyfmt(out);

                out << "{ name: '" << i.name;
                out << "', cycles: '" << dec << i.cycles << "' }";

                out.copyfmt(fmt);
                return out;
        }
};

Cpu::Cpu() {
        impl = new Impl(this);
        reset();
}

void Cpu::reset() {
        registers.r16.BC = 0;
        registers.r16.DE = 0;
        registers.r16.HL = 0;
        registers.r16.AF = 0;
        R = 0;
        PC = 0x100;
        SP = 0;
        I = 0;
        opcode = 0x0;
}

Cpu::~Cpu() {
        delete impl;
}

void Cpu::attach(Bus *bus) {
        this->bus = bus;
        impl->bus = bus;
}

void Cpu::flagSet(char c, uint8_t onOrOff) {
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
        flagSet(bit, onOrOff);
}

void Cpu::flagSet(uint8_t bit, uint8_t onOrOff) {
        uint8_t bitFlag = 1 << bit;

        if (onOrOff) {
                registers.r8.F |= bitFlag;
        } else {
                registers.r8.F &= (~bitFlag);
        }
}

uint8_t Cpu::flagGet(char c) {
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
        return flagGet(bit);
}

uint8_t Cpu::flagGet(uint8_t bitToCheck) {
        uint8_t bit = registers.r8.F & (1 << bitToCheck);
        return bit >> bitToCheck;
}

Cpu::Impl::Impl(Cpu *CpuIn) {
        cpu = CpuIn;
        InitInstructionMap();
}

Cpu::Impl::~Impl() {
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

void Cpu::Impl::Op_0006() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandValueByte>(bus->read(cpu->PC++));
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_000E() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandValueByte>(bus->read(cpu->PC++));
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0016() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandValueByte>(bus->read(cpu->PC++));
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_001E() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandValueByte>(bus->read(cpu->PC++));
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0026() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandValueByte>(bus->read(cpu->PC++));
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_002E() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandValueByte>(bus->read(cpu->PC++));
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

// These Instructions are of the form LD r1,r2 where the value of register r2 is
// loaded into the register r1.

void Cpu::Impl::Op_007F() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0078() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0079() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_007A() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_007B() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_007C() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_007D() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_007E() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0040() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0041() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0042() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0043() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0044() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0045() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0046() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0048() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0049() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_004A() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_004B() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_004C() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_004D() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_004E() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0050() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0051() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0052() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0053() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0054() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0055() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0056() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0058() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0059() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_005A() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_005B() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_005C() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_005D() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_005E() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0060() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0061() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0062() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0063() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0064() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0065() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0066() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0068() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0069() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_006A() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_006B() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_006C() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_006D() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_006E() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0070() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0071() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0072() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0073() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0074() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0075() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0036() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandValueByte>(bus->read(cpu->PC++));
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

// These Instructions are of the form LD A,n where A is the accumulator and n is
// a register, immediate value or indirect value.

void Cpu::Impl::Op_000A() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandAddress>(cpu->registers.r16.BC, bus);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_001A() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandAddress>(cpu->registers.r16.DE, bus);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_00FA() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        uint8_t lsb = bus->read(cpu->PC++);
        uint8_t msb = bus->read(cpu->PC++);
        uint16_t address = (msb << 8) | lsb;

        auto op2 = std::make_shared<OperandAddress>(address, bus);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_003E() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandValueByte>(bus->read(cpu->PC++));
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

// These Instructions are of the form LD n,A where n is a register or indirect
// address and the contents of the accumulator are copied into that.

void Cpu::Impl::Op_0047() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_004F() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0057() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_005F() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0067() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_006F() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0002() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.BC, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0012() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.DE, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_0077() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

void Cpu::Impl::Op_00EA() {
        uint8_t lsb = bus->read(cpu->PC++);
        uint8_t msb = bus->read(cpu->PC++);
        uint16_t address = (msb << 8) | lsb;
        auto op1 = std::make_shared<OperandAddress>(address, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

//! \brief LD A,(C)
//!
//! Put value at address $FF00 + register C into A. Same as: LD A,($FF00+C)
void Cpu::Impl::Op_00F2() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        uint16_t address = static_cast<uint16_t>(cpu->registers.r8.C) + 0xFF00;
        auto op2 = std::make_shared<OperandAddress>(address, bus);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

//! \brief LD (C),A
//!
//! Put A into address $FF00 + register C.
void Cpu::Impl::Op_00E2() {
        uint16_t address = static_cast<uint16_t>(cpu->registers.r8.C) + 0xFF00;
        auto op1 = std::make_shared<OperandAddress>(address, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

//! \brief LDD A,(HL)
//!
//! Put value at address HL into A. Decrement HL. Same as: LD A,(HL) - DEC HL
void Cpu::Impl::Op_003A() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();

        cpu->registers.r16.HL--;
}

//! \brief LDD (HL),A
//!
//! Put A into memory address HL. Decrement HL. Same as: LD (HL),A - DEC HL
void Cpu::Impl::Op_0032() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();

        cpu->registers.r16.HL--;
}

//! \brief LDI A,(HL)
//!
//! Put value at address HL into A. Increment HL. Same as: LD A,(HL) - INC HL
void Cpu::Impl::Op_002A() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();

        cpu->registers.r16.HL++;
}

//! \brief LDI (HL),A
//!
//! Put A into memory address HL. Increment HL. Same as: LD (HL),A - INC HL
void Cpu::Impl::Op_0022() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();

        cpu->registers.r16.HL++;
}

//! \brief LDH (n),A
//!
//! Put A into memory address $FF00+n
void Cpu::Impl::Op_00E0() {
        uint16_t address = bus->read(cpu->PC++) + 0xFF00;
        auto op1 = std::make_shared<OperandAddress>(address, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

//! \brief LDH A,(n)
//!
//! Put memory address $FF00+n into A.
void Cpu::Impl::Op_00F0() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        uint16_t address = bus->read(cpu->PC++) + 0xFF00;
        auto op2 = std::make_shared<OperandAddress>(address, bus);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

//-- 16-Bit Load Opcodes -------------------------------------------------------

//! \brief LD BC,##
//!
//! Put 16-bit value ## into register pair BC.
void Cpu::Impl::Op_0001() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->registers.r16.BC);
        operand1 = std::static_pointer_cast<Operand>(op1);

        uint16_t lo = bus->read(cpu->PC++);
        uint16_t hi = bus->read(cpu->PC++);
        auto op2 = std::make_shared<OperandValueWord>((hi << 8) | lo);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

//! \brief LD DE,##
//!
//! Put 16-bit value ## into register pair DE.
void Cpu::Impl::Op_0011() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->registers.r16.DE);
        operand1 = std::static_pointer_cast<Operand>(op1);

        uint16_t lo = bus->read(cpu->PC++);
        uint16_t hi = bus->read(cpu->PC++);
        auto op2 = std::make_shared<OperandValueWord>((hi << 8) | lo);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

//! \brief LD HL,##
//!
//! Put 16-bit value ## into register pair HL.
void Cpu::Impl::Op_0021() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->registers.r16.HL);
        operand1 = std::static_pointer_cast<Operand>(op1);

        uint16_t lo = bus->read(cpu->PC++);
        uint16_t hi = bus->read(cpu->PC++);
        auto op2 = std::make_shared<OperandValueWord>((hi << 8) | lo);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

//! \brief LD SP,##
//!
//! Put 16-bit value ## into Stack Pointer (SP).
void Cpu::Impl::Op_0031() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->SP);
        operand1 = std::static_pointer_cast<Operand>(op1);

        uint16_t lo = bus->read(cpu->PC++);
        uint16_t hi = bus->read(cpu->PC++);
        auto op2 = std::make_shared<OperandValueWord>((hi << 8) | lo);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

//! \brief LD SP,HL
//!
//! Put HL into Stack Pointer (SP).
void Cpu::Impl::Op_00F9() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->SP);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandPairReference>(cpu->registers.r16.HL);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

//! \brief LDHL SP,n
//!
//! Put SP + n effective address into HL.
void Cpu::Impl::Op_00F8() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->registers.r16.HL);
        operand1 = std::static_pointer_cast<Operand>(op1);

        uint16_t value = cpu->SP;
        uint16_t byte = bus->read(cpu->PC++);
        auto op2 = std::make_shared<OperandValueByte>(value + byte);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();

        bool halfCarry = ((value & 0xFFF) + (byte & 0xFFF)) & 0x1000;
        bool carry = ((uint32_t)value + (uint32_t)byte) & 0x10000;

        cpu->flagSet('z', 0);
        cpu->flagSet('n', 0);
        cpu->flagSet('h', halfCarry);
        cpu->flagSet('c', carry);
}

//! \brief LD (##),SP
//!
//! Put Stack Pointer (SP) at address ##.
void Cpu::Impl::Op_0008() {
        uint16_t lo = bus->read(cpu->PC++);
        uint16_t hi = bus->read(cpu->PC++);
        auto op1 = std::make_shared<OperandAddress>((hi << 8) + lo, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandPairReference>(cpu->SP);
        operand2 = std::static_pointer_cast<Operand>(op2);

        LD();
}

//! \brief PUSH AF
//!
//! Push register pair AF onto stack.  Decrement Stack Pointer (SP) twice.
void Cpu::Impl::Op_00F5() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->registers.r16.AF);
        operand1 = std::static_pointer_cast<Operand>(op1);

        PUSH();
}

//! \brief PUSH BC
//!
//! Push register pair BC onto stack.  Decrement Stack Pointer (SP) twice.
void Cpu::Impl::Op_00C5() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->registers.r16.BC);
        operand1 = std::static_pointer_cast<Operand>(op1);

        PUSH();
}

//! \brief PUSH DE
//!
//! Push register pair DE onto stack.  Decrement Stack Pointer (SP) twice.
void Cpu::Impl::Op_00D5() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->registers.r16.DE);
        operand1 = std::static_pointer_cast<Operand>(op1);

        PUSH();
}

//! \brief PUSH HL
//!
//! Push register pair HL onto stack.  Decrement Stack Pointer (SP) twice.
void Cpu::Impl::Op_00E5() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->registers.r16.HL);
        operand1 = std::static_pointer_cast<Operand>(op1);

        PUSH();
}

//! \brief POP AF
//!
//! Pop two bytes off of the stack into AF. Increment Stack Pointer (SP) twice.
void Cpu::Impl::Op_00F1() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->registers.r16.AF);
        operand1 = std::static_pointer_cast<Operand>(op1);

        POP();
}

//! \brief POP BC
//!
//! Pop two bytes off of the stack into BC. Increment Stack Pointer (SP) twice.
void Cpu::Impl::Op_00C1() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->registers.r16.BC);
        operand1 = std::static_pointer_cast<Operand>(op1);

        POP();
}

//! \brief POP DE
//!
//! Pop two bytes off of the stack into DE. Increment Stack Pointer (SP) twice.
void Cpu::Impl::Op_00D1() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->registers.r16.DE);
        operand1 = std::static_pointer_cast<Operand>(op1);

        POP();
}

//! \brief POP HL
//!
//! Pop two bytes off of the stack into HL. Increment Stack Pointer (SP) twice.
void Cpu::Impl::Op_00E1() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->registers.r16.HL);
        operand1 = std::static_pointer_cast<Operand>(op1);

        POP();
}

//-- 8-Bit ALU Opcodes ---------------------------------------------------------

//! \brief ADD A,A
void Cpu::Impl::Op_0087() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand2 = std::static_pointer_cast<Operand>(op2);

        ADD8();
}

//! \brief ADD A,B
void Cpu::Impl::Op_0080() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand2 = std::static_pointer_cast<Operand>(op2);

        ADD8();
}

//! \brief ADD A,C
void Cpu::Impl::Op_0081() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand2 = std::static_pointer_cast<Operand>(op2);

        ADD8();
}

//! \brief ADD A,D
void Cpu::Impl::Op_0082() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand2 = std::static_pointer_cast<Operand>(op2);

        ADD8();
}

//! \brief ADD A,E
void Cpu::Impl::Op_0083() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand2 = std::static_pointer_cast<Operand>(op2);

        ADD8();
}

//! \brief ADD A,H
void Cpu::Impl::Op_0084() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand2 = std::static_pointer_cast<Operand>(op2);

        ADD8();
}

//! \brief ADD A,L
void Cpu::Impl::Op_0085() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand2 = std::static_pointer_cast<Operand>(op2);

        ADD8();
}

//! \brief ADD A,(HL)
void Cpu::Impl::Op_0086() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand2 = std::static_pointer_cast<Operand>(op2);

        ADD8();
}

//! \brief ADD A,#
void Cpu::Impl::Op_00C6() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        uint8_t byte = bus->read(cpu->PC++);
        auto op2 = std::make_shared<OperandValueByte>(byte);
        operand2 = std::static_pointer_cast<Operand>(op2);

        ADD8();
}

//! \brief ADC A,A
void Cpu::Impl::Op_008F() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand2 = std::static_pointer_cast<Operand>(op2);

        ADC8();
}

//! \brief ADC A,B
void Cpu::Impl::Op_0088() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand2 = std::static_pointer_cast<Operand>(op2);

        ADC8();
}

//! \brief ADC A,C
void Cpu::Impl::Op_0089() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand2 = std::static_pointer_cast<Operand>(op2);

        ADC8();
}

//! \brief ADC A,D
void Cpu::Impl::Op_008A() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand2 = std::static_pointer_cast<Operand>(op2);

        ADC8();
}

//! \brief ADC A,E
void Cpu::Impl::Op_008B() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand2 = std::static_pointer_cast<Operand>(op2);

        ADC8();
}

//! \brief ADC A,H
void Cpu::Impl::Op_008C() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand2 = std::static_pointer_cast<Operand>(op2);

        ADC8();
}

//! \brief ADC A,L
void Cpu::Impl::Op_008D() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand2 = std::static_pointer_cast<Operand>(op2);

        ADC8();
}

//! \brief ADC A,(HL)
void Cpu::Impl::Op_008E() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand2 = std::static_pointer_cast<Operand>(op2);

        ADC8();
}

//! \brief ADC A,#
void Cpu::Impl::Op_00CE() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        uint8_t byte = bus->read(cpu->PC++);
        auto op2 = std::make_shared<OperandValueByte>(byte);
        operand2 = std::static_pointer_cast<Operand>(op2);

        ADC8();
}

//! \brief SUB A
void Cpu::Impl::Op_0097() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SUB8();
}

//! \brief SUB B
void Cpu::Impl::Op_0090() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SUB8();
}

//! \brief SUB C
void Cpu::Impl::Op_0091() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SUB8();
}

//! \brief SUB D
void Cpu::Impl::Op_0092() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SUB8();
}

//! \brief SUB E
void Cpu::Impl::Op_0093() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SUB8();
}

//! \brief SUB H
void Cpu::Impl::Op_0094() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SUB8();
}

//! \brief SUB L
void Cpu::Impl::Op_0095() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SUB8();
}

//! \brief SUB (HL)
void Cpu::Impl::Op_0096() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SUB8();
}

//! \brief SUB #
void Cpu::Impl::Op_00D6() {
        uint8_t byte = bus->read(cpu->PC++);
        auto op1 = std::make_shared<OperandValueByte>(byte);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SUB8();
}

//! \brief SBC A
void Cpu::Impl::Op_009F() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SBC8();
}

//! \brief SBC B
void Cpu::Impl::Op_0098() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SBC8();
}

//! \brief SBC C
void Cpu::Impl::Op_0099() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SBC8();
}

//! \brief SBC D
void Cpu::Impl::Op_009A() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SBC8();
}

//! \brief SBC E
void Cpu::Impl::Op_009B() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SBC8();
}

//! \brief SBC H
void Cpu::Impl::Op_009C() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SBC8();
}

//! \brief SBC L
void Cpu::Impl::Op_009D() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SBC8();
}

//! \brief SBC (HL)
void Cpu::Impl::Op_009E() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SBC8();
}

//! \brief AND A
void Cpu::Impl::Op_00A7() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        AND();
}

//! \brief AND B
void Cpu::Impl::Op_00A0() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        AND();
}

//! \brief AND C
void Cpu::Impl::Op_00A1() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand1 = std::static_pointer_cast<Operand>(op1);

        AND();
}

//! \brief AND D
void Cpu::Impl::Op_00A2() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand1 = std::static_pointer_cast<Operand>(op1);

        AND();
}

//! \brief AND E
void Cpu::Impl::Op_00A3() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand1 = std::static_pointer_cast<Operand>(op1);

        AND();
}

//! \brief AND H
void Cpu::Impl::Op_00A4() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand1 = std::static_pointer_cast<Operand>(op1);

        AND();
}

//! \brief AND L
void Cpu::Impl::Op_00A5() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand1 = std::static_pointer_cast<Operand>(op1);

        AND();
}

//! \brief AND (HL)
void Cpu::Impl::Op_00A6() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        AND();
}

//! \brief AND #
void Cpu::Impl::Op_00E6() {
        uint8_t byte = bus->read(cpu->PC++);
        auto op1 = std::make_shared<OperandValueByte>(byte);
        operand1 = std::static_pointer_cast<Operand>(op1);

        AND();
}

//! \brief OR A
void Cpu::Impl::Op_00B7() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        OR();
}

//! \brief OR B
void Cpu::Impl::Op_00B0() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        OR();
}

//! \brief OR C
void Cpu::Impl::Op_00B1() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand1 = std::static_pointer_cast<Operand>(op1);

        OR();
}

//! \brief OR D
void Cpu::Impl::Op_00B2() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand1 = std::static_pointer_cast<Operand>(op1);

        OR();
}

//! \brief OR E
void Cpu::Impl::Op_00B3() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand1 = std::static_pointer_cast<Operand>(op1);

        OR();
}

//! \brief OR H
void Cpu::Impl::Op_00B4() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand1 = std::static_pointer_cast<Operand>(op1);

        OR();
}

//! \brief OR L
void Cpu::Impl::Op_00B5() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand1 = std::static_pointer_cast<Operand>(op1);

        OR();
}

//! \brief OR (HL)
void Cpu::Impl::Op_00B6() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        OR();
}

//! \brief OR #
void Cpu::Impl::Op_00F6() {
        uint8_t byte = bus->read(cpu->PC++);
        auto op1 = std::make_shared<OperandValueByte>(byte);
        operand1 = std::static_pointer_cast<Operand>(op1);

        OR();
}

//! \brief XOR A
void Cpu::Impl::Op_00AF() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        XOR();
}

//! \brief XOR B
void Cpu::Impl::Op_00A8() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        XOR();
}

//! \brief XOR C
void Cpu::Impl::Op_00A9() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand1 = std::static_pointer_cast<Operand>(op1);

        XOR();
}

//! \brief XOR D
void Cpu::Impl::Op_00AA() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand1 = std::static_pointer_cast<Operand>(op1);

        XOR();
}

//! \brief XOR E
void Cpu::Impl::Op_00AB() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand1 = std::static_pointer_cast<Operand>(op1);

        XOR();
}

//! \brief XOR H
void Cpu::Impl::Op_00AC() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand1 = std::static_pointer_cast<Operand>(op1);

        XOR();
}

//! \brief XOR L
void Cpu::Impl::Op_00AD() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand1 = std::static_pointer_cast<Operand>(op1);

        XOR();
}

//! \brief XOR (HL)
void Cpu::Impl::Op_00AE() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        XOR();
}

//! \brief XOR #
void Cpu::Impl::Op_00EE() {
        uint8_t byte = bus->read(cpu->PC++);
        auto op1 = std::make_shared<OperandValueByte>(byte);
        operand1 = std::static_pointer_cast<Operand>(op1);

        XOR();
}

//! \brief CP A
void Cpu::Impl::Op_00BF() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        CP();
}

//! \brief CP B
void Cpu::Impl::Op_00B8() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        CP();
}

//! \brief CP C
void Cpu::Impl::Op_00B9() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand1 = std::static_pointer_cast<Operand>(op1);

        CP();
}

//! \brief CP D
void Cpu::Impl::Op_00BA() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand1 = std::static_pointer_cast<Operand>(op1);

        CP();
}

//! \brief CP E
void Cpu::Impl::Op_00BB() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand1 = std::static_pointer_cast<Operand>(op1);

        CP();
}

//! \brief CP H
void Cpu::Impl::Op_00BC() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand1 = std::static_pointer_cast<Operand>(op1);

        CP();
}

//! \brief CP L
void Cpu::Impl::Op_00BD() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand1 = std::static_pointer_cast<Operand>(op1);

        CP();
}

//! \brief CP (HL)
void Cpu::Impl::Op_00BE() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        CP();
}

//! \brief CP #
void Cpu::Impl::Op_00FE() {
        uint8_t byte = bus->read(cpu->PC++);
        auto op1 = std::make_shared<OperandValueByte>(byte);
        operand1 = std::static_pointer_cast<Operand>(op1);

        CP();
}

//! \brief INC A
void Cpu::Impl::Op_003C() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        INC8();
}

//! \brief INC B
void Cpu::Impl::Op_0004() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        INC8();
}

//! \brief INC C
void Cpu::Impl::Op_000C() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand1 = std::static_pointer_cast<Operand>(op1);

        INC8();
}

//! \brief INC D
void Cpu::Impl::Op_0014() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand1 = std::static_pointer_cast<Operand>(op1);

        INC8();
}

//! \brief INC E
void Cpu::Impl::Op_001C() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand1 = std::static_pointer_cast<Operand>(op1);

        INC8();
}

//! \brief INC H
void Cpu::Impl::Op_0024() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand1 = std::static_pointer_cast<Operand>(op1);

        INC8();
}

//! \brief INC L
void Cpu::Impl::Op_002C() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand1 = std::static_pointer_cast<Operand>(op1);

        INC8();
}

//! \brief INC (HL)
void Cpu::Impl::Op_0034() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        INC8();
}

//! \brief DEC A
void Cpu::Impl::Op_003D() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        DEC8();
}

//! \brief DEC B
void Cpu::Impl::Op_0005() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        DEC8();
}

//! \brief DEC C
void Cpu::Impl::Op_000D() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand1 = std::static_pointer_cast<Operand>(op1);

        DEC8();
}

//! \brief DEC D
void Cpu::Impl::Op_0015() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand1 = std::static_pointer_cast<Operand>(op1);

        DEC8();
}

//! \brief DEC E
void Cpu::Impl::Op_001D() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand1 = std::static_pointer_cast<Operand>(op1);

        DEC8();
}

//! \brief DEC H
void Cpu::Impl::Op_0025() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand1 = std::static_pointer_cast<Operand>(op1);

        DEC8();
}

//! \brief DEC L
void Cpu::Impl::Op_002D() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand1 = std::static_pointer_cast<Operand>(op1);

        DEC8();
}

//! \brief DEC (HL)
void Cpu::Impl::Op_0035() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        DEC8();
}

//-- 16-Bit ALU Opcodes --------------------------------------------------------

//! \brief ADD HL,BC
void Cpu::Impl::Op_0009() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->registers.r16.BC);
        operand1 = std::static_pointer_cast<Operand>(op1);

        ADD16();
}

//! \brief ADD HL,DE
void Cpu::Impl::Op_0019() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->registers.r16.DE);
        operand1 = std::static_pointer_cast<Operand>(op1);

        ADD16();
}

//! \brief ADD HL,HL
void Cpu::Impl::Op_0029() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->registers.r16.HL);
        operand1 = std::static_pointer_cast<Operand>(op1);

        ADD16();
}

//! \brief ADD HL,SP
void Cpu::Impl::Op_0039() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->SP);
        operand1 = std::static_pointer_cast<Operand>(op1);

        ADD16();
}

//! \brief ADD SP,#
void Cpu::Impl::Op_00E8() {
        auto op1 = std::make_shared<OperandValueByte>(bus->read(cpu->PC++));
        operand1 = std::static_pointer_cast<Operand>(op1);

        ADD16();
}

//! \brief INC BC
void Cpu::Impl::Op_0003() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->registers.r16.BC);
        operand1 = std::static_pointer_cast<Operand>(op1);

        INC16();
}

//! \brief INC DE
void Cpu::Impl::Op_0013() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->registers.r16.DE);
        operand1 = std::static_pointer_cast<Operand>(op1);

        INC16();
}

//! \brief INC HL
void Cpu::Impl::Op_0023() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->registers.r16.HL);
        operand1 = std::static_pointer_cast<Operand>(op1);

        INC16();
}

//! \brief INC SP
void Cpu::Impl::Op_0033() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->SP);
        operand1 = std::static_pointer_cast<Operand>(op1);

        INC16();
}

//! \brief DEC BC
void Cpu::Impl::Op_000B() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->registers.r16.BC);
        operand1 = std::static_pointer_cast<Operand>(op1);

        DEC16();
}

//! \brief DEC DE
void Cpu::Impl::Op_001B() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->registers.r16.DE);
        operand1 = std::static_pointer_cast<Operand>(op1);

        DEC16();
}

//! \brief DEC HL
void Cpu::Impl::Op_002B() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->registers.r16.HL);
        operand1 = std::static_pointer_cast<Operand>(op1);

        DEC16();
}

//! \brief DEC SP
void Cpu::Impl::Op_003B() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->SP);
        operand1 = std::static_pointer_cast<Operand>(op1);

        DEC16();
}

//-- Miscellaneous Opcodes ----------------------------------------------------

//! \brief SWAP A
//!
//! Swap upper and lower nibbles of specified register.
void Cpu::Impl::Op_CB37() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SWAP();
}

//! \brief SWAP B
//!
//! Swap upper and lower nibbles of specified register.
void Cpu::Impl::Op_CB30() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SWAP();
}

//! \brief SWAP C
//!
//! Swap upper and lower nibbles of specified register.
void Cpu::Impl::Op_CB31() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SWAP();
}

//! \brief SWAP D
//!
//! Swap upper and lower nibbles of specified register.
void Cpu::Impl::Op_CB32() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SWAP();
}

//! \brief SWAP E
//!
//! Swap upper and lower nibbles of specified register.
void Cpu::Impl::Op_CB33() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SWAP();
}

//! \brief SWAP H
//!
//! Swap upper and lower nibbles of specified register.
void Cpu::Impl::Op_CB34() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SWAP();
}

//! \brief SWAP L
//!
//! Swap upper and lower nibbles of specified register.
void Cpu::Impl::Op_CB35() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SWAP();
}

//! \brief SWAP (HL)
//!
//! Swap upper and lower nibbles of value at address.
void Cpu::Impl::Op_CB36() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SWAP();
}

//! \brief DAA
//!
//! Decimal adjust register A.
void Cpu::Impl::Op_0027() {
        DAA();
}

//! \brief CPL
//!
//! Complement register A.
void Cpu::Impl::Op_002F() {
        CPL();
}

//! \brief CCF
//!
//! Complement carry flag.
void Cpu::Impl::Op_003F() {
        CCF();
}

//! \brief SCF
//!
//! Set carry flag.
void Cpu::Impl::Op_0037() {
        SCF();
}

//! \brief NOP
void Cpu::Impl::Op_0000() {
        NOP();
}

//! \brief HALT
//!
//! Power down the CPU until an interrupt occurs.
void Cpu::Impl::Op_0076() {
        HALT();
}

//! \brief STOP
//!
//! Halt CPU & LCD display until a button is pressed.
void Cpu::Impl::Op_1000() {
        STOP();
}

//! \brief DI
//!
//! Disable interrupts.
void Cpu::Impl::Op_00F3() {
        DI();
}

//! \brief EI
//!
//! Enable interrupts.
void Cpu::Impl::Op_00FB() {
        EI();
}

//-- Rotate & Shift Opcodes ---------------------------------------------------

//! \brief RLCA
void Cpu::Impl::Op_0007() {
        RLCA();
}

//! \brief RLA
void Cpu::Impl::Op_0017() {
        RLA();
}

//! \brief RRCA
void Cpu::Impl::Op_000F() {
        RRCA();
}

//! \brief RRA
void Cpu::Impl::Op_001F() {
        RRA();
}

//! \brief RLC A
void Cpu::Impl::Op_CB07() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RLC();
}

//! \brief RLC B
void Cpu::Impl::Op_CB00() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RLC();
}

//! \brief RLC C
void Cpu::Impl::Op_CB01() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RLC();
}

//! \brief RLC D
void Cpu::Impl::Op_CB02() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RLC();
}

//! \brief RLC E
void Cpu::Impl::Op_CB03() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RLC();
}

//! \brief RLC H
void Cpu::Impl::Op_CB04() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RLC();
}

//! \brief RLC L
void Cpu::Impl::Op_CB05() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RLC();
}

//! \brief RLC (HL)
void Cpu::Impl::Op_CB06() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RLC();
}

//! \brief RL A
void Cpu::Impl::Op_CB17() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RL();
}

//! \brief RL B
void Cpu::Impl::Op_CB10() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RL();
}

//! \brief RL C
void Cpu::Impl::Op_CB11() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RL();
}

//! \brief RL D
void Cpu::Impl::Op_CB12() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RL();
}

//! \brief RL E
void Cpu::Impl::Op_CB13() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RL();
}

//! \brief RL H
void Cpu::Impl::Op_CB14() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RL();
}

//! \brief RL L
void Cpu::Impl::Op_CB15() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RL();
}

//! \brief RL (HL)
void Cpu::Impl::Op_CB16() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RL();
}

//! \brief RRC A
void Cpu::Impl::Op_CB0F() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RRC();
}

//! \brief RRC B
void Cpu::Impl::Op_CB08() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RRC();
}

//! \brief RRC C
void Cpu::Impl::Op_CB09() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RRC();
}

//! \brief RRC D
void Cpu::Impl::Op_CB0A() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RRC();
}

//! \brief RRC E
void Cpu::Impl::Op_CB0B() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RRC();
}

//! \brief RRC H
void Cpu::Impl::Op_CB0C() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RRC();
}

//! \brief RRC L
void Cpu::Impl::Op_CB0D() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RRC();
}

//! \brief RRC (HL)
void Cpu::Impl::Op_CB0E() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RRC();
}

//! \brief RR A
void Cpu::Impl::Op_CB1F() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RR();
}

//! \brief RR B
void Cpu::Impl::Op_CB18() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RR();
}

//! \brief RR C
void Cpu::Impl::Op_CB19() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RR();
}

//! \brief RR D
void Cpu::Impl::Op_CB1A() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RR();
}

//! \brief RR E
void Cpu::Impl::Op_CB1B() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RR();
}

//! \brief RR H
void Cpu::Impl::Op_CB1C() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RR();
}

//! \brief RR L
void Cpu::Impl::Op_CB1D() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RR();
}

//! \brief RR (HL)
void Cpu::Impl::Op_CB1E() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        RR();
}

//! \brief SLA A
void Cpu::Impl::Op_CB27() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SLA();
}

//! \brief SLA B
void Cpu::Impl::Op_CB20() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SLA();
}

//! \brief SLA C
void Cpu::Impl::Op_CB21() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SLA();
}

//! \brief SLA D
void Cpu::Impl::Op_CB22() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SLA();
}

//! \brief SLA E
void Cpu::Impl::Op_CB23() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SLA();
}

//! \brief SLA H
void Cpu::Impl::Op_CB24() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SLA();
}

//! \brief SLA L
void Cpu::Impl::Op_CB25() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SLA();
}

//! \brief SLA (HL)
void Cpu::Impl::Op_CB26() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SLA();
}

//! \brief SRA A
void Cpu::Impl::Op_CB2F() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SRA();
}

//! \brief SRA B
void Cpu::Impl::Op_CB28() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SRA();
}

//! \brief SRA C
void Cpu::Impl::Op_CB29() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SRA();
}

//! \brief SRA D
void Cpu::Impl::Op_CB2A() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SRA();
}

//! \brief SRA E
void Cpu::Impl::Op_CB2B() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SRA();
}

//! \brief SRA H
void Cpu::Impl::Op_CB2C() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SRA();
}

//! \brief SRA L
void Cpu::Impl::Op_CB2D() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SRA();
}

//! \brief SRA (HL)
void Cpu::Impl::Op_CB2E() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SRA();
}

//! \brief SRL A
void Cpu::Impl::Op_CB3F() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SRL();
}

//! \brief SRL B
void Cpu::Impl::Op_CB38() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SRL();
}

//! \brief SRL C
void Cpu::Impl::Op_CB39() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SRL();
}

//! \brief SRL D
void Cpu::Impl::Op_CB3A() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SRL();
}

//! \brief SRL E
void Cpu::Impl::Op_CB3B() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SRL();
}

//! \brief SRL H
void Cpu::Impl::Op_CB3C() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SRL();
}

//! \brief SRL L
void Cpu::Impl::Op_CB3D() {
        auto op1 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SRL();
}

//! \brief SRL (HL)
void Cpu::Impl::Op_CB3E() {
        auto op1 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand1 = std::static_pointer_cast<Operand>(op1);

        SRL();
}

//-- Bit Opcodes ---------------------------------------------------------------

//! \brief BIT #,A
void Cpu::Impl::Op_CB47() {
        uint8_t byte = bus->read(cpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<OperandValueByte>(bit);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand2 = std::static_pointer_cast<Operand>(op2);

        BIT();
}

//! \brief BIT #,B
void Cpu::Impl::Op_CB40() {
        uint8_t byte = bus->read(cpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<OperandValueByte>(bit);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand2 = std::static_pointer_cast<Operand>(op2);

        BIT();
}

//! \brief BIT #,C
void Cpu::Impl::Op_CB41() {
        uint8_t byte = bus->read(cpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<OperandValueByte>(bit);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand2 = std::static_pointer_cast<Operand>(op2);

        BIT();
}

//! \brief BIT #,D
void Cpu::Impl::Op_CB42() {
        uint8_t byte = bus->read(cpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<OperandValueByte>(bit);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand2 = std::static_pointer_cast<Operand>(op2);

        BIT();
}

//! \brief BIT #,E
void Cpu::Impl::Op_CB43() {
        uint8_t byte = bus->read(cpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<OperandValueByte>(bit);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand2 = std::static_pointer_cast<Operand>(op2);

        BIT();
}

//! \brief BIT #,H
void Cpu::Impl::Op_CB44() {
        uint8_t byte = bus->read(cpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<OperandValueByte>(bit);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand2 = std::static_pointer_cast<Operand>(op2);

        BIT();
}

//! \brief BIT #,L
void Cpu::Impl::Op_CB45() {
        uint8_t byte = bus->read(cpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<OperandValueByte>(bit);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand2 = std::static_pointer_cast<Operand>(op2);

        BIT();
}

//! \brief BIT #,(HL)
void Cpu::Impl::Op_CB46() {
        uint8_t byte = bus->read(cpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<OperandValueByte>(bit);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand2 = std::static_pointer_cast<Operand>(op2);

        BIT();
}

//! \brief SET #,A
void Cpu::Impl::Op_CBC7() {
        uint8_t byte = bus->read(cpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<OperandValueByte>(bit);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand2 = std::static_pointer_cast<Operand>(op2);

        SET();
}

//! \brief SET #,B
void Cpu::Impl::Op_CBC0() {
        uint8_t byte = bus->read(cpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<OperandValueByte>(bit);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand2 = std::static_pointer_cast<Operand>(op2);

        SET();
}

//! \brief SET #,C
void Cpu::Impl::Op_CBC1() {
        uint8_t byte = bus->read(cpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<OperandValueByte>(bit);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand2 = std::static_pointer_cast<Operand>(op2);

        SET();
}

//! \brief SET #,D
void Cpu::Impl::Op_CBC2() {
        uint8_t byte = bus->read(cpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<OperandValueByte>(bit);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand2 = std::static_pointer_cast<Operand>(op2);

        SET();
}

//! \brief SET #,E
void Cpu::Impl::Op_CBC3() {
        uint8_t byte = bus->read(cpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<OperandValueByte>(bit);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand2 = std::static_pointer_cast<Operand>(op2);

        SET();
}

//! \brief SET #,H
void Cpu::Impl::Op_CBC4() {
        uint8_t byte = bus->read(cpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<OperandValueByte>(bit);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand2 = std::static_pointer_cast<Operand>(op2);

        SET();
}

//! \brief SET #,L
void Cpu::Impl::Op_CBC5() {
        uint8_t byte = bus->read(cpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<OperandValueByte>(bit);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand2 = std::static_pointer_cast<Operand>(op2);

        SET();
}

//! \brief SET #,(HL)
void Cpu::Impl::Op_CBC6() {
        uint8_t byte = bus->read(cpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<OperandValueByte>(bit);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand2 = std::static_pointer_cast<Operand>(op2);

        SET();
}

//! \brief RES #,A
void Cpu::Impl::Op_CB87() {
        uint8_t byte = bus->read(cpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<OperandValueByte>(bit);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.A);
        operand2 = std::static_pointer_cast<Operand>(op2);

        RES();
}

//! \brief RES #,B
void Cpu::Impl::Op_CB80() {
        uint8_t byte = bus->read(cpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<OperandValueByte>(bit);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.B);
        operand2 = std::static_pointer_cast<Operand>(op2);

        RES();
}

//! \brief RES #,C
void Cpu::Impl::Op_CB81() {
        uint8_t byte = bus->read(cpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<OperandValueByte>(bit);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.C);
        operand2 = std::static_pointer_cast<Operand>(op2);

        RES();
}

//! \brief RES #,D
void Cpu::Impl::Op_CB82() {
        uint8_t byte = bus->read(cpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<OperandValueByte>(bit);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.D);
        operand2 = std::static_pointer_cast<Operand>(op2);

        RES();
}

//! \brief RES #,E
void Cpu::Impl::Op_CB83() {
        uint8_t byte = bus->read(cpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<OperandValueByte>(bit);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.E);
        operand2 = std::static_pointer_cast<Operand>(op2);

        RES();
}

//! \brief RES #,H
void Cpu::Impl::Op_CB84() {
        uint8_t byte = bus->read(cpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<OperandValueByte>(bit);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.H);
        operand2 = std::static_pointer_cast<Operand>(op2);

        RES();
}

//! \brief RES #,L
void Cpu::Impl::Op_CB85() {
        uint8_t byte = bus->read(cpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<OperandValueByte>(bit);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandReference>(cpu->registers.r8.L);
        operand2 = std::static_pointer_cast<Operand>(op2);

        RES();
}

//! \brief RES #,(HL)
void Cpu::Impl::Op_CB86() {
        uint8_t byte = bus->read(cpu->PC++);
        uint8_t bit = (byte >> 3) & 0x7;

        auto op1 = std::make_shared<OperandValueByte>(bit);
        operand1 = std::static_pointer_cast<Operand>(op1);

        auto op2 = std::make_shared<OperandAddress>(cpu->registers.r16.HL, bus);
        operand2 = std::static_pointer_cast<Operand>(op2);

        RES();
}

//-- Jump Opcodes --------------------------------------------------------------

//! \brief JP ##
void Cpu::Impl::Op_00C3() {
        uint16_t lo = bus->read(cpu->PC++);
        uint16_t hi = bus->read(cpu->PC++);
        auto op1 = std::make_shared<OperandValueWord>((hi << 8) | lo);
        operand1 = std::static_pointer_cast<Operand>(op1);

        JP();
}

//! \brief JP NZ,##
void Cpu::Impl::Op_00C2() {
        uint16_t lo = bus->read(cpu->PC++);
        uint16_t hi = bus->read(cpu->PC++);
        auto op1 = std::make_shared<OperandValueWord>((hi << 8) | lo);
        operand1 = std::static_pointer_cast<Operand>(op1);

        if (!cpu->flagGet('z')) {
                JP();
        }
}

//! \brief JP Z,##
void Cpu::Impl::Op_00CA() {
        uint16_t lo = bus->read(cpu->PC++);
        uint16_t hi = bus->read(cpu->PC++);
        auto op1 = std::make_shared<OperandValueWord>((hi << 8) | lo);
        operand1 = std::static_pointer_cast<Operand>(op1);

        if (cpu->flagGet('z')) {
                JP();
        }
}

//! \brief JP NC,##
void Cpu::Impl::Op_00D2() {
        uint16_t lo = bus->read(cpu->PC++);
        uint16_t hi = bus->read(cpu->PC++);
        auto op1 = std::make_shared<OperandValueWord>((hi << 8) | lo);
        operand1 = std::static_pointer_cast<Operand>(op1);

        if (!cpu->flagGet('c')) {
                JP();
        }
}

//! \brief JP C,##
void Cpu::Impl::Op_00DA() {
        uint16_t lo = bus->read(cpu->PC++);
        uint16_t hi = bus->read(cpu->PC++);
        auto op1 = std::make_shared<OperandValueWord>((hi << 8) | lo);
        operand1 = std::static_pointer_cast<Operand>(op1);

        if (cpu->flagGet('c')) {
                JP();
        }
}

//! \brief JP (HL)
void Cpu::Impl::Op_00E9() {
        auto op1 = std::make_shared<OperandPairReference>(cpu->registers.r16.HL);
        operand1 = std::static_pointer_cast<Operand>(op1);

        JP();
}

//! \brief JR #
void Cpu::Impl::Op_0018() {
        uint8_t byte = bus->read(cpu->PC++);
        auto op1 = std::make_shared<OperandValueByte>(byte);
        operand1 = std::static_pointer_cast<Operand>(op1);

        JR();
}

//! \brief JR NZ,#
void Cpu::Impl::Op_0020() {
        uint8_t byte = bus->read(cpu->PC++);
        auto op1 = std::make_shared<OperandValueByte>(byte);
        operand1 = std::static_pointer_cast<Operand>(op1);

        if (!cpu->flagGet('z')) {
                JR();
        }
}

//! \brief JR Z,#
void Cpu::Impl::Op_0028() {
        uint8_t byte = bus->read(cpu->PC++);
        auto op1 = std::make_shared<OperandValueByte>(byte);
        operand1 = std::static_pointer_cast<Operand>(op1);

        if (cpu->flagGet('z')) {
                JR();
        }
}

//! \brief JR NC,#
void Cpu::Impl::Op_0030() {
        uint8_t byte = bus->read(cpu->PC++);
        auto op1 = std::make_shared<OperandValueByte>(byte);
        operand1 = std::static_pointer_cast<Operand>(op1);

        if (!cpu->flagGet('c')) {
                JR();
        }
}

//! \brief JR C,#
void Cpu::Impl::Op_0038() {
        uint8_t byte = bus->read(cpu->PC++);
        auto op1 = std::make_shared<OperandValueByte>(byte);
        operand1 = std::static_pointer_cast<Operand>(op1);

        if (cpu->flagGet('c')) {
                JR();
        }
}

//-- Call Opcodes --------------------------------------------------------------

//! \brief CALL ##
void Cpu::Impl::Op_00CD() {
        uint16_t lo = bus->read(cpu->PC++);
        uint16_t hi = bus->read(cpu->PC++);
        auto op1 = std::make_shared<OperandValueWord>((hi << 8) | lo);
        operand1 = std::static_pointer_cast<Operand>(op1);

        CALL();
}

//! \brief CALL NZ,##
void Cpu::Impl::Op_00C4() {
        uint16_t lo = bus->read(cpu->PC++);
        uint16_t hi = bus->read(cpu->PC++);
        auto op1 = std::make_shared<OperandValueWord>((hi << 8) | lo);
        operand1 = std::static_pointer_cast<Operand>(op1);

        if (!cpu->flagGet('z')) {
                CALL();
        }
}

//! \brief CALL Z,##
void Cpu::Impl::Op_00CC() {
        uint16_t lo = bus->read(cpu->PC++);
        uint16_t hi = bus->read(cpu->PC++);
        auto op1 = std::make_shared<OperandValueWord>((hi << 8) | lo);
        operand1 = std::static_pointer_cast<Operand>(op1);

        if (cpu->flagGet('z')) {
                CALL();
        }
}

//! \brief CALL NC,##
void Cpu::Impl::Op_00D4() {
        uint16_t lo = bus->read(cpu->PC++);
        uint16_t hi = bus->read(cpu->PC++);
        auto op1 = std::make_shared<OperandValueWord>((hi << 8) | lo);
        operand1 = std::static_pointer_cast<Operand>(op1);

        if (!cpu->flagGet('c')) {
                CALL();
        }
}

//! \brief CALL C,##
void Cpu::Impl::Op_00DC() {
        uint16_t lo = bus->read(cpu->PC++);
        uint16_t hi = bus->read(cpu->PC++);
        auto op1 = std::make_shared<OperandValueWord>((hi << 8) | lo);
        operand1 = std::static_pointer_cast<Operand>(op1);

        if (cpu->flagGet('c')) {
                CALL();
        }
}

//-- Restart Opcodes -----------------------------------------------------------

// These Instructions are of the form RST n where the current address is pushed
// onto the stack, then n specifies which address to jump to.

//! \brief RST 0x00
void Cpu::Impl::Op_00C7() {
        auto op = std::make_shared<OperandAddress>(0x00, bus);
        operand1 = std::static_pointer_cast<Operand>(op);

        RST();
}

//! \brief RST 0x08
void Cpu::Impl::Op_00CF() {
        auto op = std::make_shared<OperandAddress>(0x08, bus);
        operand1 = std::static_pointer_cast<Operand>(op);

        RST();
}

//! \brief RST 0x10
void Cpu::Impl::Op_00D7() {
        auto op = std::make_shared<OperandAddress>(0x10, bus);
        operand1 = std::static_pointer_cast<Operand>(op);

        RST();
}

//! \brief RST 0x18
void Cpu::Impl::Op_00DF() {
        auto op = std::make_shared<OperandAddress>(0x18, bus);
        operand1 = std::static_pointer_cast<Operand>(op);

        RST();
}

//! \brief RST 0x20
void Cpu::Impl::Op_00E7() {
        auto op = std::make_shared<OperandAddress>(0x20, bus);
        operand1 = std::static_pointer_cast<Operand>(op);

        RST();
}

//! \brief RST 0x28
void Cpu::Impl::Op_00EF() {
        auto op = std::make_shared<OperandAddress>(0x28, bus);
        operand1 = std::static_pointer_cast<Operand>(op);

        RST();
}

//! \brief RST 0x30
void Cpu::Impl::Op_00F7() {
        auto op = std::make_shared<OperandAddress>(0x30, bus);
        operand1 = std::static_pointer_cast<Operand>(op);

        RST();
}

//! \brief RST 0x38
void Cpu::Impl::Op_00FF() {
        auto op = std::make_shared<OperandAddress>(0x38, bus);
        operand1 = std::static_pointer_cast<Operand>(op);

        RST();
}

//-- Returns -------------------------------------------------------------------

//! \brief RET
void Cpu::Impl::Op_00C9() {
        RET();
}

//! \brief RET NZ
void Cpu::Impl::Op_00C0() {
        if (0 == cpu->flagGet('z')) {
                RET();
        }
}

//! \brief RET Z
void Cpu::Impl::Op_00C8() {
        if (0 != cpu->flagGet('z')) {
                RET();
        }
}

//! \brief RET NC
void Cpu::Impl::Op_00D0() {
        if (0 == cpu->flagGet('c')) {
                RET();
        }
}

//! \brief RET C
void Cpu::Impl::Op_00D8() {
        if (0 != cpu->flagGet('c')) {
                RET();
        }
}

//! \brief RETI
void Cpu::Impl::Op_00D9() {
        RETI();
}

//-----------------------------------------------------------------------------
// Opcode Function Mapping
//-----------------------------------------------------------------------------

void Cpu::Impl::InitInstructionMap() {
        instructionMap = {
                // 8-bit Load
                { 0x0006, { "LD B,n",    &Cpu::Impl::Op_0006, 8 } },
                { 0x000E, { "LD C,n",    &Cpu::Impl::Op_000E, 8 } },
                { 0x0016, { "LD D,n",    &Cpu::Impl::Op_0016, 8 } },
                { 0x001E, { "LD E,n",    &Cpu::Impl::Op_001E, 8 } },
                { 0x0026, { "LD H,n",    &Cpu::Impl::Op_0026, 8 } },
                { 0x002E, { "LD L,n",    &Cpu::Impl::Op_002E, 8 } },
                { 0x007F, { "LD A,A",    &Cpu::Impl::Op_007F, 4 } },
                { 0x0078, { "LD A,B",    &Cpu::Impl::Op_0078, 4 } },
                { 0x0079, { "LD A,C",    &Cpu::Impl::Op_0079, 4 } },
                { 0x007A, { "LD A,D",    &Cpu::Impl::Op_007A, 4 } },
                { 0x007B, { "LD A,E",    &Cpu::Impl::Op_007B, 4 } },
                { 0x007C, { "LD A,H",    &Cpu::Impl::Op_007C, 4 } },
                { 0x007D, { "LD A,L",    &Cpu::Impl::Op_007D, 4 } },
                { 0x007E, { "LD A,(HL)", &Cpu::Impl::Op_007E, 8 } },
                { 0x0040, { "LD B,B",    &Cpu::Impl::Op_0040, 4 } },
                { 0x0041, { "LD B,C",    &Cpu::Impl::Op_0041, 4 } },
                { 0x0042, { "LD B,D",    &Cpu::Impl::Op_0042, 4 } },
                { 0x0043, { "LD B,E",    &Cpu::Impl::Op_0043, 4 } },
                { 0x0044, { "LD B,H",    &Cpu::Impl::Op_0044, 4 } },
                { 0x0045, { "LD B,L",    &Cpu::Impl::Op_0045, 4 } },
                { 0x0046, { "LD B,(HL)", &Cpu::Impl::Op_0046, 8 } },
                { 0x0048, { "LD C,B",    &Cpu::Impl::Op_0048, 4 } },
                { 0x0049, { "LD C,C",    &Cpu::Impl::Op_0049, 4 } },
                { 0x004A, { "LD C,D",    &Cpu::Impl::Op_004A, 4 } },
                { 0x004B, { "LD C,E",    &Cpu::Impl::Op_004B, 4 } },
                { 0x004C, { "LD C,H",    &Cpu::Impl::Op_004C, 4 } },
                { 0x004D, { "LD C,L",    &Cpu::Impl::Op_004D, 4 } },
                { 0x004E, { "LD C,(HL)", &Cpu::Impl::Op_004E, 8 } },
                { 0x0050, { "LD D,B",    &Cpu::Impl::Op_0050, 4 } },
                { 0x0051, { "LD D,C",    &Cpu::Impl::Op_0051, 4 } },
                { 0x0052, { "LD D,D",    &Cpu::Impl::Op_0052, 4 } },
                { 0x0053, { "LD D,E",    &Cpu::Impl::Op_0053, 4 } },
                { 0x0054, { "LD D,H",    &Cpu::Impl::Op_0054, 4 } },
                { 0x0055, { "LD D,L",    &Cpu::Impl::Op_0055, 4 } },
                { 0x0056, { "LD D,(HL)", &Cpu::Impl::Op_0056, 8 } },
                { 0x0058, { "LD E,B",    &Cpu::Impl::Op_0058, 4 } },
                { 0x0059, { "LD E,C",    &Cpu::Impl::Op_0059, 4 } },
                { 0x005A, { "LD E,D",    &Cpu::Impl::Op_005A, 4 } },
                { 0x005B, { "LD E,E",    &Cpu::Impl::Op_005B, 4 } },
                { 0x005C, { "LD E,H",    &Cpu::Impl::Op_005C, 4 } },
                { 0x005D, { "LD E,L",    &Cpu::Impl::Op_005D, 4 } },
                { 0x005E, { "LD E,(HL)", &Cpu::Impl::Op_005E, 8 } },
                { 0x0060, { "LD H,B",    &Cpu::Impl::Op_0060, 4 } },
                { 0x0061, { "LD H,C",    &Cpu::Impl::Op_0061, 4 } },
                { 0x0062, { "LD H,D",    &Cpu::Impl::Op_0062, 4 } },
                { 0x0063, { "LD H,E",    &Cpu::Impl::Op_0063, 4 } },
                { 0x0064, { "LD H,H",    &Cpu::Impl::Op_0064, 4 } },
                { 0x0065, { "LD H,L",    &Cpu::Impl::Op_0065, 4 } },
                { 0x0066, { "LD H,(HL)", &Cpu::Impl::Op_0066, 8 } },
                { 0x0068, { "LD L,B",    &Cpu::Impl::Op_0068, 4 } },
                { 0x0069, { "LD L,C",    &Cpu::Impl::Op_0069, 4 } },
                { 0x006A, { "LD L,D",    &Cpu::Impl::Op_006A, 4 } },
                { 0x006B, { "LD L,E",    &Cpu::Impl::Op_006B, 4 } },
                { 0x006C, { "LD L,H",    &Cpu::Impl::Op_006C, 4 } },
                { 0x006D, { "LD L,L",    &Cpu::Impl::Op_006D, 4 } },
                { 0x006E, { "LD L,(HL)", &Cpu::Impl::Op_006E, 8 } },
                { 0x0070, { "LD (HL),B", &Cpu::Impl::Op_0070, 8 } },
                { 0x0071, { "LD (HL),C", &Cpu::Impl::Op_0071, 8 } },
                { 0x0072, { "LD (HL),D", &Cpu::Impl::Op_0072, 8 } },
                { 0x0073, { "LD (HL),E", &Cpu::Impl::Op_0073, 8 } },
                { 0x0074, { "LD (HL),H", &Cpu::Impl::Op_0074, 8 } },
                { 0x0075, { "LD (HL),L", &Cpu::Impl::Op_0075, 8 } },
                { 0x0036, { "LD (HL),n", &Cpu::Impl::Op_0036, 12 } },
                { 0x007F, { "LD A,A",    &Cpu::Impl::Op_007F, 4 } },
                { 0x000A, { "LD A,(BC)", &Cpu::Impl::Op_000A, 8 } },
                { 0x001A, { "LD A,(DE)", &Cpu::Impl::Op_001A, 8 } },
                { 0x00FA, { "LD A,(##)", &Cpu::Impl::Op_00FA, 16 } },
                { 0x003E, { "LD A,n",    &Cpu::Impl::Op_003E, 8 } },
                { 0x0047, { "LD A,B",    &Cpu::Impl::Op_0047, 4 } },
                { 0x004F, { "LD A,C",    &Cpu::Impl::Op_004F, 4 } },
                { 0x0057, { "LD A,D",    &Cpu::Impl::Op_0057, 4 } },
                { 0x005F, { "LD A,E",    &Cpu::Impl::Op_005F, 4 } },
                { 0x0067, { "LD A,H",    &Cpu::Impl::Op_0067, 4 } },
                { 0x006F, { "LD A,L",    &Cpu::Impl::Op_006F, 4 } },
                { 0x0002, { "LD (BC),A", &Cpu::Impl::Op_0002, 8 } },
                { 0x0012, { "LD (DE),A", &Cpu::Impl::Op_0002, 8 } },
                { 0x007E, { "LD (HL),A", &Cpu::Impl::Op_007E, 8 } },
                { 0x00FA, { "LD (##),A", &Cpu::Impl::Op_00FA, 16 } },
                { 0x00F2, { "LDH A,(0x00FF00+C)", &Cpu::Impl::Op_00F2, 8 } },
                { 0x00E2, { "LDH (0x00FF00+C),A", &Cpu::Impl::Op_00E2, 8 } },
                { 0x003A, { "LDD A,(HL)", &Cpu::Impl::Op_003A, 8 } },
                { 0x0032, { "LDD (HL),A", &Cpu::Impl::Op_0032, 8 } },
                { 0x002A, { "LDI A,(HL)", &Cpu::Impl::Op_002A, 8 } },
                { 0x0022, { "LDI (HL),A", &Cpu::Impl::Op_0022, 8 } },
                { 0x002A, { "LDH (0x00FF00+n),A", &Cpu::Impl::Op_002A, 12 } },
                { 0x0022, { "LDH A,(0x00FF00+n)", &Cpu::Impl::Op_0022, 12 } },

                // 16-bit Load
                { 0x0001, { "LD BC,##", &Cpu::Impl::Op_0001, 12 } },
                { 0x0011, { "LD DE,##", &Cpu::Impl::Op_0011, 12 } },
                { 0x0021, { "LD HL,##", &Cpu::Impl::Op_0021, 12 } },
                { 0x0031, { "LD SP,##", &Cpu::Impl::Op_0031, 12 } },
                { 0x00F9, { "LD SP,HL", &Cpu::Impl::Op_00F9, 8 } },
                { 0x00F8, { "LDHL SP,n", &Cpu::Impl::Op_00F8, 12 } },
                { 0x0008, { "LD (##),SP", &Cpu::Impl::Op_0008, 20 } },
                { 0x00F5, { "PUSH AF", &Cpu::Impl::Op_00F5, 16 } },
                { 0x00C5, { "PUSH BC", &Cpu::Impl::Op_00C5, 16 } },
                { 0x00D5, { "PUSH DE", &Cpu::Impl::Op_00D5, 16 } },
                { 0x00E5, { "PUSH HL", &Cpu::Impl::Op_00E5, 16 } },
                { 0x00F1, { "POP AF", &Cpu::Impl::Op_00F1, 12 } },
                { 0x00C1, { "POP BC", &Cpu::Impl::Op_00C1, 12 } },
                { 0x00D1, { "POP DE", &Cpu::Impl::Op_00D1, 12 } },
                { 0x00E1, { "POP HL", &Cpu::Impl::Op_00E1, 12 } },

                // 8-bit ALU
                { 0x0087, { "ADD A,A", &Cpu::Impl::Op_0087, 4 } },
                { 0x0080, { "ADD A,B", &Cpu::Impl::Op_0080, 4 } },
                { 0x0081, { "ADD A,C", &Cpu::Impl::Op_0081, 4 } },
                { 0x0082, { "ADD A,D", &Cpu::Impl::Op_0082, 4 } },
                { 0x0083, { "ADD A,E", &Cpu::Impl::Op_0083, 4 } },
                { 0x0084, { "ADD A,H", &Cpu::Impl::Op_0084, 4 } },
                { 0x0085, { "ADD A,L", &Cpu::Impl::Op_0085, 4 } },
                { 0x0086, { "ADD A,(HL)", &Cpu::Impl::Op_0086, 8 } },
                { 0x00C6, { "ADD A,##", &Cpu::Impl::Op_00C6, 8 } },

                { 0x008F, { "ADC A,A", &Cpu::Impl::Op_008F, 4 } },
                { 0x0088, { "ADC A,B", &Cpu::Impl::Op_0088, 4 } },
                { 0x0089, { "ADC A,C", &Cpu::Impl::Op_0089, 4 } },
                { 0x008A, { "ADC A,D", &Cpu::Impl::Op_008A, 4 } },
                { 0x008B, { "ADC A,E", &Cpu::Impl::Op_008B, 4 } },
                { 0x008C, { "ADC A,H", &Cpu::Impl::Op_008C, 4 } },
                { 0x008D, { "ADC A,L", &Cpu::Impl::Op_008D, 4 } },
                { 0x008E, { "ADC A,(HL)", &Cpu::Impl::Op_008E, 8 } },
                { 0x00CE, { "ADC A,##", &Cpu::Impl::Op_00CE, 8 } },

                { 0x0097, { "SUB A", &Cpu::Impl::Op_0097, 4 } },
                { 0x0090, { "SUB B", &Cpu::Impl::Op_0090, 4 } },
                { 0x0091, { "SUB C", &Cpu::Impl::Op_0091, 4 } },
                { 0x0092, { "SUB D", &Cpu::Impl::Op_0092, 4 } },
                { 0x0093, { "SUB E", &Cpu::Impl::Op_0093, 4 } },
                { 0x0094, { "SUB H", &Cpu::Impl::Op_0094, 4 } },
                { 0x0095, { "SUB L", &Cpu::Impl::Op_0095, 4 } },
                { 0x0096, { "SUB (HL)", &Cpu::Impl::Op_0096, 8 } },
                { 0x00D6, { "SUB ##", &Cpu::Impl::Op_00D6, 8 } },

                { 0x009F, { "SBC A,A", &Cpu::Impl::Op_009F, 4 } },
                { 0x0098, { "SBC A,B", &Cpu::Impl::Op_0098, 4 } },
                { 0x0099, { "SBC A,C", &Cpu::Impl::Op_0099, 4 } },
                { 0x009A, { "SBC A,D", &Cpu::Impl::Op_009A, 4 } },
                { 0x009B, { "SBC A,E", &Cpu::Impl::Op_009B, 4 } },
                { 0x009C, { "SBC A,H", &Cpu::Impl::Op_009C, 4 } },
                { 0x009D, { "SBC A,L", &Cpu::Impl::Op_009D, 4 } },
                { 0x009E, { "SBC A,(HL)", &Cpu::Impl::Op_009E, 8 } },

                { 0x00A7, { "AND A", &Cpu::Impl::Op_00A7, 4 } },
                { 0x00A0, { "AND B", &Cpu::Impl::Op_00A0, 4 } },
                { 0x00A1, { "AND C", &Cpu::Impl::Op_00A1, 4 } },
                { 0x00A2, { "AND D", &Cpu::Impl::Op_00A2, 4 } },
                { 0x00A3, { "AND E", &Cpu::Impl::Op_00A3, 4 } },
                { 0x00A4, { "AND H", &Cpu::Impl::Op_00A4, 4 } },
                { 0x00A5, { "AND L", &Cpu::Impl::Op_00A5, 4 } },
                { 0x00A6, { "AND (HL)", &Cpu::Impl::Op_00A6, 8 } },
                { 0x00E6, { "AND ##", &Cpu::Impl::Op_00E6, 8 } },

                { 0x00B7, { "OR A", &Cpu::Impl::Op_00B7, 4 } },
                { 0x00B0, { "OR B", &Cpu::Impl::Op_00B0, 4 } },
                { 0x00B1, { "OR C", &Cpu::Impl::Op_00B1, 4 } },
                { 0x00B2, { "OR D", &Cpu::Impl::Op_00B2, 4 } },
                { 0x00B3, { "OR E", &Cpu::Impl::Op_00B3, 4 } },
                { 0x00B4, { "OR H", &Cpu::Impl::Op_00B4, 4 } },
                { 0x00B5, { "OR L", &Cpu::Impl::Op_00B5, 4 } },
                { 0x00B6, { "OR (HL)", &Cpu::Impl::Op_00B6, 8 } },
                { 0x00F6, { "OR ##", &Cpu::Impl::Op_00F6, 8 } },

                { 0x00AF, { "XOR A", &Cpu::Impl::Op_00AF, 4 } },
                { 0x00A8, { "XOR B", &Cpu::Impl::Op_00A8, 4 } },
                { 0x00A9, { "XOR C", &Cpu::Impl::Op_00A9, 4 } },
                { 0x00AA, { "XOR D", &Cpu::Impl::Op_00AA, 4 } },
                { 0x00AB, { "XOR E", &Cpu::Impl::Op_00AB, 4 } },
                { 0x00AC, { "XOR H", &Cpu::Impl::Op_00AC, 4 } },
                { 0x00AD, { "XOR L", &Cpu::Impl::Op_00AD, 4 } },
                { 0x00AE, { "XOR (HL)", &Cpu::Impl::Op_00AE, 8 } },
                { 0x00EE, { "XOR ##", &Cpu::Impl::Op_00EE, 8 } },

                { 0x00BF, { "CP A", &Cpu::Impl::Op_00BF, 4 } },
                { 0x00B8, { "CP B", &Cpu::Impl::Op_00B8, 4 } },
                { 0x00B9, { "CP C", &Cpu::Impl::Op_00B9, 4 } },
                { 0x00BA, { "CP D", &Cpu::Impl::Op_00BA, 4 } },
                { 0x00BB, { "CP E", &Cpu::Impl::Op_00BB, 4 } },
                { 0x00BC, { "CP H", &Cpu::Impl::Op_00BC, 4 } },
                { 0x00BD, { "CP L", &Cpu::Impl::Op_00BD, 4 } },
                { 0x00BE, { "CP (HL)", &Cpu::Impl::Op_00BE, 8 } },
                { 0x00FE, { "CP ##", &Cpu::Impl::Op_00FE, 8 } },

                { 0x003C, { "INC A", &Cpu::Impl::Op_003C, 4 } },
                { 0x0004, { "INC B", &Cpu::Impl::Op_0004, 4 } },
                { 0x000C, { "INC C", &Cpu::Impl::Op_000C, 4 } },
                { 0x0014, { "INC D", &Cpu::Impl::Op_0014, 4 } },
                { 0x001C, { "INC E", &Cpu::Impl::Op_001C, 4 } },
                { 0x0024, { "INC H", &Cpu::Impl::Op_0024, 4 } },
                { 0x002C, { "INC L", &Cpu::Impl::Op_002C, 4 } },
                { 0x0034, { "INC (HL)", &Cpu::Impl::Op_0034, 12 } },

                { 0x003D, { "DEC A", &Cpu::Impl::Op_003D, 4 } },
                { 0x0005, { "DEC B", &Cpu::Impl::Op_0005, 4 } },
                { 0x000D, { "DEC C", &Cpu::Impl::Op_000D, 4 } },
                { 0x0015, { "DEC D", &Cpu::Impl::Op_0015, 4 } },
                { 0x001D, { "DEC E", &Cpu::Impl::Op_001D, 4 } },
                { 0x0025, { "DEC H", &Cpu::Impl::Op_0025, 4 } },
                { 0x002D, { "DEC L", &Cpu::Impl::Op_002D, 4 } },
                { 0x0035, { "DEC (HL)", &Cpu::Impl::Op_0035, 12 } },

                // 16-bit Arithmetic
                { 0x0009, { "ADD HL,BC", &Cpu::Impl::Op_0009, 8 } },
                { 0x0019, { "ADD HL,DE", &Cpu::Impl::Op_0019, 8 } },
                { 0x0029, { "ADD HL,HL", &Cpu::Impl::Op_0029, 8 } },
                { 0x0039, { "ADD HL,SP", &Cpu::Impl::Op_0039, 8 } },
                { 0x00E8, { "ADD SP,n",  &Cpu::Impl::Op_00E8, 16 } },

                { 0x000B, { "DEC BC", &Cpu::Impl::Op_000B, 8 } },
                { 0x001B, { "DEC DE", &Cpu::Impl::Op_001B, 8 } },
                { 0x002B, { "DEC HL", &Cpu::Impl::Op_002B, 8 } },
                { 0x003B, { "DEC SP", &Cpu::Impl::Op_003B, 8 } },

                // Miscellaneous
                { 0x0037, { "SWAP A",    &Cpu::Impl::Op_0037, 8 } },
                { 0x0030, { "SWAP B",    &Cpu::Impl::Op_0030, 8 } },
                { 0x0031, { "SWAP C",    &Cpu::Impl::Op_0031, 8 } },
                { 0x0032, { "SWAP D",    &Cpu::Impl::Op_0032, 8 } },
                { 0x0033, { "SWAP E",    &Cpu::Impl::Op_0033, 8 } },
                { 0x0034, { "SWAP H",    &Cpu::Impl::Op_0034, 8 } },
                { 0x0035, { "SWAP L",    &Cpu::Impl::Op_0035, 8 } },
                { 0x0036, { "SWAP (HL)", &Cpu::Impl::Op_0036, 16 } },

                { 0x0027, { "DAA",  &Cpu::Impl::Op_0027, 4 } },
                { 0x002F, { "CPL",  &Cpu::Impl::Op_002F, 4 } },
                { 0x003F, { "CCF",  &Cpu::Impl::Op_003F, 4 } },
                { 0x0037, { "SCF",  &Cpu::Impl::Op_0037, 4 } },
                { 0x0000, { "NOP",  &Cpu::Impl::Op_0000, 4 } },
                { 0x0076, { "HALT", &Cpu::Impl::Op_0076, 4 } },
                { 0x1000, { "STOP", &Cpu::Impl::Op_1000, 4 } },
                { 0x00F3, { "DI",   &Cpu::Impl::Op_00F3, 4 } },
                { 0x00FB, { "EI",   &Cpu::Impl::Op_00FB, 4 } },

                // Rotates and Shifts
                { 0x0007, { "RLCA", &Cpu::Impl::Op_0007, 4 } },
                { 0x0017, { "RLA",  &Cpu::Impl::Op_0017,  4 } },
                { 0x000F, { "RRCA", &Cpu::Impl::Op_000F, 4 } },
                { 0x001F, { "RRA",  &Cpu::Impl::Op_001F,  4 } },

                { 0xCB07, { "RLC A",    &Cpu::Impl::Op_CB07, 8 } },
                { 0xCB00, { "RLC B",    &Cpu::Impl::Op_CB00, 8 } },
                { 0xCB01, { "RLC C",    &Cpu::Impl::Op_CB01, 8 } },
                { 0xCB02, { "RLC D",    &Cpu::Impl::Op_CB02, 8 } },
                { 0xCB03, { "RLC E",    &Cpu::Impl::Op_CB03, 8 } },
                { 0xCB04, { "RLC H",    &Cpu::Impl::Op_CB04, 8 } },
                { 0xCB05, { "RLC L",    &Cpu::Impl::Op_CB05, 8 } },
                { 0xCB06, { "RLC (HL)", &Cpu::Impl::Op_CB06, 16 } },

                { 0xCB17, { "RL A",    &Cpu::Impl::Op_CB17, 8 } },
                { 0xCB10, { "RL B",    &Cpu::Impl::Op_CB10, 8 } },
                { 0xCB11, { "RL C",    &Cpu::Impl::Op_CB11, 8 } },
                { 0xCB12, { "RL D",    &Cpu::Impl::Op_CB12, 8 } },
                { 0xCB13, { "RL E",    &Cpu::Impl::Op_CB13, 8 } },
                { 0xCB14, { "RL H",    &Cpu::Impl::Op_CB14, 8 } },
                { 0xCB15, { "RL L",    &Cpu::Impl::Op_CB15, 8 } },
                { 0xCB16, { "RL (HL)", &Cpu::Impl::Op_CB16, 16 } },

                { 0xCB0F, { "RRC A", &Cpu::Impl::Op_CB0F, 8 } },
                { 0xCB08, { "RRC B", &Cpu::Impl::Op_CB08, 8 } },
                { 0xCB09, { "RRC C", &Cpu::Impl::Op_CB09, 8 } },
                { 0xCB0A, { "RRC D", &Cpu::Impl::Op_CB0A, 8 } },
                { 0xCB0B, { "RRC E", &Cpu::Impl::Op_CB0B, 8 } },
                { 0xCB0C, { "RRC H", &Cpu::Impl::Op_CB0C, 8 } },
                { 0xCB0D, { "RRC L", &Cpu::Impl::Op_CB0D, 8 } },
                { 0xCB0E, { "RRC (HL)", &Cpu::Impl::Op_CB0E, 16 } },

                { 0xCB1F, { "RR A", &Cpu::Impl::Op_CB1F, 8 } },
                { 0xCB18, { "RR B", &Cpu::Impl::Op_CB18, 8 } },
                { 0xCB19, { "RR C", &Cpu::Impl::Op_CB19, 8 } },
                { 0xCB1A, { "RR D", &Cpu::Impl::Op_CB1A, 8 } },
                { 0xCB1B, { "RR E", &Cpu::Impl::Op_CB1B, 8 } },
                { 0xCB1C, { "RR H", &Cpu::Impl::Op_CB1C, 8 } },
                { 0xCB1D, { "RR L", &Cpu::Impl::Op_CB1D, 8 } },
                { 0xCB1E, { "RR (HL)", &Cpu::Impl::Op_CB1E, 16 } },

                { 0xCB27, { "SLA A", &Cpu::Impl::Op_CB27, 8 } },
                { 0xCB20, { "SLA B", &Cpu::Impl::Op_CB20, 8 } },
                { 0xCB21, { "SLA C", &Cpu::Impl::Op_CB21, 8 } },
                { 0xCB22, { "SLA D", &Cpu::Impl::Op_CB22, 8 } },
                { 0xCB23, { "SLA E", &Cpu::Impl::Op_CB23, 8 } },
                { 0xCB24, { "SLA H", &Cpu::Impl::Op_CB24, 8 } },
                { 0xCB25, { "SLA L", &Cpu::Impl::Op_CB25, 8 } },
                { 0xCB26, { "SLA (HL)", &Cpu::Impl::Op_CB26, 16 } },

                { 0xCB2F, { "SRA A", &Cpu::Impl::Op_CB2F, 8 } },
                { 0xCB28, { "SRA B", &Cpu::Impl::Op_CB28, 8 } },
                { 0xCB29, { "SRA C", &Cpu::Impl::Op_CB29, 8 } },
                { 0xCB2A, { "SRA D", &Cpu::Impl::Op_CB2A, 8 } },
                { 0xCB2B, { "SRA E", &Cpu::Impl::Op_CB2B, 8 } },
                { 0xCB2C, { "SRA H", &Cpu::Impl::Op_CB2C, 8 } },
                { 0xCB2D, { "SRA L", &Cpu::Impl::Op_CB2D, 8 } },
                { 0xCB2E, { "SRA (HL)", &Cpu::Impl::Op_CB2E, 16 } },

                { 0xCB3F, { "SRL A", &Cpu::Impl::Op_CB3F, 8 } },
                { 0xCB38, { "SRL B", &Cpu::Impl::Op_CB38, 8 } },
                { 0xCB39, { "SRL C", &Cpu::Impl::Op_CB39, 8 } },
                { 0xCB3A, { "SRL D", &Cpu::Impl::Op_CB3A, 8 } },
                { 0xCB3B, { "SRL E", &Cpu::Impl::Op_CB3B, 8 } },
                { 0xCB3C, { "SRL H", &Cpu::Impl::Op_CB3C, 8 } },
                { 0xCB3D, { "SRL L", &Cpu::Impl::Op_CB3D, 8 } },
                { 0xCB3E, { "SRL (HL)", &Cpu::Impl::Op_CB3E, 16 } },

                // Bit Operations
                { 0xCB47, { "BIT b,A",    &Cpu::Impl::Op_CB47, 8 } },
                { 0xCB40, { "BIT b,B",    &Cpu::Impl::Op_CB40, 8 } },
                { 0xCB41, { "BIT b,C",    &Cpu::Impl::Op_CB41, 8 } },
                { 0xCB42, { "BIT b,D",    &Cpu::Impl::Op_CB42, 8 } },
                { 0xCB43, { "BIT b,E",    &Cpu::Impl::Op_CB43, 8 } },
                { 0xCB44, { "BIT b,H",    &Cpu::Impl::Op_CB44, 8 } },
                { 0xCB45, { "BIT b,L",    &Cpu::Impl::Op_CB45, 8 } },
                { 0xCB46, { "BIT b,(HL)", &Cpu::Impl::Op_CB46, 16 } },

                { 0xCBC7, { "SET b,A",    &Cpu::Impl::Op_CBC7, 8 } },
                { 0xCBC0, { "SET b,B",    &Cpu::Impl::Op_CBC0, 8 } },
                { 0xCBC1, { "SET b,C",    &Cpu::Impl::Op_CBC1, 8 } },
                { 0xCBC2, { "SET b,D",    &Cpu::Impl::Op_CBC2, 8 } },
                { 0xCBC3, { "SET b,E",    &Cpu::Impl::Op_CBC3, 8 } },
                { 0xCBC4, { "SET b,H",    &Cpu::Impl::Op_CBC4, 8 } },
                { 0xCBC5, { "SET b,L",    &Cpu::Impl::Op_CBC5, 8 } },
                { 0xCBC6, { "SET b,(HL)", &Cpu::Impl::Op_CBC6, 16 } },

                { 0xCBC8, { "RES b,A",    &Cpu::Impl::Op_CB87, 8 } },
                { 0xCB80, { "RES b,B",    &Cpu::Impl::Op_CB80, 8 } },
                { 0xCB81, { "RES b,C",    &Cpu::Impl::Op_CB81, 8 } },
                { 0xCB82, { "RES b,D",    &Cpu::Impl::Op_CB82, 8 } },
                { 0xCB83, { "RES b,E",    &Cpu::Impl::Op_CB83, 8 } },
                { 0xCB84, { "RES b,H",    &Cpu::Impl::Op_CB84, 8 } },
                { 0xCB85, { "RES b,L",    &Cpu::Impl::Op_CB85, 8 } },
                { 0xCB86, { "RES b,(HL)", &Cpu::Impl::Op_CB86, 16 } },

                // Jumps
                { 0x00C3, { "JP ##",    &Cpu::Impl::Op_00C3, 12 } },
                { 0x00C2, { "JP NZ,##", &Cpu::Impl::Op_00C2, 12 } },
                { 0x00CA, { "JP Z,##",  &Cpu::Impl::Op_00CA, 12 } },
                { 0x00D2, { "JP NC,##", &Cpu::Impl::Op_00D2, 12 } },
                { 0x00DA, { "JP C,##",  &Cpu::Impl::Op_00DA, 12 } },
                { 0x00E9, { "JP (HL)",  &Cpu::Impl::Op_00E9, 4 } },
                { 0x0018, { "JR #",     &Cpu::Impl::Op_0018, 8 } },
                { 0x0020, { "JR NZ,#",  &Cpu::Impl::Op_0020, 8 } },
                { 0x0028, { "JR Z,#",   &Cpu::Impl::Op_0028, 8 } },
                { 0x0030, { "JR NC,#",  &Cpu::Impl::Op_0030, 8 } },
                { 0x0038, { "JR C,#" ,  &Cpu::Impl::Op_0038, 8 } },

                // Calls
                { 0x00CD, { "CALL ##",    &Cpu::Impl::Op_00CD, 12 } },
                { 0x00C4, { "CALL NZ,##", &Cpu::Impl::Op_00C4, 12 } },
                { 0x00C4, { "CALL Z,##",  &Cpu::Impl::Op_00C4, 12 } },
                { 0x00C4, { "CALL NC,##", &Cpu::Impl::Op_00C4, 12 } },
                { 0x00C4, { "CALL C,##",  &Cpu::Impl::Op_00C4, 12 } },

                // Restarts
                { 0x00C7, { "RST 0x00", &Cpu::Impl::Op_00C7, 32 } },
                { 0x00CF, { "RST 0x08", &Cpu::Impl::Op_00CF, 32 } },
                { 0x00D7, { "RST 0x10", &Cpu::Impl::Op_00D7, 32 } },
                { 0x00DF, { "RST 0x18", &Cpu::Impl::Op_00DF, 32 } },
                { 0x00E7, { "RST 0x20", &Cpu::Impl::Op_00E7, 32 } },
                { 0x00EF, { "RST 0x28", &Cpu::Impl::Op_00EF, 32 } },
                { 0x00F7, { "RST 0x30", &Cpu::Impl::Op_00F7, 32 } },
                { 0x00FF, { "RST 0x38", &Cpu::Impl::Op_00FF, 32 } },

                // Returns
                { 0x00C9, { "RET",    &Cpu::Impl::Op_00C9, 8 } },
                { 0x00C0, { "RET NZ", &Cpu::Impl::Op_00C0, 8 } },
                { 0x00C8, { "RET Z",  &Cpu::Impl::Op_00C8, 8 } },
                { 0x00D0, { "RET NC", &Cpu::Impl::Op_00D0, 8 } },
                { 0x00D8, { "RET C",  &Cpu::Impl::Op_00D8, 8 } },
                { 0x00D9, { "RETI",   &Cpu::Impl::Op_00D9,8 } },
        };
}

//------------------------------------------------------------------------------
// Operations
//------------------------------------------------------------------------------

void Cpu::Impl::LD() {
        operand1->set(operand2->get());
}

//! Push register pair nn onto stack. Decrement Stack Pointer (SP) twice.
//! Write Little-Endian, so the LSB occurs first in memory.
void Cpu::Impl::PUSH() {
        uint16_t word = operand1->get();
        bus->write(--cpu->SP, static_cast<uint8_t>(word >> 8));
        bus->write(--cpu->SP, static_cast<uint8_t>(word | 0xFF));
}

//! Pop two bytes off stack into register pair nn. Increment Stack Pointer (SP)
//! twice.
void Cpu::Impl::POP() {
        uint16_t word = 0;
        uint16_t byte = bus->read(cpu->SP++);
        word = byte << 8;
        byte = bus->read(cpu->SP++);
        word |= byte;

        operand1->set(word);
}

void Cpu::Impl::ADD8() {
        uint8_t left = operand1->get();
        uint8_t right = operand2->get();

        uint8_t sum = left + right;
        bool halfCarry = ((left & 0xF) + (right & 0xF)) & 0x10;
        bool carry = ((uint16_t)left + (uint16_t)right) & 0x100;

        operand1->set(sum);

        cpu->flagSet('z', 0 == sum);
        cpu->flagSet('n', 0);
        cpu->flagSet('h', halfCarry);
        cpu->flagSet('c', carry);
}

void Cpu::Impl::ADD16() {
        uint16_t left = operand1->get();
        uint16_t right = operand2->get();

        uint16_t sum = left + right;
        bool halfCarry = ((left & 0xFFF) + (right & 0xFFF)) & 0x1000;
        bool carry = ((uint32_t)left + (uint32_t)right) & 0x10000;

        operand1->set(sum);

        cpu->flagSet('n', 0);
        cpu->flagSet('h', halfCarry);
        cpu->flagSet('c', carry);
}

//! The operand, along with the Carry Flag (C in the F Register) is added to the
//! contents of the Accumulator, and the result is stored in the Accumulator.
void Cpu::Impl::ADC8() {
        uint8_t left = operand1->get();
        uint8_t right = operand2->get();

        uint8_t prevCarry = cpu->flagGet('c') ? 1 : 0;
        uint8_t sum = left + right + prevCarry;
        bool halfCarry = ((left & 0xF) + (right & 0xF) + prevCarry) & 0x10;
        bool carry = ((uint16_t)left + (uint16_t)right + prevCarry) & 0x100;

        operand1->set(sum);

        cpu->flagSet('z', 0 == sum);
        cpu->flagSet('n', 0);
        cpu->flagSet('h', halfCarry);
        cpu->flagSet('c', carry);
}

void Cpu::Impl::SUB8() {
        int16_t minuend = cpu->registers.r8.A;
        int16_t subtrahend = operand1->get();

        int16_t difference = minuend - subtrahend;
        bool halfCarry = (minuend & 0xF) < (subtrahend & 0xF);
        bool carry = minuend < subtrahend;

        operand1->set(static_cast<uint8_t>(difference));

        cpu->flagSet('z', 0 == difference);
        cpu->flagSet('n', 0);
        cpu->flagSet('h', halfCarry); // TODO: invert logic?
        cpu->flagSet('c', carry); // TODO: invert logic?
}

void Cpu::Impl::SBC8() {
        int16_t minuend = cpu->registers.r8.A;
        uint8_t prevCarry = cpu->flagGet('c') ? 1 : 0;
        int16_t subtrahend = operand1->get() + prevCarry;

        uint16_t difference = minuend - subtrahend;
        bool halfCarry = (minuend & 0xF) < (subtrahend & 0xF);
        bool carry = minuend < subtrahend;

        operand1->set(static_cast<uint8_t>(difference));

        cpu->flagSet('z', 0 == difference);
        cpu->flagSet('n', 0);
        cpu->flagSet('h', halfCarry); // TODO: invert logic?
        cpu->flagSet('c', carry); // TODO: invert logic?
}

void Cpu::Impl::AND() {
        cpu->registers.r8.A &= operand1->get();

        cpu->flagSet('z', 0 == cpu->registers.r8.A);
        cpu->flagSet('n', 0);
        cpu->flagSet('h', 0);
        cpu->flagSet('c', 0);
}

void Cpu::Impl::OR() {
        cpu->registers.r8.A |= operand1->get();

        cpu->flagSet('z', 0 == cpu->registers.r8.A);
        cpu->flagSet('n', 0);
        cpu->flagSet('h', 0);
        cpu->flagSet('c', 0);
}

void Cpu::Impl::XOR() {
        cpu->registers.r8.A ^= operand1->get();

        cpu->flagSet('z', 0 == cpu->registers.r8.A);
        cpu->flagSet('n', 0);
        cpu->flagSet('h', 0);
        cpu->flagSet('c', 0);
}

void Cpu::Impl::CP() {
        int16_t minuend = cpu->registers.r8.A;
        int16_t subtrahend = operand1->get();
        int16_t result = minuend - subtrahend;
        bool halfCarry = (minuend & 0xF) < (subtrahend & 0xF);

        cpu->flagSet('z', 0 == result);
        cpu->flagSet('n', 1);
        cpu->flagSet('h', halfCarry);
        cpu->flagSet('c', minuend < subtrahend);
}

void Cpu::Impl::INC8() {
        auto val = operand1->get();
        bool halfCarry = ((val & 0xF) + 1) & 0x10;
        operand1->set(++val);

        cpu->flagSet('z', 0 == val);
        cpu->flagSet('n', 0);
        cpu->flagSet('h', halfCarry);
}

void Cpu::Impl::DEC8() {
        auto val = operand1->get();
        bool halfCarry = (val & 0xF) < 1;
        operand1->set(--val);

        cpu->flagSet('z', 0 == val);
        cpu->flagSet('n', 0);
        cpu->flagSet('h', halfCarry);
}

void Cpu::Impl::INC16() {
        auto value = operand1->get();
        operand1->set(value++);
}

void Cpu::Impl::DEC16() {
        auto value = operand1->get();
        operand1->set(value--);
}

//! \brief Swap upper & lower nibbles of operand.
void Cpu::Impl::SWAP() {
        uint8_t oldValue = operand1->get();
        uint8_t nibbleHi = Nibble(oldValue, 1);
        uint8_t nibbleLo = Nibble(oldValue, 0);
        uint8_t newValue = (nibbleLo << 4) | nibbleHi;
        operand2->set(newValue);

        cpu->flagSet('z', !newValue);
        cpu->flagSet('n', 0);
        cpu->flagSet('h', 0);
        cpu->flagSet('c', 0);
}

//! \brief Decimal adjust register A.
//!
//! This Instruction adjusts register A so that the correct representation of
//! Binary Coded Decimal (BCD) is obtained.
//!
//! DAA is intended to be run immediately after an addition or subtraction
//! operation, where the operands were BCD encoded. It then makes the
//! corrections described above, so the result (stored in the A register) is the
//! BCD encoded result of the previous operation.
//!
//! \see https://ehaskins.com/2018-01-30%20Z80%20DAA/
void Cpu::Impl::DAA() {
        uint8_t val = cpu->registers.r8.A;

        uint8_t wasCarryHalf = cpu->flagGet('h');
        uint8_t wasCarry = cpu->flagGet('c');
        uint8_t wasSubtraction = cpu->flagGet('n');

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
        cpu->registers.r8.A = newValue;

        cpu->flagSet('z', !newValue);
        cpu->flagSet('h', 0);
        cpu->flagSet('p', Parity(newValue));
        cpu->flagSet('c', newValue > 0x99); // TODO verify against table in Z80 manual
}

//! \brief The contents of the Accumulator (Register A) are inverted (one’s complement).
void Cpu::Impl::CPL() {
        uint8_t val = ~(cpu->registers.r8.A);
        operand2->set(val);

        cpu->flagSet('h', 1);
        cpu->flagSet('n', 1);
}

// // TODO: Not used in GB?
// //! \brief Perform two's complement negation on A.
// //!
// //! The contents of the Accumulator are negated (two’s complement). This method
// //! is the same as subtracting the contents of the Accumulator from zero.
// // void NEG() {
// //         operand2 = ((*this).*(Instruction->getOperand2))();
// //         uint16_t val = static_cast<uint8_t>(operand2->get());
// //         uint16_t onesComplement = ~val;
// //         uint16_t twosComplement = onesComplement + 1;
// //        operand2->set(static_cast<uint8_t>(twosComplement));

// //         flagSet('s', twosComplement & 0x7);
// //         flagSet('z', !twosComplement);
// //         // TODO: set if borrow from bit 4. // flagSet('h', (twosComplement >> 0x4)
// //         flagSet('p', 0x80 == val);
// //         flagSet('n', 0);
// //         flagSet('c', !(0x00 == val));
// // }

//! \brief The carry flag is inverted in the F register.
void Cpu::Impl::CCF() {
        cpu->flagSet('h', 0);
        cpu->flagSet('n', 0);
        cpu->flagSet('c', !cpu->flagGet('c'));
}

//! \brief the carry flag is set in the F register.
void Cpu::Impl::SCF() {
        cpu->flagSet('h', 0);
        cpu->flagSet('n', 0);
        cpu->flagSet('c', 1);
}

void Cpu::Impl::NOP() {
}

//! \brief Power down CPU until an interrupt occurs.
//!
//! The HALT Instruction suspends CPU operation until a subsequent interrupt or
//! reset is received. While in the HALT state, the processor executes NOPs to
//! maintain memory refresh logic.
void Cpu::Impl::HALT() {
        halted = true;
        waitForButtonPress = false;
}

//! \brief Halt CPU & LCD display until button press.
void Cpu::Impl::STOP() {
        halted = true;
        waitForButtonPress = true;
}

//! \brief Disable interrupts after next Instruction.
//!
//! When the CPU executes the Instruction DI the maskable interrupt is disabled
//! until it is subsequently re-enabled by an EI Instruction. The CPU does not
//! respond to an Interrupt Request (INT) signal.
void Cpu::Impl::DI() {
        interruptsDisabledRequested = true;
}

//! \brief Enable interrupts after next Instruction.
void Cpu::Impl::EI() {
        interruptsEnabledRequested = true;
}

//! \brief Rotate A left. Old bit 7 is copied to the carry flag.
//!
//! The contents of the Accumulator (Register A) are rotated left 1 bit
//! position. The sign bit (bit 7) is copied to the Carry flag and also to bit
//! 0. Bit 0 is the least-significant bit.
void Cpu::Impl::RLCA() {
        uint8_t oldValue = cpu->registers.r8.A;
        uint8_t carry = ((uint8_t)oldValue >> 0x7) & 0x1;
        uint8_t newValue = ((uint8_t)oldValue << 1) | carry;
        cpu->registers.r8.A = newValue;

        cpu->flagSet('z', !newValue);
        cpu->flagSet('n', 0);
        cpu->flagSet('h', 0);
        cpu->flagSet('c', carry);
}

//! \brief Rotate A left through the carry flag.
//!
//! The contents of the Accumulator (Register A) are rotated left 1 bit position
//! through the Carry flag. The previous contents of the Carry flag are copied
//! to bit 0. Bit 0 is the least-significant bit.
void Cpu::Impl::RLA() {
        uint16_t oldValue = cpu->registers.r8.A;
        uint8_t carry = ((uint8_t)oldValue >> 0x7) & 0x1;
        uint8_t oldCarry = cpu->flagGet('c');
        uint8_t newValue = ((uint8_t)oldValue << 1) | oldCarry;
        cpu->registers.r8.A = newValue;

        cpu->flagSet('z', !newValue);
        cpu->flagSet('n', 0);
        cpu->flagSet('h', 0);
        cpu->flagSet('c', carry);
}

//! \brief Rotate A right. Old bit 0 is copied to the carry flag.
//!
//! The contents of the Accumulator (Register A) are rotated right 1 bit
//! position. Bit 0 is cop-ied to the Carry flag and also to bit 7. Bit 0 is the
//! least-significant bit.
void Cpu::Impl::RRCA() {
        uint16_t oldValue = cpu->registers.r8.A;
        uint8_t carry = (uint8_t)oldValue & 0x1;
        uint8_t newValue = ((uint8_t)oldValue >> 1) | (carry << 0x7);
        cpu->registers.r8.A = newValue;

        cpu->flagSet('z', !newValue);
        cpu->flagSet('n', 0);
        cpu->flagSet('h', 0);
        cpu->flagSet('c', carry);
}

//! \brief Rotate A right through the carry flag.
//!
//! The contents of the Accumulator (Register A) are rotated right 1 bit
//! position through the Carry flag. The previous contents of the Carry flag are
//! copied to bit 7. Bit 0 is the least-significant bit.
void Cpu::Impl::RRA() {
        uint16_t oldValue = cpu->registers.r8.A;
        uint8_t carry = (uint8_t)oldValue & 0x1;
        uint8_t oldCarry = cpu->flagGet('c');
        uint8_t newValue = ((uint8_t)oldValue >> 1) | (oldCarry << 0x7);
        cpu->registers.r8.A = newValue;

        cpu->flagSet('z', !newValue);
        cpu->flagSet('n', 0);
        cpu->flagSet('h', 0);
        cpu->flagSet('c', carry);
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
void Cpu::Impl::RLC() {
        uint16_t oldValue = operand1->get();
        uint8_t carry = ((uint8_t)oldValue >> 0x7) & 0x1;
        uint8_t newValue = ((uint8_t)oldValue << 1) | carry;
        operand1->set(newValue);

        cpu->flagSet('s', newValue & (0x1 << 0x7));
        cpu->flagSet('z', !newValue);
        cpu->flagSet('h', 0);
        cpu->flagSet('p', Parity(newValue));
        cpu->flagSet('n', 0);
        cpu->flagSet('c', carry);
}

//! \brief Rotate left through carry flag.
//!
//! The contents of the m operand are rotated left 1 bit position. The contents
//! of bit 7 are copied to the Carry flag, and the previous contents of the
//! Carry flag are copied to bit 0.
void Cpu::Impl::RL() {
        uint16_t oldValue = operand1->get();
        uint8_t carry = ((uint8_t)oldValue >> 0x7) & 0x1;
        uint8_t oldCarry = cpu->flagGet('c');
        uint8_t newValue = ((uint8_t)oldValue << 1) | oldCarry;
        operand1->set(newValue);

        cpu->flagSet('s', newValue & (0x1 << 0x7));
        cpu->flagSet('z', !newValue);
        cpu->flagSet('h', 0);
        cpu->flagSet('p', Parity(newValue));
        cpu->flagSet('n', 0);
        cpu->flagSet('c', carry);
}

//! \brief Rotate right. Old bit 0 is set to the carry flag.
//!
//! The contents of the m operand are rotated right 1 bit position. The contents
//! of bit 0 are copied to the Carry flag and also to bit 7. Bit 0 is the
//! least-significant bit.
void Cpu::Impl::RRC() {
        uint16_t oldValue = operand1->get();
        uint8_t carry = (uint8_t)oldValue & 0x1;
        uint8_t newValue = ((uint8_t)oldValue >> 1) | carry;
        operand1->set(newValue);

        cpu->flagSet('s', newValue & (0x1 << 0x7));
        cpu->flagSet('z', !newValue);
        cpu->flagSet('h', 0);
        cpu->flagSet('p', Parity(newValue));
        cpu->flagSet('n', 0);
        cpu->flagSet('c', carry);
}

//! \brief Rotate n right through carry flag.
//!
//! The contents of operand m are rotated right 1 bit position through the carry
//! flag. The contents of bit 0 are copied to the carry flag and the previous
//! contents of the carry flag are copied to bit 7. Bit 0 is the
//! least-significant bit.
void Cpu::Impl::RR() {
        uint16_t oldValue = operand1->get();
        uint8_t carry = (uint8_t)oldValue & 0x1;
        uint8_t oldCarry = cpu->flagGet('c');
        uint8_t newValue = ((uint8_t)oldValue >> 1) | oldCarry;
        operand1->set(newValue);

        cpu->flagSet('s', newValue & (0x1 << 0x7));
        cpu->flagSet('z', !newValue);
        cpu->flagSet('h', 0);
        cpu->flagSet('p', Parity(newValue));
        cpu->flagSet('n', 0);
        cpu->flagSet('c', carry);
}

//! \brief Shift left into carry. LSB is set to 0.
//!
//! An arithmetic shift left 1 bit position is performed on the contents of
//! operand m. The contents of bit 7 are copied to the Carry flag. Bit 0 is the
//! least-significant bit.
void Cpu::Impl::SLA() {
        uint8_t oldValue = operand1->get();
        uint8_t carry = oldValue & (0x1 << 0x7);
        uint8_t newValue = oldValue << 1;
        operand1->set(newValue);

        cpu->flagSet('s', (newValue >> 0x7) & 0x1);
        cpu->flagSet('z', !newValue);
        cpu->flagSet('h', 0);
        cpu->flagSet('p', Parity(newValue));
        cpu->flagSet('n', 0);
        cpu->flagSet('c', carry);
}

//! \brief Shift right into Carry. MSB doesn't change.
//!
//! An arithmetic shift right 1 bit position is performed on the contents of
//! operand m. The contents of bit 0 are copied to the Carry flag and the
//! previous contents of bit 7 remain unchanged. Bit 0 is the least-significant
//! bit.
void Cpu::Impl::SRA() {
        uint8_t oldValue = operand1->get();
        uint8_t msb = (oldValue & 0x7);
        uint8_t carry = oldValue & 0x1;
        uint8_t newValue = (oldValue >> 1) | msb;
        operand1->set(newValue);

        cpu->flagSet('z', !newValue);
        cpu->flagSet('n', 0);
        cpu->flagSet('h', 0);
        cpu->flagSet('c', carry);
}

//! \brief Shift right into Carry. MSB is set to 0.
//!
//! The contents of operand m are shifted right 1 bit position. The contents of
//! bit 0 are copied to the Carry flag, and bit 7 is reset. Bit 0 is the
//! least-significant bit.
void Cpu::Impl::SRL() {
        uint8_t oldValue = operand1->get();
        uint8_t carry = oldValue & 0x1;
        uint8_t newValue = oldValue >> 1;
        operand1->set(newValue);

        cpu->flagSet('z', !newValue);
        cpu->flagSet('n', 0);
        cpu->flagSet('h', 0);
        cpu->flagSet('c', carry);
}

//! \brief Test bit in register
//!
//! This Instruction tests bit b in register r and sets the Z flag
//! accordingly.
void Cpu::Impl::BIT() {
        uint8_t bit = operand1->get();
        uint8_t byte = operand2->get();
        uint8_t test = 0x1 << bit;

        cpu->flagSet('z', !(byte & test));
        cpu->flagSet('n', 0);
        cpu->flagSet('h', 1);
}

//! \brief Set bit in register
//!
//! Bit b in register r (any of registers B, C, D, E, H, L, or A) is set.
void Cpu::Impl::SET() {
        uint8_t bit = operand1->get();
        uint8_t byte = operand2->get();
        uint8_t set = 0x1 << bit;
        operand2->set(byte | set);
}

//! Reset bit in register.
void Cpu::Impl::RES() {
        uint8_t bit = operand1->get();
        uint8_t byte = operand2->get();
        uint8_t set = 0x1 << bit;
        set = ~set;
        operand2->set(byte & set);
}

void Cpu::Impl::JP() {
        cpu->PC = operand1->get();
}

//! \brief Add n to current address and jump to it
void Cpu::Impl::JR() {
        uint16_t address = cpu->PC + operand1->get();
        cpu->PC = address;
}

//! \brief Push address of next Instruction onto the stack and then jump to address nn
void Cpu::Impl::CALL() {
        bus->write(--cpu->SP, static_cast<uint8_t>(cpu->PC & 0xFF));
        bus->write(--cpu->SP, static_cast<uint8_t>(cpu->PC >> 8));
        cpu->PC = operand1->get();
}

//! Push present address onto stack. Jump to address $0000 + n.
void Cpu::Impl::RST() {
        bus->write(--cpu->SP, static_cast<uint8_t>(cpu->PC & 0xFF));
        bus->write(--cpu->SP, static_cast<uint8_t>(cpu->PC >> 8));
        cpu->PC = operand1->get();
}

void Cpu::Impl::RET() {
        uint16_t address;
        uint8_t byte = bus->read(cpu->SP++);
        address = byte << 8;
        byte = bus->read(cpu->SP++);
        address = address | byte;
        cpu->PC = address;
}

//! This Instruction is used at the end of a maskable interrupt service routine
//! to:
//!
//! • Restore the contents of the Program Counter (analogous to the RET
//!   Instruction)
//!
//! • Signal an I/O device that the interrupt routine is
//!   completed. The RETI Instruction also facilitates the nesting of interrupts,
//!   allowing higher priority devices to temporarily suspend service of lower
//!   priority service routines. However, this Instruction does not enable
//!   interrupts that were disabled when the interrupt routine was entered. Before
//!   doing the RETI Instruction, the enable interrupt Instruction (EI) should be
//!   executed to allow recognition of interrupts after completion of the current
//!   service routine.
void Cpu::Impl::RETI() {
        RET();
        interruptsDisabled = false;
        interruptsDisabledRequested = false;
        interruptsEnabledRequested = false;
}

//------------------------------------------------------------------------------
// Public interface
//------------------------------------------------------------------------------

void Cpu::instructionFetch() {
        uint8_t opcodeByte;
        opcode = 0;

        std::ostream fmt(NULL);
        fmt.copyfmt(std::cout);

        uint16_t debug_byte_1 = static_cast<uint16_t>(bus->read(PC + 0));
        uint16_t debug_byte_2 = static_cast<uint16_t>(bus->read(PC + 1));
        uint16_t debug_byte_3 = static_cast<uint16_t>(bus->read(PC + 2));
        uint16_t debug_byte_4 = static_cast<uint16_t>(bus->read(PC + 3));

        std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << debug_byte_1 << " ";
        std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << debug_byte_2 << " ";
        std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << debug_byte_3 << " ";
        std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << debug_byte_4 << " ";
        std::cout.copyfmt(fmt);

        std::cout << "pc: " << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << PC;
        opcodeByte = bus->read(PC++);

        // Some opcodes are two-bytes long; these are prefixed with the byte
        // 0x10 or 0xCB.
        // NOTE: I'm not sure if these are ordered B1|B2 or B2|B1.
        //       I'm assuming B1|B2 in this impl but may need to
        //       follow up on that.
        if (0x10 == opcodeByte || 0xCB == opcodeByte) {
                opcode = (opcodeByte << 8);
                opcodeByte = bus->read(PC++);
        }

        opcode = (opcode | opcodeByte);
        std::cout << ", opcode: 0x" << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << opcode << " ";
        std::cout.copyfmt(fmt);

        impl->instruction = std::make_shared<Instruction>(impl->instructionMap[opcode]);
        std::cout << *(impl->instruction) << std::endl;
}

void Cpu::instructionExecute() {
        ((*impl).*(impl->instruction->op))();
}

void Cpu::dumpState() {
        printf(
                "\tB:    %02X C:    %02X D:    %02X E:    %02X H:    %02X L:    %02X A:    %02X F:    %02X\n",
                registers.r8.B,
                registers.r8.C,
                registers.r8.D,
                registers.r8.E,
                registers.r8.H,
                registers.r8.L,
                registers.r8.A,
                registers.r8.F
                );

        printf(
                "\t        BC:  %04X         DE:  %04X         HL:  %04X         AF:  %04X\n",
                registers.r16.BC,
                registers.r16.DE,
                registers.r16.HL,
                registers.r16.AF
                );
}

} // namespace gs
