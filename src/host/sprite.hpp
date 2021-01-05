/******************************************************************************
 * File: host/sprite.hpp
 * Created: 2021-01-04
 * Updated: 2021-01-04
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2021, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
//! \file host/sprite.hpp
#ifndef SPRITE_VERSION
#define SPRITE_VERSION "0.2.0-gsgb"

#include <cstdint>

namespace gs {

        class Sprite {
        public:

                Sprite(uint32_t width, uint32_t height);
                ~Sprite();

                void SetPixel(uint32_t x, uint32_t y, uint32_t rgba);
                uint32_t GetPixel(uint32_t x, uint32_t y);
                uint32_t Sample(float x, float y);

                uint32_t *pixels;
                uint32_t width;
                uint32_t height;

        private:
        };

} // namespace gs

#endif // SPRITE_VERSION
