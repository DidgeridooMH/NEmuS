#ifndef NEMUS_NROM_H
#define NEMUS_NROM_H

#include <array>
#include <vector>

#include "Mapper.h"

namespace nemus::core
{

    enum class NameTableId
    {
        A,
        B
    };

    class NROM : public Mapper
    {
    public:
        NROM(const std::vector<char> &romStart);

        unsigned char readByte(unsigned int address) override;

        unsigned char readBytePPU(unsigned int address) override;

        void writeByte(unsigned char data, unsigned int address) override;

        void writeBytePPU(unsigned char data, unsigned int address) override;

        MirrorMode getMirroring() override { return m_mirroring; }

    private:
        NameTableId getMirroringTable(unsigned address);

        std::vector<uint8_t> m_fixedCPUMemory;
        std::vector<uint8_t> m_fixedPPUMemory;

        std::array<std::vector<uint8_t>, 4> m_nameTables;

        MirrorMode m_mirroring;
    };
}

#endif
