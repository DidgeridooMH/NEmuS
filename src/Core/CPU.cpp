#include <QMessageBox>
#include <fmt/format.h>

#include <Core/Opcodes.hpp>
#include <Core/CPU.h>

nemus::core::CPU::CPU(Memory *memory, debug::Logger *logger)
    : m_reg(comp::Registers{
          .x = 0,
          .y = 0,
          .a = 0,
          .pc = memory->ReadWord(0xFFFCU),
          .sp = 0xFD,
          .pFull = 0x34}),
      m_interrupt(comp::INT_NONE),
      m_memory(memory),
      m_logger(logger),
      m_running(true)
{
    m_logger->write(fmt::format("Entry Point: {:X}", m_reg.pc));
    m_logger->write("CPU Initialized");
    m_running = true;
}

int nemus::core::CPU::tick()
{
    // NMI
    if (m_interrupt != comp::INT_NONE)
    {
        if (m_interrupt == comp::INT_NMI || !m_reg.p.interruptDisable)
        {
            interrupt();
            return 0;
        }
    }

    unsigned int op = m_memory->ReadByte(m_reg.pc);
    unsigned int pageCycle = 0;

    switch (op)
    {
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
        m_logger->writeInstruction(m_reg, "NOP", 0, comp::ADDR_MODE_IMPLIED);
        break;

    // SEI
    case 0x78:
        m_reg.p.interruptDisable = 1;
        m_logger->writeInstruction(m_reg, "SEI", 0, comp::ADDR_MODE_IMPLIED);
        break;

    // SEC
    case 0x38:
        m_reg.p.carry = 1;
        m_logger->writeInstruction(m_reg, "SEC", 0, comp::ADDR_MODE_IMPLIED);
        break;

    // SED
    case 0xF8:
        m_reg.p.decimal = 1;
        m_logger->writeInstruction(m_reg, "SED", 0, comp::ADDR_MODE_IMPLIED);
        break;
    // CLV
    case 0xB8:
        m_reg.p.overflow = 0;
        m_logger->writeInstruction(m_reg, "CLV", 0, comp::ADDR_MODE_IMPLIED);
        break;

    // CLC
    case 0x18:
        m_reg.p.carry = 0;
        m_logger->writeInstruction(m_reg, "CLC", 0, comp::ADDR_MODE_IMPLIED);
        break;

    // CLI
    case 0x58:
        m_reg.p.interruptDisable = 0;
        m_logger->writeInstruction(m_reg, "CLI", 0, comp::ADDR_MODE_IMPLIED);
        break;

    // CLD
    case 0xD8:
        m_reg.p.decimal = 0;
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
        pageCycle = m_memory->CheckPageCross(m_reg, comp::ADDR_MODE_ABSOLUTE_X);
        load(m_reg.a, comp::ADDR_MODE_ABSOLUTE_X);
        break;
    case 0xB9:
        pageCycle = m_memory->CheckPageCross(m_reg, comp::ADDR_MODE_ABSOLUTE_Y);
        load(m_reg.a, comp::ADDR_MODE_ABSOLUTE_Y);
        break;
    case 0xA1:
        load(m_reg.a, comp::ADDR_MODE_INDIRECT_X);
        break;
    case 0xB1:
        pageCycle = m_memory->CheckPageCross(m_reg, comp::ADDR_MODE_INDIRECT_Y);
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
        pageCycle = m_memory->CheckPageCross(m_reg, comp::ADDR_MODE_ABSOLUTE_Y);
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
        pageCycle = m_memory->CheckPageCross(m_reg, comp::ADDR_MODE_ABSOLUTE_Y);
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
        m_logger->writeInstruction(m_reg, "TXS", 0, comp::ADDR_MODE_IMMEDIATE);
        break;

    // TSX
    case 0xBA:
        m_reg.x = m_reg.sp;
        m_reg.p.zero = m_reg.x == 0;
        m_reg.p.negative = m_reg.x >> 7;
        m_logger->writeInstruction(m_reg, "TSX", 0, comp::ADDR_MODE_IMMEDIATE);
        break;

    // Branching
    case 0x10:
    {
        auto address = m_reg.pc;
        branch(!m_reg.p.negative);
        pageCycle = ~m_reg.p.negative + ((address & 0xFF00) != m_reg.pc + INSTRUCTION_SIZES[op]) * 2;
    }
    break;
    case 0x30:
    {
        auto address = m_reg.pc;
        branch(m_reg.p.negative);
        pageCycle = m_reg.p.negative + ((address & 0xFF00) != m_reg.pc + INSTRUCTION_SIZES[op]) * 2;
    }
    break;
    case 0x50:
    {
        auto address = m_reg.pc;
        branch(!m_reg.p.overflow);
        pageCycle = ~m_reg.p.overflow + ((address & 0xFF00) != m_reg.pc + INSTRUCTION_SIZES[op]) * 2;
    }
    break;
    case 0x70:
    {
        auto address = m_reg.pc;
        branch(m_reg.p.overflow);
        pageCycle = m_reg.p.overflow + ((address & 0xFF00) != m_reg.pc + INSTRUCTION_SIZES[op]) * 2;
    }
    break;
    case 0x90:
    {
        auto address = m_reg.pc;
        branch(!m_reg.p.carry);
        pageCycle = ~m_reg.p.carry + ((address & 0xFF00) != m_reg.pc + INSTRUCTION_SIZES[op]) * 2;
    }
    break;
    case 0xB0:
    {
        auto address = m_reg.pc;
        branch(m_reg.p.carry);
        pageCycle = m_reg.p.carry + ((address & 0xFF00) != m_reg.pc + INSTRUCTION_SIZES[op]) * 2;
    }
    break;
    case 0xD0:
    {
        auto address = m_reg.pc;
        branch(!m_reg.p.zero);
        pageCycle = ~m_reg.p.zero + ((address & 0xFF00) != m_reg.pc + INSTRUCTION_SIZES[op]) * 2;
    }
    break;
    case 0xF0:
    {
        auto address = m_reg.pc;
        branch(m_reg.p.zero);
        pageCycle = m_reg.p.zero + ((address & 0xFF00) != m_reg.pc + INSTRUCTION_SIZES[op]) * 2;
    }
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
        pageCycle = m_memory->CheckPageCross(m_reg, comp::ADDR_MODE_ABSOLUTE_X);
        compare(m_reg.a, comp::ADDR_MODE_ABSOLUTE_X);
        break;
    case 0xD9:
        pageCycle = m_memory->CheckPageCross(m_reg, comp::ADDR_MODE_ABSOLUTE_Y);
        compare(m_reg.a, comp::ADDR_MODE_ABSOLUTE_Y);
        break;
    case 0xC1:
        compare(m_reg.a, comp::ADDR_MODE_INDIRECT_X);
        break;
    case 0xD1:
        pageCycle = m_memory->CheckPageCross(m_reg, comp::ADDR_MODE_INDIRECT_Y);
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
        pageCycle = m_memory->CheckPageCross(m_reg, comp::ADDR_MODE_ABSOLUTE_X);
        ora(comp::ADDR_MODE_ABSOLUTE_X);
        break;
    case 0x19:
        pageCycle = m_memory->CheckPageCross(m_reg, comp::ADDR_MODE_ABSOLUTE_Y);
        ora(comp::ADDR_MODE_ABSOLUTE_Y);
        break;
    case 0x01:
        ora(comp::ADDR_MODE_INDIRECT_X);
        break;
    case 0x11:
        pageCycle = m_memory->CheckPageCross(m_reg, comp::ADDR_MODE_INDIRECT_Y);
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
        pageCycle = m_memory->CheckPageCross(m_reg, comp::ADDR_MODE_ABSOLUTE_X);
        xora(comp::ADDR_MODE_ABSOLUTE_X);
        break;
    case 0x59:
        pageCycle = m_memory->CheckPageCross(m_reg, comp::ADDR_MODE_ABSOLUTE_Y);
        xora(comp::ADDR_MODE_ABSOLUTE_Y);
        break;
    case 0x41:
        xora(comp::ADDR_MODE_INDIRECT_X);
        break;
    case 0x51:
        pageCycle = m_memory->CheckPageCross(m_reg, comp::ADDR_MODE_INDIRECT_Y);
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
        m_reg.p.zero = m_reg.a == 0;
        m_reg.p.negative = m_reg.a >> 7;
        m_logger->writeInstruction(m_reg, "TXA", m_reg.a, comp::ADDR_MODE_IMMEDIATE);
        break;

    // TAX
    case 0xAA:
        m_reg.x = m_reg.a;
        m_reg.p.zero = m_reg.x == 0;
        m_reg.p.negative = m_reg.x >> 7;
        m_logger->writeInstruction(m_reg, "TAX", m_reg.x, comp::ADDR_MODE_IMMEDIATE);
        break;

    // TAY
    case 0xA8:
        m_reg.y = m_reg.a;
        m_reg.p.zero = m_reg.y == 0;
        m_reg.p.negative = m_reg.y >> 7;
        m_logger->writeInstruction(m_reg, "TAY", m_reg.y, comp::ADDR_MODE_IMPLIED);
        break;

    // TYA
    case 0x98:
        m_reg.a = m_reg.y;
        m_reg.p.zero = m_reg.a == 0;
        m_reg.p.negative = m_reg.a >> 7;
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
        m_memory->Push(m_reg.a, m_reg.sp);
        m_logger->writeInstruction(m_reg, "pha", m_reg.a, comp::ADDR_MODE_IMPLIED);
        break;

    // PLA
    case 0x68:
        m_reg.a = m_memory->Pop(m_reg.sp);
        m_reg.p.zero = m_reg.a == 0;
        m_reg.p.negative = m_reg.a >> 7;
        m_logger->writeInstruction(m_reg, "pla", m_reg.a, comp::ADDR_MODE_IMPLIED);
        break;

    // PHP
    case 0x08:
        m_memory->Push(m_reg.pFull | 0x3, m_reg.sp);
        m_logger->writeInstruction(m_reg, "php", m_reg.pFull, comp::ADDR_MODE_IMPLIED);
        break;

    // PLP
    case 0x28:
        m_reg.pFull = m_memory->Pop(m_reg.sp);
        m_reg.p.pushed = 0;
        m_logger->writeInstruction(m_reg, "plp", m_reg.pFull, comp::ADDR_MODE_IMPLIED);
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
        pageCycle = m_memory->CheckPageCross(m_reg, comp::ADDR_MODE_ABSOLUTE_X);
        adc(comp::ADDR_MODE_ABSOLUTE_X);
        break;
    case 0x79:
        pageCycle = m_memory->CheckPageCross(m_reg, comp::ADDR_MODE_ABSOLUTE_Y);
        adc(comp::ADDR_MODE_ABSOLUTE_Y);
        break;
    case 0x61:
        adc(comp::ADDR_MODE_INDIRECT_X);
        break;
    case 0x71:
        pageCycle = m_memory->CheckPageCross(m_reg, comp::ADDR_MODE_INDIRECT_Y);
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
        pageCycle = m_memory->CheckPageCross(m_reg, comp::ADDR_MODE_ABSOLUTE_X);
        subtract(comp::ADDR_MODE_ABSOLUTE_X);
        break;
    case 0xF9:
        pageCycle = m_memory->CheckPageCross(m_reg, comp::ADDR_MODE_ABSOLUTE_Y);
        subtract(comp::ADDR_MODE_ABSOLUTE_Y);
        break;
    case 0xE1:
        subtract(comp::ADDR_MODE_INDIRECT_X);
        break;
    case 0xF1:
        pageCycle = m_memory->CheckPageCross(m_reg, comp::ADDR_MODE_INDIRECT_Y);
        subtract(comp::ADDR_MODE_INDIRECT_Y);
        break;

    // SLO
    case 0x03:
        m_logger->write("SOL Instruction");
        asl(comp::ADDR_MODE_INDIRECT_X);
        ora(comp::ADDR_MODE_INDIRECT_X);
        break;
    case 0x07:
        m_logger->write("SOL Instruction");
        asl(comp::ADDR_MODE_ZERO_PAGE);
        ora(comp::ADDR_MODE_ZERO_PAGE);
        break;
    case 0x0F:
        m_logger->write("SOL Instruction");
        asl(comp::ADDR_MODE_ABSOLUTE);
        ora(comp::ADDR_MODE_ABSOLUTE);
        break;
    case 0x13:
        m_logger->write("SOL Instruction");
        asl(comp::ADDR_MODE_INDIRECT_Y);
        ora(comp::ADDR_MODE_INDIRECT_Y);
        break;
    case 0x17:
        asl(comp::ADDR_MODE_ZERO_PAGE_X);
        ora(comp::ADDR_MODE_ZERO_PAGE_X);
        break;
    case 0x1B:
        m_logger->write("SOL Instruction");
        asl(comp::ADDR_MODE_ABSOLUTE_Y);
        ora(comp::ADDR_MODE_ABSOLUTE_Y);
        break;
    case 0x1F:
        m_logger->write("SOL Instruction");
        asl(comp::ADDR_MODE_ABSOLUTE_X);
        ora(comp::ADDR_MODE_ABSOLUTE_X);
        break;

    // RLA
    case 0x23:
        m_logger->write("RLA Instruction");
        rotateLeft(comp::ADDR_MODE_INDIRECT_X);
        bitAnd(comp::ADDR_MODE_INDIRECT_X);
        break;
    case 0x27:
        m_logger->write("RLA Instruction");
        rotateLeft(comp::ADDR_MODE_ZERO_PAGE);
        bitAnd(comp::ADDR_MODE_ZERO_PAGE);
        break;
    case 0x2F:
        m_logger->write("RLA Instruction");
        rotateLeft(comp::ADDR_MODE_ABSOLUTE);
        bitAnd(comp::ADDR_MODE_ABSOLUTE);
        break;
    case 0x33:
        m_logger->write("RLA Instruction");
        rotateLeft(comp::ADDR_MODE_INDIRECT_Y);
        bitAnd(comp::ADDR_MODE_INDIRECT_Y);
        break;
    case 0x37:
        m_logger->write("RLA Instruction");
        rotateLeft(comp::ADDR_MODE_ZERO_PAGE_X);
        bitAnd(comp::ADDR_MODE_ZERO_PAGE_X);
        break;
    case 0x3B:
        m_logger->write("RLA Instruction");
        rotateLeft(comp::ADDR_MODE_ABSOLUTE_Y);
        bitAnd(comp::ADDR_MODE_ABSOLUTE_Y);
        break;
    case 0x3F:
        m_logger->write("RLA Instruction");
        rotateLeft(comp::ADDR_MODE_ABSOLUTE_X);
        bitAnd(comp::ADDR_MODE_ABSOLUTE_X);
        break;

    // RRA
    case 0x63:
        m_logger->write("RRA Instruction");
        rotateRight(comp::ADDR_MODE_INDIRECT_X);
        bitAnd(comp::ADDR_MODE_INDIRECT_X);
        break;
    case 0x67:
        m_logger->write("RRA Instruction");
        rotateRight(comp::ADDR_MODE_ZERO_PAGE);
        bitAnd(comp::ADDR_MODE_ZERO_PAGE);
        break;
    case 0x6F:
        m_logger->write("RRA Instruction");
        rotateRight(comp::ADDR_MODE_ABSOLUTE);
        bitAnd(comp::ADDR_MODE_ABSOLUTE);
        break;
    case 0x73:
        m_logger->write("RRA Instruction");
        rotateRight(comp::ADDR_MODE_INDIRECT_Y);
        bitAnd(comp::ADDR_MODE_INDIRECT_Y);
        break;
    case 0x77:
        m_logger->write("RRA Instruction");
        rotateRight(comp::ADDR_MODE_ZERO_PAGE_X);
        bitAnd(comp::ADDR_MODE_ZERO_PAGE_X);
        break;
    case 0x7B:
        m_logger->write("RRA Instruction");
        rotateRight(comp::ADDR_MODE_ABSOLUTE_Y);
        bitAnd(comp::ADDR_MODE_ABSOLUTE_Y);
        break;
    case 0x7F:
        m_logger->write("RRA Instruction");
        rotateRight(comp::ADDR_MODE_ABSOLUTE_X);
        bitAnd(comp::ADDR_MODE_ABSOLUTE_X);
        break;

    // RTI
    case 0x40:
        m_reg.pFull = m_memory->Pop(m_reg.sp);
        m_reg.p.pushed = 0;
        m_reg.pc = m_memory->Pop16(m_reg.sp) - 1;
        m_logger->writeInstruction(m_reg, "rti", m_reg.pc, comp::ADDR_MODE_IMPLIED);
        break;

    // BRK
    case 0x00:
        m_memory->Push16(m_reg.pc + 2, m_reg.sp);
        m_memory->Push(m_reg.pFull | 0x30, m_reg.sp);
        m_reg.p.interruptDisable = 1;
        m_reg.pc = m_memory->ReadWord(0xFFFE) - 1;
        break;

    default:
        m_logger->writeError(OPCODES[op], m_reg.pc);
        QMessageBox(
            QMessageBox::Critical,
            "Unimplemented Opcode",
            QString::fromStdString(
                fmt::format("Unknown Opcode: ${:X}:{} at a ${:X}",
                            op, OPCODES[op], m_reg.pc)),
            QMessageBox::StandardButton::Ok)
            .exec();
        m_running = false;
        return 0;
    }

    m_reg.pc += INSTRUCTION_SIZES[op];

    return CYCLES[op] + pageCycle;
}

