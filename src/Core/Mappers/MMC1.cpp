#include <cstring>
#include "MMC1.h"

nemus::core::MMC1::MMC1(char* romStart, long romSize, char* savStart, long savSize) : MMC1(romStart, romSize) {
    for (unsigned int i = 0; i < 0x2000; i++) {
        if (i < savSize) {
            m_CPUMemory[i] = savStart[i];
        }
    }
}

nemus::core::MMC1::MMC1(char* romStart, long size) {
    m_CPUMemory = new unsigned char[static_cast<long>(romStart[4]) * static_cast<long>(0x4000) + static_cast<long>(0x2000)];

    m_PPUMemory = new unsigned char[0x2000];

    for (unsigned int i = 0; i < romStart[4] * 0x4000; i++) {
        if (i < size) {
            m_CPUMemory[i + 0x2000] = romStart[i + 0x10];
        }
    }

    if (romStart[5] > 0) {
        for (unsigned int i = 0; i < 0x2000; i++) {
            m_PPUMemory[i] = romStart[romStart[4] * 0x4000 + i + 0x10];
        }
    } else {
        // Implement CHR Ram checking
    }

    m_prgBank0 = 0;
    m_prgBank1 = romStart[4] - 1;
    
    m_chrBank0 = 0;
    m_chrBank1 = 1;

    m_prgBank = 0;
    m_chrBank = 0;

    m_maxPrgBanks = romStart[4];

    m_control.prg_mode = 0;
    m_control.chr_mode = 0;
    m_control.mirroring = 0;
}

nemus::core::MMC1::~MMC1() {
    delete[] m_CPUMemory;
    delete[] m_PPUMemory;
}

unsigned nemus::core::MMC1::readByte(unsigned address) {
    if(address >= 0x6000 && address < 0x8000) {
        return m_CPUMemory[address - 0x6000];
    }
    
    if (address >= 0x8000 && address < 0xC000) {
        return m_CPUMemory[(address - 0x6000) + (0x4000 * m_prgBank0)];
    }

    return m_CPUMemory[(address - 0xA000) + (0x4000 * m_prgBank1)];
}

unsigned nemus::core::MMC1::readBytePPU(unsigned address) {
    return m_PPUMemory[address];
}

void nemus::core::MMC1::writeByte(unsigned char data, unsigned address) {
    if (address >= 0x6000 && address < 0x8000) {
        m_CPUMemory[address - 0x6000] = data;
    } else {
        adjustShiftRegister(data, address);
    }
}

void nemus::core::MMC1::writeBytePPU(unsigned char data, unsigned address) {
    if (address < 0x2000) {
        m_PPUMemory[address] = data;
    }
}

void nemus::core::MMC1::adjustShiftRegister(unsigned char data, unsigned address) {
    if(data & 0x80) {
        m_shiftRegister = 0x10;

        unsigned char control = 0;
        control |= (m_control.prg_mode & 3) << 2;
        control |= (m_control.chr_mode & 1) << 4;
        control |= m_control.mirroring & 3;
        control |= 0xC;

        m_control.mirroring = control & 3;
        m_control.prg_mode = (control >> 2) & 3;
        m_control.chr_mode = (control >> 4) & 1;

        updateBanks();
    } else {
        bool write = m_shiftRegister & 1;

        m_shiftRegister >>= 1;
        m_shiftRegister |= (data & 1) << 4;
        
        if (write) {
            if (address >= 0x8000 && address < 0xA000) {
                writeControl();
            }
            else if (address >= 0xA000 && address < 0xC000) {
                writeCHRBank0();
            }
            else if (address >= 0xC000 && address < 0xE000) {
                writeCHRBank1();
            }
            else {
                writePRGBank();
            }
        }
    }
}

void nemus::core::MMC1::writeControl() {
    m_control.mirroring = m_shiftRegister & 3;

    m_control.prg_mode = (m_shiftRegister >> 2) & 3;

    m_control.chr_mode = (m_shiftRegister >> 4) & 1;

    updateBanks();

    m_shiftRegister = 0x10;
}

void nemus::core::MMC1::writeCHRBank0() {
    if (m_control.chr_mode) {
        m_chrBank0 = m_shiftRegister;
    } else {
        m_shiftRegister &= 0x1E;
        m_chrBank0 = m_shiftRegister;
        m_chrBank1 = m_shiftRegister + 1;
    }

    m_shiftRegister = 0x10;
}

void nemus::core::MMC1::writeCHRBank1() {
    m_chrBank1 = m_shiftRegister;

    m_shiftRegister = 0x10;
}

void nemus::core::MMC1::writePRGBank() {
    m_prgBank = m_shiftRegister;

    m_prgBank &= 0xF;

    updateBanks();

    m_shiftRegister = 0x10;
}

void nemus::core::MMC1::updateBanks() {
    switch (m_control.prg_mode) {
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
