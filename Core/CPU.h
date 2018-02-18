#ifndef NEMUS_CPU_H
#define NEMUS_CPU_H

#include "../Debug/Logger.h"
#include "include/ComponentHelper.h"

namespace nemus::core {

    class Memory;

    class CPU {
    private:
        bool m_running;

        // Instructions table
        std::string m_opcodes[256];

        int m_opsize[256];

        int m_cyclesTable[256];

        comp::Registers m_reg;

        comp::Interrupt m_interrupt;

        struct {
            bool N;
            bool Z;
            bool C;
            bool I;
            bool D;
            bool V;
        } m_flags;

        Memory* m_memory = nullptr;

        debug::Logger* m_logger;

        void generateOP();

        void resetRegisters();

        void checkFlags(unsigned int operand, int flagbits);

        void setFlags(unsigned int flagbits);

        void setFlags(comp::Flag flag);

        void unsetFlags(comp::Flag flag);

        unsigned int generateFlags();

        void branch(bool doJump);

        void load(unsigned int &dest, comp::AddressMode addr);

        void store(unsigned int &src, comp::AddressMode addr);

        void compare(unsigned int &src, comp::AddressMode addr);

        void decrement(comp::AddressMode addr);

        void decrement(unsigned int &src);

        void increment(comp::AddressMode addr);

        void increment(unsigned int &src);

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

    public:
        CPU(Memory* memory, debug::Logger* logger);

        int tick();

        bool isRunning() { return m_running; }

        void setInterrupt(comp::Interrupt interrupt) { m_interrupt = interrupt; }
    };

}


#endif
