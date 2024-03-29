#ifndef NEMUS_MMC1_H
#define NEMUS_MMC1_H

#include <vector>

#include "Mapper.h"

namespace nemus::core
{

    class MMC1 : public Mapper
    {
    private:
        unsigned char *m_CPUMemory;

        unsigned char *m_PPUMemory;

        unsigned char *m_tableA;
        unsigned char *m_tableB;
        unsigned char *m_tableC;
        unsigned char *m_tableD;

        int m_prgBank0;
        int m_prgBank1;
        int m_chrBank0;
        int m_chrBank1;

        int m_maxPrgBanks;

        unsigned char m_shiftRegister = 0x10;

        struct
        {
            int chr_mode;
            int prg_mode;
            int mirroring;
        } m_control;

        unsigned char m_prgBank;
        unsigned char m_chrBank;

        void adjustShiftRegister(unsigned char data, unsigned int address);

        void writeControl();
        void writeCHRBank0();
        void writeCHRBank1();
        void writePRGBank();

        void updateBanks();

        unsigned getMirroringTable(unsigned address);
        void writeNametable(unsigned char data, unsigned address);
        unsigned char readNametable(unsigned address);

    public:
        MMC1(const std::vector<char> &gameData);
        MMC1(const std::vector<char> &gameData, char *savStart, long savSize);
        ~MMC1();

        unsigned char readByte(unsigned int address) override;

        unsigned char readBytePPU(unsigned int address) override;

        void writeByte(unsigned char data, unsigned int address) override;

        void writeBytePPU(unsigned char data, unsigned int address) override;

        int getMirroring() override { return m_control.mirroring; };
    };

}

#endif