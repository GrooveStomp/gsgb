/******************************************************************************
 * File: ram.hpp
 * Created: 2019-09-24
 * Updated: 2020-12-23
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2020, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
//! \file ram.hpp
#ifndef GB_RAM
#define GB_RAM "0.1.0" //!< include guard

#include <cstdint>

namespace gs {

//! Used to access bank switching, memory-mapped I/O, ROM-based bank switching, etc.
class Ram {
private:
        uint8_t *memory;
public:
        Ram();
        ~Ram();

        uint8_t &operator[](int i);

        uint8_t *WX;
        uint8_t *WY;
        uint8_t *OBP1;
        uint8_t *OBP0;
        uint8_t *BGP;
        uint8_t *DMA;
        uint8_t *LYC;
        uint8_t *LY;
        uint8_t *SCX;
        uint8_t *SCY;
        uint8_t *STAT;
        uint8_t *LCDC;
};

}

#endif // GB_RAM
