/******************************************************************************
  GrooveStomp's GB Emulator
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: operand.cpp
  Created: 2019-08-31
  Updated: 2019-08-31
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)

  This program comes with ABSOLUTELY NO WARRANTY. This is free software, and
  you are welcome to redistribute it under certain conditions; See LICENSE for
  details.
 ******************************************************************************/
#include "operand.hpp"
#include "bus.hpp"

namespace gs {

uint16_t operand_value::Get() {
        return value;
}

uint16_t operand_address::Get() {
        return msgBus->Read(address);
}

// TODO NOTE Only uses the lower 8 bits!
void operand_address::Set(uint16_t value) {
        msgBus->Write(address, static_cast<uint8_t>(value));
}

uint16_t operand_reference::Get() {
        return *ref;
}

void operand_reference::Set(uint16_t value) {
        *ref = value;
}

uint16_t operand_pair_reference::Get() {
        uint8_t hi = *ref1;
        uint8_t lo = *ref2;
        return (hi << 8) | lo;
}

void operand_pair_reference::Set(uint16_t value) {
        *ref1 = static_cast<uint8_t>(value >> 8);
        *ref2 = static_cast<uint8_t>(value);
}

} // namespace gs
