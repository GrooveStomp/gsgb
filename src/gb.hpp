/******************************************************************************
 * File: gb.hpp
 * Created: 2019-08-29
 * Updated: 2020-12-23
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2020, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
#ifndef GB_VERSION
#define GB_VERSION "0.1.0" //!< include guard

#include <cstdint>
#include <cstddef>

//! \file gb.hpp
//! \see http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf

namespace gs {

class Bus;
class Cpu;

class GB {
private:
        char *memory;
        uint8_t *videoMemory; //!< 256x256
        uint8_t *videoDisplay; //!< 160x144
        Bus *bus;
        Cpu *cpu;

        static char *bootRom;

public:
        uint8_t *bgTileMap; //!< Tiles to be displayed.

        GB();
        ~GB();

        void boot();
        void romLoad(char *rom, size_t size);
        void romExecute();
};

#endif // GB_VERSION

} // namespace gs