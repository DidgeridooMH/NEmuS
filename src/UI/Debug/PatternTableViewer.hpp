#pragma once

#include <QWidget>

#include <Core/PPU.h>
#include <Core/Memory.h>

namespace nemus::ui
{
  class PatternTableViewer : public QWidget
  {
    Q_OBJECT

  public:
    // TODO: because memory is reinitialized when a game is reset, this will leave a corrupted pointer here.
    PatternTableViewer(core::Memory *memory, QWidget *parent);

    void paintEvent(QPaintEvent *event);

  private:
    std::vector<uint32_t> m_patternTable0;
    core::Memory *m_memory;
  };
};
