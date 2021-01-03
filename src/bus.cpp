/******************************************************************************
 * File: bus.cpp
 * Created: 2019-09-07
 * Updated: 2020-12-30
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2021, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
//! \file bus.cpp
// See: https://gbdev.io/pandocs/
#include <cassert>

#include "bus.hpp"
#include "cpu.hpp"
#include "cartridge.hpp"

namespace gs {

        enum AddrMemRegEnum {
                RegBOOT = 0xFF50,
                RegWX = 0xFF4B,
                RegWY = 0xFF4A,
                RegOBP1 = 0xFF49,
                RegOBP0 = 0xFF48,
                RegBGP = 0xFF47,
                RegDMA = 0xFF46,
                RegLYC = 0xFF45,
                RegLY = 0xFF44,
                RegSCX = 0xFF43,
                RegSCY = 0xFF42,
                RegSTAT = 0xFF41,
                RegLCDC = 0xFF40,
        };

        enum AddrSerialEnum {
                SerialTransfer = 0xFF01,
                SerialControl = 0xFF02,
        };

        enum AddrEnum {
                MemRestartInterrupt = 0x0000,
                MemCartHeader = 0x0100,
                MemCartRomBank0 = 0x0150,
                MemCartRomBankN = 0x4000,
                MemCharRam = 0x8000,
                MemBgMapData1 = 0x9800,
                MemBgMapData2 = 0x9C00,
                MemCartRam = 0xA000,
                MemInternalRamBank0 = 0xC000,
                MemOam = 0xFE00,
                MemRegisters = 0xFF00, // Hardware I/O registers.
                MemZeroPage = 0xFF80,
                MemInterruptEnable = 0xFFFF,
        };

        //! The first thing the program does is read the cartridge locations from
        //! $104 to $133 and place this graphic of a Nintendo logo on the screen
        //! at the top. This image is then scrolled until it is in the middle of
        //! the screen. Two musical notes are then played on the internal
        //! speaker. Again, the cartridge locations $104 to $133 are read but this
        //! time they are compared with a table in the internal rom.  If any byte
        //! fails to compare, then the GB stops comparing bytes and simply halts
        //! all operations.
        char *bootRom = {}; //!< Boot rom and DRM check.

        Bus::Bus() {
                cart = nullptr;
                cpu = nullptr;

                // RAM starts at 0xC000
                memory = new uint8_t[8 * 1024];
                // Video memory starts at 0x8000
                videoMemory = new uint8_t[8 * 1024];

                // Alternatively, 0x8800-0x97FF
                bgTileMap = &videoMemory[0x8000]; //!< Up to 0x8FFF

                // Set boot state.
                write(RegBOOT, 0x0);

                memRegisters.boot = 0x0;
        }

        Bus::~Bus() {
                delete[] videoMemory;
                delete[] memory;
        }

        void Bus::write(uint16_t ptr, uint8_t value) {
                if (cart != nullptr && cart->write(ptr, value)) {
                        return;
                }

                switch (ptr) {
                        case AddrMemRegEnum::RegBOOT:
                                memRegisters.boot = value;
                                break;

                        // Before a transfer, it holds the next byte that will
                        // go out.
                        // During a transfer, it has a blend of the outgoing and
                        // incoming bytes. Each cycle, the leftmost bit is
                        // shifted out (and over the wire) and the incoming bit
                        // is shifted in from the other side.
                        case AddrSerialEnum::SerialTransfer:
                                memRegisters.sb = value;
                                break;

                        // bit 7: transfer start flag
                        //        0: no transfer in progress or requested
                        //        1: transfer in progress or requested
                        // bit 1: shift clock
                        //        0: external clock
                        //        1: internal clock
                        case AddrSerialEnum::SerialControl:
                                memRegisters.sc = value;
                                break;

                        default:
                                break;
                }
        }

        uint8_t Bus::read(uint16_t ptr) {
                uint8_t value;
                if (cart != nullptr && cart->read(ptr, value)) {
                        return value;
                }

                switch (ptr) {
                        case AddrMemRegEnum::RegBOOT:
                                return memRegisters.boot;

                        case AddrSerialEnum::SerialTransfer:
                                return memRegisters.sb;

                        case AddrSerialEnum::SerialControl:
                                return memRegisters.sc;
                }

                return 0;
        }

        void Bus::attach(Cartridge *cart) {
                cpu->registers.r16.AF = 0x0001;
                cpu->registers.r16.BC = 0x0013;
                cpu->registers.r16.DE = 0x00D8;
                cpu->registers.r16.HL = 0x014D;
                cpu->SP = 0xFFFE;
                this->cart = cart;
        }

        void Bus::attach(Cpu *cpu) {
                this->cpu = cpu;
                cpu->attach(this);
        }

        //! \see https://gbdev.io/pandocs/#power-up-sequence
        void Bus::reset() {
                cpu->reset();
                cpu->registers.r16.AF = 0x01B0;
                cpu->registers.r16.BC = 0x0013;
                cpu->registers.r16.DE = 0x00D8;
                cpu->registers.r16.HL = 0x014D;
                cpu->SP = 0xFFFE;

                write(0xFF05, 0x00); // TIMA
                write(0xFF06, 0x00); // TMA
                write(0xFF07, 0x00); // TAC
                write(0xFF10, 0x80); // NR10
                write(0xFF11, 0xBF); // NR11
                write(0xFF12, 0xF3); // NR12
                write(0xFF14, 0xBF); // NR14
                write(0xFF16, 0x3F); // NR21
                write(0xFF17, 0x00); // NR22
                write(0xFF19, 0xBF); // NR24
                write(0xFF1A, 0x7F); // NR30
                write(0xFF1B, 0xFF); // NR31
                write(0xFF1C, 0x9F); // NR32
                write(0xFF1E, 0xBF); // NR34
                write(0xFF20, 0xFF); // NR41
                write(0xFF21, 0x00); // NR42
                write(0xFF22, 0x00); // NR43
                write(0xFF23, 0xBF); // NR44
                write(0xFF24, 0x77); // NR50
                write(0xFF25, 0xF3); // NR51
                write(0xFF26, 0xF1); // NR52: $F1-GB, $F0-SGB
                write(0xFF40, 0x91); // LCDC
                write(0xFF42, 0x00); // SCY
                write(0xFF43, 0x00); // SCX
                write(0xFF45, 0x00); // LYC
                write(0xFF47, 0xFC); // BGP
                write(0xFF48, 0xFF); // OBP0
                write(0xFF49, 0xFF); // OBP1
                write(0xFF4A, 0x00); // WY
                write(0xFF4B, 0x00); // WX
                write(0xFFFF, 0x00); // IE
        }

} // namespace gs
