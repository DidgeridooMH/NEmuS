#ifndef NEMUS_PPU_H
#define NEMUS_PPU_H

#include <SDL_quit.h>
#include <iostream>
#include <vector>
#include "CPU.h"

#define PATTERN_TABLE_0 0x0000
#define PATTERN_TABLE_1 0x1000

#define PPU_COLOR_BLACK 0x00000000;
#define PPU_COLOR_BLUE  0xFF0000FF;
#define PPU_COLOR_RED   0xFFFF0000;
#define PPU_COLOR_WHITE 0xFFFFFFFF;

namespace nemus::core {
    class PPU {
    private:
        nemus::core::CPU* m_cpu = nullptr;

        nemus::core::Memory* m_memory = nullptr;

        Uint32 *m_pixelFIFO;

        unsigned char* m_vram;
        unsigned char m_oam[0xFF];

        int m_cycle = 0;

        int m_scanline = 0;

        unsigned char m_dataBuffer = 0;

        // TODO: implement nmi here
        struct {
            bool nmi;
            bool master_slave;
            bool sprite_height;
            bool bg_tile_select;
            bool sprite_select;
            bool inc_mode;
            int name_select;
        } m_ppuCtrl;

        struct {
            int color_emph;
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

        unsigned char m_oamData;

        unsigned char m_ppuScrollX;

        unsigned char m_ppuScrollY;

        unsigned int m_ppuAddr;

        unsigned int m_ppuTmpAddr;

        unsigned int m_oamDMA;

        unsigned int m_ppuRegister;

        bool m_addressLatch = false;

        void initVRam();

        void createPatternRow(int address, unsigned char* pixels);

        void drawTile(int x, int y, int id);

        void renderPixel();

        void renderNametable();

        int renderSprites(int x, int y, int sliver, int pixel);

        void renderBackground();

        void dumpOAM(std::string filename);

    public:
        PPU();

        ~PPU();

        void setCPU(nemus::core::CPU* cpu) { m_cpu = cpu; }

        void setMemory(nemus::core::Memory* memory) { m_memory = memory; initVRam(); }

        void tick();

        Uint32* getPixels() { return m_pixelFIFO; };

        void writePPU(unsigned int data, unsigned int address);

        unsigned int readPPU(unsigned int address);

        void writePPUCtrl(unsigned int data);

        unsigned int readPPUCtrl();

        void writePPUMask(unsigned int data);

        unsigned int readPPUMask();

        void writePPUStatus(unsigned int data);

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

        unsigned char readVRAM(int address);

        void writeVRAM(unsigned char data, int address);

        void dumpRam(std::string filename);
    };
}

#endif
