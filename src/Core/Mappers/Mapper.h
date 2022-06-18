#ifndef NEMUS_MAPPER_H
#define NEMUS_MAPPER_H

#include <cstdint>

namespace nemus::core
{
    enum class MapperId
    {
        NROM,
        MMC1
    };

    enum class MirrorMode
    {
        Horizontal,
        Vertical,
        OsLower,
        OsUpper,
    };

    class Mapper
    {
    public:
        virtual ~Mapper() = default;

        virtual uint8_t ReadByte(uint16_t address) = 0;

        virtual uint8_t ReadBytePPU(uint16_t address) = 0;

        virtual void WriteByte(uint8_t data, uint16_t address) = 0;

        virtual void WriteBytePPU(uint8_t data, uint16_t address) = 0;

        virtual size_t GetMirroringTable(uint16_t address) = 0;

    protected:
        static constexpr size_t INESRomHeaderSize = 0x10;
    };

}

#endif