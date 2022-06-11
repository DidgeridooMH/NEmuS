#ifndef NEMUS_PPU_H
#define NEMUS_PPU_H

#include <vector>
#include "CPU.h"

#define PATTERN_TABLE_0 0x0000
#define PATTERN_TABLE_1 0x1000

#define PPU_COLOR_BLACK 0x00000000;
#define PPU_COLOR_BLUE 0xFF0000FF;
#define PPU_COLOR_RED 0xFFFF0000;
#define PPU_COLOR_WHITE 0xFFFFFFFF;

namespace nemus::core
{
    struct OAMEntry
    {
        unsigned int id;
        unsigned int y;
        unsigned int index;
        unsigned int attributes;
        unsigned int x;
    };
    struct AddressRegister
    {
        uint16_t coarseXScroll : 5;
        uint16_t coarseYScroll : 5;
        uint16_t nameTableSelect : 2;
        uint16_t fineYScroll : 3;
    };

    class PPU
    {
    private:
        CPU *m_cpu = nullptr;

        Memory *m_memory = nullptr;

        unsigned int *m_frontBuffer = nullptr;
        unsigned int *m_backBuffer = nullptr;

        unsigned char m_oam[0x100];

        OAMEntry m_oamEntries[8];
        unsigned int m_spriteCount;
        unsigned char m_spriteScanline[0x100];
        bool m_hitNextLine = false;

        std::vector<unsigned int> m_sprite0Pixels;

        unsigned char m_dataBuffer = 0;

        struct
        {
            bool nmi;
            bool master_slave;
            bool sprite_height;
            bool bg_tile_select;
            bool sprite_select;
            bool inc_mode;
            int name_select;
        } m_ppuCtrl;

        struct
        {
            int color_emph;
            bool sprite_enable;
            bool bg_enable;
            bool slc_enable;
            bool blc_enable;
            bool greyscale;
        } m_ppuMask;

        struct
        {
            bool vblank;
            bool s0_hit;
            bool sprite_overflow;
        } m_ppuStatus;

        unsigned char m_oamAddr;

        unsigned char m_ppuScrollX;

        unsigned char m_ppuScrollY;

        unsigned int m_oamDMA;

        unsigned int m_ppuRegister;

        unsigned char m_oamTransfer;

        /*----------------*/
        union
        {
            uint16_t addr;
            AddressRegister r;
        } m_v, m_t;
        uint8_t m_fineXScroll;
        bool m_writeLatch;
        uint16_t m_cycle;
        uint16_t m_scanline;
        uint8_t m_attributeBuffer;
        uint8_t m_nameTableBuffer;
        uint8_t m_patternLowBuffer;
        uint8_t m_patternHighBuffer;
        /*----------------*/

        void renderPixel();

        void evaluateSprites();

        int getNameTableAddress(unsigned cycle, unsigned scanline);

    public:
        static constexpr uint16_t MaxCycles() { return 340U; }
        static constexpr uint16_t MaxScanlines() { return 261U; }

        PPU();

        ~PPU();

        void reset();

        void setCPU(nemus::core::CPU *cpu) { m_cpu = cpu; }

        void setMemory(nemus::core::Memory *memory) { m_memory = memory; }

        void tick();

        void IncrementX();

        void IncrementY();

        uint32_t FetchBackgroundPixel();

        unsigned int *getPixels() { return m_frontBuffer; };

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

        void writePPUScroll(uint8_t data);

        void writePPUAddr(uint8_t data);

        void writePPUData(unsigned int data);

        unsigned int readPPUData();

        void writeOAMDMA(unsigned int data);

        void writeVRAM(unsigned char data, int address);

        void dumpRam(std::string filename);
    };
}

#endif
