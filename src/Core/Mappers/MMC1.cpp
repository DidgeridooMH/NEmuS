#include <cstring>
#include "MMC1.h"

nemus::core::MMC1::MMC1(const std::vector<char> &gameData, char *savStart, size_t savSize)
    : MMC1(gameData)
{
  memcpy(m_CPUMemory.data(), savStart, std::min(SaveRomSize, savSize));
}

nemus::core::MMC1::MMC1(const std::vector<char> &gameData)
    : m_CPUMemory(static_cast<size_t>(gameData[4]) * RomBankSize + PPUCharRomSize),
      m_PPUMemory(PPUMemorySize),
      m_tableA(NameTableSize),
      m_tableB(NameTableSize),
      m_tableC(NameTableSize),
      m_tableD(NameTableSize),
      m_prgBank0(0),
      m_prgBank1(gameData[4] - 1),
      m_chrBank0(0),
      m_chrBank1(0),
      m_maxPrgBanks(gameData[4]),
      m_shiftRegister(InitialShiftRegister),
      m_control(MMC1Control{
          .mirroring = static_cast<uint8_t>(MirrorMode::Horizontal),
          .prg_mode = 0,
          .chr_mode = 0}),
      m_prgBank(0),
      m_chrBank(0)
{
  memcpy(&m_CPUMemory[0x2000], &gameData[INESRomHeaderSize],
         std::min(gameData.size() - INESRomHeaderSize, m_CPUMemory.size()));

  if (gameData[5] > 0)
  {
    memcpy(m_PPUMemory.data(), &gameData[gameData[4] * RomBankSize + INESRomHeaderSize], PPUCharRomSize);
  }

  m_prgBank1 = gameData[4] - 1;
  m_maxPrgBanks = gameData[4];
}

// TODO: Maybe need OS frames?
uint32_t nemus::core::MMC1::getMirroringTable(uint32_t address)
{
  switch (static_cast<MirrorMode>(m_control.mirroring))
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

void nemus::core::MMC1::writeNametable(uint8_t data, uint32_t address)
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

uint8_t nemus::core::MMC1::readNametable(uint32_t address)
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

uint8_t nemus::core::MMC1::readByte(uint32_t address)
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

uint8_t nemus::core::MMC1::readBytePPU(uint32_t address)
{
  if (address >= 0x2000 && address < 0x3000)
  {
    return readNametable(address);
  }

  return m_PPUMemory[address];
}

void nemus::core::MMC1::writeByte(uint8_t data, uint32_t address)
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

void nemus::core::MMC1::writeBytePPU(uint8_t data, uint32_t address)
{
  if (address >= 0x2000 && address < 0x3000)
  {
    writeNametable(data, address);
  }
  else
  {
    m_PPUMemory[address] = data;
  }
}

void nemus::core::MMC1::adjustShiftRegister(uint8_t data, uint32_t address)
{
  if (data & 0x80)
  {
    m_control = {.mirroring = m_control.mirroring,
                 .prg_mode = 3,
                 .chr_mode = m_control.chr_mode};

    m_shiftRegister = InitialShiftRegister;

    updateBanks();
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
    m_control.mirroring = (uint8_t)MirrorMode::OsLower;
    break;
  case 1:
    m_control.mirroring = (uint8_t)MirrorMode::OsUpper;
    break;
  case 2:
    m_control.mirroring = (uint8_t)MirrorMode::Vertical;
    break;
  case 3:
    m_control.mirroring = (uint8_t)MirrorMode::Horizontal;
    break;
  }

  m_control.prg_mode = (m_shiftRegister >> 2) & 3;

  m_control.chr_mode = (m_shiftRegister >> 4) & 1;

  updateBanks();

  m_shiftRegister = InitialShiftRegister;
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

  m_shiftRegister = InitialShiftRegister;
}

void nemus::core::MMC1::writeCHRBank1()
{
  m_chrBank1 = m_shiftRegister;

  m_shiftRegister = InitialShiftRegister;
}

void nemus::core::MMC1::writePRGBank()
{
  m_prgBank = m_shiftRegister & 0xF;
  updateBanks();
  m_shiftRegister = InitialShiftRegister;
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
