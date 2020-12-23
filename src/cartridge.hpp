/******************************************************************************
  GrooveStomp's GB Emulator
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: cartridge.hpp
  Created: 2019-09-24
  Updated: 2019-09-24
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)

  This program comes with ABSOLUTELY NO WARRANTY. This is free software, and
  you are welcome to redistribute it under certain conditions; See LICENSE for
  details.
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
