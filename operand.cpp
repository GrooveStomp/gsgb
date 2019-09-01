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
        return bus->Read(address);
}

void operand_address::Set(uint8_t value) {
        bus->Write(address, value);
}

uint16_t operand_reference::Get() {
        return *ref;
}

void operand_reference::Set(uint8_t value) {
        *ref = value;
}

} // namespace gs
