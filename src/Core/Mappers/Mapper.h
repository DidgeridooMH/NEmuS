#ifndef NEMUS_MAPPER_H
#define NEMUS_MAPPER_H

namespace nemus::core {

    class Mapper {
    public:
        virtual unsigned int readByte(unsigned int address) = 0;

        virtual void writeByte(unsigned char data, unsigned int address) = 0;
    };

}

#endif