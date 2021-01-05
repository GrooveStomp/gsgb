/******************************************************************************
 * File: host/main.cpp
 * Created: 2019-08-29
 * Updated: 2021-01-04
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2021, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
//! \file host/main.cpp
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

#include "../bus.hpp"
#include "../cpu.hpp"
#include "../cartridge.hpp"
#include "../video.hpp"
#include "graphics.hpp"
#include "input.hpp"

using namespace std;
using namespace gs;

int main(int argc, char *argv[]) {
        Cpu cpu;
        Cartridge *cart = nullptr;
        Bus gb;
        Video video;
        Graphics graphics(std::string("gsgb").c_str(), 160, 144);
        Input input;

        ifstream stream("data/cpu_instrs/individual/03-op sp,hl.gb", ios::in | ios::binary | ios::ate);
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
        gb.attach(&video);
        gb.reset();

        bool running = true;
        vector<char> buf;
        while (running) {
                // TODO: Simulate frequency
                graphics.begin();
                graphics.clear(0xFFFFFFFF);
                input.process();
                running = !input.isQuitRequested();

                cpu.instructionFetch();
                cpu.dumpState();
                cpu.instructionExecute();

                if (gb.memRegisters.sc == 0x81) {
                        cout << "hi" << endl;
                        char byte = static_cast<char>(gb.memRegisters.sb);
                        buf.push_back(byte);
                        if (byte == '\n') {
                                for (auto it = buf.begin(); it != buf.end(); it++) {
                                        cout << *it;
                                }
                                buf.clear();
                        }
                }

                graphics.end();
        }

        return 0;
}
