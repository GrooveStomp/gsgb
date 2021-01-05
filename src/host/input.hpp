/******************************************************************************
 * File: host/input.hpp
 * Created: 2021-01-04
 * Updated: 2021-01-04
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2021, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
//! \file host/input.hpp
//! A small interface to manage handling input separately from main().

#ifndef INPUT_VERSION
#define INPUT_VERSION "0.3-gsgb" //!< include guard and version info

#include <cstdbool>
#include <cstdint>

union SDL_Event;

namespace gs {
        enum InputKeyEnum {
                KEY_A = 0,
                KEY_B,
                KEY_C,
                KEY_D,
                KEY_E,
                KEY_F,
                KEY_G,
                KEY_H,
                KEY_I,
                KEY_J,
                KEY_K,
                KEY_L,
                KEY_M,
                KEY_N,
                KEY_O,
                KEY_P,
                KEY_Q,
                KEY_R,
                KEY_S,
                KEY_T,
                KEY_U,
                KEY_V,
                KEY_W,
                KEY_X,
                KEY_Y,
                KEY_Z,
                KEY_LEFT,
                KEY_RIGHT,
                KEY_UP,
                KEY_DOWN,
                KEY_ENTER,
                KEY_ESC,
                KEY_LSHIFT,
                KEY_RSHIFT,
                KEY_LCTRL,
                KEY_RCTRL,
                KEY_SPACE,
                KEY_1,
                KEY_2,
                KEY_3,
                KEY_4,
                KEY_5,
                KEY_6,
                KEY_7,
                KEY_8,
                KEY_9,
                KEY_0,
                KEY_COUNT
        };

        class Button {
        public:
                bool pressed;
                bool released;
                bool held;
        };

        class Input {
        public:
                const unsigned char *sdlKeyStates;
                SDL_Event *event;
                bool isQuitPressed;
                Button *keyStates;
                bool *keyPressesThisFrame; // boolmap of pressed keys.
                bool *keyPressesLastFrame; // boolmap of pressed keys.

                //! \brief Creates and initializes new input state
                //! \return The initialized input state
                Input();

                //! \brief De-initializes and frees memory for the input state
                //! \param[in,out] input
                ~Input();

                //! \brief Handle program inputs
                //!
                //! Processes all program inputs and stores relevant data internally in input.
                //!
                //! \param[in,out] input
                void process();

                //! \brief Check if the user has tried to quit the program
                //!
                //! \param[in,out] input
                //! \return 1 if quit has been pressed, otherwise 0
                bool isQuitRequested();

                uint32_t isKeyPressed(InputKeyEnum e);
                Button getKey(InputKeyEnum e);
        };

} // namespace gs

#endif // INPUT_VERSION
