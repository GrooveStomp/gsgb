/******************************************************************************
 * File: display.hpp
 * Created: 2019-09-24
 * Updated: 2020-12-23
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2020, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
//! \file display.hpp
#ifndef GB_DISPLAY
#define GB_DISPLAY "0.1.0" //!< include guard

#include <cstdint>

namespace gs {

class Ram;

class Display {
private:
        Ram *ram;
public:
        Display(Ram *r);
        ~Display();

        void Render();
};

} // namespace gs

#endif // GB_DISPLAY
