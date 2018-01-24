#include <fstream>
#include <cstring>
#include "Memory.h"

nemus::core::Memory::Memory(debug::Logger* logger, core::PPU *ppu) {
    m_logger = logger;
    m_ppu = ppu;

    m_ram = new unsigned char[0xFFFF];
    memset(m_ram, 0, 0xFFFF);

    initRam();

    loadRom("");

    m_logger->write("Memory initialized");
}

nemus::core::Memory::~Memory() {
    delete[] m_ram;
}

void nemus::core::Memory::initRam() {
    for(int i = 0; i < 0x2000; i++) {
        if((i / 4) % 2 == 0) {
            m_ram[i] = 0;
        } else {
            m_ram[i] = 0xFF;
        }
    }

    for(int i = 0x2000; i < 0x4000; i++) {
        if((i % 16) == 6 || (i % 16) == 14) {
            m_ram[i] = 5;
        }
    }

    m_ram[0x4014] = 2;

    for(int i = 0x4018; i < 0x5000; i++) {
        m_ram[i] = 0xFF;
    }

}

void nemus::core::Memory::loadRom(std::string filename) {
    std::ifstream file;

    // Test rom
    // TODO: Add way to load different games
    file.open("ROMS/mario.nes", std::ios::ate | std::ios::binary);

    if(!file) {
        m_logger->write("Error while loading rom");
        return;
    }

    unsigned int size = file.tellg();
    file.seekg(file.beg);

    m_rom = new char[size];

    file.read(m_rom, size);

    // Load PRG-ROM
    // TODO: Move to mapper for support
    for(unsigned int i = 0; i < 0x8000; i++) {
        writeByte(m_rom[i + 0x10], 0x8000 + i);
    }
}

unsigned int nemus::core::Memory::readByte(unsigned int address) {
    if(address < 0x2000) {
        return m_ram[address % 0x800];
    } else if(address >= 0x2000 && address < 0x2008) {
        return m_ppu->readPPU(address);
    } else if(address >= 2008 && address < 0x4000) {
        return readByte(address - 8);
    } else if(address >= 0x4000 && address < 0x4020) {
        // TODO: Implement IO registers
        m_logger->write("IO Registers are not implemented yet");
        return 0;
    } else if(address >= 0x4020 && address < 0x10000) {
        return m_ram[address];
    } else {
        m_logger->write("Illegal Out of Bounds Read!!!");
        return 0;
    }
}

unsigned int nemus::core::Memory::readByte(comp::Registers registers, comp::AddressMode addr) {
    unsigned int address = 0;
    switch(addr) {
        case comp::ADDR_MODE_IMMEDIATE:
            return readByte(registers.pc + 1);
        case comp::ADDR_MODE_ZERO_PAGE:
            address = readByte(registers.pc + 1);

            return readByte(address % 0x100);
        case comp::ADDR_MODE_ZERO_PAGE_X:
            address = readByte(registers.pc + 1);

            address += registers.x;

            return readByte(address % 256);
        case comp::ADDR_MODE_ZERO_PAGE_Y:
            address = readByte(registers.pc + 1);

            address += registers.y;

            return readByte(address % 256);
        case comp::ADDR_MODE_ABSOLUTE:
            address = readWord(registers.pc + 1);
            return readByte(address);
        case comp::ADDR_MODE_ABSOLUTE_X:
            address = readWord(registers.pc + 1);

            address = (address & 0xFF00) | (((address & 0xFF) + registers.x) % 0x100);

            return readByte(address);
        case comp::ADDR_MODE_ABSOLUTE_Y:
            address = readWord(registers.pc + 1);

            address = (address & 0xFF00) | (((address & 0xFF) + registers.y) % 0x100);

            return readByte(address);
        case comp::ADDR_MODE_INDIRECT_X:
            address = readByte(registers.pc + 1);

            address = (address & 0xFF00) | (((address & 0xFF) + registers.x) % 0x100);

            address = readWord(address);

            return readByte(address);
        case comp::ADDR_MODE_INDIRECT_Y:
            address = readByte(registers.pc + 1);

            address = readWord(address);

            address = (address & 0xFF00) | (((address & 0xFF) + registers.y) % 256);

            return readByte(address);
        default:
            m_logger->write("Unknown address mode!!!");
            break;
    }
}