void nemus::core::CPU::interrupt()
{
    switch (m_interrupt)
    {
    case comp::INT_NMI:
    {
        m_memory->Push16(m_reg.pc, m_reg.sp);
        m_memory->Push(m_reg.pFull, m_reg.sp);

        unsigned int address = m_memory->ReadWord(0xFFFA);

        m_reg.pc = address;

        m_interrupt = comp::INT_NONE;

        m_reg.p.interruptDisable = 1;

        m_logger->write("NMI has occurred!\n");
    }
    break;
    case comp::INT_IRQ:
    {
        m_memory->Push16(m_reg.pc, m_reg.sp);
        m_memory->Push(m_reg.pFull, m_reg.sp);

        unsigned int address = m_memory->ReadWord(0xFFFE);

        m_reg.pc = address;

        m_reg.p.interruptDisable = 1;

        m_interrupt = comp::INT_NONE;
        m_logger->write("IRQ has occured!\n");
    }
    break;
    case comp::INT_RESET:
        // TODO: Implement reset vector
        break;
    case comp::INT_NONE:
        break;
    }
}

void nemus::core::CPU::resetRegisters()
{
    m_reg.pFull = 0x34;
    m_reg.a = 0;
    m_reg.x = 0;
    m_reg.y = 0;
    m_reg.sp = 0xFD;
}

