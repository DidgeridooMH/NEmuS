#include <UI/Debug/Debugger.hpp>

namespace nemus::ui
{
  Debugger::Debugger(QWidget *parent)
      : QWidget(parent),
        m_registerLayout(std::make_unique<QFormLayout>()),
        m_pcRegisterLine(std::make_unique<QLineEdit>("$0000"))
  {
    m_registerLayout->addRow(tr("&PC:"), m_pcRegisterLine.get());
    setLayout(m_registerLayout.get());

    setWindowTitle(tr("Debugger"));

    setFocusPolicy(Qt::ClickFocus);
    show();
  }
} // namespace nemus::ui
