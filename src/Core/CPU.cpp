#include <iostream>
#include <cstring>
#include <sstream>
#include "CPU.h"
#include "Memory.h"

nemus::core::CPU::CPU(Memory* memory, debug::Logger* logger) {
    generateOP();

    resetRegisters();

    m_interrupt = comp::INT_NONE;

    m_memory = memory;

    m_logger = logger;

    m_reg.pc = m_memory->readWord(0xFFFC);

    std::stringstream sstream;
    sstream << "Entry point: " << std::hex << m_reg.pc;

    m_logger->write(sstream.str());

    setFlags(0x34);

    m_logger->write("CPU Initialized");

    m_running = true;
}

int nemus::core::CPU::tick() {
    // NMI
    if(m_interrupt != comp::INT_NONE) {
        if(m_interrupt == comp::INT_NMI || !m_flags.I) {
            interrupt();
            return 0;
        }
    }

    unsigned int op = m_memory->readByte(m_reg.pc);

    switch(op) {
        // NOP
        case 0x04:
        case 0x44:
        case 0x64:
        case 0x0C:
        case 0x14:
        case 0x1A:
        case 0x1C:
        case 0x34:
        case 0x3A:
        case 0x3C:
        case 0x54:
        case 0x5A:
        case 0x5C:
        case 0x74:
        case 0x7A:
        case 0x7C:
        case 0xD4:
        case 0xDA:
        case 0xDC:
        case 0xF4:
        case 0xFA:
        case 0xFC:
        case 0x80:
        case 0xEA:
            m_reg.p = generateFlags();
            m_logger->writeInstruction(m_reg, "NOP", 0, comp::ADDR_MODE_IMPLIED);
            break;

        // SEI
        case 0x78:
            setFlags(comp::FLAG_INTERRUPT);
            m_reg.p = generateFlags();
            m_logger->writeInstruction(m_reg, "SEI", 0, comp::ADDR_MODE_IMPLIED);
            break;

        // SEC
        case 0x38:
            setFlags(comp::FLAG_CARRY);
            m_reg.p = generateFlags();
            m_logger->writeInstruction(m_reg, "SEC", 0, comp::ADDR_MODE_IMPLIED);
            break;

        // SED
        case 0xF8:
            setFlags(comp::FLAG_DECIMAL);
            m_reg.p = generateFlags();
            m_logger->writeInstruction(m_reg, "SED", 0, comp::ADDR_MODE_IMPLIED);
            break;
        // CLV
        case 0xB8:
            unsetFlags(comp::FLAG_OVERFLOW);
            m_reg.p = generateFlags();
            m_logger->writeInstruction(m_reg, "CLV", 0, comp::ADDR_MODE_IMPLIED);
            break;

        // CLC
        case 0x18:
            unsetFlags(comp::FLAG_CARRY);
            m_reg.p = generateFlags();
            m_logger->writeInstruction(m_reg, "CLC", 0, comp::ADDR_MODE_IMPLIED);
            break;

        // CLI
        case 0x58:
            unsetFlags(comp::FLAG_INTERRUPT);
            m_reg.p = generateFlags();
            m_logger->writeInstruction(m_reg, "CLI", 0, comp::ADDR_MODE_IMPLIED);
            break;

        // CLD
        case 0xD8:
            unsetFlags(comp::FLAG_DECIMAL);
            m_reg.p = generateFlags();
            m_logger->writeInstruction(m_reg, "CLD", 0, comp::ADDR_MODE_IMPLIED);
            break;

        // LDA
        case 0xA9:
            load(m_reg.a, comp::ADDR_MODE_IMMEDIATE);
            break;
        case 0xA5:
            load(m_reg.a, comp::ADDR_MODE_ZERO_PAGE);
            break;
        case 0xB5:
            load(m_reg.a, comp::ADDR_MODE_ZERO_PAGE_X);
            break;
        case 0xAD:
            load(m_reg.a, comp::ADDR_MODE_ABSOLUTE);
            break;
        case 0xBD:
            load(m_reg.a, comp::ADDR_MODE_ABSOLUTE_X);
            break;
        case 0xB9:
            load(m_reg.a, comp::ADDR_MODE_ABSOLUTE_Y);
            break;
        case 0xA1:
            load(m_reg.a, comp::ADDR_MODE_INDIRECT_X);
            break;
        case 0xB1:
            load(m_reg.a, comp::ADDR_MODE_INDIRECT_Y);
            break;

        // LDX
        case 0xA2:
            load(m_reg.x, comp::ADDR_MODE_IMMEDIATE);
            break;
        case 0xA6:
            load(m_reg.x, comp::ADDR_MODE_ZERO_PAGE);
            break;
        case 0xB6:
            load(m_reg.x, comp::ADDR_MODE_ZERO_PAGE_Y);
            break;
        case 0xAE:
            load(m_reg.x, comp::ADDR_MODE_ABSOLUTE);
            break;
        case 0xBE:
            load(m_reg.x, comp::ADDR_MODE_ABSOLUTE_Y);
            break;

        // LDY
        case 0xA0:
            load(m_reg.y, comp::ADDR_MODE_IMMEDIATE);
            break;
        case 0xA4:
            load(m_reg.y, comp::ADDR_MODE_ZERO_PAGE);
            break;
        case 0xB4:
            load(m_reg.y, comp::ADDR_MODE_ZERO_PAGE_X);
            break;
        case 0xAC:
            load(m_reg.y, comp::ADDR_MODE_ABSOLUTE);
            break;
        case 0xBC:
            load(m_reg.y, comp::ADDR_MODE_ABSOLUTE_X);
            break;

        // STA
        case 0x85:
            store(m_reg.a, comp::ADDR_MODE_ZERO_PAGE);
            break;
        case 0x95:
            store(m_reg.a, comp::ADDR_MODE_ZERO_PAGE_X);
            break;
        case 0x8D:
            store(m_reg.a, comp::ADDR_MODE_ABSOLUTE);
            break;
        case 0x9D:
            store(m_reg.a, comp::ADDR_MODE_ABSOLUTE_X);
            break;
        case 0x99:
            store(m_reg.a, comp::ADDR_MODE_ABSOLUTE_Y);
            break;
        case 0x81:
            store(m_reg.a, comp::ADDR_MODE_INDIRECT_X);
            break;
        case 0x91:
            store(m_reg.a, comp::ADDR_MODE_INDIRECT_Y);
            break;

        // STX
        case 0x86:
            store(m_reg.x, comp::ADDR_MODE_ZERO_PAGE);
            break;
        case 0x96:
            store(m_reg.x, comp::ADDR_MODE_ZERO_PAGE_Y);
            break;
        case 0x8E:
            store(m_reg.x, comp::ADDR_MODE_ABSOLUTE);
            break;

        // STY
        case 0x84:
            store(m_reg.y, comp::ADDR_MODE_ZERO_PAGE);
            break;
        case 0x94:
            store(m_reg.y, comp::ADDR_MODE_ZERO_PAGE_X);
            break;
        case 0x8C:
            store(m_reg.y, comp::ADDR_MODE_ABSOLUTE);
            break;

        // TXS
        case 0x9A:
            m_reg.sp = m_reg.x;
            m_reg.p = generateFlags();
            m_logger->writeInstruction(m_reg, "TXS", 0, comp::ADDR_MODE_IMMEDIATE);
            break;

        // TSX
        case 0xBA:
            m_reg.x = m_reg.sp;
            checkFlags(m_reg.x, comp::FLAG_NEGATIVE | comp::FLAG_ZERO);
            m_reg.p = generateFlags();
            m_logger->writeInstruction(m_reg, "TSX", 0, comp::ADDR_MODE_IMMEDIATE);
            break;

        // Branching
        case 0x10:
            branch(!m_flags.N);
            break;
        case 0x30:
            branch(m_flags.N);
            break;
        case 0x50:
            branch(!m_flags.V);
            break;
        case 0x70:
            branch(m_flags.V);
            break;
        case 0x90:
            branch(!m_flags.C);
            break;
        case 0xB0:
            branch(m_flags.C);
            break;
        case 0xD0:
            branch(!m_flags.Z);
            break;
        case 0xF0:
            branch(m_flags.Z);
            break;

        // CMP
        case 0xC9:
            compare(m_reg.a, comp::ADDR_MODE_IMMEDIATE);
            break;
        case 0xC5:
            compare(m_reg.a, comp::ADDR_MODE_ZERO_PAGE);
            break;
        case 0xD5:
            compare(m_reg.a, comp::ADDR_MODE_ZERO_PAGE_X);
            break;
        case 0xCD:
            compare(m_reg.a, comp::ADDR_MODE_ABSOLUTE);
            break;
        case 0xDD:
            compare(m_reg.a, comp::ADDR_MODE_ABSOLUTE_X);
            break;
        case 0xD9:
            compare(m_reg.a, comp::ADDR_MODE_ABSOLUTE_Y);
            break;
        case 0xC1:
            compare(m_reg.a, comp::ADDR_MODE_INDIRECT_X);
            break;
        case 0xD1:
            compare(m_reg.a, comp::ADDR_MODE_INDIRECT_Y);
            break;

        // CPX
        case 0xE0:
            compare(m_reg.x, comp::ADDR_MODE_IMMEDIATE);
            break;
        case 0xE4:
            compare(m_reg.x, comp::ADDR_MODE_ZERO_PAGE);
            break;
        case 0xEC:
            compare(m_reg.x, comp::ADDR_MODE_ABSOLUTE);
            break;

        // CPY
        case 0xC0:
            compare(m_reg.y, comp::ADDR_MODE_IMMEDIATE);
            break;
        case 0xC4:
            compare(m_reg.y, comp::ADDR_MODE_ZERO_PAGE);
            break;
        case 0xCC:
            compare(m_reg.y, comp::ADDR_MODE_ABSOLUTE);
            break;

        // INC
        case 0xE6:
            increment(comp::ADDR_MODE_ZERO_PAGE);
            break;
        case 0xF6:
            increment(comp::ADDR_MODE_ZERO_PAGE_X);
            break;
        case 0xEE:
            increment(comp::ADDR_MODE_ABSOLUTE);
            break;
        case 0xFE:
            increment(comp::ADDR_MODE_ABSOLUTE_X);
            break;

        // INX
        case 0xE8:
            increment(m_reg.x);
            break;

        // INY
        case 0xC8:
            increment(m_reg.y);
            break;

        // DEC
        case 0xC6:
            decrement(comp::ADDR_MODE_ZERO_PAGE);
            break;
        case 0xD6:
            decrement(comp::ADDR_MODE_ZERO_PAGE_X);
            break;
        case 0xCE:
            decrement(comp::ADDR_MODE_ABSOLUTE);
            break;
        case 0xDE:
            decrement(comp::ADDR_MODE_ABSOLUTE_X);
            break;

        // DEX
        case 0xCA:
            decrement(m_reg.x);
            break;

        // DEY
        case 0x88:
            decrement(m_reg.y);
            break;

        // JMP
        case 0x20:
            subJump();
            break;
        case 0x4C:
            jump(comp::ADDR_MODE_ABSOLUTE);
            break;
        case 0x6C:
            jump(comp::ADDR_MODE_INDIRECT);
            break;

        // RTS
        case 0x60:
            returnSub();
            break;

        // BIT
        case 0x24:
            bit(comp::ADDR_MODE_ZERO_PAGE);
            break;
        case 0x2C:
            bit(comp::ADDR_MODE_ABSOLUTE);
            break;

        // ORA
        case 0x09:
            ora(comp::ADDR_MODE_IMMEDIATE);
            break;
        case 0x05:
            ora(comp::ADDR_MODE_ZERO_PAGE);
            break;
        case 0x15:
            ora(comp::ADDR_MODE_ZERO_PAGE_X);
            break;
        case 0x0D:
            ora(comp::ADDR_MODE_ABSOLUTE);
            break;
        case 0x1D:
            ora(comp::ADDR_MODE_ABSOLUTE_X);
            break;
        case 0x19:
            ora(comp::ADDR_MODE_ABSOLUTE_Y);
            break;
        case 0x01:
            ora(comp::ADDR_MODE_INDIRECT_X);
            break;
        case 0x11:
            ora(comp::ADDR_MODE_INDIRECT_Y);
            break;

        // XOR
        case 0x49:
            xora(comp::ADDR_MODE_IMMEDIATE);
            break;
        case 0x45:
            xora(comp::ADDR_MODE_ZERO_PAGE);
            break;
        case 0x55:
            xora(comp::ADDR_MODE_ZERO_PAGE_X);
            break;
        case 0x4D:
            xora(comp::ADDR_MODE_ABSOLUTE);
            break;
        case 0x5D:
            xora(comp::ADDR_MODE_ABSOLUTE_X);
            break;
        case 0x59:
            xora(comp::ADDR_MODE_ABSOLUTE_Y);
            break;
        case 0x41:
            xora(comp::ADDR_MODE_INDIRECT_X);
            break;
        case 0x51:
            xora(comp::ADDR_MODE_INDIRECT_Y);
            break;

        // AND
        case 0x29:
            bitAnd(comp::ADDR_MODE_IMMEDIATE);
            break;
        case 0x25:
            bitAnd(comp::ADDR_MODE_ZERO_PAGE);
            break;
        case 0x35:
            bitAnd(comp::ADDR_MODE_ZERO_PAGE_X);
            break;
        case 0x2D:
            bitAnd(comp::ADDR_MODE_ABSOLUTE);
            break;
        case 0x3D:
            bitAnd(comp::ADDR_MODE_ABSOLUTE_X);
            break;
        case 0x39:
            bitAnd(comp::ADDR_MODE_ABSOLUTE_Y);
            break;
        case 0x21:
            bitAnd(comp::ADDR_MODE_INDIRECT_X);
            break;
        case 0x31:
            bitAnd(comp::ADDR_MODE_INDIRECT_Y);
            break;

        // TXA
        case 0x8A:
            m_reg.a = m_reg.x;
            checkFlags(m_reg.a, comp::FLAG_NEGATIVE | comp::FLAG_ZERO);
            m_reg.p = generateFlags();
            m_logger->writeInstruction(m_reg, "TXA", m_reg.a, comp::ADDR_MODE_IMMEDIATE);
            break;

        // TAX
        case 0xAA:
            m_reg.x = m_reg.a;
            checkFlags(m_reg.x, comp::FLAG_NEGATIVE | comp::FLAG_ZERO);
            m_reg.p = generateFlags();
            m_logger->writeInstruction(m_reg, "TAX", m_reg.x, comp::ADDR_MODE_IMMEDIATE);
            break;

        // TAY
        case 0xA8:
            m_reg.y = m_reg.a;
            checkFlags(m_reg.y, comp::FLAG_NEGATIVE | comp::FLAG_ZERO);
            m_reg.p = generateFlags();
            m_logger->writeInstruction(m_reg, "TAY", m_reg.y, comp::ADDR_MODE_IMPLIED);
            break;

        // TYA
        case 0x98:
            m_reg.a = m_reg.y;
            checkFlags(m_reg.a, comp::FLAG_NEGATIVE | comp::FLAG_ZERO);
            m_reg.p = generateFlags();
            m_logger->writeInstruction(m_reg, "TYA", m_reg.a, comp::ADDR_MODE_IMPLIED);
            break;

        // LSR
        case 0x4A:
            shiftRight(comp::ADDR_MODE_ACCUMULATOR);
            break;
        case 0x46:
            shiftRight(comp::ADDR_MODE_ZERO_PAGE);
            break;
        case 0x56:
            shiftRight(comp::ADDR_MODE_ZERO_PAGE_X);
            break;
        case 0x4E:
            shiftRight(comp::ADDR_MODE_ABSOLUTE);
            break;
        case 0x5E:
            shiftRight(comp::ADDR_MODE_ABSOLUTE_X);
            break;

        // ROR
        case 0x6A:
            rotateRight(comp::ADDR_MODE_ACCUMULATOR);
            break;
        case 0x66:
            rotateRight(comp::ADDR_MODE_ZERO_PAGE);
            break;
        case 0x76:
            rotateRight(comp::ADDR_MODE_ZERO_PAGE_X);
            break;
        case 0x6E:
            rotateRight(comp::ADDR_MODE_ABSOLUTE);
            break;
        case 0x7E:
            rotateRight(comp::ADDR_MODE_ABSOLUTE_X);
            break;

        // ASL
        case 0x0A:
            asl(comp::ADDR_MODE_ACCUMULATOR);
            break;
        case 0x06:
            asl(comp::ADDR_MODE_ZERO_PAGE);
            break;
        case 0x16:
            asl(comp::ADDR_MODE_ZERO_PAGE_X);
            break;
        case 0x0E:
            asl(comp::ADDR_MODE_ABSOLUTE);
            break;
        case 0x1E:
            asl(comp::ADDR_MODE_ABSOLUTE_X);
            break;

        // ROL
        case 0x2A:
            rotateLeft(comp::ADDR_MODE_ACCUMULATOR);
            break;
        case 0x26:
            rotateLeft(comp::ADDR_MODE_ZERO_PAGE);
            break;
        case 0x36:
            rotateLeft(comp::ADDR_MODE_ZERO_PAGE_X);
            break;
        case 0x2E:
            rotateLeft(comp::ADDR_MODE_ABSOLUTE);
            break;
        case 0x3E:
            rotateLeft(comp::ADDR_MODE_ABSOLUTE_X);
            break;

        // PHA
        case 0x48:
            m_memory->push(m_reg.a, m_reg.sp);
            m_reg.p = generateFlags();
            m_logger->writeInstruction(m_reg, "pha", m_reg.a, comp::ADDR_MODE_IMPLIED);
            break;

        // PLA
        case 0x68:
            m_reg.a = m_memory->pop(m_reg.sp);
            checkFlags(m_reg.a, comp::FLAG_NEGATIVE | comp::FLAG_ZERO);
            m_reg.p = generateFlags();
            m_logger->writeInstruction(m_reg, "pla", m_reg.a, comp::ADDR_MODE_IMPLIED);
            break;

        // PHP
        case 0x08:
            setFlags(comp::FLAG_PUSHED);
            m_memory->push(generateFlags(), m_reg.sp);
            unsetFlags(comp::FLAG_PUSHED);
            m_reg.p = generateFlags();
            m_logger->writeInstruction(m_reg, "php", generateFlags(), comp::ADDR_MODE_IMPLIED);
            break;

        // PLP
        case 0x28:
            setFlags(m_memory->pop(m_reg.sp));
            m_reg.p = generateFlags();
            m_logger->writeInstruction(m_reg, "plp", generateFlags(), comp::ADDR_MODE_IMPLIED);
            break;

        // ADC
        case 0x69:
            adc(comp::ADDR_MODE_IMMEDIATE);
            break;
        case 0x65:
            adc(comp::ADDR_MODE_ZERO_PAGE);
            break;
        case 0x75:
            adc(comp::ADDR_MODE_ZERO_PAGE_X);
            break;
        case 0x6D:
            adc(comp::ADDR_MODE_ABSOLUTE);
            break;
        case 0x7D:
            adc(comp::ADDR_MODE_ABSOLUTE_X);
            break;
        case 0x79:
            adc(comp::ADDR_MODE_ABSOLUTE_Y);
            break;
        case 0x61:
            adc(comp::ADDR_MODE_INDIRECT_X);
            break;
        case 0x71:
            adc(comp::ADDR_MODE_INDIRECT_Y);
            break;

        // SBC
        case 0xE9:
            subtract(comp::ADDR_MODE_IMMEDIATE);
            break;
        case 0xE5:
            subtract(comp::ADDR_MODE_ZERO_PAGE);
            break;
        case 0xF5:
            subtract(comp::ADDR_MODE_ZERO_PAGE_X);
            break;
        case 0xED:
            subtract(comp::ADDR_MODE_ABSOLUTE);
            break;
        case 0xFD:
            subtract(comp::ADDR_MODE_ABSOLUTE_X);
            break;
        case 0xF9:
            subtract(comp::ADDR_MODE_ABSOLUTE_Y);
            break;
        case 0xE1:
            subtract(comp::ADDR_MODE_INDIRECT_X);
            break;
        case 0xF1:
            subtract(comp::ADDR_MODE_INDIRECT_Y);
            break;

        // RTI
        case 0x40:
            setFlags(m_memory->pop(m_reg.sp));
            m_reg.pc = m_memory->pop16(m_reg.sp) - 1;

            m_reg.p = generateFlags();
            m_logger->writeInstruction(m_reg, "rti", m_reg.pc, comp::ADDR_MODE_IMPLIED);
            break;

        default:
            m_logger->writeError(m_opcodes[op], m_reg.pc);
            m_running = false;
            return 0;
    }

    m_reg.pc += m_opsize[op];

    return m_cyclesTable[op];
}

