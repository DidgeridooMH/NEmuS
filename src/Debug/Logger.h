#ifndef NEMUS_LOGGER_H
#define NEMUS_LOGGER_H

#include <string>
#include <fstream>
#include "../Core/ComponentHelper.h"

namespace nemus::debug
{
    class Logger
    {
    public:
        Logger();

        void enable()
        {
            m_enable = true;
            if (!m_fileOut.is_open())
            {
                m_fileOut = std::ofstream("instructions.log");
            }
        }

        void write(std::string message);
        void writeError(std::string message, unsigned int address);
        void writeInstruction(comp::Registers registers,
                              std::string instruction,
                              unsigned int result,
                              comp::AddressMode mode);

    private:
        std::ofstream m_fileOut;
        bool m_enable;
    };
}

#endif