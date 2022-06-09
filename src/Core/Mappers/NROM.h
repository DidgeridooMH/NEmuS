#ifndef NEMUS_NROM_H
#define NEMUS_NROM_H

#include <vector>

#include "Mapper.h"

namespace nemus::core
{

    class NROM : public Mapper
    {
    private:
        unsigned char *m_fixedCPUMemory;

        unsigned char *m_fixedPPUMemory;

        unsigned char *m_tableA;
        unsigned char *m_tableB;
        unsigned char *m_tableC;
        unsigned char *m_tableD;

        int m_mirroring;

        unsigned char *getMirroringTable(unsigned address);

    public:
        NROM(const std::vector<char> &romStart);
        ~NROM();

        unsigned char readByte(unsigned int address) override;

        unsigned char readBytePPU(unsigned int address) override;

        void writeByte(unsigned char data, unsigned int address) override;

        void writeBytePPU(unsigned char data, unsigned int address) override;

        int getMirroring() override { return m_mirroring; }
    };

}

#endif