void nemus::core::CPU::interrupt() {
    switch(m_interrupt) {
        case comp::INT_NMI:
        {
            m_memory->push16(m_reg.pc, m_reg.sp);
            m_memory->push(generateFlags(), m_reg.sp);

            unsigned int address = m_memory->readWord(0xFFFA);

            m_reg.pc = address;

            m_interrupt = comp::INT_NONE;

            setFlags(comp::FLAG_INTERRUPT);

            m_logger->write("NMI has occurred!\n");
        }
            break;
        case comp::INT_IRQ:
        {
            m_memory->push16(m_reg.pc, m_reg.sp);
            m_memory->push(generateFlags(), m_reg.sp);

            unsigned int address = m_memory->readWord(0xFFFE);

            m_reg.pc = address;

            setFlags(comp::FLAG_INTERRUPT);

            m_interrupt = comp::INT_NONE;
            m_logger->write("IRQ has occured!\n");
        }
            break;
//        case comp::INT_RESET:
//            // TODO: Implement reset vector
//            break;
    }
}

void nemus::core::CPU::generateOP() {
    std::string opcodes[256] = {    "BRK", "ORA", "KIL", "SLO", "NOP", "ORA", "ASL", "SLO",
                                    "PHP", "ORA", "ASL", "ANC", "NOP", "ORA", "ASL", "SLO",
                                    "BPL", "ORA", "KIL", "SLO", "NOP", "ORA", "ASL", "SLO",
                                    "CLC", "ORA", "NOP", "SLO", "NOP", "ORA", "ASL", "SLO",
                                    "JSR", "AND", "KIL", "RLA", "BIT", "AND", "ROL", "RLA",
                                    "PLP", "AND", "ROL", "ANC", "BIT", "AND", "ROL", "RLA",
                                    "BMI", "AND", "KIL", "RLA", "NOP", "AND", "ROL", "RLA",
                                    "SEC", "AND", "NOP", "RLA", "NOP", "AND", "ROL", "RLA",
                                    "RTI", "EOR", "KIL", "SRE", "NOP", "EOR", "LSR", "SRE",
                                    "PHA", "EOR", "LSR", "ALR", "JMP", "EOR", "LSR", "SRE",
                                    "BVC", "EOR", "KIL", "SRE", "NOP", "EOR", "LSR", "SRE",
                                    "CLI", "EOR", "NOP", "SRE", "NOP", "EOR", "LSR", "SRE",
                                    "RTS", "ADC", "KIL", "RRA", "NOP", "ADC", "ROR", "RRA",
                                    "PLA", "ADC", "ROR", "ARR", "JMP", "ADC", "ROR", "RRA",
                                    "BVS", "ADC", "KIL", "RRA", "NOP", "ADC", "ROR", "RRA",
                                    "SEI", "ADC", "NOP", "RRA", "NOP", "ADC", "ROR", "RRA",
                                    "NOP", "STA", "NOP", "SAX", "STY", "STA", "STX", "SAX",
                                    "DEY", "NOP", "TXA", "XAA", "STY", "STA", "STX", "SAX",
                                    "BCC", "STA", "KIL", "AHX", "STY", "STA", "STX", "SAX",
                                    "TYA", "STA", "TXS", "TAS", "SHY", "STA", "SHX", "AHX",
                                    "LDY", "LDA", "LDX", "LAX", "LDY", "LDA", "LDX", "LAX",
                                    "TAY", "LDA", "TAX", "LAX", "LDY", "LDA", "LDX", "LAX",
                                    "BCS", "LDA", "KIL", "LAX", "LDY", "LDA", "LDX", "LAX",
                                    "CLV", "LDA", "TSX", "LAS", "LDY", "LDA", "LDX", "LAX",
                                    "CPY", "CMP", "NOP", "DCP", "CPY", "CMP", "DEC", "DCP",
                                    "INY", "CMP", "DEX", "AXS", "CPY", "CMP", "DEC", "DCP",
                                    "BNE", "CMP", "KIL", "DCP", "NOP", "CMP", "DEC", "DCP",
                                    "CLD", "CMP", "NOP", "DCP", "NOP", "CMP", "DEC", "DCP",
                                    "CPX", "SBC", "NOP", "ISC", "CPX", "SBC", "INC", "ISC",
                                    "INX", "SBC", "NOP", "SBC", "CPX", "SBC", "INC", "ISC",
                                    "BEQ", "SBC", "KIL", "ISC", "NOP", "SBC", "INC", "ISC",
                                    "SED", "SBC", "NOP", "ISC", "NOP", "SBC", "INC", "ISC" };

    for(int i = 0; i < 256; i++) {
        m_opcodes[i] = opcodes[i];
    }

    int instructionSizes[256] = {
            1, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
            2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
            3, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
            2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
            1, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
            2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
            1, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
            2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
            2, 2, 0, 0, 2, 2, 2, 0, 1, 0, 1, 0, 3, 3, 3, 0,
            2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 0, 3, 0, 0,
            2, 2, 2, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
            2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
            2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
            2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
            2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
            2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
    };

    for(int i = 0; i < 256; i++) {
        m_opsize[i] = instructionSizes[i];
    }

    int cyclesTable[256] = {    7, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
                                2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
                                6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,
                                2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
                                6, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,
                                2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
                                6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,
                                2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
                                2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
                                2, 6, 2, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5,
                                2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
                                2, 5, 2, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4,
                                2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
                                2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
                                2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
                                2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
    };

    for(int i = 0; i < 256; i++) {
        m_cyclesTable[i] = cyclesTable[i];
    }
}