void nemus::core::CPU::adc(comp::AddressMode addr)
{
    uint8_t operand = m_memory->ReadByte(m_reg, addr);
    uint8_t result = m_reg.a + operand + m_reg.p.carry;

    m_reg.a = result;
    m_reg.p.overflow = (~(m_reg.a ^ operand) & (m_reg.a ^ result)) >> 7;
    m_reg.p.carry = m_reg.p.overflow;
    m_reg.p.zero = m_reg.a == 0;
    m_reg.p.negative = m_reg.a >> 7;

    m_logger->writeInstruction(m_reg, "adc", result, addr);
}

void nemus::core::CPU::bitAnd(comp::AddressMode addr)
{
    uint8_t operand = m_memory->ReadByte(m_reg, addr);

    m_reg.a &= operand;
    m_reg.p.zero = m_reg.a == 0;
    m_reg.p.negative = m_reg.a >> 7;

    m_logger->writeInstruction(m_reg, "and", operand, addr);
}

void nemus::core::CPU::asl(comp::AddressMode addr)
{
    if (addr == comp::ADDR_MODE_ACCUMULATOR)
    {
        m_reg.p.carry = m_reg.a >> 7;
        m_reg.a <<= 1;
        m_reg.p.zero = m_reg.a == 0;
        m_reg.p.negative = m_reg.a >> 7;
    }
    else
    {
        auto operand = m_memory->ReadByte(m_reg, addr);
        m_reg.p.carry = operand >> 7;
        operand <<= 1;
        m_reg.p.zero = operand == 0;
        m_reg.p.negative = operand >> 7;
        m_memory->WriteByte(m_reg, operand, addr);
    }
    m_logger->writeInstruction(m_reg, "asl", 0, addr);
}

