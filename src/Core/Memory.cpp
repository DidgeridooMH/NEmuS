#include <fstream>
#include <cstring>
#include "Memory.h"
#include "Mappers/NROM.h"
#include "Mappers/MMC1.h"

nemus::core::Memory::Memory(debug::Logger *logger, core::PPU *ppu, core::Input *input,
                            const std::vector<char> &gameData)
{
    m_logger = logger;
    m_ppu = ppu;
    m_input = input;

    m_ram = new unsigned char[0x10000];

    loadRom(gameData);

    m_logger->write("Memory initialized");
}

nemus::core::Memory::~Memory()
{
    delete[] m_ram;
    delete m_mapper;
}

void nemus::core::Memory::loadRom(const std::vector<char> &gameData, const std::string &filename)
{
    unsigned char mapperID = (gameData[6] >> 4) & 0xF;
    mapperID |= (gameData[7] & 0xF0);

    // TODO: Add a mapper enum for easier implementation.
    switch (mapperID)
    {
    case 0:
        m_mapper = new NROM(gameData);
        break;
    case 1:
    {
        int extOffset = filename.find('.');
        FileInfo savFile = filename.length() > 0
                               ? loadSaveFile(filename.substr(0, extOffset))
                               : FileInfo{.start_of_file = 0, .size = 0};

        if (savFile.size > 0)
        {
            m_mapper = new MMC1(gameData, savFile.start_of_file, savFile.size);
        }
        else
        {
            m_mapper = new MMC1(gameData);
        }
        break;
    }
    default:
        m_logger->write("Invalid mapper id...using NROM and hoping for the best.");
        m_mapper = new NROM(gameData);
        break;
    }
}

// TODO: extract this functionality to helper functions.
nemus::core::FileInfo nemus::core::Memory::loadSaveFile(std::string filename)
{
    filename += ".sav";

    std::ifstream file;

    file.open(filename, std::ios::ate | std::ios::binary);

    if (!file)
    {
        return {nullptr, 0};
    }

    long size = file.tellg();
    file.seekg(file.beg);

    char *savRam = new char[size];

    file.read(savRam, size);

    file.close();

    return {savRam, size};
}

unsigned int nemus::core::Memory::readByte(unsigned int address)
{
    address &= 0xFFFF;

    if (address < 0x2000)
    {
        return m_ram[address % 0x800];
    }
    else if (address < 0x4000)
    {
        return m_ppu->readPPU(0x2000 + (address % 8));
    }
    else if (address == 0x4014)
    {
        return m_ppu->readPPU(0x4014);
    }
    else if (address == 0x4016)
    {
        return m_input->read();
    }
    else if (address >= 0x6000)
    {
        return m_mapper->readByte(address);
    }
    else
    {
        m_logger->write("Illegal Out of Bounds Read at " + std::to_string(address));
        return 0;
    }
}

unsigned int nemus::core::Memory::readByte(comp::Registers registers, comp::AddressMode addr)
{
    unsigned int address = 0;

    switch (addr)
    {
    case comp::ADDR_MODE_IMMEDIATE:
        return readByte(registers.pc + 1);
    case comp::ADDR_MODE_ZERO_PAGE:
        address = readByte(registers.pc + 1);
        return readByte(address & 0xFF);
    case comp::ADDR_MODE_ZERO_PAGE_X:
        address = readByte(registers.pc + 1);
        address += registers.x;
        return readByte(address & 0xFF);
    case comp::ADDR_MODE_ZERO_PAGE_Y:
        address = readByte(registers.pc + 1);
        address += registers.y;
        return readByte(address & 0xFF);
    case comp::ADDR_MODE_ABSOLUTE:
        address = readWord(registers.pc + 1);
        return readByte(address);
    case comp::ADDR_MODE_ABSOLUTE_X:
        address = readWord(registers.pc + 1) + registers.x;
        return readByte(address);
    case comp::ADDR_MODE_ABSOLUTE_Y:
        address = readWord(registers.pc + 1) + registers.y;
        return readByte(address);
    case comp::ADDR_MODE_INDIRECT_X:
        address = readByte(registers.pc + 1) + registers.x;
        address &= 0xFF;
        address = readWordBug(address);
        return readByte(address);
    case comp::ADDR_MODE_INDIRECT_Y:
        address = readByte(registers.pc + 1);
        address = readWordBug(address) + registers.y;
        return readByte(address);
    default:
        m_logger->write("Unknown address mode!!!");
        break;
    }

    return 0;
}

unsigned int nemus::core::Memory::readWordBug(unsigned int address)
{
    // Hardware bug causes only low byte to be incremented across pages
    unsigned int lowByte = readByte(address) & 0xFF;
    unsigned int highByte = readByte((address & 0xFF00) + ((address + 1) & 0xFF)) & 0xFF;
    return highByte << 8 | lowByte;
}

