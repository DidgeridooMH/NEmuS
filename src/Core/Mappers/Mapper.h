#ifndef NEMUS_MAPPER_H
#define NEMUS_MAPPER_H

#define MIRROR_HORIZONTAL 0
#define MIRROR_VERTICAL   1
#define MIRROR_OS_LOWER   2
#define MIRROR_OS_UPPER   3

namespace nemus::core {

    class Mapper {
    public:
        virtual ~Mapper() = default;

        virtual unsigned char readByte(unsigned int address) = 0;

        virtual unsigned char readBytePPU(unsigned int address) = 0;

        virtual void writeByte(unsigned char data, unsigned int address) = 0;

        virtual void writeBytePPU(unsigned char data, unsigned int address) = 0;

        virtual int getMirroring() = 0;
    };

}

#endif