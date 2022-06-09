#ifndef NEMUS_MEMORY_H
#define NEMUS_MEMORY_H

#include <memory>
#include <vector>

#include "../Debug/Logger.h"
#include "ComponentHelper.h"
#include "PPU.h"
#include "Mappers/Mapper.h"
#include "Input.h"

namespace nemus::core
{
    class CPU;

    struct FileInfo
    {
        char *start_of_file;
        long size;
    };

    class Memory
    {
    private:
        debug::Logger *m_logger;

        PPU *m_ppu;

        Mapper *m_mapper;

        Input *m_input;

        unsigned char *m_ram;

        std::shared_ptr<std::vector<char>> m_rom;

        FileInfo loadSaveFile(std::string filename);

    public:
        Memory(debug::Logger *logger, core::PPU *ppu, core::Input *input,
               const std::vector<char> &gameData);

        ~Memory();

        void loadRom(const std::vector<char> &gameData, const std::string &filename = "");

        inline char readRom(int address) { return m_rom->at(address); }

        int getMirroring() { return m_mapper->getMirroring(); }

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

        unsigned int checkPageCross(comp::Registers registers, comp::AddressMode mode);
    };

}

#endif
