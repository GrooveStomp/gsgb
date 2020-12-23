/******************************************************************************
  GrooveStomp's GB Emulator
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: bus.hpp
  Created: 2019-08-30
  Updated: 2019-09-07
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)

  This program comes with ABSOLUTELY NO WARRANTY. This is free software, and
  you are welcome to redistribute it under certain conditions; See LICENSE for
  details.
 ******************************************************************************/

#ifndef BUS_VERSION
#define BUS_VERSION "0.1.0" //!< include guard

#include <cstdint>

namespace gs {

class bus {
public:
        void Write(uint16_t ptr, uint8_t value);
        uint8_t Read(uint16_t ptr);
};

} // namespace gs

#endif // BUS_VERSION
