#pragma once

#include <cstdint>

namespace nemus::comp
{
    struct StatusFlag
    {
        uint8_t carry : 1;
        uint8_t zero : 1;
        uint8_t interruptDisable : 1;
        uint8_t decimal : 1;
        uint8_t pushed : 2;
        uint8_t overflow : 1;
        uint8_t negative : 1;
    };

    struct Registers
    {
        uint8_t x;
        uint8_t y;
        uint8_t a;
        uint16_t pc;
        uint8_t sp;
        union
        {
            StatusFlag p;
            uint8_t pFull;
        };
    };

    // TODO: Make these enum class
    enum AddressMode
    {
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

    constexpr const char *AddressModeToString(AddressMode mode)
    {
        switch (mode)
        {
        case ADDR_MODE_IMMEDIATE:
            return "IMMEDIATE MODE";
        case ADDR_MODE_ZERO_PAGE:
            return "ZEROPAGE MODE";
        case ADDR_MODE_ZERO_PAGE_X:
            return "ZEROPAGE INDEXED X MODE";
        case ADDR_MODE_ABSOLUTE:
            return "ABSOLUTE MODE";
        case ADDR_MODE_ABSOLUTE_X:
            return "ABSOLUTE MODE X";
        case ADDR_MODE_ABSOLUTE_Y:
            return "ABSOLUTE MODE Y";
        case ADDR_MODE_INDIRECT:
            return "INDIRECT";
        case ADDR_MODE_INDIRECT_X:
            return "INDIRECT X";
        case ADDR_MODE_INDIRECT_Y:
            return "INDIRECT Y";
        case ADDR_MODE_ACCUMULATOR:
            return "ACCUMULATOR";
        case ADDR_MODE_IMPLIED:
            return "IMPLIED";
        default:
            return "ADDR UNKNOWN";
        }
    }

    enum Interrupt
    {
        INT_NONE,
        INT_NMI,
        INT_RESET,
        INT_IRQ
    };
}