unsigned int nemus::core::Memory::readWord(unsigned int address) {
    // Hardware bug causes only low byte to be incremented across pages
    unsigned int lowByte = readByte(address);
    unsigned int highByte = readByte((address & 0xFF00) | ((address + 1) & 0xFF));
    return highByte << 8 | lowByte;
}

bool nemus::core::Memory::writeByte(unsigned char data, unsigned int address) {
    if(address < 0x2000) {
        m_ram[address % 0x800] = data;
    } else if(address >= 0x2000 && address < 0x2008) {
        m_ppu->writePPU(data, address);
    } else if(address >= 0x2008 && address < 0x3FFF) {
        writeByte(data, address - 8);
        return true;
    } else if(address == 0x4014) {
        m_ppu->writePPU(data, address);
        return true;
    } else if(address >= 0x4000 && address < 0x4020) {
        // TODO: Implement IO registers
        m_logger->write("Audio IO Registers are not implemented yet");
        return true;
    } else if(address >= 0x4020 && address < 0x10000) {
        m_ram[address] = data;
    } else {
        m_logger->write("Illegal Out of Bounds Write!!!");
        return true;
    }

    return false;
}

void nemus::core::Memory::writeByte(comp::Registers registers, unsigned int src, comp::AddressMode addr) {
    unsigned int address = 0;
    switch(addr) {
        case comp::ADDR_MODE_IMMEDIATE:
            writeByte(src, registers.pc + 1);
            return;
        case comp::ADDR_MODE_ZERO_PAGE:
            address = readByte(registers.pc + 1);

            writeByte(src, address % 0x100);
        case comp::ADDR_MODE_ZERO_PAGE_X:
            address = readByte(registers.pc + 1);

            writeByte(src, address % 0x100);
            return;
        case comp::ADDR_MODE_ZERO_PAGE_Y:
            address = readByte(registers.pc + 1);

            address += registers.y;

            writeByte(src, address % 0x100);
        case comp::ADDR_MODE_ABSOLUTE:
            address = readWord(registers.pc + 1);
            writeByte(src, address);
            return;
        case comp::ADDR_MODE_ABSOLUTE_X:
            address = readWord(registers.pc + 1);

            address = (address & 0xFF00) | (((address & 0xFF) + registers.x) % 0x100);

            writeByte(src, address);
            return;
        case comp::ADDR_MODE_ABSOLUTE_Y:
            address = readWord(registers.pc + 1);

            address = (address & 0xFF00) | (((address & 0xFF) + registers.y) % 0x100);

            writeByte(src, address);
            return;
        case comp::ADDR_MODE_INDIRECT_X:
            address = readWord(registers.pc + 1);

            address = (address & 0xFF00) | (((address & 0xFF) + registers.x) % 0x100);

            address = readWord(address);
            writeByte(src, address);
            return;
        case comp::ADDR_MODE_INDIRECT_Y:
            address = readWord(registers.pc + 1);
            address = readWord(address);

            address = (address & 0xFF00) | (((address & 0xFF) + registers.y) % 0x100);
            writeByte(src, address);
            return;
        default:
            m_logger->write("Unknown address mode!!!");
            break;
    }
}

void nemus::core::Memory::push(unsigned int data, unsigned int &sp) {
    m_ram[sp + 0x100] = (unsigned char)data;
    sp--;
}

void nemus::core::Memory::push16(unsigned int data, unsigned int &sp) {
    unsigned int highByte = (data >> 8) & 0xFF;
    push(highByte, sp);
    unsigned int lowByte = data & 0xFF;
    push(lowByte, sp);
}

unsigned int nemus::core::Memory::pop(unsigned int &sp) {
    sp++;
    return m_ram[sp + 0x100];
}

unsigned int nemus::core::Memory::pop16(unsigned int &sp) {
    unsigned int lowByte = pop(sp);
    unsigned int highByte = pop(sp);

    unsigned int address = (highByte << 8) | lowByte;

    return address;
}