void nemus::core::CPU::resetRegisters() {
    m_reg.p = 0x34;
    setFlags(0x34);
    m_reg.a, m_reg.x, m_reg.y = 0;
    m_reg.sp = 0xFD;
}

void nemus::core::CPU::adc(comp::AddressMode addr) {
    unsigned int operand = m_memory->readByte(m_reg, addr);

    unsigned int result = m_reg.a + operand + (int)(m_flags.C);

    m_flags.V = ~(m_reg.a ^ operand) & (m_reg.a ^ result) & 0x80;

    m_reg.a = result & 0xFF;

    m_flags.C = result > 0xFF;

    checkFlags(result & 0xFF, comp::FLAG_ZERO | comp::FLAG_NEGATIVE);
    m_reg.p = generateFlags();

    m_logger->writeInstruction(m_reg, "adc", result, addr);
}

void nemus::core::CPU::bitAnd(comp::AddressMode addr) {
    unsigned int operand = m_memory->readByte(m_reg, addr);

    m_reg.a = (operand & m_reg.a) & 0xFF;

    checkFlags(m_reg.a, comp::FLAG_ZERO | comp::FLAG_NEGATIVE);
    m_reg.p = generateFlags();

    m_logger->writeInstruction(m_reg, "and", operand, addr);
}

