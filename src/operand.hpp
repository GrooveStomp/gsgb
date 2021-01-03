/******************************************************************************
 * File: operand.hpp
 * Created: 2019-08-31
 * Updated: 2021-01-02
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2021, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
#ifndef OPERAND_VERSION
#define OPERAND_VERSION "0.1.0" //!< include guard

#include <cstdint>

//! \file operand.hpp

namespace gs {

        class Bus;

        class Operand {
        public:
                virtual uint16_t get() = 0;
                virtual void set(uint16_t) = 0;
        };

        class OperandValueByte : public Operand {
        public:
                uint8_t value;

                OperandValueByte(uint8_t);
                virtual uint16_t get();
                virtual void set(uint16_t value) {}; // nop for value types.
        };

        class OperandValueWord : public Operand {
        public:
                uint16_t value;

                OperandValueWord(uint16_t);
                virtual uint16_t get();
                virtual void set(uint16_t value) {}; // nop for value types.
        };

        class OperandAddress : public Operand {
        public:
                Bus *bus;
                uint16_t address;

                OperandAddress(uint16_t, Bus *);
                virtual uint16_t get();
                virtual void set(uint16_t value);
        };

        class OperandReference : public Operand {
        public:
                uint8_t *ref;

                OperandReference(uint8_t &);
                virtual uint16_t get();
                virtual void set(uint16_t value);
        };

        class OperandPairReference : public Operand {
        public:
                uint16_t *ref;

                OperandPairReference(uint16_t &);
                virtual uint16_t get();
                void set(uint16_t value);
        };

} // namespace gs

#endif // OPERAND_VERSION
