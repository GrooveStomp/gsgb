/******************************************************************************
  GrooveStomp's GB Emulator
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: main.cpp
  Created: 2019-08-29
  Updated: 2019-09-13
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)

  This program comes with ABSOLUTELY NO WARRANTY. This is free software, and
  you are welcome to redistribute it under certain conditions; See LICENSE for
  details.
 ******************************************************************************/

#include <cstdint>
#include <fstream>

using namespace std;

#include "gb.hpp"

int main(int argc, char *argv[]) {
        streampos size = 0;
        char *fileMem = nullptr;
        ifstream rom("data/sample.gb", ios::in | ios::binary | ios::ate);
        if (rom.is_open()) {
                size = rom.tellg();
                fileMem = new char[size];

                rom.seekg(0, ios::beg);
                rom.read(fileMem, size);
                rom.close();
        } else {
                // TODO handle not being able to open file.
        }

        gs::gb system;
        system.Boot();
        system.RomLoad(fileMem, size);
        system.RomExecute();

        if (fileMem != nullptr) delete[] fileMem;
        return 0;
}