void nemus::core::CPU::asl(comp::AddressMode addr) {
    if(addr == comp::ADDR_MODE_ACCUMULATOR) {
        m_flags.C = (bool)(m_reg.a & 0x80);

        m_reg.a = (m_reg.a << 1) & 0xFF;

        checkFlags(m_reg.a, comp::FLAG_ZERO | comp::FLAG_NEGATIVE);
    } else {
        unsigned int operand = m_memory->readByte(m_reg, addr);

        m_flags.C = (bool)(operand & 0x80);

        operand = (operand << 1) & 0xFF;

        checkFlags(operand, comp::FLAG_ZERO | comp::FLAG_NEGATIVE);

        m_memory->writeByte(m_reg, operand, addr);
    }

    m_reg.p = generateFlags();
    m_logger->writeInstruction(m_reg, "asl", 0, addr);
}

void nemus::core::CPU::branch(bool doJump) {
    auto b = static_cast<signed char>(m_memory->readByte(m_reg.pc + 1));

    m_reg.p = generateFlags();
    m_logger->writeInstruction(m_reg, "branch", static_cast<int>(b) & 0xFF, comp::ADDR_MODE_IMMEDIATE);

    if(doJump) {
        m_reg.pc += b;
    }
}

void nemus::core::CPU::bit(comp::AddressMode addr) {
    unsigned int operand = m_memory->readByte(m_reg, addr);

    unsigned int result = (m_reg.a & 0xFF) & (operand & 0xFF);

    m_flags.N = (bool)(operand & 0x80);

    m_flags.V = (bool)(operand & 0x40);

    m_flags.Z = result == 0;

    m_reg.p = generateFlags();
    m_logger->writeInstruction(m_reg, "bit_test", operand, addr);
}