void nemus::core::CPU::branch(bool doJump)
{
    auto b = static_cast<int8_t>(m_memory->ReadByte(m_reg.pc + 1));
    if (doJump)
    {
        m_reg.pc += b;
    }
    m_logger->writeInstruction(m_reg, "branch", b, comp::ADDR_MODE_IMMEDIATE);
}

void nemus::core::CPU::bit(comp::AddressMode addr)
{
    auto operand = m_memory->ReadByte(m_reg, addr) & m_reg.a;

    m_reg.p.negative = operand >> 7;
    m_reg.p.overflow = operand >> 6;
    m_reg.p.zero = operand == 0;

    m_logger->writeInstruction(m_reg, "bit_test", operand, addr);
}

void nemus::core::CPU::compare(uint8_t src, comp::AddressMode addr)
{
    auto operand = m_memory->ReadByte(m_reg, addr);
    m_reg.p.carry = src >= operand;

    uint8_t result = src - operand;
    m_reg.p.zero = result == 0;
    m_reg.p.negative = result >> 7;

    m_logger->writeInstruction(m_reg, "compare", src, addr);
}

void nemus::core::CPU::decrement(comp::AddressMode addr)
{
    auto operand = m_memory->ReadByte(m_reg, addr);

    m_memory->WriteByte(m_reg, operand - 1, addr);
    m_reg.p.zero = (operand - 1) == 0;
    m_reg.p.negative = (operand - 1) >> 7;

    m_logger->writeInstruction(m_reg, "decrement memory", operand, addr);
}

