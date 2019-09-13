/******************************************************************************
  GrooveStomp's GB Emulator
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: gb.hpp
  Created: 2019-08-29
  Updated: 2019-08-29
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)

  This program comes with ABSOLUTELY NO WARRANTY. This is free software, and
  you are welcome to redistribute it under certain conditions; See LICENSE for
  details.
 ******************************************************************************/
#ifndef GB_VERSION
#define GB_VERSION "0.1.0" //!< include guard

#include <cstdint>
#include <cstddef>

//! \file gb.hpp
//! \see http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf

namespace gs {

class bus;
class cpu;

class gb {
        char *memory;
        uint8_t *videoMemory;
        bus *mBus;
        cpu *mCpu;

        static char *bootRom;

public:
        gb();
        ~gb();

        void Boot();
        void RomLoad(char *rom, size_t size);
        void RomExecute();
};

#endif // GB_VERSION

} // namespace gs
