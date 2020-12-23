/******************************************************************************
 * File: cartridge.hpp
 * Created: 2019-09-24
 * Updated: 2020-12-23
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2020, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
#ifndef GB_CARTRIDGE
#define GB_CARTRIDGE "0.1.0" //!< include guard

#include <cstdint>

//! \file cartridge.hpp

namespace gs {

class Cartridge {
private:
        uint8_t blocks[16 * 1024];
public:
        Cartridge(uint8_t *rom, unsigned int size);
        ~Cartridge();
};

} // namespace gs

#endif // GB_CARTRIDGE
