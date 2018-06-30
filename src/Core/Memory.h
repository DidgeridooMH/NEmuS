#ifndef NEMUS_MEMORY_H
#define NEMUS_MEMORY_H

#include "../Debug/Logger.h"
#include "include/ComponentHelper.h"
#include "PPU.h"
#include "Mappers/Mapper.h"
#include "Input.h"

namespace nemus::core {
    class CPU;

    struct FileInfo {
        char* start_of_file;
        long size;
    };

    class Memory {
    private:
        debug::Logger *m_logger;

        PPU *m_ppu;

        Mapper* m_mapper;

        Input* m_input;

        unsigned char *m_ram;

        char *m_rom;

        // TODO: Move to mapper
        char m_mirroring;

        FileInfo loadSaveFile(std::string filename);

    public:
        Memory(debug::Logger *logger, core::PPU *ppu, core::Input* input, std::string filename);

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

        unsigned int readPPUByte(unsigned int address);

        void writePPUByte(unsigned char data, unsigned int address);

        void push(unsigned int data, unsigned int &sp);

        void push16(unsigned int data, unsigned int &sp);

        unsigned int pop(unsigned int &sp);

        unsigned int pop16(unsigned int &sp);

    };

}

#endif
