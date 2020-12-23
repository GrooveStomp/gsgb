/******************************************************************************
 * File: gb.cpp
 * Created: 2019-09-13
 * Updated: 2020-12-23
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2020, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
#include <cstdint>
#include <cstddef>

#include "gb.hpp"
#include "bus.hpp"
#include "cpu.hpp"

namespace gs {

//! The first thing the program does is read the cartridge locations from $104
//! to $133 and place this graphic of a Nintendo logo on the screen at the
//! top. This image is then scrolled until it is in the middle of the
//! screen. Two musical notes are then played on the internal speaker. Again,
//! the cartridge locations $104 to $133 are read but this time they are
//! compared with a table in the internal rom.  If any byte fails to compare,
//! then the GB stops comparing bytes and simply halts all operations.
char *bootRom = {}; //!< Boot rom and DRM check.


GB::GB() {
        // RAM starts at 0xC000
        memory = new char[8 * 1024];
        // Video memory starts at 0x8000
        videoMemory = new uint8_t[8 * 1024];

        // Alternatively, 0x8800-0x97FF
        bgTileMap = &videoMemory[0x8000]; //!< Up to 0x8FFF

        bus = new Bus;
        cpu = new Cpu(bus);
}

GB::~GB() {
        delete[] videoMemory;
        delete[] memory;

        delete cpu;
        delete bus;
}

void GB::boot() {
        // TODO Run bootRom first.
}

void GB::romLoad(char *rom, size_t size) {
        // TODO: Assert size is less than available size in memory for GB system.
        size_t j = 0x100;
        for (size_t i = 0; i < size; i++) {
                memory[j++] = rom[i];
        }

        cpu->registers.r16.AF = 0x0001;
        cpu->registers.r16.BC = 0x0013;
        cpu->registers.r16.DE = 0x00D8;
        cpu->registers.r16.HL = 0x014D;
        cpu->SP = 0xFFFE;
}

void GB::romExecute() {
        // TODO main loop here.
}

} // namespace gs