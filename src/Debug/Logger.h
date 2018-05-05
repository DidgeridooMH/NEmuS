#ifndef NEMUS_LOGGER_H
#define NEMUS_LOGGER_H

#include <string>
#include <fstream>
#include "../Core/include/ComponentHelper.h"

namespace nemus::debug {

    class Logger {
    private:
        std::ofstream m_fileOut;
        bool m_enable = true;

    public:
        Logger();
        ~Logger();

        void disable() { m_enable = false; }

        void write(std::string message);
        void writeError(std::string message, unsigned int address);
        void writeInstruction(comp::Registers registers,
                              std::string instruction,
                              unsigned int result,
                              comp::AddressMode mode);
    };

}

#endif