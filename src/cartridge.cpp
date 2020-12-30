/******************************************************************************
 * File: cartridge.cpp
 * Created: 2019-09-24
 * Updated: 2020-12-30
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2020, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
//! \file cartridge.cpp

#include <iostream> // TODO: Remove me
#include <iomanip> // TODO: Remove me
#include <cmath>

#include "cartridge.hpp"
#include "mbc.hpp"

namespace gs {

        enum CartRamSizeEnum {
                CartRam0Kb = 0x00,   // None
                CartRam2Kb = 0x01,   // 2 KBytes
                CartRam8Kb = 0x02,   // 8 KBytes
                CartRam32Kb = 0x03,  // 32 KBytes (4 banks of 8KBytes each)
                CartRam128Kb = 0x04, // 128 KBytes (16 banks of 8KBytes each)
                CartRam64KB = 0x05,  // 64 KBytes (8 banks of 8KBytes each)
        };

        enum CartTypeEnum {
                CartTypeRomOnly = 0x00,
                CartTypeMbc1 = 0x01,
                CartTypeMbc1Ram = 0x02,
                CartTypeMbc1RamBattery = 0x03,
                CartTypeMbc2 = 0x05,
                CartTypeMbc2Battery = 0x06,
                CartTypeRomRam = 0x08,
                CartTypeRomRamBattery = 0x09,
                CartTypeMmm01 = 0x0B,
                CartTypeMmm01Ram = 0x0C,
                CartTypeMmm01RamBattery = 0x0D,
                CartTypeMbc3TimerBattery = 0x0F,
                CartTypeMbc3TimerRamBattery = 0x10,
                CartTypeMbc3 = 0x11,
                CartTypeMbc3Ram = 0x12,
                CartTypeMbc3RamBattery = 0x13,
                CartTypeMbc5 = 0x19,
                CartTypeMbc5Ram = 0x1A,
                CartTypeMbc5RamBattery = 0x1B,
                CartTypeMbc5Rumble = 0x1C,
                CartTypeMbc5RumbleRam = 0x1D,
                CartTypeMbc5RumbleRamBattery = 0x1E,
                CartTypeMbc6 = 0x20,
                CartTypeMbc7SENSORRumbleRamBattery = 0x22,
                CartTypePocketCamera = 0xFC,
                CartTypeBandaiTama5 = 0xFD,
                CartTypeHuC3 = 0xFE,
                CartTypeHuC1RamBattery = 0xFF,
        };

        //! \see https://gbdev.io/pandocs/#the-cartridge-header
#pragma pack(push, 1)
        struct CartHeader {
                uint8_t entrypoint[4];
                uint8_t logo[48];     // nintendo logo
                char    title[16];    // manufacturer code (unused in GB)
                                      // cgb flag (unused in gb)
                uint16_t nl_code;     // 0x0144-0x0145 new licensee code
                uint8_t  sgb_flag;    // 0x0146 sgb flag
                uint8_t  cart_type;   // 0x0147 mbc/cartridge type
                uint8_t  rom_size;    // 0x0148 rom size
                uint8_t  ram_size;    // 0x0149 ram size
                uint8_t  destination; // 0x014A destination (region)
                uint8_t  ol_code;     // 0x014B old licensee code
                uint8_t  version;     // 0x014C mask rom version number
                uint8_t  h_check;     // 0x014D header checksum
                uint16_t g_check;     // 0x014E-0x014F global checksum (unused)

                friend std::ostream& operator<<(std::ostream& out, const CartHeader& header) {
                        using namespace std;
                        out << "title:       " << header.title << endl;
                        out << "nl_code:     0x" << setw(2) << setfill('0') << uppercase << hex << static_cast<uint16_t>(header.nl_code) << endl;
                        out << "sgb_flag:    0x" << setw(2) << setfill('0') << uppercase << hex << static_cast<uint16_t>(header.sgb_flag) << endl;
                        out << "cart_type:   0x" << setw(2) << setfill('0') << uppercase << hex << static_cast<uint16_t>(header.cart_type) << endl;
                        out << "rom_size:    0x" << setw(2) << setfill('0') << uppercase << hex << static_cast<uint16_t>(header.rom_size) << endl;
                        out << "ram_size:    0x" << setw(2) << setfill('0') << uppercase << hex << static_cast<uint16_t>(header.ram_size) << endl;
                        out << "destination: 0x" << setw(2) << setfill('0') << uppercase << hex << static_cast<uint16_t>(header.destination) << endl;
                        out << "ol_code:     0x" << setw(2) << setfill('0') << uppercase << hex << static_cast<uint16_t>(header.ol_code) << endl;
                        out << "version:     0x" << setw(2) << setfill('0') << uppercase << hex << static_cast<uint16_t>(header.version) << endl;
                        out << "h_check:     0x" << setw(2) << setfill('0') << uppercase << hex << static_cast<uint16_t>(header.h_check) << endl;
                        out << "g_check:     0x" << setw(2) << setfill('0') << uppercase << hex << static_cast<uint16_t>(header.g_check) << endl;
                        return out;
                }
        };
#pragma pack(pop)

        Cartridge::Cartridge(uint8_t *rom, unsigned int size) {
                CartHeader header = *reinterpret_cast<CartHeader*>(&rom[0x100]);

                std::cout << header;

                // Compute checksum.
                {
                        int x = 0;
                        uint8_t *ptr = reinterpret_cast<uint8_t*>(&(header.title));
                        const uint8_t header_size = 24;
                        for (int i = 0; i <= header_size; ++i) {
                                x = x - ptr[i] - 1;
                        }

                        // The checksum is the lower 8 bits of x.
                        uint8_t checksum = static_cast<uint8_t>(x);

                        if (reinterpret_cast<uint8_t>(header.h_check) != checksum) {
                                // TODO: Cleanup; proper library-style error handling.
                                using namespace std;
                                cout << "Checksum fail: expected '";
                                cout << setw(2) << setfill('0') << uppercase << hex << static_cast<uint16_t>(header.h_check);
                                cout << "' but got '";
                                cout << setw(2) << setfill('0') << uppercase << hex << static_cast<uint16_t>(checksum);
                                cout << "'" << std::endl;
                        }
                }

                uint32_t ram_size = 0;
                switch (header.ram_size) {
                        case CartRam0Kb: {
                                ram_size = 0;
                                break;
                        }
                        case CartRam2Kb: {
                                ram_size = 2 * 1024;
                                break;
                        }
                        case CartRam8Kb: {
                                ram_size = 8 * 1024;
                                break;
                        }
                        case CartRam32Kb: {
                                ram_size = 32 * 1024;
                                break;
                        }
                        case CartRam128Kb: {
                                ram_size = 128 * 1024;
                                break;
                        }
                        case CartRam64KB: {
                                ram_size = 64 * 1024;
                                break;
                        }
                }

                // Set the MBC type.
                switch (header.cart_type) {
                        case CartTypeRomOnly:
                        case CartTypeRomRam:
                        case CartTypeRomRamBattery:
                                mbc = new MbcNone(ram_size);
                                break;

                        case CartTypeMbc1:
                        case CartTypeMbc1Ram:
                        case CartTypeMbc1RamBattery:
                                // The rom size is usually specified such that:
                                // 2 ^ (rom_size + 1) == num_banks
                                // where the size of a bank is 16kb.
                                uint32_t rom_size = pow(2, header.rom_size + 1) * (16 * 1024);
                                mbc = new Mbc1(rom_size, ram_size);
                                break;

                        // TODO: More MBC/RAM/special support
                }

                mbc->loadRom(rom);
        }

        Cartridge::~Cartridge() {
                delete mbc;
        }

        void Cartridge::write(uint16_t addr, uint8_t value) {
                bool was_written = mbc->write(addr, value);
                // TODO: Handle was_written.
        }

        uint8_t Cartridge::read(uint16_t addr) {
                uint8_t value;
                bool was_read = mbc->read(addr, value);
                // TODO: Handle was_read.
                return value;
        }

} // namespace gs
