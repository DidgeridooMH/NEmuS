#include <fstream>
#include <cstring>
#include "Memory.h"
#include "Mappers/NROM.h"
#include "Mappers/MMC1.h"

namespace nemus::core
{
  Memory::Memory(debug::Logger *logger, core::PPU *ppu, core::Input *input,
                 const std::vector<char> &gameData)
      : m_logger(logger),
        m_ppu(ppu),
        m_input(input),
        m_nameTables({std::vector<uint8_t>(NameTableSize),
                      std::vector<uint8_t>(NameTableSize),
                      std::vector<uint8_t>(NameTableSize),
                      std::vector<uint8_t>(NameTableSize)}),
        m_palette(PaletteSize),
        m_ram(InternalRamSize)
  {
    m_logger = logger;
    m_ppu = ppu;
    m_input = input;

    LoadRom(gameData);
  }

  void Memory::LoadRom(const std::vector<char> &gameData)
  {
    switch (static_cast<MapperId>((gameData[6] >> 4) | (gameData[7] & 0xF0)))
    {
    case MapperId::NROM:
      m_mapper = std::make_unique<NROM>(gameData);
      break;
    case MapperId::MMC1:
      m_mapper = std::make_unique<MMC1>(gameData);
      break;
    default:
      m_logger->write("Invalid mapper id...using NROM and hoping for the best.");
      m_mapper = std::make_unique<NROM>(gameData);
      break;
    }
  }

  uint16_t Memory::GetAddress(const comp::Registers &registers, comp::AddressMode mode)
  {
    switch (mode)
    {
    case comp::ADDR_MODE_IMMEDIATE:
      return registers.pc + 1;
    case comp::ADDR_MODE_ZERO_PAGE:
      return ReadByte(registers.pc + 1);
    case comp::ADDR_MODE_ZERO_PAGE_X:
      return ReadByte(registers.pc + 1) + registers.x;
    case comp::ADDR_MODE_ZERO_PAGE_Y:
      return ReadByte(registers.pc + 1) + registers.y;
    case comp::ADDR_MODE_ABSOLUTE:
      return ReadWord(registers.pc + 1);
    case comp::ADDR_MODE_ABSOLUTE_X:
      return ReadWord(registers.pc + 1) + registers.x;
    case comp::ADDR_MODE_ABSOLUTE_Y:
      return ReadWord(registers.pc + 1) + registers.y;
    case comp::ADDR_MODE_INDIRECT_X:
      return ReadWordBug(ReadByte(registers.pc + 1) + registers.x);
    case comp::ADDR_MODE_INDIRECT_Y:
      return ReadWordBug(ReadByte(registers.pc + 1)) + registers.y;
    default:
      m_logger->write("Unknown address mode!!!");
      break;
    }
    return 0;
  }

  uint8_t Memory::ReadByte(uint16_t address)
  {
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
    else if (address == 0x4017)
    {
      // TODO: This is a player two.
    }
    else if (address >= 0x6000)
    {
      return m_mapper->ReadByte(address);
    }
    else
    {
      m_logger->write("Illegal Out of Bounds Read at " + std::to_string(address));
      throw std::runtime_error("Illegal out of bounds read!!!");
    }

    return 0;
  }

  uint8_t Memory::ReadByte(const comp::Registers &registers, comp::AddressMode mode)
  {
    return ReadByte(GetAddress(registers, mode));
  }

  uint16_t Memory::ReadWordBug(uint16_t address)
  {
    // Hardware bug causes only low byte to be incremented across pages
    uint16_t highByte = ReadByte((address & 0xFF00) + ((address + 1) & 0xFF));
    return (highByte << 8) | ReadByte(address);
  }

  uint16_t Memory::ReadWord(uint16_t address)
  {
    // Implicit conversion to 16 bits so that the shift left later doesn't truncate.
    uint16_t highByte = ReadByte(address + 1);
    return (highByte << 8) | ReadByte(address);
  }

  void Memory::WriteByte(uint8_t data, uint16_t address)
  {
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
    }
    else if (address >= 0x6000)
    {
      m_mapper->WriteByte(data, address);
    }
    else
    {
      m_logger->write("Illegal Out of Bounds Write!!!");
      throw std::runtime_error("Illegal out of bounds write!!!");
    }
  }

  void Memory::WriteByte(const comp::Registers &registers, uint8_t src, comp::AddressMode mode)
  {
    WriteByte(src, GetAddress(registers, mode));
  }

  uint8_t Memory::ReadPPUByte(uint16_t address)
  {
    if (address < 0x2000)
    {
      return m_mapper->ReadBytePPU(address);
    }
    else if (address < 0x3F00)
    {
      return m_nameTables[m_mapper->GetMirroringTable(address)][address % 0x400];
    }
    return m_palette[address % 0x20];
  }

  void Memory::WritePPUByte(uint8_t data, uint16_t address)
  {
    if (address < 0x2000)
    {
      m_mapper->WriteBytePPU(data, address);
    }
    else if (address < 0x3F00)
    {
      m_nameTables[m_mapper->GetMirroringTable(address)][address % 0x400] = data;
    }
    m_palette[address % 0x20] = data;
  }

  void Memory::Push(uint8_t data, uint8_t &sp)
  {
    m_ram[sp-- + 0x100] = data;
  }

  void Memory::Push16(uint16_t data, uint8_t &sp)
  {
    Push(data >> 8, sp);
    Push(data, sp);
  }

  uint8_t Memory::Pop(uint8_t &sp)
  {
    return m_ram[++sp + 0x100];
  }

  uint16_t Memory::Pop16(uint8_t &sp)
  {
    uint16_t lowByte = Pop(sp);
    uint16_t highByte = Pop(sp);

    return (highByte << 8) | lowByte;
  }

  bool Memory::CheckPageCross(const comp::Registers &registers, comp::AddressMode mode)
  {
    switch (mode)
    {
    case comp::ADDR_MODE_ABSOLUTE_X:
    {
      const auto address = ReadWord(registers.pc + 1);
      return ((address + registers.x) & 0xFF00) != (address & 0xFF00);
    }
    case comp::ADDR_MODE_ABSOLUTE_Y:
    {
      const auto address = ReadWord(registers.pc + 1);
      return ((address + registers.y) & 0xFF00) != (address & 0xFF00);
    }
    case comp::ADDR_MODE_INDIRECT_Y:
    {
      auto address = ReadByte(registers.pc + 1);
      return (address & 0xFF00) != ((ReadWordBug(address) + registers.y) & 0xFF00);
    }
    default:
      break;
    }

    return false;
  }
}