void nemus::core::CPU::decrement(uint8_t &src)
{
    src--;
    m_reg.p.zero = src == 0;
    m_reg.p.negative = src >> 7;
    m_logger->writeInstruction(m_reg, "decrement reg", src, comp::ADDR_MODE_IMMEDIATE);
}

void nemus::core::CPU::xora(comp::AddressMode addr)
{
    auto operand = m_memory->ReadByte(m_reg, addr);
    m_reg.a ^= operand;
    m_reg.p.zero = m_reg.a == 0;
    m_reg.p.negative = m_reg.a >> 7;
    m_logger->writeInstruction(m_reg, "xor", operand, addr);
}

void nemus::core::CPU::increment(comp::AddressMode addr)
{
    auto operand = m_memory->ReadByte(m_reg, addr) + 1;
    m_memory->WriteByte(m_reg, operand, addr);
    m_reg.p.zero = operand == 0;
    m_reg.p.negative = operand >> 7;
    m_logger->writeInstruction(m_reg, "increment memory", 0, addr);
}

void nemus::core::CPU::increment(uint8_t &src)
{
    src++;
    m_reg.p.zero = src == 0;
    m_reg.p.negative = src >> 7;
    m_logger->writeInstruction(m_reg, "increment reg", 0, comp::ADDR_MODE_IMMEDIATE);
}

