#ifndef NEMUS_NROM_H
#define NEMUS_NROM_H

#include "Mapper.h"

namespace nemus::core {

    class NROM : public Mapper {
    private:
        unsigned char* m_fixedCPUMemory;

        unsigned char* m_fixedPPUMemory;

        int m_mirroring;

    public:
        NROM(char* romStart);
        ~NROM();

        unsigned int readByte(unsigned int address) override;

        unsigned int readBytePPU(unsigned int address) override;

        void writeByte(unsigned char data, unsigned int address) override;

        void writeBytePPU(unsigned char data, unsigned int address) override;

        int getMirroring() override { return m_mirroring; }
    };

}

#endif
