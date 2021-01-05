/******************************************************************************
 * File: host/input.cpp
 * Created: 2021-01-04
 * Updated: 2021-01-04
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2021, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
//! \file host/input.cpp
#include <cstdlib> // calloc, free
#include <cstring> // memset

#include "SDL2/SDL.h"

#include "input.hpp"

namespace gs {

        int MapToSdlEnum(InputKeyEnum e);

        Input::Input() {
                event = new SDL_Event;
                if (NULL == event) {
                        // TODO: Error handling ?!?!
                }

                keyStates = new Button[KEY_COUNT];
                if (NULL == keyStates) {
                        // TODO: Error handling ?!?!
                }

                keyPressesThisFrame = new bool[KEY_COUNT];
                if (NULL == keyPressesThisFrame) {
                        // TODO: Error handling ?!?!
                }

                keyPressesLastFrame = new bool[KEY_COUNT];
                if (NULL == keyPressesLastFrame) {
                        // TODO: Error handling ?!?!
                }

                sdlKeyStates = SDL_GetKeyboardState(NULL);
                isQuitPressed = false;
        }

        Input::~Input() {
                if (NULL != event)
                        delete event;

                if (NULL != keyStates)
                        delete[] keyStates;

                if (NULL != keyPressesThisFrame)
                        delete[] keyPressesThisFrame;

                if (NULL != keyPressesLastFrame)
                        delete[] keyPressesLastFrame;
        }

        void Input::process() {
                isQuitPressed = 0;
                SDL_PumpEvents(); // Update sdlKeyState;

                for (int i = KEY_A; i < KEY_COUNT; i++) {
                        int sdlScancode = MapToSdlEnum(InputKeyEnum(i));
                        keyPressesThisFrame[i] = sdlKeyStates[sdlScancode];

                        // The key is not in a _changed_ state - if it was held, it is
                        // still held; if it was not, it is still not.
                        if (keyPressesThisFrame[i] == keyPressesLastFrame[i]) {
                                keyStates[i].pressed = false;
                                keyStates[i].released = false;
                                continue;
                        }

                        // The key has _changed_ state.

                        // Pressed
                        if (keyPressesThisFrame[i]) {
                                keyStates[i].pressed = !keyStates[i].held;
                                keyStates[i].held = true;
                        }
                        // Released
                        else {
                                keyStates[i].released = true;
                                keyStates[i].held = false;
                        }
                        keyPressesLastFrame[i] = keyPressesThisFrame[i];
                }

                while (SDL_PollEvent(event)) {
                        switch (event->type) {
                                case SDL_QUIT:
                                        isQuitPressed = 1;
                                        break;

                                case SDL_KEYUP:
                                        break;

                                case SDL_KEYDOWN:
                                        if (event->key.keysym.sym == SDLK_ESCAPE) {
                                                isQuitPressed = 1;
                                                break;
                                        }
                                        break;
                        }
                }
        }

        bool Input::isQuitRequested() {
                return isQuitPressed;
        }

        Button Input::getKey(InputKeyEnum e) {
                return keyStates[e];
        }

        int MapToSdlEnum(InputKeyEnum e) {
                switch(e) {
                        case KEY_A:
                                return SDL_SCANCODE_A;
                        case KEY_B:
                                return SDL_SCANCODE_B;
                        case KEY_C:
                                return SDL_SCANCODE_C;
                        case KEY_D:
                                return SDL_SCANCODE_D;
                        case KEY_E:
                                return SDL_SCANCODE_E;
                        case KEY_F:
                                return SDL_SCANCODE_F;
                        case KEY_G:
                                return SDL_SCANCODE_G;
                        case KEY_H:
                                return SDL_SCANCODE_H;
                        case KEY_I:
                                return SDL_SCANCODE_I;
                        case KEY_J:
                                return SDL_SCANCODE_J;
                        case KEY_K:
                                return SDL_SCANCODE_K;
                        case KEY_L:
                                return SDL_SCANCODE_L;
                        case KEY_M:
                                return SDL_SCANCODE_M;
                        case KEY_N:
                                return SDL_SCANCODE_N;
                        case KEY_O:
                                return SDL_SCANCODE_O;
                        case KEY_P:
                                return SDL_SCANCODE_P;
                        case KEY_Q:
                                return SDL_SCANCODE_Q;
                        case KEY_R:
                                return SDL_SCANCODE_R;
                        case KEY_S:
                                return SDL_SCANCODE_S;
                        case KEY_T:
                                return SDL_SCANCODE_T;
                        case KEY_U:
                                return SDL_SCANCODE_U;
                        case KEY_V:
                                return SDL_SCANCODE_V;
                        case KEY_W:
                                return SDL_SCANCODE_W;
                        case KEY_X:
                                return SDL_SCANCODE_X;
                        case KEY_Y:
                                return SDL_SCANCODE_Y;
                        case KEY_Z:
                                return SDL_SCANCODE_Z;
                        case KEY_LEFT:
                                return SDL_SCANCODE_LEFT;
                        case KEY_RIGHT:
                                return SDL_SCANCODE_RIGHT;
                        case KEY_UP:
                                return SDL_SCANCODE_UP;
                        case KEY_DOWN:
                                return SDL_SCANCODE_DOWN;
                        case KEY_ENTER:
                                return SDL_SCANCODE_RETURN;
                        case KEY_ESC:
                                return SDL_SCANCODE_ESCAPE;
                        case KEY_SPACE:
                                return SDL_SCANCODE_SPACE;
                        case KEY_1:
                                return SDL_SCANCODE_1;
                        case KEY_2:
                                return SDL_SCANCODE_2;
                        case KEY_3:
                                return SDL_SCANCODE_3;
                        case KEY_4:
                                return SDL_SCANCODE_4;
                        case KEY_5:
                                return SDL_SCANCODE_5;
                        case KEY_6:
                                return SDL_SCANCODE_6;
                        case KEY_7:
                                return SDL_SCANCODE_7;
                        case KEY_8:
                                return SDL_SCANCODE_8;
                        case KEY_9:
                                return SDL_SCANCODE_9;
                        case KEY_0:
                                return SDL_SCANCODE_0;
                        case KEY_LSHIFT:
                                return SDL_SCANCODE_LSHIFT;
                        case KEY_RSHIFT:
                                return SDL_SCANCODE_RSHIFT;
                        case KEY_LCTRL:
                                return SDL_SCANCODE_LCTRL;
                        case KEY_RCTRL:
                                return SDL_SCANCODE_RCTRL;
                        default:
                                return -1;
                };
                return -1;
        }

        uint32_t Input::isKeyPressed(InputKeyEnum e) {
                int i = MapToSdlEnum(e);
                if (i < 0) {
                        return false;
                }

                return sdlKeyStates[i];
        }

} // namespace gs
