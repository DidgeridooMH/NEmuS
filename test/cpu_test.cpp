#include <gtest/gtest.h>

#define private public
#include <Core/CPU.h>
#undef private

class CPUTest : public ::testing::Test
{
protected:
  void SetUp(const std::vector<char> &gameData)
  {
    memory = std::make_unique<nemus::core::Memory>(&logger, &ppu, &input, gameData);
    cpu = std::make_unique<nemus::core::CPU>(memory.get(), &logger);
  }

  std::unique_ptr<nemus::core::CPU> cpu;

private:
  std::unique_ptr<nemus::core::Memory> memory;
  nemus::core::PPU ppu;
  nemus::core::Input input;
  nemus::debug::Logger logger;
};

TEST_F(CPUTest, AddWithCarryImmediateTest)
{
  std::vector<char> rom(0x8000 + 0x2000 + 0x10);
  rom[0x10] = 0x69;
  rom[0x11] = 0x05;
  SetUp(rom);

  cpu->m_reg.pc = 0x8000;
  cpu->m_reg.a = 0x0;
  cpu->m_reg.p.carry = 0;

  cpu->tick();

  ASSERT_EQ(cpu->m_reg.a, 0x05);
  ASSERT_EQ(cpu->m_reg.pc, 0x8002);
  ASSERT_EQ(cpu->m_reg.p.carry, 0);
  ASSERT_EQ(cpu->m_reg.p.zero, 0);
  ASSERT_EQ(cpu->m_reg.p.overflow, 0);
  ASSERT_EQ(cpu->m_reg.p.negative, 0);
}

TEST_F(CPUTest, AddWithCarryImmediateCarryTest)
{
  std::vector<char> rom(0x8000 + 0x2000 + 0x10);
  rom[0x10] = 0x69;
  rom[0x11] = 0x05;
  SetUp(rom);

  cpu->m_reg.pc = 0x8000;
  cpu->m_reg.a = 0x0;
  cpu->m_reg.p.carry = 1;

  cpu->tick();

  ASSERT_EQ(cpu->m_reg.a, 0x06);
  ASSERT_EQ(cpu->m_reg.pc, 0x8002);
  ASSERT_EQ(cpu->m_reg.p.carry, 0);
  ASSERT_EQ(cpu->m_reg.p.zero, 0);
  ASSERT_EQ(cpu->m_reg.p.overflow, 0);
  ASSERT_EQ(cpu->m_reg.p.negative, 0);
}
