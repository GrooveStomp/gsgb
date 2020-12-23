/******************************************************************************
  GrooveStomp's GB Emulator
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: ram.hpp
  Created: 2019-09-24
  Updated: 2019-09-24
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)

  This program comes with ABSOLUTELY NO WARRANTY. This is free software, and
  you are welcome to redistribute it under certain conditions; See LICENSE for
  details.
 ******************************************************************************/
#ifndef GB_RAM
#define GB_RAM "0.1.0" //!< include guard

#include <cstdint>

//! \file ram.hpp

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
