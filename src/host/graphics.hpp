/******************************************************************************
 * File: host/graphics.hpp
 * Created: 2021-01-04
 * Updated: 2021-01-04
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2021, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
//! \file host/graphics.h
//! Drawing interface to the operating system.
#ifndef GRAPHICS_VERSION
#define GRAPHICS_VERSION "0.3-gsgb" //!< include guard and version info

#include <cstdint>

struct stbtt_fontinfo;
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

namespace gs {

        class Sprite;

        class Graphics {
        public:
                Graphics(const char *title, uint32_t width, uint32_t height);
                ~Graphics();

                //! \brief Initializes the graphics subsystem for drawing routines
                //!
                //! Internally locks streaming texture for direct manipulation.
                void begin();

                //! \brief Prepares the graphics subsystem for presentation, then presents
                //!
                //! Internally unlocks streaming texture then calls presentation routines.
                void end();

                //! \brief Sets all pixels in the display buffer to the given color
                //!
                //! \param[in, out] graphics
                //! \param[in] color 32-bit color (R|G|B|A) to clear the screen to
                void clear(uint32_t color);

                //! \brief Put a pixel into the display buffer
                //!
                //! \param[in] x display buffer x coordinate
                //! \param[in] y display buffer y coordinate
                //! \param[in] color 32-bit color (R|G|B|A) to set the pixel to
                void putPixel(uint32_t x, uint32_t y, uint32_t color);

                //! \brief Get the color fo the pixel at (x,y) in the display buffer
                //!
                //! \param[in] x display buffer x coordinate
                //! \param[in] y display buffer y coordinate
                //! \return 32-bit color (R|G|B|A) of the target pixel
                uint32_t getPixel(uint32_t x, uint32_t y);

                //! \brief Initialize graphics state for text rendering
                //!
                //! \param[in] ttfBuffer the contents of a truetype font file loaded into memory
                void initText(uint8_t *ttfBuffer);

                //! \brief Draw text to the display buffer
                //!
                //! \param[in] x leftmost position to start drawing text from
                //! \param[in] y position from the bottom of the dispaly buffer to start drawing
                //! text from
                //! \param[in] string text to render
                //! \param[in] fontHeight sets the height of the font in pixels
                //! \param[in] color 32-bit color (R|G|B|A) to render text in
                void drawText(int x, int y, char *string, int fontHeight, uint32_t color);

                //! \brief Draws a line from (x1,y1) to (x2,y2)
                //!
                //! \param[in] x1 horizontal position of the line start
                //! \param[in] y1 vertical position of the line start
                //! \param[in] x2 horizontal position of the line end
                //! \param[in] y2 vertical position of the line end
                //! \param[in] color 32-bit color (R|G|B|A) to render line with
                void drawLine(int x1, int y1, int x2, int y2, uint32_t color);

                //! \brief Draws a sprite starting with lower left corner at (x,y)
                //!
                //! \param[in] x x coordinate to start drawing from
                //! \param[in] y y coordinate to start drawing from
                //! \param[in] sprite sprite to draw
                //! \param[in] scale integer scale of sprite
                void drawSprite(int x, int y, Sprite *sprite, int scale);

                //! \brief Draws a filled rectangle at (x,y) of width w and height h
                //!
                //! \param[in] x lower left corner x coordinate
                //! \param[in] y lower left corner y coordinate
                //! \param[in] w width of the rectangle in pixels
                //! \param[in] h height of the rectangle in pixels
                //! \param[in] color 32-bit color (R|G|B|A) to render line with
                void drawFilledRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);

                //! \brief Draws a rectangle outline at (x,y) of width w and height h
                //!
                //! \param[in] x lower left corner x coordinate
                //! \param[in] y lower left corner y coordinate
                //! \param[in] w width of the rectangle in pixels
                //! \param[in] h height of the rectangle in pixels
                //! \param[in] color 32-bit color (R|G|B|A) to render line with
                void drawRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);

                SDL_Window *window;
                SDL_Renderer *renderer;
                SDL_Texture *texture;
                uint32_t width;
                uint32_t height;

                uint8_t *pixels;
                int bytesPerRow; // Must be int for SDL API.
                stbtt_fontinfo *fontInfo;
        private:
        };

} // namespace gs

#endif // GRAPHICS_VERSION
