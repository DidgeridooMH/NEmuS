#include "NES.h"

nemus::NES::NES() {
    m_logger = new debug::Logger();
    m_logger->disable();

    m_ppu = new core::PPU();

    m_memory = new core::Memory(m_logger, m_ppu);

    m_cpu = new core::CPU(m_memory, m_logger);

    m_screen = new nemus::ui::Screen(m_ppu);

    m_ppu->setCPU(m_cpu);

    m_ppu->setMemory(m_memory);
}

nemus::NES::~NES() {
    delete m_logger;
    delete m_ppu;
    delete m_memory;
    delete m_cpu;
    delete m_screen;
}

void nemus::NES::run() {
    double clockRatio = 1788908.0 / 60.0;
    int updateCounter = 0;

    while(m_cpu->isRunning() && !m_screen->getQuit()) {
        int cycles = m_cpu->tick();

        for(int i = 0; i < cycles * 3; i++) {
            m_ppu->tick();
        }

        updateCounter += cycles * 3;

        if(updateCounter * m_speedmodifier > (clockRatio)) {
            m_screen->update();
            updateCounter = 0;
        }
    }
}