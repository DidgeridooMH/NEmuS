#include <chrono>
#include <thread>
#include <fmt/printf.h>

#include "NES.h"

nemus::NES::NES()
{
    m_ppu = new core::PPU();
    m_input = new core::Input();
    m_screen = new ui::Screen(m_ppu, this, m_input, nullptr);
}

nemus::NES::~NES()
{
    delete m_logger;
    delete m_ppu;
    delete m_memory;
    delete m_cpu;
    delete m_screen;
    delete m_input;
}

void nemus::NES::run()
{
    auto frameStart = std::chrono::steady_clock::now();
    constexpr double cpuPerFrame = 29780.5;
    int updateCounter = 0;

    while (!m_screen->getQuit())
    {
        if (m_gameLoaded && m_cpu->isRunning() && !m_paused)
        {
            int cycles = m_cpu->tick();

            for (int i = 0; i < cycles * 3; i++)
            {
                m_ppu->tick();
            }

            updateCounter += cycles;
            if (static_cast<long>(updateCounter) > cpuPerFrame)
            {
                m_screen->updateFPS();
                m_screen->updateWindow();
                int64_t frameDelta = 0L;
                do
                {
                    auto frameEnd = std::chrono::steady_clock::now();
                    auto frameTime = std::chrono::duration_cast<std::chrono::microseconds>(frameEnd - frameStart).count();
                    frameDelta = 16000L - frameTime;
                    std::this_thread::yield();
                } while (frameDelta > 0L);
                frameStart = std::chrono::steady_clock::now();

                updateCounter = 0;
            }
        }
        else
        {
            m_screen->updateWindow();
        }
    }
}

void nemus::NES::loadGame(const std::vector<char> &gameData)
{
    reset();

    m_logger = new debug::Logger();
    // m_logger->enable();

    m_memory = new core::Memory(m_logger, m_ppu, m_input, gameData);

    m_cpu = new core::CPU(m_memory, m_logger);

    m_ppu->setCPU(m_cpu);

    m_ppu->setMemory(m_memory);

    m_gameLoaded = true;
}

void nemus::NES::reset()
{
    delete m_logger;
    delete m_memory;
    delete m_cpu;

    m_ppu->reset();
}
