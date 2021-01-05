/******************************************************************************
 * File: host/sprite.cpp
 * Created: 2021-01-04
 * Updated: 2021-01-04
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2021, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
//! \file host/sprite.cpp

#include <cstdlib> // calloc, free
#include <cmath> // fmin

#include "sprite.hpp"
#include "color.hpp"

namespace gs {

        Sprite::Sprite(uint32_t width, uint32_t height) {
                pixels = new uint32_t[width * height];
                if (nullptr == pixels) {
                        // TODO: Error handling ?!?!?
                }

                this->width = width;
                this->height = height;
        }

        Sprite::~Sprite() {
                if (nullptr != pixels) {
                        delete[] pixels;
                }
        }

        void Sprite::SetPixel(uint32_t x, uint32_t y, uint32_t rgba) {
                if (x >= 0 && x < width && y >= 0 && y < height) {
                        pixels[y * width + x] = rgba;
                }
        }

        uint32_t Sprite::GetPixel(uint32_t x, uint32_t y) {
                if (x >= 0 && x < width && y >= 0 && y < height) {
                        return pixels[y * width + x];
                }
                return ColorBlack.rgba;
        }

        uint32_t Sprite::Sample(float x, float y) {
                int32_t sx = fminf(x * (float)width, width - 1);
                int32_t sy = fminf(y * (float)height, height - 1);

                if (sx >= 0 && sx < width && sy >= 0 && sy < height) {
                        return pixels[sy * width + sx];
                }

                return ColorBlack.rgba;
        }

} // namespace gs
