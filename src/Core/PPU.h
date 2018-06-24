#ifndef NEMUS_PPU_H
#define NEMUS_PPU_H

#include "CPU.h"

#define PATTERN_TABLE_0 0x0000
#define PATTERN_TABLE_1 0x1000

#define PPU_COLOR_BLACK 0x00000000;
#define PPU_COLOR_BLUE  0xFF0000FF;
#define PPU_COLOR_RED   0xFFFF0000;
#define PPU_COLOR_WHITE 0xFFFFFFFF;

namespace nemus::core {
    struct OAMEntry {
        unsigned int y;
        unsigned int index;
        unsigned int attributes;
        unsigned int x;
    };

    class PPU {
    private:
        CPU* m_cpu = nullptr;

        Memory* m_memory = nullptr;

        unsigned int *m_pixelBuffer;

        unsigned char* m_vram;

        // Primary OAM holds data for all available sprites
        unsigned char m_oam[0x100];

        OAMEntry m_oamEntries[8];
        unsigned int m_spriteCount;
        unsigned char m_spriteScanline[0x100];

        unsigned int m_cycle = 0;

        unsigned int m_scanline = 0;

        unsigned char m_dataBuffer = 0;

        struct {
            bool nmi;
            bool master_slave;
            bool sprite_height;
            bool bg_tile_select;
            bool sprite_select;
            bool inc_mode;
            int  name_select;
        } m_ppuCtrl;

        struct {
            int  color_emph;
            bool sprite_enable;
            bool bg_enable;
            bool slc_enable;
            bool blc_enable;
            bool greyscale;
        } m_ppuMask;

        struct {
            bool vblank;
            bool s0_hit;
            bool sprite_overflow;
        } m_ppuStatus;

        unsigned char m_oamAddr;

        unsigned char m_ppuScrollX;

        unsigned char m_ppuScrollY;

        unsigned int m_ppuAddr;

        unsigned int m_ppuTmpAddr;

        unsigned int m_oamDMA;

        unsigned int m_ppuRegister;

        unsigned char m_oamTransfer;

        bool m_addressLatch = false;

        void initVRam();

        void renderPixel();

        int renderSprites(int x, int y, int sliver, int pixel);

        void dumpOAM(std::string filename);

        void evaluateSprites();

    public:
        PPU();

        ~PPU();

        void setCPU(nemus::core::CPU* cpu) { m_cpu = cpu; }

        void setMemory(nemus::core::Memory* memory) { m_memory = memory; initVRam(); }

        void tick();

        unsigned int* getPixels() { return m_pixelBuffer; };

        void writePPU(unsigned int data, unsigned int address);

        unsigned int readPPU(unsigned int address);

        void writePPUCtrl(unsigned int data);

        void writePPUMask(unsigned int data);

        unsigned int readPPUMask();

        unsigned int readPPUStatus();

        void writeOAMAddr(unsigned int data) { m_oamAddr = data; }

        unsigned int readOAMAddr() { return m_oamAddr; }

        void writeOAMData(unsigned int data);

        unsigned int readOAMData();

        void writePPUScroll(unsigned int data);

        void writePPUAddr(unsigned int data);

        void writePPUData(unsigned int data);

        unsigned int readPPUData();

        void writeOAMDMA(unsigned int data);

        void writeVRAM(unsigned char data, int address);

        void dumpRam(std::string filename);
    };
}

#endif
