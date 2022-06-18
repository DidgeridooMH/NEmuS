#include <gtest/gtest.h>

#define private public
#include <Core/Memory.h>
#undef private

constexpr size_t NROMRomSize = 0x8000UL + 0x2000UL + 0x10UL;

class MemoryTest : public ::testing::Test
{
protected:
  void SetUp() override {}
  void TearDown() override
  {
    memory = nullptr;
  }

  void SetUp(const std::vector<char> &data)
  {
    memory = std::make_unique<nemus::core::Memory>(&logger, &ppu, &input, data);
  }

  std::unique_ptr<nemus::core::Memory> memory;

private:
  nemus::debug::Logger logger;
  nemus::core::PPU ppu;
  nemus::core::Input input;
};

TEST_F(MemoryTest, SensibleConstruction)
{
  std::vector<char> gameData(NROMRomSize);
  SetUp(gameData);

  ASSERT_NE(memory->m_mapper, nullptr);
  ASSERT_GT(memory->m_nameTables[0].size(), 0);
  ASSERT_GT(memory->m_nameTables[1].size(), 0);
  ASSERT_GT(memory->m_nameTables[2].size(), 0);
  ASSERT_GT(memory->m_nameTables[3].size(), 0);
  ASSERT_GT(memory->m_palette.size(), 0);
  ASSERT_GT(memory->m_ram.size(), 0);
}
