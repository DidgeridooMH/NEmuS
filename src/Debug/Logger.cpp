#include <iomanip>
#include <sstream>
#include "Logger.h"

nemus::debug::Logger::Logger() {
    m_fileOut.open("instructions.log");

    m_fileBuffer = new char[FILE_BUFFER_SIZE];

    m_bufferPtr = 0;
}

nemus::debug::Logger::~Logger() {
    m_fileOut.write(m_fileBuffer, FILE_BUFFER_SIZE);

    if(m_fileOut.is_open()) {
        m_fileOut.close();
    }
}

void nemus::debug::Logger::write(std::string message) {
    if(!m_enable) {
        return;
    }

    std::stringstream stringBuilder;

    stringBuilder << message << std::endl;

    m_fileOut.write(stringBuilder.str().c_str(), stringBuilder.str().size());
}

void nemus::debug::Logger::writeError(std::string message, unsigned int address) {
    if(!m_enable) {
        return;
    }

    std::stringstream stringBuilder;

    stringBuilder << "ERROR at $" << std::hex << address << std::dec << ": " << message << std::endl;

    m_fileOut.write(stringBuilder.str().c_str(), stringBuilder.str().size());
}

void nemus::debug::Logger::writeInstruction(comp::Registers registers,
                                            std::string instruction,
                                            unsigned int result,
                                            comp::AddressMode mode) {
    if(!m_enable) {
        return;
    }

    std::stringstream stringBuilder;

    stringBuilder << std::hex << std::uppercase
              << std::setfill('0') << std::setw(4) << registers.pc
              << "\tA:" << std::setfill('0') << std::setw(2) << registers.a
              << " X:"  << std::setfill('0') << std::setw(2) << registers.x
              << " Y:"  << std::setfill('0') << std::setw(2) << registers.y
              << " SP:" << std::setfill('0') << std::setw(2) << registers.sp
              << " P:"  << std::setfill('0') << std::setw(2) << registers.p
              << " " << instruction
              << " " << result << std::dec << " ";

    switch(mode) {
        case comp::ADDR_MODE_IMMEDIATE:
            stringBuilder << "IMMEDIATE MODE" << std::endl;
            break;
        case comp::ADDR_MODE_ZERO_PAGE:
            stringBuilder << "ZEROPAGE MODE" << std::endl;
            break;
        case comp::ADDR_MODE_ZERO_PAGE_X:
            stringBuilder << "ZEROPAGE INDEXED X MODE" << std::endl;
            break;
        case comp::ADDR_MODE_ABSOLUTE:
            stringBuilder << "ABSOLUTE MODE" << std::endl;
            break;
        case comp::ADDR_MODE_ABSOLUTE_X:
            stringBuilder << "ABSOLUTE MODE X" << std::endl;
            break;
        case comp::ADDR_MODE_ABSOLUTE_Y:
            stringBuilder << "ABSOLUTE MODE Y" << std::endl;
            break;
        case comp::ADDR_MODE_INDIRECT_X:
            stringBuilder << "INDIRECT X" << std::endl;
            break;
        case comp::ADDR_MODE_INDIRECT_Y:
            stringBuilder << "INDIRECT Y" << std::endl;
            break;
        default:
            stringBuilder << "ADDR UNKNOWN" << std::endl;
            break;
    }
    
    m_fileOut.write(stringBuilder.str().c_str(), stringBuilder.str().size());
}