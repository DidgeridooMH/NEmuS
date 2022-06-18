#include <gtest/gtest.h>

#define private public
#include <Core/Mappers/NROM.h>
#undef private

constexpr size_t NROMRomSize = 0x8000UL + 0x2000UL + 0x10UL;

TEST(MapperTest, NROMSensibleConstruction)
{
  std::vector<char> rom(NROMRomSize);
  nemus::core::NROM mapper(rom);

  ASSERT_GT(mapper.m_fixedCPUMemory.size(), 0);
  ASSERT_GT(mapper.m_fixedPPUMemory.size(), 0);
  ASSERT_EQ(mapper.m_mirroring, nemus::core::MirrorMode::Horizontal);
}

TEST(MapperTest, NROMReadByteZeroIndex)
{
  std::vector<char> rom(NROMRomSize);
  rom[0x10] = static_cast<char>(0xFE);
  nemus::core::NROM mapper(rom);

  auto result = mapper.ReadByte(0x8000);
  ASSERT_EQ(result, 0xFE);
}

TEST(MapperTest, NROMReadByteLastIndex)
{
  std::vector<char> rom(NROMRomSize);
  rom[0x800F] = static_cast<char>(0xFE);
  nemus::core::NROM mapper(rom);

  auto result = mapper.ReadByte(0xFFFF);
  ASSERT_EQ(result, 0xFE);
}

TEST(MapperTest, NROMReadByteOdd)
{
  std::vector<char> rom(NROMRomSize);
  rom[0x21] = static_cast<char>(0xFE);
  nemus::core::NROM mapper(rom);

  auto result = mapper.ReadByte(0x8011);
  ASSERT_EQ(result, 0xFE);
}

TEST(MapperTest, NROMReadByteEven)
{
  std::vector<char> rom(NROMRomSize);
  rom[0x22] = static_cast<char>(0xFE);
  nemus::core::NROM mapper(rom);

  auto result = mapper.ReadByte(0x8012);
  ASSERT_EQ(result, 0xFE);
}

TEST(MapperTest, NROMWriteByteOdd)
{
  std::vector<char> rom(NROMRomSize);
  nemus::core::NROM mapper(rom);

  mapper.WriteByte(0xFE, 0x6001);
  auto result = mapper.ReadByte(0x6001);
  ASSERT_EQ(result, 0xFE);
}

TEST(MapperTest, NROMWriteByteEven)
{
  std::vector<char> rom(NROMRomSize);
  nemus::core::NROM mapper(rom);

  mapper.WriteByte(0xFE, 0x6000);
  auto result = mapper.ReadByte(0x6000);
  ASSERT_EQ(result, 0xFE);
}

TEST(MapperTest, NROMRamMirroringHighWrite)
{
  std::vector<char> rom(NROMRomSize);
  nemus::core::NROM mapper(rom);

  mapper.WriteByte(0xFE, 0x7001);
  auto result = mapper.ReadByte(0x6001);
  ASSERT_EQ(result, 0xFE);
}

TEST(MapperTest, NROMRamMirroringLowWrite)
{
  std::vector<char> rom(NROMRomSize);
  nemus::core::NROM mapper(rom);

  mapper.WriteByte(0xFE, 0x7001);
  auto result = mapper.ReadByte(0x6001);
  ASSERT_EQ(result, 0xFE);
}

TEST(MapperTest, NROMPPUReadFirst)
{
  std::vector<char> rom(NROMRomSize);
  rom[0x10 + 0x8000] = static_cast<char>(0xFE);
  nemus::core::NROM mapper(rom);

  auto result = mapper.ReadBytePPU(0x0000);
  ASSERT_EQ(result, 0xFE);
}

TEST(MapperTest, NROMPPUReadLast)
{
  std::vector<char> rom(NROMRomSize);
  *(rom.end() - 1) = static_cast<char>(0xFE);
  nemus::core::NROM mapper(rom);

  auto result = mapper.ReadBytePPU(0x1FFF);
  ASSERT_EQ(result, 0xFE);
}

TEST(MapperTest, NROMHorizontalMirrorFirst)
{
  std::vector<char> rom(NROMRomSize);
  nemus::core::NROM mapper(rom);

  auto result = mapper.GetMirroringTable(0x2000);
  ASSERT_EQ(result, 0);
}

TEST(MapperTest, NROMHorizontalMirrorSecond)
{
  std::vector<char> rom(NROMRomSize);
  nemus::core::NROM mapper(rom);

  auto result = mapper.GetMirroringTable(0x2400);
  ASSERT_EQ(result, 0);
}

TEST(MapperTest, NROMHorizontalMirrorThird)
{
  std::vector<char> rom(NROMRomSize);
  nemus::core::NROM mapper(rom);

  auto result = mapper.GetMirroringTable(0x2800);
  ASSERT_EQ(result, 1);
}

TEST(MapperTest, NROMHorizontalMirrorFour)
{
  std::vector<char> rom(NROMRomSize);
  nemus::core::NROM mapper(rom);

  auto result = mapper.GetMirroringTable(0x2C00);
  ASSERT_EQ(result, 1);
}
TEST(MapperTest, NROMVerticalMirrorFirst)
{
  std::vector<char> rom(NROMRomSize);
  rom[6] |= 1;
  nemus::core::NROM mapper(rom);

  auto result = mapper.GetMirroringTable(0x2000);
  ASSERT_EQ(result, 0);
}

TEST(MapperTest, NROMVerticalMirrorSecond)
{
  std::vector<char> rom(NROMRomSize);
  rom[6] |= 1;
  nemus::core::NROM mapper(rom);

  auto result = mapper.GetMirroringTable(0x2400);
  ASSERT_EQ(result, 1);
}

TEST(MapperTest, NROMVerticalMirrorThird)
{
  std::vector<char> rom(NROMRomSize);
  rom[6] |= 1;
  nemus::core::NROM mapper(rom);

  auto result = mapper.GetMirroringTable(0x2800);
  ASSERT_EQ(result, 0);
}

TEST(MapperTest, NROMVerticalMirrorFour)
{
  std::vector<char> rom(NROMRomSize);
  rom[6] |= 1;
  nemus::core::NROM mapper(rom);

  auto result = mapper.GetMirroringTable(0x2C00);
  ASSERT_EQ(result, 1);
}