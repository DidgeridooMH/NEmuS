#ifndef NEMUS_PPU_H
#define NEMUS_PPU_H

#include <vector>
#include <array>

#include "CPU.h"

#define PATTERN_TABLE_0 0x0000
#define PATTERN_TABLE_1 0x1000

#define PPU_COLOR_BLACK 0xFF000000;
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

    class CPU;
    class Memory;
    class PPU
    {
    public:
        static constexpr uint16_t MaxCycles() { return 340U; }
        static constexpr uint16_t MaxScanlines() { return 261U; }
        static constexpr uint32_t Palette[] = {
            0xFF666666, 0xFF002A88, 0xFF1412A7, 0xFF3B00A4, 0xFF5C007E, 0xFF6E0040, 0xFF6C0600, 0xFF561D00,
            0xFF333500, 0xFF0B4800, 0xFF005200, 0xFF004F08, 0xFF00404D, 0xFF000000, 0xFF000000, 0xFF000000,
            0xFFADADAD, 0xFF155FD9, 0xFF4240FF, 0xFF7527FE, 0xFFA01ACC, 0xFFB71E7B, 0xFFB53120, 0xFF994E00,
            0xFF6B6D00, 0xFF388700, 0xFF0C9300, 0xFF008F32, 0xFF007C8D, 0xFF000000, 0xFF000000, 0xFF000000,
            0xFFFFFEFF, 0xFF64B0FF, 0xFF9290FF, 0xFFC676FF, 0xFFF36AFF, 0xFFFE6ECC, 0xFFFE8170, 0xFFEA9E22,
            0xFFBCBE00, 0xFF88D800, 0xFF5CE430, 0xFF45E082, 0xFF48CDDE, 0xFF4F4F4F, 0xFF000000, 0xFF000000,
            0xFFFFFEFF, 0xFFC0DFFF, 0xFFD3D2FF, 0xFFE8C8FF, 0xFFFBC2FF, 0xFFFEC4EA, 0xFFFECCC5, 0xFFF7D8A5,
            0xFFE4E594, 0xFFCFEF96, 0xFFBDF4AB, 0xFFB3F3CC, 0xFFB5EBF2, 0xFFB8B8B8, 0xFF000000, 0xFF000000};

        PPU();

        void reset();

        void setCPU(CPU *cpu) { m_cpu = cpu; }

        Memory *GetMemory() const { return m_memory; }
        void setMemory(Memory *memory) { m_memory = memory; }

        void tick();

        void IncrementX();

        void IncrementY();

        uint32_t FetchBackgroundPixel();

        unsigned int *getPixels();

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

        uint8_t ReadPPUData();

        void writeOAMDMA(unsigned int data);

        void writeVRAM(unsigned char data, int address);

        void dumpRam(std::string filename);

    private:
        CPU *m_cpu = nullptr;
        Memory *m_memory = nullptr;

        std::array<std::vector<uint32_t>, 2> m_frameBuffers;
        size_t m_activeBuffer;

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

        unsigned int m_oamDMA;

        unsigned int m_ppuRegister;

        unsigned char m_oamTransfer;

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
        uint64_t m_tileData;

        void renderPixel();

        void evaluateSprites();
    };
}

#endif
