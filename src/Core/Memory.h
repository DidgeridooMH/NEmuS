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
    struct FileInfo
    {
        char *start_of_file;
        long size;
    };

    class PPU;
    class Memory
    {
    public:
        Memory(debug::Logger *logger, PPU *ppu, Input *input,
               const std::vector<char> &gameData);

        void LoadRom(const std::vector<char> &gameData);

        uint8_t ReadByte(uint16_t address);
        uint8_t ReadByte(const comp::Registers &registers, comp::AddressMode mode);

        uint16_t ReadWord(uint16_t address);
        uint16_t ReadWordBug(uint16_t address);

        void WriteByte(uint8_t data, uint16_t address);
        void WriteByte(const comp::Registers &registers, uint8_t src, comp::AddressMode mode);

        uint8_t ReadPPUByte(uint16_t address);
        void WritePPUByte(uint8_t data, uint16_t address);

        void Push(uint8_t data, uint8_t &sp);
        void Push16(uint16_t data, uint8_t &sp);

        uint8_t Pop(uint8_t &sp);
        uint16_t Pop16(uint8_t &sp);

        bool CheckPageCross(const comp::Registers &registers, comp::AddressMode mode);

    private:
        static constexpr size_t InternalRamSize = 0x800;
        static constexpr size_t NameTableSize = 0x400UL;
        static constexpr size_t PaletteSize = 0x20;

        uint16_t GetAddress(const comp::Registers &registers, comp::AddressMode mode);

        debug::Logger *m_logger;
        PPU *m_ppu;
        Input *m_input;

        std::unique_ptr<Mapper> m_mapper;
        std::array<std::vector<uint8_t>, 4> m_nameTables;
        std::vector<uint8_t> m_palette;

        std::vector<uint8_t> m_ram;
    };

}

#endif
