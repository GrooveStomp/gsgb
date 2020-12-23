/******************************************************************************
 * File: main.cpp
 * Created: 2019-08-29
 * Updated: 2020-12-23
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2020, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
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
