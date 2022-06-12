#pragma once

#include <QWidget>

#include <Core/PPU.h>
#include <Core/Memory.h>

namespace nemus::ui
{
  class PatternTableImage : public QWidget
  {
    Q_OBJECT

  public:
    PatternTableImage(uint32_t tableId, core::Memory *memory, QWidget *parent = nullptr);

    void paintEvent(QPaintEvent *event);

  private:
    static constexpr uint32_t ScreenWidth = 0x10 * 8;
    static constexpr uint32_t ScreenHeight = 0x10 * 8;

    std::vector<uint32_t> m_patternTable;
    uint32_t m_tableId;
    core::Memory *m_memory;
  };
} // namespace nemus::ui
