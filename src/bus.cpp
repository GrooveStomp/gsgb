/******************************************************************************
 * File: bus.cpp
 * Created: 2019-09-07
 * Updated: 2020-12-23
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2020, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
#include <cassert>

#include "bus.hpp"
#include "cpu.hpp"

namespace gs {

        //! The first thing the program does is read the cartridge locations from
        //! $104 to $133 and place this graphic of a Nintendo logo on the screen
        //! at the top. This image is then scrolled until it is in the middle of
        //! the screen. Two musical notes are then played on the internal
        //! speaker. Again, the cartridge locations $104 to $133 are read but this
        //! time they are compared with a table in the internal rom.  If any byte
        //! fails to compare, then the GB stops comparing bytes and simply halts
        //! all operations.
        char *bootRom = {}; //!< Boot rom and DRM check.

        Bus::Bus() {
                // RAM starts at 0xC000
                memory = new uint8_t[8 * 1024];
                // Video memory starts at 0x8000
                videoMemory = new uint8_t[8 * 1024];

                // Alternatively, 0x8800-0x97FF
                bgTileMap = &videoMemory[0x8000]; //!< Up to 0x8FFF

                cpu = new Cpu(this);

                // TODO: Figure out how memory works...
                //memory[BOOT] = 0x0; // After the boot rom executes, this gets set to 0x1.
        }

        Bus::~Bus() {
                delete[] videoMemory;
                delete[] memory;

                delete cpu;
        }

        void Bus::write(uint16_t ptr, uint8_t value) {
        }

        uint8_t Bus::read(uint16_t ptr) {
                // TODO implement me!
                return 0;
        }

        void Bus::setBootRom(char *rom, size_t size) {
                assert(size <= 256);

                for (size_t i = 0; i < size; i++) {
                        memory[i] = rom[i];
                }
        }

        void Bus::attachCart(char *cart, size_t size) {
                // TODO: Assert size is less than available size in memory for GB system.
                size_t j = 0x100;
                for (size_t i = 0; i < size; i++) {
                        memory[j++] = cart[i];
                }

                cpu->registers.r16.AF = 0x0001;
                cpu->registers.r16.BC = 0x0013;
                cpu->registers.r16.DE = 0x00D8;
                cpu->registers.r16.HL = 0x014D;
                cpu->SP = 0xFFFE;
        }

} // namespace gs
