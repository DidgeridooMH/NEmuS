#include <cstring>
#include "NROM.h"

nemus::core::NROM::NROM(char* romStart) {
    memset(m_fixedMemory, 0, 0xA000);
    
    for (unsigned int i = 0; i < 0x8000; i++) {
        m_fixedMemory[0x2000 + i] = romStart[i + 0x10];
    }
}

unsigned int nemus::core::NROM::readByte(unsigned int address) {
    return m_fixedMemory[address - 0x6000];
}

void nemus::core::NROM::writeByte(unsigned char data, unsigned address) {
    if (address < 0x8000) {
        m_fixedMemory[address - 0x6000] = data;
    }
}
