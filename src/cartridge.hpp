/******************************************************************************
 * File: cartridge.hpp
 * Created: 2019-09-24
 * Updated: 2020-12-30
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2020, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
//! \file cartridge.hpp
//!
//! A cartridge is physical media that is inserted into the GameBoy containing
//! read-only memory with optional memory bank controller (mbc) wirting to
//! extend the memory space of the GB.

#ifndef GB_CARTRIDGE
#define GB_CARTRIDGE "0.1.0" //!< include guard

#include <cstdint>

namespace gs {
        class Mbc;

        class Cartridge {
        private:
                Mbc *mbc;
        public:
                Cartridge(uint8_t *rom, unsigned int size);
                ~Cartridge();

                bool write(uint16_t ptr, uint8_t value);
                bool read(uint16_t ptr, uint8_t &value);
        };
} // namespace gs

#endif // GB_CARTRIDGE
