/******************************************************************************
 * File: video.cpp
 * Created: 2021-01-04
 * Updated: 2021-01-04
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2021, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
//! \file video.cpp

#include "video.hpp"

namespace gs {

        bool Video::write(uint16_t addr, uint8_t value) {
                if (addr >= 0x8000 && addr <= 0x9FFF) {
                        uint16_t addr2 = addr - 0x8000;
                        vram[addr2] = value;
                        return true;

                } else if (addr >= 0xFE00 && addr <= 0xFE9F) {
                        uint16_t addr2 = addr - 0xFE00;
                        oam[addr2] = value;
                        return true;
                }

                switch (addr) {
                        case 0xFF40:
                                lcdc = value;
                                return true;

                        case 0xFF42:
                                scrolly = value;
                                return true;

                        case 0xFF43:
                                scrollx = value;
                                return true;

                        case 0xFF4A:
                                wndposy = value;
                                return true;

                        case 0xFF4B:
                                wndposx = value;
                                return true;
                }

                return false;
        }

        bool Video::read(uint16_t addr, uint8_t &value) {
                if (addr >= 0x8000 && addr <= 0x9FFF) {
                        uint16_t addr2 = addr - 0x8000;
                        value = vram[addr2];
                        return true;

                } else if (addr >= 0xFE00 && addr <= 0xFE9F) {
                        uint16_t addr2 = addr - 0xFE00;
                        value = oam[addr2];
                        return true;
                }

                switch (addr) {
                        case 0xFF40:
                                value = lcdc;
                                return true;

                        case 0xFF42:
                                value = scrolly;
                                return true;

                        case 0xFF43:
                                value = scrollx;
                                return true;

                        case 0xFF4A:
                                value = wndposy;
                                return true;

                        case 0xFF4B:
                                value = wndposx;
                                return true;
                }

                return false;
        }

} // namespace gs
