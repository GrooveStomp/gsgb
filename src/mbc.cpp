/******************************************************************************
 * File: mbc.cpp
 * Created: 2020-12-28
 * Updated: 2020-12-28
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2020, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
#include "mbc.hpp"

namespace gs {

        /**********************************************************************
         * Mbc
         **********************************************************************/
        Mbc::~Mbc() {}

        /**********************************************************************
         * MbcNone
         **********************************************************************/
        MbcNone::MbcNone(uint32_t ram_size) {
                if (ram_size > 0&& ram_size <= (8 * 1024)) {
                        ram = new uint8_t[ram_size];
                        ram_size = ram_size;
                } else {
                        ram_size = 0;
                        ram = nullptr;
                }
        }

        MbcNone::~MbcNone() {
                delete[] ram;
        }

        bool MbcNone::write(uint16_t addr, uint8_t value) {
                if (addr >= 0x0000 && addr <= 0x7FFF) {
                        rom[addr] = value;
                        return true;
                } else if (addr >= 0xA000 && addr <= 0xBFFF && (addr - 0xA000) < ram_size) {
                        ram[addr] = value;
                        return true;
                }

                return false;
        }

        bool MbcNone::read(uint16_t addr, uint8_t &value) {
                if (addr >= 0x0000 && addr <= 0x7FFF) {
                        value = rom[addr];
                        return true;
                } else if (addr >= 0xA000 && addr <= 0xBFFF && (addr - 0xA000) < ram_size) {
                        value = ram[addr];
                        return true;
                }

                return false;
        }

        /**********************************************************************
         * Mbc1
         **********************************************************************/
        Mbc1::Mbc1(uint32_t rom_size, uint32_t ram_size) {
        }

        Mbc1::~Mbc1() {
        }

        bool Mbc1::write(uint16_t addr, uint8_t value) {
                return false;
        }

        bool Mbc1::read(uint16_t addr, uint8_t &value) {
                if (addr >= 0x0000 && addr <= 0x3FFF) {
                        value = bank0[addr];
                }
                return false;
        }

} // namespace gs
