#ifndef INPUT_H
#define INPUT_H

#define BUTTON_A      0
#define BUTTON_B      1
#define BUTTON_START  2
#define BUTTON_SELECT 3
#define BUTTON_UP     4
#define BUTTON_DOWN   5
#define BUTTON_LEFT   6
#define BUTTON_RIGHT  7

namespace nemus::core {
    class Input {
    private:
        bool m_buttons[8];
        int  m_currentButton;
        bool m_strobe;

    public:
        Input();

        void setButton(int button);
        void unsetButton(int button);

        unsigned char read();
        void write(unsigned char value);
    };
}

#endif