#pragma once

#include <QWidget>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <UI/Debug/PatternTableImage.hpp>
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

  private:
    // These are all automatically free'd when the parent window is destroyed.
    QGroupBox *m_groupBox[2];
    PatternTableImage *m_table[2];
    QHBoxLayout *m_layout;

    core::Memory *m_memory;
  };
};
