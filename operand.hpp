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
        virtual void Set(uint8_t) = 0;
};

class operand_value : public operand {
public:
        uint16_t value;

        virtual uint16_t Get();
        virtual void Set(uint8_t value) {}; // nop for value types.
};

class operand_address : public operand {
public:
        std::shared_ptr<gs::bus> bus;
        uint16_t address;

        virtual uint16_t Get();
        virtual void Set(uint8_t value);
};

class operand_reference : public operand {
public:
        std::shared_ptr<uint8_t> ref;
        virtual uint16_t Get();
        virtual void Set(uint8_t value);
};

} // namespace gs

#endif // OPERAND_VERSION
