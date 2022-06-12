#ifndef NEMUS_MMC1_H
#define NEMUS_MMC1_H

#include <vector>

#include "Mapper.h"

namespace nemus::core
{
    struct MMC1Control
    {
        int chr_mode;
        int prg_mode;
        MirrorMode mirroring;
    };

    class MMC1 : public Mapper
    {
    public:
        MMC1(const std::vector<char> &gameData);
        MMC1(const std::vector<char> &gameData, char *savStart, size_t savSize);

        uint8_t readByte(uint32_t address) override;
        void writeByte(uint8_t data, uint32_t address) override;

        uint8_t readBytePPU(uint32_t address) override;
        void writeBytePPU(uint8_t data, uint32_t address) override;

        MirrorMode getMirroring() override { return m_control.mirroring; };

    private:
        static constexpr size_t NameTableSize = 0x400;
        static constexpr size_t PPUMemorySize = 0x8000;
        static constexpr size_t PPUCharRomSize = 0x2000;
        static constexpr size_t RomBankSize = 0x4000;
        static constexpr size_t SaveRomSize = 0x2000;
        static constexpr uint8_t InitialShiftRegister = 0x10;

        std::vector<uint8_t> m_CPUMemory;
        std::vector<uint8_t> m_PPUMemory;

        std::vector<uint8_t> m_tableA;
        std::vector<uint8_t> m_tableB;
        std::vector<uint8_t> m_tableC;
        std::vector<uint8_t> m_tableD;

        int32_t m_prgBank0;
        int32_t m_prgBank1;
        int32_t m_chrBank0;
        int32_t m_chrBank1;

        int32_t m_maxPrgBanks;

        uint8_t m_shiftRegister;

        MMC1Control m_control;

        uint8_t m_prgBank;
        uint8_t m_chrBank;

        void adjustShiftRegister(uint8_t data, uint32_t address);

        void writeControl();
        void writeCHRBank0();
        void writeCHRBank1();
        void writePRGBank();

        void updateBanks();

        uint32_t getMirroringTable(uint32_t address);
        void writeNametable(uint8_t data, uint32_t address);
        unsigned char readNametable(uint32_t address);
    };

}

#endif