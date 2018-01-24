#include <iostream>
#include "Logger.h"

nemus::debug::Logger::Logger() {
    m_fileOut.open("instructions.log");
}

nemus::debug::Logger::~Logger() {
    if(m_fileOut.is_open()) { m_fileOut.close(); }
}

void nemus::debug::Logger::write(std::string message) {
    m_fileOut << message << std::endl;
}

void nemus::debug::Logger::writeError(std::string message, unsigned int address) {
    m_fileOut << "ERROR at $" << std::hex << address << std::dec << ": " << message << std::endl;
}

void nemus::debug::Logger::writeInstruction(comp::Registers registers,
                                            std::string instruction,
                                            unsigned int result,
                                            comp::AddressMode mode) {
    m_fileOut << std::hex
              << "A: " << registers.a
              << " X: " << registers.x
              << " Y: " << registers.y
              << " SP: " << registers.sp
              << " $" << registers.pc
              << ": " << instruction
              << " " << result << std::dec << " ";

    switch(mode) {
        case comp::ADDR_MODE_IMMEDIATE:
            m_fileOut << "IMMEDIATE MODE" << std::endl;
            break;
        case comp::ADDR_MODE_ZERO_PAGE:
            m_fileOut << "ZEROPAGE MODE" << std::endl;
            break;
        case comp::ADDR_MODE_ZERO_PAGE_X:
            m_fileOut << "ZEROPAGE INDEXED X MODE" << std::endl;
            break;
        case comp::ADDR_MODE_ABSOLUTE:
            m_fileOut << "ABSOLUTE MODE" << std::endl;
            break;
        case comp::ADDR_MODE_ABSOLUTE_X:
            m_fileOut << "ABSOLUTE MODE X" << std::endl;
            break;
        case comp::ADDR_MODE_ABSOLUTE_Y:
            m_fileOut << "ABSOLUTE MODE Y" << std::endl;
            break;
        case comp::ADDR_MODE_INDIRECT_X:
            m_fileOut << "INDIRECT X" << std::endl;
            break;
        case comp::ADDR_MODE_INDIRECT_Y:
            m_fileOut << "INDIRECT Y" << std::endl;
            break;
        default:
            m_fileOut << "ADDR UNKNOWN" << std::endl;
            break;
    }
}