#ifndef NEMUS_MMC1_H
#define NEMUS_MMC1_H

#include <vector>

#include "Mapper.h"

namespace nemus::core
{
    struct MMC1Control
    {
        uint8_t mirroring : 2;
        uint8_t prg_mode : 2;
        uint8_t chr_mode : 1;
    };

    class MMC1 : public Mapper
    {
    public:
        MMC1(const std::vector<char> &gameData);
        MMC1(const std::vector<char> &gameData, char *savStart, size_t savSize);

        uint8_t ReadByte(uint16_t address) override;
        uint8_t ReadBytePPU(uint16_t address) override;

        void WriteByte(uint8_t data, uint16_t address) override;
        void WriteBytePPU(uint8_t data, uint16_t address) override;

    private:
        // TODO: This is incorrect with the new enumeration.
        MirrorMode GetMirroring() { return static_cast<MirrorMode>(m_control.mirroring); }
        void AdjustShiftRegister(uint8_t data, uint16_t address);

        void WriteControl();
        void WriteCHRBank0();
        void WriteCHRBank1();
        void WritePRGBank();

        void UpdateBanks();

        size_t GetMirroringTable(uint16_t address);
        void WriteNametable(uint8_t data, uint16_t address);
        uint8_t ReadNametable(uint16_t address);

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
    };

}

#endif