/******************************************************************************
 * File: operand.cpp
 * Created: 2019-08-31
 * Updated: 2020-12-23
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2020, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
#include "operand.hpp"
#include "bus.hpp"

namespace gs {

operand_value::operand_value(uint8_t in) : value(in) {
}

uint16_t operand_value::Get() {
        return static_cast<uint16_t>(value);
}

operand_address::operand_address(uint16_t in, bus *b) {
        address = in;
        msgBus = b;
}

uint16_t operand_address::Get() {
        return static_cast<uint16_t>(msgBus->Read(address));
}

// TODO NOTE Only uses the lower 8 bits!
void operand_address::Set(uint16_t value) {
        msgBus->Write(address, static_cast<uint8_t>(value));
}

operand_reference::operand_reference(uint8_t &in) : ref(&in) {
}

uint16_t operand_reference::Get() {
        return static_cast<uint8_t>(*ref);
}

void operand_reference::Set(uint16_t value) {
        *ref = static_cast<uint8_t>(value);
}

operand_pair_reference::operand_pair_reference(uint16_t &in) : ref(&in) {
}

uint16_t operand_pair_reference::Get() {
        return *ref;
}

void operand_pair_reference::Set(uint16_t value) {
        *ref = value;
}

} // namespace gs
