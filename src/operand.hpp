/******************************************************************************
 * File: operand.hpp
 * Created: 2019-08-31
 * Updated: 2020-12-23
 * Package: gsgb
 * Creator: Aaron Oman (GrooveStomp)
 * Homepage: https://git.sr.ht/~groovestomp/gsgb/
 * Copyright 2019 - 2020, Aaron Oman and the gsgb contributors
 * SPDX-License-Identifier: AGPL-3.0-only
 ******************************************************************************/
#ifndef OPERAND_VERSION
#define OPERAND_VERSION "0.1.0" //!< include guard

#include <cstdint>

//! \file operand.hpp

namespace gs {

class bus;

class operand {
public:
        virtual uint16_t Get() = 0;
        virtual void Set(uint16_t) = 0;
};

class operand_value : public operand {
public:
        uint8_t value;

        operand_value(uint8_t);
        virtual uint16_t Get();
        virtual void Set(uint16_t value) {}; // nop for value types.
};

class operand_address : public operand {
public:
        bus *msgBus;
        uint16_t address;

        operand_address(uint16_t, bus *);
        virtual uint16_t Get();
        virtual void Set(uint16_t value);
};

class operand_reference : public operand {
public:
        uint8_t *ref;

        operand_reference(uint8_t &);
        virtual uint16_t Get();
        virtual void Set(uint16_t value);
};

class operand_pair_reference : public operand {
public:
        uint16_t *ref;

        operand_pair_reference(uint16_t &);
        virtual uint16_t Get();
        void Set(uint16_t value);
};

} // namespace gs

#endif // OPERAND_VERSION
