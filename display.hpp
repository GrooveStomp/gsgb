/******************************************************************************
  GrooveStomp's GB Emulator
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: display.hpp
  Created: 2019-09-24
  Updated: 2019-09-24
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)

  This program comes with ABSOLUTELY NO WARRANTY. This is free software, and
  you are welcome to redistribute it under certain conditions; See LICENSE for
  details.
 ******************************************************************************/
#ifndef GB_DISPLAY
#define GB_DISPLAY "0.1.0" //!< include guard

#include <cstdint>

//! \file display.hpp

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
