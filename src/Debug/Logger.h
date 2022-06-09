#ifndef NEMUS_LOGGER_H
#define NEMUS_LOGGER_H

#include <string>
#include <fstream>
#include "../Core/ComponentHelper.h"

#define FILE_BUFFER_SIZE 10000000

namespace nemus::debug {
    class Logger {
    private:
        std::ofstream m_fileOut;
        bool m_enable = false;

        char* m_fileBuffer;
        int m_bufferPtr;

    public:
        Logger();
        ~Logger();

        void enable() { m_enable = true; }

        void write(std::string message);
        void writeError(std::string message, unsigned int address);
        void writeInstruction(comp::Registers registers,
                              std::string instruction,
                              unsigned int result,
                              comp::AddressMode mode);
    };
}

#endif