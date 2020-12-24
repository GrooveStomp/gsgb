/******************************************************************************
 * File: bus.hpp
 * Created: 2019-08-30
 * Updated: 2020-12-23
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2020, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
#ifndef BUS_VERSION
#define BUS_VERSION "0.1.0" //!< include guard

#include <cstdint>
#include <cstddef>

namespace gs {
        class Cpu;

        class Bus {
        private:
                uint8_t *memory;
                uint8_t *videoMemory; //!< 256x256
                uint8_t *videoDisplay; //!< 160x144
                Cpu *cpu;

                static char *bootRom;

                const uint16_t BOOT = 0xFF50;
                const uint16_t WX = 0xFF4B;
                const uint16_t WY = 0xFF4A;
                const uint16_t OBP1 = 0xFF49;
                const uint16_t OBP0 = 0xFF48;
                const uint16_t BGP = 0xFF47;
                const uint16_t DMA = 0xFF46;
                const uint16_t LYC = 0xFF45;
                const uint16_t LY = 0xFF44;
                const uint16_t SCX = 0xFF43;
                const uint16_t SCY = 0xFF42;
                const uint16_t STAT = 0xFF41;
                const uint16_t LCDC = 0xFF40;
        public:

                uint8_t *bgTileMap; //!< Tiles to be displayed.

                Bus();
                ~Bus();

                void write(uint16_t ptr, uint8_t value);
                uint8_t read(uint16_t ptr);

                void setBootRom(char *rom, size_t size);
                void attachCart(char *cart, size_t size);
                void reset();
        };

} // namespace gs

#endif // BUS_VERSION
