/******************************************************************************
 * File: bus.hpp
 * Created: 2019-08-30
 * Updated: 2020-12-28
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2021, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
#ifndef BUS_VERSION
#define BUS_VERSION "0.1.0" //!< include guard

#include <cstdint>
#include <cstddef>

namespace gs {
        class Cpu;
        class Cartridge;

        class Bus {
        private:
                uint8_t *memory;
                uint8_t *videoMemory; //!< 256x256
                uint8_t *videoDisplay; //!< 160x144
                Cpu *cpu;
                Cartridge *cart;

        public:
                struct {
                        uint8_t boot; // boot flag
                        uint8_t sb; // serial byte
                        uint8_t sc; // serial control
                } memRegisters;

                uint8_t *bgTileMap; //!< Tiles to be displayed.

                Bus();
                ~Bus();

                void write(uint16_t ptr, uint8_t value);
                uint8_t read(uint16_t ptr);

                void attach(Cartridge *cart);
                void attach(Cpu *cpu);
                void reset();
        };

} // namespace gs

#endif // BUS_VERSION