void nemus::core::CPU::compare(unsigned int &src, comp::AddressMode addr) {
    unsigned int operand = m_memory->readByte(m_reg, addr);

    m_flags.C = src >= operand;

    m_flags.Z = src - operand == 0;
    m_flags.N = (bool)(((unsigned char)(src) - (unsigned char)(operand)) & 0x80);

    m_reg.p = generateFlags();
    m_logger->writeInstruction(m_reg, "compare", src, addr);
}

void nemus::core::CPU::decrement(comp::AddressMode addr) {
    auto operand = (unsigned char)(m_memory->readByte(m_reg, addr));

    m_memory->writeByte(m_reg, (unsigned char)(operand - 1), addr);

    checkFlags((unsigned char)(operand - 1), comp::FLAG_ZERO | comp::FLAG_NEGATIVE);

    m_reg.p = generateFlags();
    m_logger->writeInstruction(m_reg, "decrement memory", operand, addr);
}

void nemus::core::CPU::decrement(unsigned int &src) {
    src = (unsigned char)((src) - 1);
    src &= 0xFF;

    checkFlags(src, comp::FLAG_NEGATIVE | comp::FLAG_ZERO);

    m_reg.p = generateFlags();
    m_logger->writeInstruction(m_reg, "decrement reg", src, comp::ADDR_MODE_IMMEDIATE);
}

