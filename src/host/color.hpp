/******************************************************************************
 * File: host/color.hpp
 * Created: 2021-01-04
 * Updated: 2021-01-04
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2021, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
//! \file host/color.hpp
//! This interface attempts to provide an intuitive wrapper around "raw"
//! uint32_teger colors.
//!
//! An uint32_teger color is packed 32-bit value consisting of 4 pixel
//! elements: RGBA.  These elements are stored as written: RGBA, or, visually
//! mapped as hex symbols: RRGGBBAA.
#include <cstdint>

#ifndef COLOR_VERSION
#define COLOR_VERSION "0.3-gsgb" //!< include guard

namespace gs {

        //! RGBA color quad
        class Color {
        public:
                uint32_t rgba;

                //! \brief Initialize a new color with individual R, G, B, A components as floats.
                //!
                //! \param r Red component from 0 to 1
                //! \param g Green componenet from 0 to 1
                //! \param b Blue component from 0 to 1
                //! \param a Alpha component, from 0 to 1
                //! \return resulting color object
                Color(float r, float g, float b, float a);

                //! \brief Initialize a new color with individual R, G, B, A components as ints.
                //!
                //! \param r Red component from 0 to 255
                //! \param g Green componenet from 0 to 255
                //! \param b Blue component from 0 to 255
                //! \param a Alpha component, from 0 to 255
                //! \return resulting color object
                Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

                //! \brief Initialize a new color with an rgba component from a color
                //!
                //! \param rgba 32-bit r,g,b,a packed int.
                //! \return resulting color object
                Color(uint32_t rgba);

                //! \brief Get the color component
                //!
                //! The component is returned as the raw integer value, in the range [0,255]
                //!
                //! \param color color object to read
                //! \param component 'r', 'g', 'b' or 'a' exclusively.
                //! \return value of the color component
                uint32_t GetInt(char component);

                //! \brief Get the color component
                //!
                //! The component is returned as a float in the range [0.0,1.0]
                //!
                //! \param color color object to read
                //! \param component 'r', 'g', 'b' or 'a' exclusively.
                //! \return value of the color component
                float GetFloat(char component);

                //! \brief Set the color component
                //!
                //! The value should be an integer in the range [0,255]
                //!
                //! \param color pointer to the color object to write
                //! \param component 'r', 'g', 'b' or 'a' exclusively.
                //! \param value value of the color component to set
                void SetInt(char component, uint32_t value);

                //! \brief Set the color component
                //!
                //! The value should be a float in the range [0.0,1.0]
                //!
                //! \param color pointer to the color object to write
                //! \param component 'r', 'g', 'b' or 'a' exclusively.
                //! \param value value of the color component to set
                void SetFloat(char component, float value);
        private:
        };

        extern Color ColorWhite;
        extern Color ColorBlack;
        extern Color ColorRed;
        extern Color ColorGreen;
        extern Color ColorBlue;
        extern Color ColorPurple;
        extern Color ColorYellow;
        extern Color ColorCyan;
        extern Color ColorPink;
        extern Color ColorGray;

} // namespace gs

#endif // COLOR_VERSION