void nemus::core::CPU::subJump()
{
    m_memory->Push16(m_reg.pc + 2, m_reg.sp);
    m_reg.pc = m_memory->ReadWord(m_reg.pc + 1) - 3;
    m_logger->writeInstruction(m_reg, "jump_sub", m_reg.pc + 3, comp::ADDR_MODE_ABSOLUTE);
}

void nemus::core::CPU::returnSub()
{
    m_reg.pc = m_memory->Pop16(m_reg.sp);
    m_logger->writeInstruction(m_reg, "return_sub", m_reg.pc, comp::ADDR_MODE_IMMEDIATE);
}

void nemus::core::CPU::jump(comp::AddressMode addr)
{
    m_reg.pc = ((addr == comp::ADDR_MODE_ABSOLUTE)
                    ? m_memory->ReadWord(m_reg.pc + 1)
                    : m_memory->ReadWordBug(m_memory->ReadWord(m_reg.pc + 1))) -
               3;
    m_logger->writeInstruction(m_reg, "jump", m_reg.pc + 3, comp::ADDR_MODE_ABSOLUTE);
}

void nemus::core::CPU::shiftRight(comp::AddressMode addr)
{
    if (addr == comp::ADDR_MODE_ACCUMULATOR)
    {
        m_reg.p.carry = m_reg.a & 1;
        m_reg.a >>= 1;
        m_reg.p.zero = m_reg.a == 0;
        m_reg.p.negative = m_reg.a >> 7;
        m_logger->writeInstruction(m_reg, "lsr", m_reg.a, addr);
    }
    else
    {
        auto operand = m_memory->ReadByte(m_reg, addr);
        m_reg.p.carry = operand & 1;
        operand >>= 1;
        m_reg.p.zero = operand == 0;
        m_reg.p.negative = operand >> 7;
        m_memory->WriteByte(m_reg, operand, addr);
        m_logger->writeInstruction(m_reg, "lsr", operand, addr);
    }
}