unsigned int nemus::core::Memory::readWord(unsigned int address)
{
    unsigned int lowByte = readByte(address) & 0xFF;
    unsigned int highByte = readByte(address + 1) & 0xFF;
    return highByte << 8 | lowByte;
}

bool nemus::core::Memory::writeByte(unsigned char data, unsigned int address)
{
    address &= 0xFFFF;

    if (address < 0x2000)
    {
        m_ram[address % 0x800] = data;
    }
    else if (address < 0x4000)
    {
        m_ppu->writePPU(data, 0x2000 + (address % 8));
    }
    else if (address == 0x4014)
    {
        m_ppu->writePPU(data, address);
    }
    else if (address == 0x4016)
    {
        m_input->write(data);
    }
    else if (address < 0x4020)
    {
        // TODO: Implement IO registers
        return true;
    }
    else if (address >= 0x6000)
    {
        m_mapper->writeByte(data, address);
    }
    else
    {
        m_logger->write("Illegal Out of Bounds Write!!!");
        return true;
    }

    return false;
}

void nemus::core::Memory::writeByte(comp::Registers registers, unsigned int src, comp::AddressMode addr)
{
    unsigned int address = 0;
    switch (addr)
    {
    case comp::ADDR_MODE_IMMEDIATE:
        writeByte(src, registers.pc + 1);
        break;
    case comp::ADDR_MODE_ZERO_PAGE:
        address = readByte(registers.pc + 1);
        writeByte(src, address & 0xFF);
        break;
    case comp::ADDR_MODE_ZERO_PAGE_X:
        address = readByte(registers.pc + 1) + registers.x;
        writeByte(src, address & 0xFF);
        break;
    case comp::ADDR_MODE_ZERO_PAGE_Y:
        address = readByte(registers.pc + 1) + registers.y;
        writeByte(src, address & 0xFF);
        break;
    case comp::ADDR_MODE_ABSOLUTE:
        address = readWord(registers.pc + 1);
        writeByte(src, address);
        break;
    case comp::ADDR_MODE_ABSOLUTE_X:
        address = readWord(registers.pc + 1) + registers.x;
        writeByte(src, address);
        break;
    case comp::ADDR_MODE_ABSOLUTE_Y:
        address = readWord(registers.pc + 1) + registers.y;
        writeByte(src, address);
        break;
    case comp::ADDR_MODE_INDIRECT_X:
        address = readByte(registers.pc + 1) + registers.x;
        address &= 0xFF;
        address = readWordBug(address);
        writeByte(src, address);
        break;
    case comp::ADDR_MODE_INDIRECT_Y:
        address = readByte(registers.pc + 1);
        address = readWordBug(address) + registers.y;
        writeByte(src, address);
        break;
    default:
        m_logger->write("Unknown address mode!!!");
        break;
    }
}

unsigned int nemus::core::Memory::readPPUByte(unsigned int address)
{
    return m_mapper->readBytePPU(address);
}

void nemus::core::Memory::writePPUByte(unsigned char data, unsigned address)
{
    m_mapper->writeBytePPU(data, address);
}

void nemus::core::Memory::push(unsigned int data, unsigned int &sp)
{
    m_ram[sp + 0x100] = (unsigned char)data;
    sp--;
    sp &= 0xFF;
}

void nemus::core::Memory::push16(unsigned int data, unsigned int &sp)
{
    unsigned int highByte = (data >> 8) & 0xFF;
    push(highByte, sp);
    unsigned int lowByte = data & 0xFF;
    push(lowByte, sp);
}

unsigned int nemus::core::Memory::pop(unsigned int &sp)
{
    sp++;
    sp &= 0xFF;
    return (m_ram[sp + 0x100] & 0xFF);
}

unsigned int nemus::core::Memory::pop16(unsigned int &sp)
{
    unsigned int lowByte = pop(sp);
    unsigned int highByte = pop(sp);

    unsigned int address = ((highByte << 8) & 0xFF00) | (lowByte & 0xFF);

    return address;
}

unsigned int nemus::core::Memory::checkPageCross(comp::Registers registers, comp::AddressMode mode)
{
    switch (mode)
    {
    case comp::ADDR_MODE_ABSOLUTE_X:
    {
        const auto address = readWord(registers.pc + 1);
        return ((address + registers.x) & 0xFF00) != (address & 0xFF00);
    }
    case comp::ADDR_MODE_ABSOLUTE_Y:
    {
        const auto address = readWord(registers.pc + 1);
        return ((address + registers.y) & 0xFF00) != (address & 0xFF00);
    }
    case comp::ADDR_MODE_INDIRECT_Y:
    {
        auto address = readByte(registers.pc + 1);
        return (address & 0xFF00) != ((readWordBug(address) + registers.y) & 0xFF00);
    }
    default:
        return 0;
    }
}