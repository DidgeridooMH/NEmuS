#include <cstring>
#include "NROM.h"

nemus::core::NROM::NROM(char* romStart) {
    m_fixedCPUMemory = new unsigned char[0xA000];

    m_fixedPPUMemory = new unsigned char[0x8000];
    
    for (unsigned int i = 0; i < 0x8000; i++) {
        m_fixedCPUMemory[0x2000 + i] = romStart[i + 0x10];
    }

    for (unsigned int i = 0; i < 0x2000; i++) {
        m_fixedPPUMemory[i] = romStart[0x8000 + i + 0x10];
    }

    m_tableA = new unsigned char[0x400];
    m_tableB = new unsigned char[0x400];
    m_tableC = new unsigned char[0x400];
    m_tableD = new unsigned char[0x400];

    m_mirroring = romStart[6] & 1;
}

nemus::core::NROM::~NROM() {
    delete[] m_fixedCPUMemory;
    delete[] m_fixedPPUMemory;
    
    delete[] m_tableA;
    delete[] m_tableB;
    delete[] m_tableC;
    delete[] m_tableD;
}

unsigned char* nemus::core::NROM::getMirroringTable(unsigned int address) {
    unsigned char* nametablePtr = nullptr;

    switch(m_mirroring) {
        case MIRROR_HORIZONTAL:
            if(address >= 0x2000 && address < 0x2800) {
                nametablePtr = m_tableA;
            } else {
                nametablePtr = m_tableB;
            }
            break;
        case MIRROR_VERTICAL:
            if((address >= 0x2000 && address < 0x2400) 
                || (address >= 0x2800 && address < 0x2C00)) {
                nametablePtr = m_tableA;
            } else {
                nametablePtr = m_tableB;
            }
            break;
        default:
            // This will cause visual glitches but won't crash the program.
            nametablePtr = m_tableA;
            break;
    }

    return nametablePtr;
}

unsigned char nemus::core::NROM::readByte(unsigned int address) {
    return m_fixedCPUMemory[address - 0x6000];
}

unsigned char nemus::core::NROM::readBytePPU(unsigned address) {
    if(address >= 0x2000 && address < 0x3000) {
        unsigned char* nametable = getMirroringTable(address);
        address %= 0x400;
        return nametable[address];
    }
     
    return m_fixedPPUMemory[address];
}

void nemus::core::NROM::writeByte(unsigned char data, unsigned address) {
    if (address < 0x8000) {
        m_fixedCPUMemory[address - 0x6000] = data;
    }
}

void nemus::core::NROM::writeBytePPU(unsigned char data, unsigned address) {
    if(address >= 0x2000 && address < 0x3000) {
        unsigned char* nametable = getMirroringTable(address);
        address %= 0x400;
        nametable[address] = data;
    } else {
        m_fixedPPUMemory[address] = data;
    }
}
