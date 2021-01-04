/******************************************************************************
 * File: video.hpp
 * Created: 2021-01-04
 * Updated: 2021-01-04
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2021, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
//! \file video.hpp
#ifndef VIDEO_VERSION
#define VIDEO_VERSION "0.1.0" //!< include guard

#include <cstdint>

namespace gs {

        /*
          Background Tile Map:
          32 rows of 32bytes
          32 x 32 tiles
          each byte is the number of a tile to show
          LCDC register specifies which Tile Data Table is in use.

          Tile Data Tables:
          $8000 - $8FFF Foreground (Sprites)
           > 0 - 255
          $8800 - $97FF Background
           > -128 - 127 (0 is $9000)

          Object Attribute Memory (OAM):
          $FE00 - $FE9F
          40 4-byte blocks, each representing a sprite.
        */


        /*
#
FF40 - LCD Control Register

LCDC is the main LCD Control register. Its bits toggle what elements are displayed on the screen, and how.
Bit 	Name 	Usage notes
7 	LCD Display Enable 	0=Off, 1=On
6 	Window Tile Map Display Select 	0=9800-9BFF, 1=9C00-9FFF
5 	Window Display Enable 	0=Off, 1=On
4 	BG & Window Tile Data Select 	0=8800-97FF, 1=8000-8FFF
3 	BG Tile Map Display Select 	0=9800-9BFF, 1=9C00-9FFF
2 	OBJ (Sprite) Size 	0=Off, 1=On
1 	OBJ (Sprite) Display Enable 	0=Off, 1=On
0 	BG/Window Display/Priority
         */

        // $8000 - $A000 is VRAM
        class Video {
        public:
                // $8000 - $8FFF sprite data table
                // $8800 - $97FF bg data table
                // $9800 - $98FF tile map 1
                // $9C99 - $9FFF tile map 2
                uint8_t vram[8 * 1024];
                uint8_t oam[40 * 4]; // $FE00 - $FE9F

                uint8_t lcdc;
                uint8_t scrollx;
                uint8_t scrolly;
                uint8_t wndposx;
                uint8_t wndposy;

                bool write(uint16_t addr, uint8_t value);
                bool read(uint16_t addr, uint8_t &value);
        private:
                // TODO: X window stuff here.
        };

} // namespace gs

#endif // VIDEO_VERSION
