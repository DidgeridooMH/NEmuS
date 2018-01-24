#ifndef NEMUS_NES_H
#define NEMUS_NES_H

#include "include/ComponentHelper.h"
#include "Memory.h"
#include "CPU.h"
#include "../UI/Screen.h"

namespace nemus {
    class NES {
    private:
        comp::State m_state = comp::STOP;

        core::CPU* m_cpu = nullptr;

        core::Memory* m_memory = nullptr;

        core::PPU* m_ppu = nullptr;

        debug::Logger* m_logger = nullptr;

        ui::Screen* m_screen = nullptr;

    public:
        NES();

        ~NES();

        comp::State getState() { return m_state; }

        void setState(comp::State state) {m_state = state;}

        void run();

    };
}


#endif //NEMUS_NES_H
