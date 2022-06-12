#include <QPainter>

#include <UI/Debug/PatternTableImage.hpp>

namespace nemus::ui
{
  PatternTableImage::PatternTableImage(uint32_t tableId, core::Memory *memory, QWidget *parent)
      : QWidget(parent),
        m_patternTable(ScreenWidth * ScreenHeight),
        m_tableId(tableId),
        m_memory(memory)
  {
    setFixedSize({ScreenWidth * 2, ScreenHeight * 2});
  }

  void PatternTableImage::paintEvent(QPaintEvent *)
  {
    QPainter painter(this);

    if (m_memory != nullptr)
    {
      for (auto y = 0U; y < ScreenHeight; y++)
      {
        for (auto x = 0U; x < ScreenWidth; x++)
        {
          auto col = x / 8;
          auto row = y / 8;
          auto fineX = x % 8;
          auto fineY = y % 8;

          auto address = fineY | col << 4 | row << 8 | m_tableId << 12;

          auto plane0 = m_memory->readPPUByte(address);
          auto plane1 = m_memory->readPPUByte(address | 0x8);
          auto bit0 = (plane0 >> (7 - fineX)) & 1;
          auto bit1 = (plane1 >> (7 - fineX)) & 1;
          switch (bit0 | (bit1 << 1))
          {
          case 0:
            m_patternTable[x + (y * ScreenWidth)] = PPU_COLOR_BLACK;
            break;
          case 1:
            m_patternTable[x + (y * ScreenWidth)] = PPU_COLOR_RED;
            break;
          case 2:
            m_patternTable[x + (y * ScreenWidth)] = PPU_COLOR_BLUE;
            break;
          case 3:
            m_patternTable[x + (y * ScreenWidth)] = PPU_COLOR_WHITE;
            break;
          }
        }
      }
    }

    QImage image(reinterpret_cast<uint8_t *>(m_patternTable.data()),
                 ScreenWidth, ScreenHeight, QImage::Format_ARGB32);

    painter.drawPixmap(0, 0, QPixmap::fromImage(image).scaled(ScreenWidth * 2, ScreenHeight * 2));
  }
} // namespace nemus::ui