void nemus::core::CPU::xora(comp::AddressMode addr) {
    unsigned int operand = m_memory->readByte(m_reg, addr);

    m_reg.a = (operand & 0xFF) ^ (m_reg.a & 0xFF);

    checkFlags(m_reg.a, comp::FLAG_NEGATIVE | comp::FLAG_ZERO);

    m_reg.p = generateFlags();
    m_logger->writeInstruction(m_reg, "xor", operand, addr);
}

void nemus::core::CPU::increment(comp::AddressMode addr) {
    unsigned char operand = m_memory->readByte(m_reg, addr);

    m_memory->writeByte(m_reg, (unsigned char)(operand + 1), addr);

    checkFlags((unsigned char)(operand + 1), comp::FLAG_ZERO | comp::FLAG_NEGATIVE);

    m_reg.p = generateFlags();
    m_logger->writeInstruction(m_reg, "increment memory", 0, addr);
}

void nemus::core::CPU::increment(unsigned int &src) {
    src = static_cast<unsigned char>(src) + 1;
    src &= 0xFF;

    checkFlags(src, comp::FLAG_NEGATIVE | comp::FLAG_ZERO);

    m_reg.p = generateFlags();
    m_logger->writeInstruction(m_reg, "increment reg", 0, comp::ADDR_MODE_IMMEDIATE);
}

