#ifndef NEMUS_MAPPER_H
#define NEMUS_MAPPER_H

#include <cstdint>

namespace nemus::core
{
    enum class MirrorMode
    {
        Horizontal,
        Vertical,
        OsLower,
        OsUpper
    };

    class Mapper
    {
    public:
        virtual ~Mapper() = default;

        virtual uint8_t readByte(uint32_t address) = 0;

        virtual uint8_t readBytePPU(uint32_t address) = 0;

        virtual void writeByte(uint8_t data, uint32_t address) = 0;

        virtual void writeBytePPU(uint8_t data, uint32_t address) = 0;

        virtual MirrorMode getMirroring() = 0;

    protected:
        static constexpr size_t INESRomHeaderSize = 0x10;
    };

}

#endif