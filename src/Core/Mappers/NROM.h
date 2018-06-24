#ifndef NEMUS_NROM_H
#define NEMUS_NROM_H

#include "Mapper.h"

namespace nemus::core {

    class NROM : public Mapper {
    private:
        unsigned char m_fixedMemory[0xA000];

    public:
        NROM(char* romStart);
        ~NROM();

        unsigned int readByte(unsigned int address) override;

        void writeByte(unsigned char data, unsigned int address) override;
    };

}

#endif
