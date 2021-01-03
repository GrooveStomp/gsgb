/******************************************************************************
 * File: mbc.cpp
 * Created: 2020-12-28
 * Updated: 2020-12-30
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2021, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
//! \file mbc.cpp
#include <cassert>
#include <cstdio>

#include "mbc.hpp"

namespace gs {
        static const uint32_t BANK_SIZE = 16 * 1024;

        /**********************************************************************
         * Mbc
         **********************************************************************/
        Mbc::~Mbc() {}

        /**********************************************************************
         * MbcNone
         **********************************************************************/
        MbcNone::MbcNone(uint32_t ram_size) {
                rom_size = static_cast<uint32_t>(32 * 1024);

                if (ram_size > 0 && ram_size <= (8 * 1024)) {
                        ram = new uint8_t[ram_size];
                        this->ram_size = ram_size;
                } else {
                        ram = nullptr;
                        this->ram_size = 0;
                }
        }

        MbcNone::~MbcNone() {
                if (ram != nullptr) {
                        delete[] ram;
                }
        }

        bool MbcNone::write(uint16_t addr, uint8_t value) {
                if (addr >= 0x0000 && addr <= 0x7FFF) {
                        rom[addr] = value;
                        return true;
                } else if (addr >= 0xA000 && addr <= 0xBFFF) {
                        assert(addr <= ram_size + 0xA000);
                        uint16_t addr2 = static_cast<uint16_t>(addr - 0xA000);
                        ram[addr2] = value;
                        return true;
                }

                return false;
        }

        bool MbcNone::read(uint16_t addr, uint8_t &value) {
                if (addr >= 0x0000 && addr <= 0x7FFF) {
                        value = rom[addr];
                        return true;
                } else if (addr >= 0xA000 && addr <= 0xBFFF) {
                        assert(addr <= ram_size + 0xA000);
                        uint16_t addr2 = static_cast<uint16_t>(addr - 0xA000);
                        value = ram[addr2];
                        return true;
                }

                return false;
        }

        void MbcNone::loadRom(uint8_t *data) {
                for (uint32_t i = 0; i < rom_size; ++i) {
                        rom[i] = data[i];
                }
        }

        /**********************************************************************
         * Mbc1
         **********************************************************************/
        Mbc1::Mbc1(uint32_t rom_size, uint32_t ram_size) {
                assert((rom_size % BANK_SIZE) == 0); // rom must be multiple of 16kb.
                assert(rom_size >= BANK_SIZE); // rom must be at least one 16kb bank.
                assert(rom_size <= (2 * 1024 * 1024)); // rom must be at most 2MB.
                assert(rom_size > 0);

                // The RAM can only be 0kb, 2kb, 8kb or 32kb.
                assert(ram_size == 0 || ram_size == (2 * 1024) || ram_size == (8 * 1024) || ram_size == (32 * 1024));

                this->rom = new uint8_t[rom_size];
                this->rom_size = rom_size;

                this->ram = new uint8_t[ram_size];
                this->ram_size = ram_size;

                ram_enabled = false;
                mem_mode = MemoryModeRom;
                rom_bank = 0;
                ram_bank = 0;
        }

        Mbc1::~Mbc1() {
                delete[] rom;
                delete[] ram;
        }

        bool Mbc1::write(uint16_t addr, uint8_t value) {
                // This address range is read-only; so special circuitry exists
                // such that when we write 0x0A to it, we enable RAM.
                if (addr >= 0x0000 && addr <= 0x1FFF) {
                        if ((value & 0x00FF) == 0x000A) {
                                ram_enabled = true;
                        } else {
                                ram_enabled = false;
                        }
                        return true;
                }

                // Writing to this address space selects the lower 5 bits of the
                // ROM Bank Number (in range 01-1Fh). When 00h is written, the
                // MBC translates that to bank 01h also. That doesn't harm so
                // far, because ROM Bank 00h can be always directly accessed by
                // reading from 0000-3FFF. But (when using the register below to
                // specify the upper ROM Bank bits), the same happens for Bank
                // 20h, 40h, and 60h. Any attempt to address these ROM Banks
                // will select Bank 21h, 41h, and 61h instead.
                else if (addr >= 0x2000 && addr <= 0x3FFF) {
                        // 0x001F is lower 5 bits.
                        uint8_t bank_num = value & 0x1F;
                        if (bank_num == 0x0000) {
                                bank_num = 0x0001;
                        }

                        // 0xE0 is upper 3 bits.
                        rom_bank = rom_bank & 0xE0;
                        rom_bank = rom_bank | bank_num;
                        return true;
                }

                // This 2bit register can be used to select a RAM Bank in range
                // from 00-03h, or to specify the upper two bits (Bit 5-6) of
                // the ROM Bank number, depending on the current ROM/RAM
                // Mode. (See below.)
                else if (addr >= 0x4000 && addr <= 0x5FFF) {
                        switch (mem_mode) {
                                case MemoryModeRom: {
                                        // Left shift 5 bits.
                                        uint8_t bank_num = value << 5;
                                        // Drop the first 3 bits.
                                        rom_bank = rom_bank & 0x1F;
                                        // Add the new 3 front bits.
                                        rom_bank = rom_bank | bank_num;
                                        break;
                                }

                                case MemoryModeRam: {
                                        ram_bank = value;
                                        break;
                                }
                        }
                        return true;
                }

                // Toggle between RAM and ROM modes.
                else if (addr >= 0x6000 && addr <= 0x7FFF) {
                        if ((value & 0x00FF) == 0x0000) {
                                mem_mode = MemoryModeRom;
                        } else if ((value &0x00FF) == 0x0001) {
                                mem_mode = MemoryModeRam;
                        }
                        return true;
                }

                // RAM
                else if (addr >= 0xA000 && addr <= 0xBFFF && ram_enabled) {
                        assert(ram_bank >= 0 && ram_bank <= 3);

                        uint16_t addr2 = static_cast<uint16_t>(addr - 0xA000);
                        addr2 = ram_bank * (8 * 1024); // 0, 8, 16 or 24kb offset.

                        ram[addr2] = value;
                        return true;
                }

                return false;
        }

        bool Mbc1::read(uint16_t addr, uint8_t &value) {
                // Read from the first ROM bank - always available.
                if (addr >= 0x0000 && addr <= 0x3FFF) {
                        value = rom[addr];
                        return true;
                }

                // Read the currently mapped ROM bank.
                else if (addr >= 0x4000 && addr <= 0x7FFF) {
                        uint16_t addr2 = static_cast<uint16_t>(addr - 0x4000);
                        addr2 = rom_bank * (16 * 1024); // Some multiple of 16kb offset.
                        value = rom[addr2];
                        return true;
                }

                // RAM
                else if (addr >= 0xA000 && addr <= 0xBFFF && ram_enabled) {
                        assert(ram_bank >= 0 && ram_bank <= 3);

                        uint16_t addr2 = static_cast<uint16_t>(addr - 0xA000);
                        addr2 = ram_bank * (8 * 1024); // 0, 8, 16 or 24kb offset.

                        value = ram[addr2];
                        return true;
                }

                return false;
        }

        void Mbc1::loadRom(uint8_t *data) {
                for (uint32_t i = 0; i < rom_size; ++i) {
                        rom[i] = data[i];
                }

                printf("-- Begin ROM Dump\n");
                uint32_t width = 16;
                uint32_t r = 0;
                printf("     ");
                for (uint32_t i = 0; i < width; i++) {
                        printf("%02X ", i);
                }
                printf("\n");

                bool running = true;
                while (running) {
                        printf("%04X ", r * width);
                        for (uint32_t c = 0; c < width; c++) {
                                if ((r * width + c) >= rom_size) {
                                        running = false;
                                        break;
                                }
                                printf("%02X ", rom[r * width + c]);
                        }
                        r++;
                        printf("\n");
                }
                printf("-- End ROM Dump\n");
        }

} // namespace gs
