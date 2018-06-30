#include "NES.h"

nemus::NES::NES() {
    m_ppu = new core::PPU();
    m_input = new core::Input();
    m_screen = new ui::Screen(m_ppu, this, m_input, nullptr);
}

nemus::NES::~NES() {
    delete m_logger;
    delete m_ppu;
    delete m_memory;
    delete m_cpu;
    delete m_screen;
    delete m_input;
}

void nemus::NES::run() {
    const double clockRatio = 1788908.0 / 60.0;
    int updateCounter = 0;

    while(!m_screen->getQuit()) {
        if (m_gameLoaded && m_cpu->isRunning()) {
            int cycles = m_cpu->tick();

            for (int i = 0; i < cycles * 3; i++) {
                m_ppu->tick();
            }

            updateCounter += cycles * 3;

            if (static_cast<long>(updateCounter) * m_speedmodifier > (clockRatio)) {
                m_screen->updateFPS();
                m_screen->updateWindow();
                updateCounter = 0;
            }
        } else {
            m_screen->updateWindow();
        }
    }
}

void nemus::NES::loadGame(std::string filename) {
    reset();

    m_logger = new debug::Logger();
    m_logger->disable();

    m_memory = new core::Memory(m_logger, m_ppu, m_input, filename);

    m_cpu = new core::CPU(m_memory, m_logger);

    m_ppu->setCPU(m_cpu);

    m_ppu->setMemory(m_memory);

    m_gameLoaded = true;
}

void nemus::NES::reset() {
    delete m_logger;
    delete m_memory;
    delete m_cpu;

    m_ppu->reset();
}
