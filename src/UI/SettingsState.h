#ifndef NEMUS_SETTINGS_STATE_H
#define NEMUS_SETTINGS_STATE_H

enum Scale {
    SCALE_1X = 0,
    SCALE_2X = 1,
    SCALE_3X = 2,
    SCALE_4X = 3
};

namespace nemus::ui {
    class SettingsState {
    private:
        Scale m_scale;

    public:
        SettingsState(Scale scale) : m_scale(scale) {}

        Scale getScale() { return m_scale; }

        void setState(Scale scale) {
            m_scale = scale;
        }
    };
}

#endif
