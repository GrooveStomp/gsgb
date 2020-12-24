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

#include "bus.hpp"

int main(int argc, char *argv[]) {
        gs::Bus gb;
        ifstream stream;
        streampos size;

        stream.open("data/DMG_ROM.bin", ios::in | ios::binary | ios::ate);
        if (stream.is_open()) {
                size = stream.tellg();
                char *buf = new char[size];
                stream.seekg(0, ios::beg);
                stream.read(buf, size);
                stream.close();
                gb.setBootRom(buf, size);
                delete[] buf;
        } else {
                fputs("Couldn't open boot rom.\n", stderr);
                exit(1);
        }

        stream.open("data/sample.gb", ios::in | ios::binary | ios::ate);
        if (stream.is_open()) {
                size = stream.tellg();
                char *buf = new char[size];
                stream.seekg(0, ios::beg);
                stream.read(buf, size);
                stream.close();
                gb.attachCart(buf, size);
                delete[] buf;
        } else {
                fputs("Couldn't open rom.\n", stderr);
                exit(1);
        }

        // TODO: CPU fetch/decode/execute loop

        return 0;
}
