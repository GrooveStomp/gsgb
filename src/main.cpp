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

#include "bus.hpp"
#include "cpu.hpp"
#include "cartridge.hpp"

using namespace std;
using namespace gs;

int main(int argc, char *argv[]) {
        Cpu cpu;
        Cartridge *cart = nullptr;
        Bus gb;

        ifstream stream("data/cpu_instrs/individual/01-special.gb", ios::in | ios::binary | ios::ate);
        if (stream.is_open()) {
                streampos size = stream.tellg();
                char *buf = new char[size];
                stream.seekg(0, ios::beg);
                stream.read(buf, size);
                stream.close();
                cart = new Cartridge(reinterpret_cast<uint8_t*>(buf), size);
                // TODO: Error handling on allocating new cartridge.
                delete[] buf;
        } else {
                fputs("Couldn't open rom.\n", stderr);
                exit(1);
        }

        gb.attach(&cpu);
        gb.attach(cart);
        gb.reset();

        // TODO: CPU fetch/decode/execute loop

        return 0;
}
