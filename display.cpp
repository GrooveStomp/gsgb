/******************************************************************************
  GrooveStomp's GB Emulator
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: display.cpp
  Created: 2019-09-24
  Updated: 2019-09-24
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)

  This program comes with ABSOLUTELY NO WARRANTY. This is free software, and
  you are welcome to redistribute it under certain conditions; See LICENSE for
  details.
 ******************************************************************************/
#include "display.hpp"

//! \file display.cpp

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
