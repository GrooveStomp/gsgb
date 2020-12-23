/******************************************************************************
 * File: display.cpp
 * Created: 2019-09-24
 * Updated: 2020-12-23
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2020, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
//! \file display.cpp
#include "display.hpp"

namespace gs {

Display::Display(Ram *r) {
        ram = r;
}

Display::~Display() {
}

void Display::Render() {
        /*
          SCY: Scroll Y-coord
          SCX: Scroll X-coord
          LY: LCD Y-coord
          LYC: LY Compare
          BGP: Background color palette
          OBP0: 1. Sprite color palette
          OBP1: 2. Sprite color palette
          WY: Window Y-coord
          WX: Window X-coord
         */
        /*
          - Load row LY of the background tile view to the line buffer.
          - Overwrite the line buffer with row LY from the window tile view.
          - Sprite engine generates a 1-pixel section of the sprites, where they
            intersect LY and overwrites the line buffer with this.
         */
}

} // namespace gs
