#include <cstring>
#include "NROM.h"

namespace nemus::core
{
    NROM::NROM(const std::vector<char> &romStart)
        : m_fixedCPUMemory(0xA000),
          m_fixedPPUMemory(0x8000),
          m_nameTables({std::vector<uint8_t>(0x400),
                        std::vector<uint8_t>(0x400)})
    {
        // TODO: Check size of rom file.
        memcpy(&m_fixedCPUMemory[0x2000], &romStart[INESRomHeaderSize], 0x8000);
        memcpy(m_fixedPPUMemory.data(), &romStart[0x8000 + INESRomHeaderSize], 0x2000);

        m_mirroring = static_cast<MirrorMode>(romStart[6] & 1);
    }

    NameTableId NROM::getMirroringTable(unsigned int address)
    {
        switch (m_mirroring)
        {
        case MirrorMode::Horizontal:
            return (address >= 0x2000 && address < 0x2800) ? NameTableId::A : NameTableId::B;
        case MirrorMode::Vertical:
            return ((address >= 0x2000 && address < 0x2400) || (address >= 0x2800 && address < 0x2C00)) ? NameTableId::A : NameTableId::B;
        default:
            break;
        }

        return NameTableId::A;
    }

    unsigned char NROM::readByte(unsigned int address)
    {
        return m_fixedCPUMemory[address - 0x6000];
    }

    unsigned char NROM::readBytePPU(unsigned address)
    {
        if (address >= 0x2000 && address < 0x3000)
        {
            auto nametable = getMirroringTable(address);
            address %= 0x400;
            return m_nameTables[static_cast<int>(nametable)][address];
        }

        return m_fixedPPUMemory[address];
    }

    void NROM::writeByte(unsigned char data, unsigned address)
    {
        if (address < 0x8000)
        {
            m_fixedCPUMemory[address - 0x6000] = data;
        }
    }

    void NROM::writeBytePPU(unsigned char data, unsigned address)
    {
        if (address >= 0x2000 && address < 0x3000)
        {
            auto nametable = getMirroringTable(address);
            address %= 0x400;
            m_nameTables[static_cast<int>(nametable)][address] = data;
        }
        else
        {
            m_fixedPPUMemory[address] = data;
        }
    }
}
