#include <fmt/format.h>

#include "Logger.h"

namespace nemus::debug
{
    Logger::Logger()
        : m_enable(false)
    {
    }

    void Logger::write(std::string message)
    {
        if (!m_enable)
        {
            return;
        }

        m_fileOut << fmt::format("{}", message) << std::endl;
    }

    void Logger::writeError(std::string message, unsigned int address)
    {
        if (!m_enable)
        {
            return;
        }

        m_fileOut << fmt::format("ERROR at ${:X}: {}", address, message) << std::endl;
    }

    void Logger::writeInstruction(comp::Registers registers,
                                  std::string instruction,
                                  unsigned int result,
                                  comp::AddressMode mode)
    {
        if (!m_enable)
        {
            return;
        }

        m_fileOut << fmt::format("{:04X}\tA:{:02X} X:{:04X} Y:{:04X} SP:{:04X} P:{:04X} {} {} {}",
                                 registers.pc, registers.a, registers.x, registers.y, registers.sp, registers.pFull,
                                 instruction, result, comp::AddressModeToString(mode))
                  << std::endl;
    }
}