void nemus::core::CPU::subJump() {
    m_memory->push16(m_reg.pc + 2, m_reg.sp);

    unsigned int address = m_memory->readWord(m_reg.pc + 1) - 3;

    m_reg.p = generateFlags();
    m_logger->writeInstruction(m_reg, "jump_sub", address + 3, comp::ADDR_MODE_ABSOLUTE);

    m_reg.pc = address;
}

void nemus::core::CPU::jump(comp::AddressMode addr) {
    unsigned int address = 0;

    if(addr == comp::ADDR_MODE_ABSOLUTE) {
        address = m_memory->readWord(m_reg.pc + 1);
    } else {
        unsigned int indirect_address = m_memory->readWord(m_reg.pc + 1);
        address = m_memory->readWordBug(indirect_address);
    }

    m_reg.p = generateFlags();
    m_logger->writeInstruction(m_reg, "jump", address, comp::ADDR_MODE_ABSOLUTE);

    m_reg.pc = address - 3;
}

void nemus::core::CPU::shiftRight(comp::AddressMode addr) {
    if(addr == comp::ADDR_MODE_ACCUMULATOR) {
        m_flags.C = (bool)(m_reg.a & 0x01);
        m_reg.a = (m_reg.a & 0xFF) >> 1;
        checkFlags(m_reg.a, comp::FLAG_ZERO | comp::FLAG_NEGATIVE);
    } else {
        unsigned int operand = m_memory->readByte(m_reg, addr);

        m_flags.C = (bool)(operand & 0x01);

        operand = (operand & 0xFF) >> 1;
        checkFlags(operand, comp::FLAG_ZERO | comp::FLAG_NEGATIVE);

        m_memory->writeByte(m_reg, operand, addr);
    }

    m_reg.p = generateFlags();
    m_logger->writeInstruction(m_reg, "lsr", 0, addr);
}

void nemus::core::CPU::load(unsigned int &dest, comp::AddressMode addr) {
    dest = m_memory->readByte(m_reg, addr);

    checkFlags(dest, comp::FLAG_NEGATIVE | comp::FLAG_ZERO);

    m_reg.p = generateFlags();

    m_logger->writeInstruction(m_reg, "load", dest, addr);
}

void nemus::core::CPU::ora(comp::AddressMode addr) {
    unsigned int operand = m_memory->readByte(m_reg, addr);

    m_reg.a = (operand & 0xFF) | (m_reg.a & 0xFF);

    checkFlags(m_reg.a, comp::FLAG_ZERO | comp::FLAG_NEGATIVE);

    m_reg.p = generateFlags();
    m_logger->writeInstruction(m_reg, "ora", operand, addr);
}

void nemus::core::CPU::store(unsigned int &src, comp::AddressMode addr) {
    m_memory->writeByte(m_reg, src, addr);
    m_reg.p = generateFlags();
    m_logger->writeInstruction(m_reg, "store", src, addr);
}

void nemus::core::CPU::returnSub() {
    unsigned int address = m_memory->pop16(m_reg.sp);

    m_reg.p = generateFlags();
    m_logger->writeInstruction(m_reg, "return_sub", address, comp::ADDR_MODE_IMMEDIATE);

    m_reg.pc = address;
}

