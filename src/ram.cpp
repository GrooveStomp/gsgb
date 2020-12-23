/******************************************************************************
  GrooveStomp's GB Emulator
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: ram.cpp
  Created: 2019-09-24
  Updated: 2019-09-24
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)

  This program comes with ABSOLUTELY NO WARRANTY. This is free software, and
  you are welcome to redistribute it under certain conditions; See LICENSE for
  details.
 ******************************************************************************/
#include "ram.hpp"

//! \file ram.cpp

namespace gs {

Ram::Ram() {
        memory = new uint8_t[16 * 1024];

        WX  = &memory[0xFF4B];
        WY  = &memory[0xFF4A];
        OBP1 = &memory[0xFF49];
        OBP0 = &memory[0xFF48];
        BGP = &memory[0xFF47];
        DMA = &memory[0xFF46];
        LYC = &memory[0xFF45];
        LY  = &memory[0xFF44];
        SCX = &memory[0xFF43];
        SCY = &memory[0xFF42];
        STAT = &memory[0xFF41];
        LCDC = &memory[0xFF40];
}

Ram::~Ram() {
        delete[] memory;

        WX  = nullptr;
        WY  = nullptr;
        OBP1 = nullptr;
        OBP0 = nullptr;
        BGP = nullptr;
        DMA = nullptr;
        LYC = nullptr;
        LY  = nullptr;
        SCX = nullptr;
        SCY = nullptr;
        STAT = nullptr;
        LCDC = nullptr;
}

uint8_t &Ram::operator[](int i) {
        /*
          TODO: Delegate as necessary, depending on bank switching, etc.
        */
        return memory[i];
}

} // namespace gs
