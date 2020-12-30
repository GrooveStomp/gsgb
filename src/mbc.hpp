/******************************************************************************
 * File: mbc.hpp
 * Created: 2020-12-28
 * Updated: 2020-12-28
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2020, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
//! \file mbc.hpp
//!
//! MBC is short for Memory Bank Controller and is the mechanism through which
//! GameBoy cartridges can extend the memory space of the GameBoy.
//!
//! \see https://gbdev.gg8.se/wiki/articles/Memory_Bank_Controllers

#ifndef MBC_VERSION
#define MBC_VERSION "0.1.0" //!< include guard

#include <cstdint>

namespace gs {

        class Mbc {
        public:
                virtual ~Mbc() = 0;
                virtual bool write(uint16_t addr, uint8_t value) = 0;
                virtual bool read(uint16_t addr, uint8_t &value) = 0;
        };

        class MbcNone: public Mbc {
        public:
                MbcNone(uint32_t ram_size);
                virtual ~MbcNone();
                virtual bool write(uint16_t addr, uint8_t value);
                virtual bool read(uint16_t addr, uint8_t &value);

                uint8_t rom[32 * 1024];
                uint8_t *ram;
                uint8_t ram_size;
        };

        class Mbc1 : public Mbc {
        public:
                Mbc1(uint32_t rom_size, uint32_t ram_size);
                virtual ~Mbc1();
                virtual bool write(uint16_t addr, uint8_t value);
                virtual bool read(uint16_t addr, uint8_t &value);

                uint8_t bank0[16 * 1024];
        };

} // namespace gs

#endif // MBC_VERSION
