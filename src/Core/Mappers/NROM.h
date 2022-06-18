#ifndef NEMUS_NROM_H
#define NEMUS_NROM_H

#include <array>
#include <vector>

#include "Mapper.h"

namespace nemus::core
{
    class NROM : public Mapper
    {
    public:
        NROM(const std::vector<char> &romStart);

        uint8_t ReadByte(uint16_t address) override;
        uint8_t ReadBytePPU(uint16_t address) override;

        void WriteByte(uint8_t data, uint16_t address) override;
        void WriteBytePPU(uint8_t data, uint16_t address) override;

        size_t GetMirroringTable(uint16_t address) override;

    private:
        static constexpr size_t CPURamSize = 0x2000UL;
        static constexpr size_t CPURomBankSize = 0x4000UL;
        static constexpr size_t CharacterRomSize = 0x2000;

        std::vector<uint8_t> m_fixedCPUMemory;
        std::vector<uint8_t> m_fixedPPUMemory;

        MirrorMode m_mirroring;
    };
}

#endif
