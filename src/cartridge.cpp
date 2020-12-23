/******************************************************************************
 * File: cartridge.cpp
 * Created: 2019-09-24
 * Updated: 2020-12-23
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2020, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
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
