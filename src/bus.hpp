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

namespace gs {

class Bus {
public:
        void write(uint16_t ptr, uint8_t value);
        uint8_t read(uint16_t ptr);
};

} // namespace gs

#endif // BUS_VERSION
