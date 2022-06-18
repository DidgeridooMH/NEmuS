#ifndef NEMUS_CPU_H
#define NEMUS_CPU_H

#include "../Debug/Logger.h"
#include "ComponentHelper.h"

#include <Core/Memory.h>

namespace nemus::core
{
    // TODO: Better architecture to avoid this.
    class Memory;
    class CPU
    {
    public:
        CPU(Memory *memory, debug::Logger *logger);
        int tick();
        bool isRunning() { return m_running; }
        void setInterrupt(comp::Interrupt interrupt) { m_interrupt = interrupt; }

    private:
        void resetRegisters();
        void branch(bool doJump);
        void load(uint8_t &dest, comp::AddressMode addr);
        void store(uint8_t src, comp::AddressMode addr);
        void compare(uint8_t src, comp::AddressMode addr);
        void decrement(comp::AddressMode addr);
        void decrement(uint8_t &src);
        void increment(comp::AddressMode addr);
        void increment(uint8_t &src);
        void subJump();
        void jump(comp::AddressMode addr);
        void returnSub();
        void bit(comp::AddressMode addr);
        void ora(comp::AddressMode addr);
        void xora(comp::AddressMode addr);
        void bitAnd(comp::AddressMode addr);
        void shiftRight(comp::AddressMode addr);
        void rotateRight(comp::AddressMode addr);
        void asl(comp::AddressMode addr);
        void rotateLeft(comp::AddressMode addr);
        void adc(comp::AddressMode addr);
        void subtract(comp::AddressMode addr);
        void interrupt();

        comp::Registers m_reg;
        comp::Interrupt m_interrupt;
        Memory *m_memory;
        debug::Logger *m_logger;
        bool m_running;
    };

}

#endif
