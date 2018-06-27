#include <cstring>
#include "NROM.h"

nemus::core::NROM::NROM(char* romStart) {
    m_fixedCPUMemory = new unsigned char[0xA000];
    memset(m_fixedCPUMemory, 0, 0xA000);

    m_fixedPPUMemory = new unsigned char[0x2000];
    memset(m_fixedPPUMemory, 0, 0x2000);
    
    for (unsigned int i = 0; i < 0x8000; i++) {
        m_fixedCPUMemory[0x2000 + i] = romStart[i + 0x10];
    }

    for (unsigned int i = 0; i < 0x2000; i++) {
        m_fixedPPUMemory[i] = romStart[0x8000 + i + 0x10];
    }
}

nemus::core::NROM::~NROM() {
    delete[] m_fixedCPUMemory;
    delete[] m_fixedPPUMemory;
}

unsigned int nemus::core::NROM::readByte(unsigned int address) {
    return m_fixedCPUMemory[address - 0x6000];
}

unsigned nemus::core::NROM::readBytePPU(unsigned address) {
    return m_fixedPPUMemory[address];
}

void nemus::core::NROM::writeByte(unsigned char data, unsigned address) {
    if (address < 0x8000) {
        m_fixedCPUMemory[address - 0x6000] = data;
    }
}

void nemus::core::NROM::writeBytePPU(unsigned char data, unsigned address) {
    if (address < 0x2000) {
        m_fixedPPUMemory[address] = data;
    }
}
