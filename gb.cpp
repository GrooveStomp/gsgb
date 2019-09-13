/******************************************************************************
  GrooveStomp's GB Emulator
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: gb.cpp
  Created: 2019-09-13
  Updated: 2019-09-13
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)

  This program comes with ABSOLUTELY NO WARRANTY. This is free software, and
  you are welcome to redistribute it under certain conditions; See LICENSE for
  details.
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


gb::gb() {
        memory = new char[1];
        videoMemory = new uint8_t[1];

        mBus = new bus;
        mCpu = new cpu(mBus);
}

gb::~gb() {
        delete[] memory;
        delete[] videoMemory;

        delete mCpu;
        delete mBus;
}

void gb::Boot() {
        // TODO Run bootRom first.
}

void gb::RomLoad(char *rom, size_t size) {
        // TODO: Assert size is less than available size in memory for GB system.
        size_t j = 0x100;
        for (size_t i = 0; i < size; i++) {
                memory[j++] = rom[i];
        }

        mCpu->AF = 0x0001;
        mCpu->BC = 0x0013;
        mCpu->DE = 0x00D8;
        mCpu->HL = 0x014D;
        mCpu->SP = 0xFFFE;
}

void gb::RomExecute() {
        // TODO main loop here.
}

} // namespace gs
