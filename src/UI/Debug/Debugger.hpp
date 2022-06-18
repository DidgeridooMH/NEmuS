#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QFormLayout>

namespace nemus::ui
{
  class Debugger : public QWidget
  {
    Q_OBJECT

  public:
    Debugger(QWidget *parent = nullptr);

  private:
    std::unique_ptr<QFormLayout> m_registerLayout;
    std::unique_ptr<QLineEdit> m_pcRegisterLine;
  };
} // namespace nemus::ui