void nemus::core::CPU::load(uint8_t &dest, comp::AddressMode addr)
{
    dest = m_memory->ReadByte(m_reg, addr);
    m_reg.p.zero = dest == 0;
    m_reg.p.negative = dest >> 7;
    m_logger->writeInstruction(m_reg, "load", dest, addr);
}

void nemus::core::CPU::ora(comp::AddressMode addr)
{
    auto operand = m_memory->ReadByte(m_reg, addr);
    m_reg.a |= operand;
    m_reg.p.zero = m_reg.a == 0;
    m_reg.p.negative = m_reg.a >> 7;
    m_logger->writeInstruction(m_reg, "ora", operand, addr);
}

void nemus::core::CPU::store(uint8_t src, comp::AddressMode addr)
{
    m_memory->WriteByte(m_reg, src, addr);
    m_logger->writeInstruction(m_reg, "store", src, addr);
}

void nemus::core::CPU::rotateRight(comp::AddressMode addr)
{
    auto carry = m_reg.p.carry;
    if (addr == comp::ADDR_MODE_ACCUMULATOR)
    {
        m_reg.p.carry = m_reg.a & 0x01;
        m_reg.a >>= 1;
        m_reg.a += (carry * 0x80);
        m_reg.p.zero = m_reg.a == 0;
        m_reg.p.negative = m_reg.a >> 7;
    }
    else
    {
        auto operand = m_memory->ReadByte(m_reg, addr);
        m_reg.p.carry = operand & 0x01;
        operand >>= 1;
        operand += carry * 0x80;
        m_reg.p.zero = operand == 0;
        m_reg.p.negative = operand >> 7;
        m_memory->WriteByte(m_reg, operand, addr);
    }
    m_logger->writeInstruction(m_reg, "ror", 0, addr);
}

void nemus::core::CPU::rotateLeft(comp::AddressMode addr)
{
    auto carry = m_reg.p.carry;
    if (addr == comp::ADDR_MODE_ACCUMULATOR)
    {
        m_reg.p.carry = m_reg.a >> 7;
        m_reg.a = (m_reg.a << 1) + m_reg.p.carry;
        m_reg.a += carry;
        m_reg.p.zero = m_reg.a == 0;
        m_reg.p.negative = m_reg.a >> 7;
    }
    else
    {
        auto operand = m_memory->ReadByte(m_reg, addr);
        m_reg.p.carry = operand >> 7;
        operand <<= 1;
        operand += carry;
        m_memory->WriteByte(m_reg, operand, addr);
        m_reg.p.zero = operand == 0;
        m_reg.p.negative = operand >> 7;
    }
    m_logger->writeInstruction(m_reg, "rol", 0, addr);
}

void nemus::core::CPU::subtract(comp::AddressMode addr)
{
    uint16_t operand = m_memory->ReadByte(m_reg, addr);
    uint16_t result = m_reg.a - operand - (1 - m_reg.p.carry);

    m_reg.p.overflow = (~(m_reg.a ^ operand) & (m_reg.a ^ (result & 0xFF))) >> 7;
    m_reg.p.carry = (result & 0xFF00) > 0 ? 1 : 0;
    m_reg.p.zero = (result & 0xFF) == 0;
    m_reg.p.negative = (result & 0xFF) >> 7;

    m_reg.a = result;

    m_logger->writeInstruction(m_reg, "sbc", operand, addr);
}
