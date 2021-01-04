/******************************************************************************
 * File: bus.hpp
 * Created: 2019-08-30
 * Updated: 2021-01-04
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
        class Video;

        class Bus {
        private:
                uint8_t *memory;
                Cpu *cpu;
                Cartridge *cart;
                Video *video;

        public:
                struct {
                        uint8_t boot; // boot flag
                        uint8_t sb; // serial byte
                        uint8_t sc; // serial control
                } memRegisters;

                Bus();
                ~Bus();

                void write(uint16_t ptr, uint8_t value);
                uint8_t read(uint16_t ptr);

                void attach(Cartridge *cart);
                void attach(Cpu *cpu);
                void attach(Video *video);
                void reset();
        };

} // namespace gs

#endif // BUS_VERSION
