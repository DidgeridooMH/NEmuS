#ifndef NEMUS_COMPONENTHELPER_H
#define NEMUS_COMPONENTHELPER_H

namespace nemus::comp {
    struct Registers {
        unsigned int x;
        unsigned int y;
        unsigned int a;
        unsigned int pc;
        unsigned int sp;
        unsigned int p;
    };

    enum AddressMode {
        ADDR_MODE_IMMEDIATE,
        ADDR_MODE_ZERO_PAGE,
        ADDR_MODE_ZERO_PAGE_X,
        ADDR_MODE_ZERO_PAGE_Y,
        ADDR_MODE_ABSOLUTE,
        ADDR_MODE_ABSOLUTE_X,
        ADDR_MODE_ABSOLUTE_Y,
        ADDR_MODE_INDIRECT,
        ADDR_MODE_INDIRECT_X,
        ADDR_MODE_INDIRECT_Y,
        ADDR_MODE_ACCUMULATOR,
        ADDR_MODE_IMPLIED,
    };

    enum Flag {
        FLAG_CARRY = 0x1,
        FLAG_ZERO = 0x2,
        FLAG_INTERRUPT = 0x8,
        FLAG_DECIMAL = 0x10,
        FLAG_OVERFLOW = 0x40,
        FLAG_NEGATIVE = 0x80
    };

    enum Interrupt {
        INT_NONE,
        INT_NMI,
        INT_RESET,
        INT_IRQ
    };
}

#endif
