#include "Input.h"

nemus::core::Input::Input() {
    for(int i = 0; i < 8; i++) {
        m_buttons[i] = false;
    }

    m_currentButton = 0;

    m_strobe = false;
}

void nemus::core::Input::setButton(int button) {
    m_buttons[button] = true;
}

void nemus::core::Input::unsetButton(int button) {
    m_buttons[button] = false;
}

unsigned char nemus::core::Input::read() {
    unsigned char ret = 0;

    if(m_currentButton < 8 && m_buttons[m_currentButton]) {
        ret = 1;
    }

    m_currentButton++;

    if(m_strobe) {
        m_currentButton = 0;
    }

    return ret;
}

void nemus::core::Input::write(unsigned char value) {
    m_strobe = value;

    if(m_strobe) {
        m_currentButton = 0;
    }
}
