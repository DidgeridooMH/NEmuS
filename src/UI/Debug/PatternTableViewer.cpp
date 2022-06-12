#include <QPainter>

#include <UI/Debug/PatternTableViewer.hpp>

namespace nemus::ui
{
  constexpr uint32_t SCREEN_WIDTH = 256;
  constexpr uint32_t SCREEN_HEIGHT = 256;

  PatternTableViewer::PatternTableViewer(core::Memory *memory, QWidget *parent)
      : QWidget(parent),
        m_patternTable0(SCREEN_WIDTH * SCREEN_HEIGHT),
        m_memory(memory)
  {
    setWindowTitle(tr("Pattern Table"));
    setFixedSize({SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2});

    setFocusPolicy(Qt::ClickFocus);
    show();
  }

  void PatternTableViewer::paintEvent(QPaintEvent *)
  {
    QPainter painter(this);

    painter.fillRect(rect(), Qt::black);

    for (auto y = 0U; y < SCREEN_HEIGHT; y++)
    {
      for (auto x = 0U; x < SCREEN_WIDTH; x++)
      {
        auto col = y / 8;
        auto row = x / 8;
        auto fineX = x % 8;
        auto fineY = y % 8;

        auto address = fineY | col << 4 | row << 8;

        auto plane0 = m_memory->readPPUByte(address);
        auto plane1 = m_memory->readPPUByte(address | 0x8);
        auto bit0 = (plane0 >> (7 - fineX)) & 1;
        auto bit1 = (plane1 >> (7 - fineX)) & 1;
        switch (bit0 | (bit1 << 1))
        {
        case 0:
          m_patternTable0[x + (y * 256)] = PPU_COLOR_BLACK;
          break;
        case 1:
          m_patternTable0[x + (y * 256)] = PPU_COLOR_RED;
          break;
        case 2:
          m_patternTable0[x + (y * 256)] = PPU_COLOR_BLUE;
          break;
        case 3:
          m_patternTable0[x + (y * 256)] = PPU_COLOR_WHITE;
          break;
        }
      }
    }

    QImage image(reinterpret_cast<uint8_t *>(m_patternTable0.data()),
                 SCREEN_WIDTH, SCREEN_HEIGHT, QImage::Format_ARGB32);

    painter.drawPixmap(0, 0, QPixmap::fromImage(image).scaled(SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2));
  }
} // namespace nemus::ui
