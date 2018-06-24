#ifndef NEMUS_MEMORY_H
#define NEMUS_MEMORY_H

#include "../Debug/Logger.h"
#include "include/ComponentHelper.h"
#include "PPU.h"
#include "Mappers/Mapper.h"

namespace nemus::core {

        class CPU;

        class Memory {
        private:
            debug::Logger *m_logger;

            PPU *m_ppu;

            Mapper* m_mapper;

            unsigned char *m_ram;

            char *m_rom;

            // TODO: Move to mapper
            char m_mirroring;

        public:
            Memory(debug::Logger *logger, core::PPU *ppu);

            ~Memory();

            void loadRom(std::string filename);

            char readRom(int address) { return m_rom[address]; }

            // TODO: Move to mapper
            char getMirroring() { return m_mirroring; }

            unsigned int readByte(unsigned int address);

            unsigned int readByte(comp::Registers registers, comp::AddressMode addr);

            unsigned int readWord(unsigned int address);

            unsigned int readWordBug(unsigned int address);

            bool writeByte(unsigned char data, unsigned int address);

            void writeByte(comp::Registers registers, unsigned int src, comp::AddressMode addr);

            void push(unsigned int data, unsigned int &sp);

            void push16(unsigned int data, unsigned int &sp);

            unsigned int pop(unsigned int &sp);

            unsigned int pop16(unsigned int &sp);

        };

}

#endif
