#ifndef NEMUS_SETTINGS_H
#define NEMUS_SETTINGS_H

#include <QDialog>
#include <QGroupBox>
#include <QRadioButton>
#include "SettingsState.h"

namespace nemus::ui {
    class Settings : public QDialog {
       
    Q_OBJECT

    private:
        QGroupBox* m_scaleGroup;

        QRadioButton* m_timesOne;
        QRadioButton* m_timesTwo;
        QRadioButton* m_timesThree;
        QRadioButton* m_timesFour;

        SettingsState* m_state;

        QGroupBox* createScaleGroup(Scale def);

    public:
        Settings(QWidget* parent, SettingsState* state);

    signals:

    public slots:
        void applySettings();
    };
}

#endif