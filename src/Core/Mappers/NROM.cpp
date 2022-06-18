#include <cstring>
#include <cassert>

#include "NROM.h"

namespace nemus::core
{
    NROM::NROM(const std::vector<char> &romStart)
        : m_fixedCPUMemory(CPURomBankSize * 2 + CPURamSize),
          m_fixedPPUMemory(CharacterRomSize)
    {
        memcpy(&m_fixedCPUMemory[CPURamSize], &romStart[INESRomHeaderSize], CPURomBankSize * 2);
        memcpy(m_fixedPPUMemory.data(), &romStart[CPURomBankSize * 2 + INESRomHeaderSize], CPURamSize);

        m_mirroring = (romStart[6] & 1) > 0 ? MirrorMode::Vertical : MirrorMode::Horizontal;
    }

    size_t NROM::GetMirroringTable(uint16_t address)
    {
        assert(address >= 0x2000 && address < 0x3000);
        switch (m_mirroring)
        {
        case MirrorMode::Horizontal:
            return (address >= 0x2000 && address < 0x2800) ? 0 : 1;
        case MirrorMode::Vertical:
            return ((address >= 0x2000 && address < 0x2400) || (address >= 0x2800 && address < 0x2C00)) ? 0 : 1;
        default:
            break;
        }

        return 0;
    }

    uint8_t NROM::ReadByte(uint16_t address)
    {
        assert(address >= 0x6000);
        if (address < 0x8000)
        {
            return m_fixedCPUMemory[(address - 0x6000) % 0x1000];
        }
        return m_fixedCPUMemory[address - 0x6000];
    }

    uint8_t NROM::ReadBytePPU(uint16_t address)
    {
        assert(address < 0x2000);
        return m_fixedPPUMemory[address];
    }

    void NROM::WriteByte(uint8_t data, uint16_t address)
    {
        assert(address < 0x8000);
        m_fixedCPUMemory[(address - 0x6000) % 0x1000] = data;
    }

    void NROM::WriteBytePPU(uint8_t data, uint16_t address)
    {
        assert(address < 0x2000);
        m_fixedPPUMemory[address] = data;
    }
}