void nemus::core::CPU::rotateRight(comp::AddressMode addr) {
    bool carry = m_flags.C;

    if(addr == comp::ADDR_MODE_ACCUMULATOR) {
        m_flags.C = (bool)(m_reg.a & 0x01);
        m_reg.a = (m_reg.a & 0xFF) >> 1;
        m_reg.a += (carry * 0x80);

        checkFlags(m_reg.a, comp::FLAG_ZERO | comp::FLAG_NEGATIVE);
    } else {
        unsigned int operand = m_memory->readByte(m_reg, addr);

        m_flags.C = (bool)(m_reg.a & 0x01);

        operand = (operand & 0xFF) >> 1;

        operand += (carry * 0x80);

        m_memory->writeByte(m_reg, operand, addr);

        checkFlags(operand, comp::FLAG_ZERO | comp::FLAG_NEGATIVE);
    }

    m_reg.p = generateFlags();
    m_logger->writeInstruction(m_reg, "ror", 0, addr);
}

void nemus::core::CPU::rotateLeft(comp::AddressMode addr) {
    bool carry = m_flags.C;

    if(addr == comp::ADDR_MODE_ACCUMULATOR) {
        m_flags.C = (bool)(m_reg.a & 0x80);
        m_reg.a = (m_reg.a << 1) & 0xFF;
        m_reg.a += carry;

        checkFlags(m_reg.a, comp::FLAG_ZERO | comp::FLAG_NEGATIVE);
    } else {
        unsigned int operand = m_memory->readByte(m_reg, addr);

        m_flags.C = (bool)(operand & 0x80);

        operand = (operand << 1) & 0xFF;

        operand += carry;

        m_memory->writeByte(m_reg, operand, addr);

        checkFlags(operand, comp::FLAG_ZERO | comp::FLAG_NEGATIVE);
    }

    m_reg.p = generateFlags();
    m_logger->writeInstruction(m_reg, "rol", 0, addr);
}

void nemus::core::CPU::subtract(comp::AddressMode addr) {
    unsigned int operand = m_memory->readByte(m_reg, addr) ^ 0xFF;

    unsigned int result = m_reg.a + operand + m_flags.C;

    m_flags.V = ~(m_reg.a ^ operand) & (m_reg.a ^ (result & 0xFF)) & 0x80;

    m_flags.C = result & 0xFF00;

    checkFlags(result & 0xFF, comp::FLAG_ZERO | comp::FLAG_NEGATIVE);

    m_reg.a = result & 0xFF;

    m_reg.p = generateFlags();
    m_logger->writeInstruction(m_reg, "sbc", operand, addr);
}

void nemus::core::CPU::setFlags(unsigned int flagbits) {
    m_flags.C = (flagbits & 0x01) != 0;
    m_flags.Z = (flagbits & 0x02) != 0;
    m_flags.I = (flagbits & 0x04) != 0;
    m_flags.D = (flagbits & 0x08) != 0;
    m_flags.V = (flagbits & 0x40) != 0;
    m_flags.N = (flagbits & 0x80) != 0;
}

void nemus::core::CPU::setFlags(comp::Flag flag) {
    switch(flag) {
        case comp::FLAG_CARRY:
            m_flags.C = true;
            break;
        case comp::FLAG_ZERO:
            m_flags.Z = true;
            break;
        case comp::FLAG_INTERRUPT:
            m_flags.I = true;
            break;
        case comp::FLAG_PUSHED:
            m_flags.P = true;
            break;
        case comp::FLAG_DECIMAL:
            m_flags.D = true;
            break;
        case comp::FLAG_OVERFLOW:
            m_flags.V = true;
            break;
        case comp::FLAG_NEGATIVE:
            m_flags.N = true;
            break;
    }
}

void nemus::core::CPU::unsetFlags(comp::Flag flag) {
    switch(flag) {
        case comp::FLAG_CARRY:
            m_flags.C = false;
            break;
        case comp::FLAG_ZERO:
            m_flags.Z = false;
            break;
        case comp::FLAG_INTERRUPT:
            m_flags.I = false;
            break;
        case comp::FLAG_PUSHED:
            m_flags.P = false;
            break;
        case comp::FLAG_DECIMAL:
            m_flags.D = false;
            break;
        case comp::FLAG_OVERFLOW:
            m_flags.V = false;
            break;
        case comp::FLAG_NEGATIVE:
            m_flags.N = false;
            break;
    }
}

void nemus::core::CPU::checkFlags(unsigned int operand, int flagbits) {
    if(flagbits & comp::FLAG_NEGATIVE) {
        if(operand > 0x7F) {
            setFlags(comp::FLAG_NEGATIVE);
        } else {
            unsetFlags(comp::FLAG_NEGATIVE);
        }
    }

    if(flagbits & comp::FLAG_ZERO) {
        if(operand == 0) {
            setFlags(comp::FLAG_ZERO);
        } else {
            unsetFlags(comp::FLAG_ZERO);
        }
    }
}

unsigned int nemus::core::CPU::generateFlags() {
    unsigned int result = 0;

    if(m_flags.N) result += 0x80;
    if(m_flags.V) result += 0x40;
    result += 0x20;
    if(m_flags.P) result += 0x10;
    if(m_flags.D) result += 0x08;
    if(m_flags.I) result += 0x04;
    if(m_flags.Z) result += 0x02;
    if(m_flags.C) result += 0x01;

    return result;
}