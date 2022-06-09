#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include "Settings.h"

nemus::ui::Settings::Settings(QWidget *parent, SettingsState *state) : QDialog(parent), m_state(state)
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(QMargins(5, 5, 5, 5));

    createScaleGroup(state->getScale());

    QPushButton *apply = new QPushButton;
    apply->setText(tr("Apply"));

    layout->addWidget(m_scaleGroup);
    layout->addWidget(apply);
    centralWidget->setLayout(layout);

    connect(apply, SIGNAL(released()), this, SLOT(applySettings()));

    setWindowTitle(tr("Settings"));
    resize(200, 200);
}

void nemus::ui::Settings::applySettings()
{
    Scale scale = SCALE_1X;

    if (m_timesTwo->isChecked())
    {
        scale = SCALE_2X;
    }
    else if (m_timesThree->isChecked())
    {
        scale = SCALE_3X;
    }
    else if (m_timesFour->isChecked())
    {
        scale = SCALE_4X;
    }

    m_state->setState(scale);

    this->close();
}

QGroupBox *nemus::ui::Settings::createScaleGroup(Scale def)
{
    m_scaleGroup = new QGroupBox(tr("Screen Scale"));

    m_timesOne = new QRadioButton(tr("1x"));
    m_timesTwo = new QRadioButton(tr("2x"));
    m_timesThree = new QRadioButton(tr("3x"));
    m_timesFour = new QRadioButton(tr("4x"));

    switch (def)
    {
    case SCALE_1X:
        m_timesOne->setChecked(true);
        break;
    case SCALE_2X:
        m_timesTwo->setChecked(true);
        break;
    case SCALE_3X:
        m_timesThree->setChecked(true);
        break;
    case SCALE_4X:
        m_timesFour->setChecked(true);
        break;
    }

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_timesOne);
    layout->addWidget(m_timesTwo);
    layout->addWidget(m_timesThree);
    layout->addWidget(m_timesFour);
    layout->addStretch(1);
    m_scaleGroup->setLayout(layout);

    return m_scaleGroup;
}
