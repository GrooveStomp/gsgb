/******************************************************************************
  GrooveStomp's GB Emulator
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: operand.hpp
  Created: 2019-08-31
  Updated: 2019-08-31
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)

  This program comes with ABSOLUTELY NO WARRANTY. This is free software, and
  you are welcome to redistribute it under certain conditions; See LICENSE for
  details.
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
        uint16_t value;

        operand_value(uint16_t);
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

        operand_reference(uint8_t);
        virtual uint16_t Get();
        virtual void Set(uint16_t value);
};

class operand_pair_reference : public operand {
public:
        uint8_t *ref1;
        uint8_t *ref2;

        operand_pair_reference(uint16_t);
        virtual uint16_t Get();
        void Set(uint16_t value);
};

class operand_sp_reference : public operand {
public:
        uint16_t *ref;

        operand_sp_reference(uint16_t);
        virtual uint16_t Get();
        virtual void Set(uint16_t value);
};

} // namespace gs

#endif // OPERAND_VERSION
