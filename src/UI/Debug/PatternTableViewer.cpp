#include <QPainter>

#include <UI/Debug/PatternTableViewer.hpp>

namespace nemus::ui
{
  PatternTableViewer::PatternTableViewer(core::Memory *memory, QWidget *parent)
      : QWidget(parent),
        m_memory(memory)
  {
    m_groupBox[0] = new QGroupBox(tr("Pattern Table 0"), this);
    m_groupBox[1] = new QGroupBox(tr("Pattern Table 1"), this);

    m_table[0] = new PatternTableImage(0, memory);
    m_table[1] = new PatternTableImage(1, memory);

    auto groupBoxLayout = new QVBoxLayout;
    groupBoxLayout->addWidget(m_table[0]);
    groupBoxLayout->addStretch(1);
    m_groupBox[0]->setLayout(groupBoxLayout);

    groupBoxLayout = new QVBoxLayout;
    groupBoxLayout->addWidget(m_table[1]);
    groupBoxLayout->addStretch(1);
    m_groupBox[1]->setLayout(groupBoxLayout);

    m_layout = new QHBoxLayout;
    m_layout->setContentsMargins(QMargins(5, 5, 5, 5));
    m_layout->addWidget(m_groupBox[0]);
    m_layout->addWidget(m_groupBox[1]);
    setLayout(m_layout);

    setWindowTitle(tr("Pattern Tables"));

    setFocusPolicy(Qt::ClickFocus);
    show();
  }
} // namespace nemus::ui
