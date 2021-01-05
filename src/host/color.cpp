/******************************************************************************
 * File: host/color.cpp
 * Created: 2021-01-04
 * Updated: 2021-01-04
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2021, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
//! \file host/color.cpp
#include "color.hpp"

namespace gs {

        Color ColorWhite(0xFFFFFFFF);
        Color ColorBlack(0x000000FF);
        Color ColorRed(0xFF0000FF);
        Color ColorGreen(0x00FF00FF);
        Color ColorBlue(0x0000FFFF);
        Color ColorPurple(0x7F00FFFF);
        Color ColorYellow(0xFFFF00FF);
        Color ColorCyan(0x00FFFFFF);
        Color ColorPink(0xFF00FFFF);
        Color ColorGray(0x888888FF);

        void Color::SetInt(char component, uint32_t value) {
                uint32_t pos = 0;
                switch (component) {
                        case 'r':
                                pos = 3;
                                break;
                        case 'g':
                                pos = 2;
                                break;
                        case 'b':
                                pos = 1;
                                break;
                        case 'a':
                                pos = 0;
                                break;
                        default:
                                pos = 0;
                }

                uint32_t shift = pos * 8;

                uint32_t rgba2 = rgba & ~(0xFF << shift);
                rgba = rgba2 | (value << shift);
        }

        void Color::SetFloat(char component, float value) {
                uint32_t intVal = (uint32_t)(value * 255.0f);
                SetInt(component, intVal);
        }

        Color::Color(float r, float g, float b, float a) {
                SetFloat('r', r);
                SetFloat('g', g);
                SetFloat('b', b);
                SetFloat('a', a);
        }

        uint32_t Color::GetInt(char component) {
                uint32_t pos = 0;
                switch (component) {
                        case 'r':
                                pos = 3;
                                break;
                        case 'g':
                                pos = 2;
                                break;
                        case 'b':
                                pos = 1;
                                break;
                        case 'a':
                                pos = 0;
                                break;
                        default:
                                pos = 0;
                }

                uint32_t shift = pos * 8;
                return (this->rgba >> shift) & 0xFF;
        }

        float Color::GetFloat(char component) {
                uint32_t value = GetInt(component);
                return (float)value / 255.0f;
        }

        Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
                SetInt('r', r);
                SetInt('g', g);
                SetInt('b', b);
                SetInt('a', a);
        }

        Color::Color(uint32_t rgba) {
                this->rgba = rgba;
        }

} // namespace gs
