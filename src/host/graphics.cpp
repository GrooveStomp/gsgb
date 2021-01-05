/******************************************************************************
 * File: host/graphics.cpp
 * Created: 2021-01-04
 * Updated: 2021-01-04
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2021, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
//! \file host/graphics.cpp
#include <string.h> // memset, memmove
#include <cstdio> // fprintf
#include <SDL2/SDL.h>

#include "graphics.hpp"
#include "sprite.hpp"

#define STB_TRUETYPE_IMPLEMENTATION
#include "external/stb_truetype.h"

namespace gs {

        Graphics::Graphics(const char *title, uint32_t width, uint32_t height) {
                width = width;
                height = height;
                fontInfo = new stbtt_fontinfo;
                if (nullptr == fontInfo) {
                        fprintf(stderr, "Couldn't create font info\n");
                        // TODO: Error?!?
                }

                SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS);

                window = SDL_CreateWindow(
                        title,
                        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                        width, height,
                        SDL_WINDOW_SHOWN
                        );
                if (NULL == window) {
                        fprintf(stderr, "Couldn't create window: %s\n", SDL_GetError());
                        // TODO: Error?!?
                }

                renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
                if (NULL == renderer) {
                        fprintf(stderr, "Couldn't create renderer: %s\n", SDL_GetError());
                        // TODO: Error?!?
                }

                texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
                if (NULL == texture) {
                        fprintf(stderr, "Couldn't create texture: %s\n", SDL_GetError());
                        // TODO: Error?!?
                }
        }

        Graphics::~Graphics() {
                if (nullptr != fontInfo) {
                        delete fontInfo;
                }

                if (nullptr != texture) {
                        SDL_DestroyTexture(texture);
                }

                if (nullptr != renderer) {
                        SDL_DestroyRenderer(renderer);
                }

                if (nullptr != window) {
                        SDL_DestroyWindow(window);
                }

                SDL_Quit();
        }

        void Graphics::begin() {
                SDL_LockTexture(texture, NULL, (void **)&pixels, &bytesPerRow);
        }

        void Graphics::end() {
                SDL_UnlockTexture(texture);
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture, 0, 0);
                SDL_RenderPresent(renderer);
        }

        void Graphics::clear(uint32_t color) {
                for (uint32_t i = 0; i < bytesPerRow * height; i+=4) {
                        uint32_t *pixel = (uint32_t *)&pixels[i];
                        *pixel = color;
                }
        }

        //! \brief converts a 0-255 int value to a float value
        //!
        //! \param[in] value int in the range 0-255
        //! \return float in the range [0,1]
        float ByteToFloat(uint8_t value) {
                return (float)value / 255.0;
        }

        //! \brief compute the ADD OVERLAY alpha blend of 1 over 2
        //!
        //! c1 and a1 come from object 1, which is overlaid upon object 2's attributes:
        //! c2, a2
        //!
        //! \param[in] c1 R,G or B component from object 1 in [0,1]
        //! \param[in] a1 Alpha component of object 1 in [0,1]
        //! \param[in] c2 Corresponding R,G or B component from object 2 in [0,1]
        //! \param[in] a2 Alpha component of object 2 in [0,1]
        //! \return resulting R,G or B component in [0,1]
        //!
        //! \see https://en.wikipedia.org/wiki/Alpha_compositing#Description
        //! \see AlphaBlendPixels()
        float AlphaBlendComponent(float c1, float a1, float c2, float a2) {
                float dividend = (c1 * a1) + ((c2 * a2) * (1.0f - a1));
                float divisor = a1 + (a2 * (1.0f - a1));
                return dividend / divisor;
        }

        //! \brief compute the ADD OVERLAY alpha blend of top over bottom
        //!
        //! top is a 32-bit (R|G|B|A) color and bottom is also a 32-bit (R|G|B|A)
        //! color. We compute the ADD OVERLAY operation of top overlayed onto bottom.
        //!
        //! To achieve this, we splite each 32-bit integer into its corresponding
        //! R,G,B,A components and then invoke AlphaBlendComponent() on each R,G,B
        //! component.
        //!
        //! \param[in] top 32-bit color (R|G|B|A) to overlay
        //! \param[in] bottom 32-bit color (R|G|B|A) to be overlaid upon
        //! \return 32-bit color (R|G|B|A) result of the ADD OVERLAY operation
        //!
        //! \see https://en.wikipedia.org/wiki/Alpha_compositing#Description
        //! \see AlphaBlendComponent()
        uint32_t AlphaBlendPixels(uint32_t top, uint32_t bottom) {
                float bottomRed   = ByteToFloat((bottom >> 24) & 0xFF);
                float bottomGreen = ByteToFloat((bottom >> 16) & 0xFF);
                float bottomBlue  = ByteToFloat((bottom >>  8) & 0xFF);
                float bottomAlpha = ByteToFloat((bottom >>  0) & 0xFF);

                float topRed   = ByteToFloat((top >> 24) & 0xFF);
                float topGreen = ByteToFloat((top >> 16) & 0xFF);
                float topBlue  = ByteToFloat((top >>  8) & 0xFF);
                float topAlpha = ByteToFloat((top >>  0) & 0xFF);

                float newRed   = AlphaBlendComponent(topRed,   topAlpha, bottomRed,   bottomAlpha);
                float newGreen = AlphaBlendComponent(topGreen, topAlpha, bottomGreen, bottomAlpha);
                float newBlue  = AlphaBlendComponent(topBlue,  topAlpha, bottomBlue,  bottomAlpha);

                uint32_t newColor = (
                        ((uint8_t)(newRed   * 255.0f) << 24) |
                        ((uint8_t)(newGreen * 255.0f) << 16) |
                        ((uint8_t)(newBlue  * 255.0f) <<  8) |
                        0xFF);

                return newColor;
        }

        void Graphics::putPixel(uint32_t x, uint32_t y, uint32_t color) {
                if (x >= 0 && x < width && y >= 0 && y < height) {
                        uint8_t opacity = color & 0xFF;
                        if (0 == opacity) {
                                return;
                        }

                        uint32_t pixel = getPixel(x, y);
                        uint32_t newPixel = AlphaBlendPixels(color, pixel);

                        int y_flipped = (height - y - 1);
                        uint32_t *screen = (uint32_t *)&pixels[y_flipped * bytesPerRow + x * 4];
                        *screen = newPixel;
                }
        }

        uint32_t Graphics::getPixel(uint32_t x, uint32_t y) {
                if (x >= 0 && x < width && y >= 0 && y < height) {
                        return *(uint32_t *)&pixels[y * bytesPerRow + x * 4];
                }

                return 0x00000000;
        }

        void Graphics::drawLine(int x1, int y1, int x2, int y2, uint32_t color) {
                int dx = x2 - x1;
                int dy = y2 - y1;

                int dx1 = abs(dx);
                int dy1 = abs(dy);

                int px = 2 * dy1 - dx1;
                int py = 2 * dx1 - dy1;

                int x, y, xe, ye;

                if (dy1 <= dx1) {
                        // Line is horizontal.
                        if (dx >= 0) {
                                x = x1;
                                y = y1;
                                xe = x2;
                        } else {
                                x = x2;
                                y = y2;
                                xe = x1;
                        }

                        putPixel(x, y, color);

                        for (int i = 0; x < xe; i++) {
                                x = x + 1;
                                if (px < 0) {
                                        px = px + 2 * dy1;
                                } else {
                                        if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                                                y = y + 1;
                                        } else {
                                                y = y - 1;
                                        }
                                        px = px + 2 * (dy1 - dx1);
                                }
                                putPixel(x, y, color);
                        }
                } else {
                        // Line is vertical.
                        if (dy >= 0) {
                                x = x1;
                                y = y1;
                                ye = y2;
                        } else {
                                x = x2;
                                y = y2;
                                ye = y1;
                        }

                        putPixel(x, y, color);

                        for (int i = 0; y < ye; i++) {
                                y = y + 1;
                                if (py <= 0) {
                                        py = py + 2 * dx1;
                                } else {
                                        if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                                                x = x + 1;
                                        } else {
                                                x = x - 1;
                                        }
                                        py = py + 2 * (dx1 - dy1);
                                }
                                putPixel(x, y, color);
                        }
                }
        }

        void Graphics::initText(unsigned char *ttfBuffer) {
                if (NULL == ttfBuffer) {
                        return;
                }

                stbtt_InitFont(fontInfo, (const unsigned char *)ttfBuffer, stbtt_GetFontOffsetForIndex(ttfBuffer, 0));
        }

        void Graphics::drawText(int x, int y, char *string, int fontHeight, uint32_t color) {
                int len = strlen(string);
                auto advanceWidths = new int[len + 1];
                advanceWidths[0] = 0;

                float scale = stbtt_ScaleForPixelHeight(fontInfo, fontHeight);

                // Render the text.
                int xOffset = 0;
                for (int c = 0; c < len; c++) {
                        stbtt_GetCodepointHMetrics(fontInfo, string[c], &advanceWidths[c+1], NULL);

                        int x0, x1, y0, y1;
                        stbtt_GetCodepointBitmapBox(fontInfo, string[c], scale, scale, &x0, &y0, &x1, &y1);

                        int width, height;
                        uint8_t *bitmap = stbtt_GetCodepointBitmap(fontInfo, scale, scale, string[c], &width, &height, 0, 0);

                        xOffset += (int)((float)(advanceWidths[c]) * scale);

                        // Draw the character.
                        for (int h = 0; h < height; h++) {
                                for (int w = 0; w < width; w++) {
                                        uint8_t opacity = bitmap[h * width + w];
                                        color = (color & 0xFFFFFF00) | opacity;
                                        putPixel(x + xOffset + (w + x0), y - (h + y0), color);
                                }
                        }
                        free(bitmap);
                }
                delete[] advanceWidths;
        }

        void Graphics::drawSprite(int x, int y, Sprite *sprite, int scale) {
                if (nullptr == sprite) return;

                if (scale > 1) {
                        for (uint32_t i = 0; i < sprite->width; i++) {
                                for (uint32_t j = 0; j < sprite->height; j++) {
                                        int j1 = sprite->height - j - 1;
                                        for (int is = 0; is < scale; is++)
                                                for (int js = 0; js < scale; js++) {
                                                        uint32_t color = sprite->GetPixel(i, j);
                                                        int xp = x + (i * scale) + is;
                                                        int yp = y + (j1 * scale) + js;
                                                        putPixel(xp, yp, color);
                                                }
                                }
                        }
                } else {
                        for (uint32_t i = 0; i < sprite->width; i++)
                                for (uint32_t j = 0; j < sprite->height; j++) {
                                        int yp = sprite->height - j - 1;
                                        uint32_t color = sprite->GetPixel(i, j);
                                        putPixel(x + i, y + yp, color);
                                }
                }
        }

        void Graphics::drawFilledRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
                uint32_t x2 = x + w;
                uint32_t y2 = y + h;

                if (x >= width) x = width;
                if (y >= height) y = height;

                if (x2 >= width) x2 = width;
                if (y2 >= height) y2 = height;

                for (uint32_t i = x; i < x2; i++)
                        for (uint32_t j = y; j < y2; j++)
                                putPixel(i, j, color);
        }

        void Graphics::drawRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
                drawLine(x, y, x + w, y, color);
                drawLine(x + w, y, x + w, y + h, color);
                drawLine(x + w, y + h, x, y + h, color);
                drawLine(x, y + h, x, y, color);
        }

} // namespace gs
