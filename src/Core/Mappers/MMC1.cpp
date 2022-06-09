#include <cstring>
#include "MMC1.h"

nemus::core::MMC1::MMC1(const std::vector<char> &gameData, char *savStart, long savSize)
    : MMC1(gameData)
{
    for (unsigned int i = 0; i < std::min(0x2000L, savSize); i++)
    {
        m_CPUMemory[i] = savStart[i];
    }
    memcpy(m_CPUMemory, savStart, std::min(0x2000L, savSize));
}

nemus::core::MMC1::MMC1(const std::vector<char> &gameData)
{
    m_CPUMemory = new unsigned char[static_cast<long>(gameData[4]) * static_cast<long>(0x4000) + static_cast<long>(0x2000)];

    m_PPUMemory = new unsigned char[0x8000];

    m_tableA = new unsigned char[0x400];
    m_tableB = new unsigned char[0x400];
    m_tableC = new unsigned char[0x400];
    m_tableD = new unsigned char[0x400];

    for (unsigned int i = 0; i < gameData[4] * 0x4000U; i++)
    {
        if (i + 0x10 < gameData.size())
        {
            m_CPUMemory[i + 0x2000] = gameData[i + 0x10];
        }
    }

    if (gameData[5] > 0)
    {
        for (unsigned int i = 0; i < 0x2000; i++)
        {
            m_PPUMemory[i] = gameData[gameData[4] * 0x4000 + i + 0x10];
        }
    }

    m_prgBank0 = 0;
    m_prgBank1 = gameData[4] - 1;

    m_chrBank0 = 0;
    m_chrBank1 = 1;

    m_prgBank = 0;
    m_chrBank = 0;

    m_maxPrgBanks = gameData[4];

    m_control.prg_mode = 0;
    m_control.chr_mode = 0;
    m_control.mirroring = 0;
}

nemus::core::MMC1::~MMC1()
{
    delete[] m_CPUMemory;
    delete[] m_PPUMemory;

    delete[] m_tableA;
    delete[] m_tableB;
    delete[] m_tableC;
    delete[] m_tableD;
}

unsigned nemus::core::MMC1::getMirroringTable(unsigned int address)
{
    unsigned int nametableID = 0;

    switch (m_control.mirroring)
    {
    case MIRROR_HORIZONTAL:
        if (address >= 0x2000 && address < 0x2800)
        {
            nametableID = 0;
        }
        else
        {
            nametableID = 1;
        }
        break;
    case MIRROR_VERTICAL:
        if ((address >= 0x2000 && address < 0x2400) || (address >= 0x2800 && address < 0x2C00))
        {
            nametableID = 0;
        }
        else
        {
            nametableID = 1;
        }
        break;
    default:
        // This will cause visual glitches but won't crash the program.
        nametableID = 0;
        break;
    }

    return nametableID;
}

void nemus::core::MMC1::writeNametable(unsigned char data, unsigned address)
{
    switch (getMirroringTable(address))
    {
    case 0:
        m_tableA[address % 0x400] = data;
        break;
    case 1:
        m_tableB[address % 0x400] = data;
        break;
    }
}

unsigned char nemus::core::MMC1::readNametable(unsigned address)
{
    switch (getMirroringTable(address))
    {
    case 0:
        return m_tableA[address % 0x400];
    case 1:
        return m_tableB[address % 0x400];
    }

    return 0;
}

unsigned char nemus::core::MMC1::readByte(unsigned address)
{
    if (address >= 0x6000 && address < 0x8000)
    {
        return m_CPUMemory[address - 0x6000];
    }

    if (address >= 0x8000 && address < 0xC000)
    {
        return m_CPUMemory[(address - 0x6000) + (0x4000 * m_prgBank0)];
    }

    return m_CPUMemory[(address - 0xA000) + (0x4000 * m_prgBank1)];
}

unsigned char nemus::core::MMC1::readBytePPU(unsigned address)
{
    if (address >= 0x2000 && address < 0x3000)
    {
        //        unsigned char* nametable = getMirroringTable(address);
        //        address %= 0x400;
        //        return nametable[address];
        return readNametable(address);
    }

    return m_PPUMemory[address];
}

void nemus::core::MMC1::writeByte(unsigned char data, unsigned address)
{
    if (address >= 0x6000 && address < 0x8000)
    {
        m_CPUMemory[address - 0x6000] = data;
    }
    else
    {
        adjustShiftRegister(data, address);
    }
}

void nemus::core::MMC1::writeBytePPU(unsigned char data, unsigned address)
{
    if (address >= 0x2000 && address < 0x3000)
    {
        //        unsigned char* nametable = getMirroringTable(address);
        //        address %= 0x400;
        //        nametable[address] = data;
        writeNametable(data, address);
    }
    else
    {
        m_PPUMemory[address] = data;
    }
}

void nemus::core::MMC1::adjustShiftRegister(unsigned char data, unsigned address)
{
    if (data & 0x80)
    {
        unsigned char control = 0;
        control |= (m_control.prg_mode & 3) << 2;
        control |= (m_control.chr_mode & 1) << 4;
        control |= m_control.mirroring & 3;
        control |= 0xC;

        m_shiftRegister = control;

        writeControl();
    }
    else
    {
        bool write = m_shiftRegister & 1;

        m_shiftRegister >>= 1;
        m_shiftRegister |= (data & 1) << 4;

        if (write)
        {
            if (address >= 0x8000 && address < 0xA000)
            {
                writeControl();
            }
            else if (address >= 0xA000 && address < 0xC000)
            {
                writeCHRBank0();
            }
            else if (address >= 0xC000 && address < 0xE000)
            {
                writeCHRBank1();
            }
            else
            {
                writePRGBank();
            }
        }
    }
}

void nemus::core::MMC1::writeControl()
{
    switch (m_shiftRegister & 3)
    {
    case 0:
        m_control.mirroring = MIRROR_OS_LOWER;
        break;
    case 1:
        m_control.mirroring = MIRROR_OS_UPPER;
        break;
    case 2:
        m_control.mirroring = MIRROR_VERTICAL;
        break;
    case 3:
        m_control.mirroring = MIRROR_HORIZONTAL;
        break;
    }

    m_control.prg_mode = (m_shiftRegister >> 2) & 3;

    m_control.chr_mode = (m_shiftRegister >> 4) & 1;

    updateBanks();

    m_shiftRegister = 0x10;
}

void nemus::core::MMC1::writeCHRBank0()
{
    if (m_control.chr_mode)
    {
        m_chrBank0 = m_shiftRegister;
    }
    else
    {
        m_shiftRegister &= 0x1E;
        m_chrBank0 = m_shiftRegister;
        m_chrBank1 = m_shiftRegister + 1;
    }

    m_shiftRegister = 0x10;
}

void nemus::core::MMC1::writeCHRBank1()
{
    m_chrBank1 = m_shiftRegister;

    m_shiftRegister = 0x10;
}

void nemus::core::MMC1::writePRGBank()
{
    m_prgBank = m_shiftRegister;

    m_prgBank &= 0xF;

    updateBanks();

    m_shiftRegister = 0x10;
}

void nemus::core::MMC1::updateBanks()
{
    switch (m_control.prg_mode)
    {
    case 0:
    case 1:
        m_prgBank0 = m_prgBank & 0xFE;
        m_prgBank1 = m_prgBank | 1;
        break;
    case 2:
        m_prgBank0 = 0;
        m_prgBank1 = m_prgBank;
        break;
    case 3:
        m_prgBank0 = m_prgBank;
        m_prgBank1 = m_maxPrgBanks - 1;
        break;
    }
}
