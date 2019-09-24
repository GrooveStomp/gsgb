/******************************************************************************
  GrooveStomp's GB Emulator
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: cartridge.cpp
  Created: 2019-09-24
  Updated: 2019-09-24
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)

  This program comes with ABSOLUTELY NO WARRANTY. This is free software, and
  you are welcome to redistribute it under certain conditions; See LICENSE for
  details.
 ******************************************************************************/
#include "cartridge.hpp"

//! \file cartridge.cpp

namespace gs {

Cartridge::Cartridge(uint8_t *rom, unsigned int size) {
        for (unsigned int b = 0; b < 16; b++) {
                for (unsigned int i = 0; i < 1024; i++) {
                        blocks[b * 1024 + i] = rom[b * 1024 + i];
                }
        }
}

Cartridge::~Cartridge() {
}

} // namespace gs
