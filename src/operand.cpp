/******************************************************************************
 * File: Operand.cpp
 * Created: 2019-08-31
 * Updated: 2020-12-31
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2020, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
#include "operand.hpp"
#include "bus.hpp"

namespace gs {

OperandValueByte::OperandValueByte(uint8_t in) : value(in) {
}

uint16_t OperandValueByte::get() {
        return static_cast<uint16_t>(value);
}

OperandValueWord::OperandValueWord(uint16_t in) : value(in) {
}

uint16_t OperandValueWord::get() {
        return value;
}

OperandAddress::OperandAddress(uint16_t in, Bus *b) {
        address = in;
        bus = b;
}

uint16_t OperandAddress::get() {
        return static_cast<uint16_t>(bus->read(address));
}

// TODO NOTE Only uses the lower 8 bits!
void OperandAddress::set(uint16_t value) {
        bus->write(address, static_cast<uint8_t>(value));
}

OperandReference::OperandReference(uint8_t &in) : ref(&in) {
}

uint16_t OperandReference::get() {
        return static_cast<uint8_t>(*ref);
}

void OperandReference::set(uint16_t value) {
        *ref = static_cast<uint8_t>(value);
}

OperandPairReference::OperandPairReference(uint16_t &in) : ref(&in) {
}

uint16_t OperandPairReference::get() {
        return *ref;
}

void OperandPairReference::set(uint16_t value) {
        *ref = value;
}

} // namespace gs
