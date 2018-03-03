#include "PPU.h"

#include "Memory.h"
#include "../UI/Screen.h"

nemus::core::PPU::PPU() {
    m_cycle = 0;
    m_scanline = 0;

    m_oamDMA = 2;

    m_vram = new unsigned char[0x8000];

    m_pixelFIFO = new Uint32[SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32)];
}

nemus::core::PPU::~PPU() {
    delete[] m_pixelFIFO;
}

void nemus::core::PPU::initVRam() {
    for(int i = 0; i < 0x2000; i++) {
        m_vram[i] = m_memory->readRom(0x8000 + i + 0x10);
    }
}

// TODO: Compensate for scrolling
void nemus::core::PPU::renderPixel() {
    int x = m_cycle / 8;
    int y = m_scanline / 8;
    int sliver = m_scanline % 8;
    int pixel = m_cycle % 8;
    int tileNum = x + (y * 32);

    int nameTableAddress = 0x2000 + (0x400 * m_ppuCtrl.name_select);

    int tileID = m_vram[nameTableAddress + tileNum];

    int plane0 = 0, plane1 = 0;

    if(m_ppuCtrl.bg_tile_select) {
        plane0 = m_vram[PATTERN_TABLE_1 + (tileID * 0x10) + sliver];
        plane1 = m_vram[PATTERN_TABLE_1 + (tileID * 0x10) + sliver + 0x08];
    } else  {
        plane0 = m_vram[PATTERN_TABLE_0 + (tileID * 0x10) + sliver];
        plane1 = m_vram[PATTERN_TABLE_0 + (tileID * 0x10) + sliver + 0x08];
    }

    int color = (plane0 >> (7 - pixel)) & 0x1;

    if(pixel < 7) {
        color |= (plane1 >> (6 - pixel)) & 0x2;
    } else {
        color |= (plane1 << 1) & 0x2;
    }

    switch(color) {
        case 0:
            m_pixelFIFO[m_cycle + (m_scanline * 256)] = PPU_COLOR_BLACK;
            break;
        case 1:
            m_pixelFIFO[m_cycle + (m_scanline * 256)] = PPU_COLOR_RED;
            break;
        case 2:
            m_pixelFIFO[m_cycle + (m_scanline * 256)] = PPU_COLOR_BLUE;
            break;
        case 3:
            m_pixelFIFO[m_cycle + (m_scanline * 256)] = PPU_COLOR_WHITE;
            break;
    }
}

void nemus::core::PPU::tick() {
    // Visible Scanlines
    if(m_scanline < 240) {
        // Cycle 0 idle
        if(m_cycle == 0) {
            m_ppuStatus.vblank = false;
        }

        renderPixel();

    } else if(m_scanline == 240) {
        // IDLE
    } else if(m_scanline < 261) {
        if(m_cycle == 1) {
            if(m_ppuCtrl.nmi) {
                m_cpu->setInterrupt(comp::INT_NMI);
            }

            m_ppuStatus.vblank = true;
        }
    } else {
        m_scanline = 0;
        m_cycle = 0;
        return;
    }

    if(++m_cycle > 256) {
        m_scanline++;
        m_cycle = 0;
    }
}

void nemus::core::PPU::writePPU(unsigned int data, unsigned int address) {
    switch(address) {
        case 0x2000:
            writePPUCtrl(data);
            break;
        case 0x2001:
            writePPUMask(data);
            break;
        case 0x2002:
            writePPUStatus(data);
            break;
        case 0x2003:
            writeOAMAddr(data);
            break;
        case 0x2004:
            writeOAMData(data);
            break;
        case 0x2005:
            writePPUScroll(data);
            break;
        case 0x2006:
            writePPUAddr(data);
            break;
        case 0x2007:
            writePPUData(data);
            break;
        case 0x4014:
            writeOAMDMA(data);
            break;
    }
}

unsigned int nemus::core::PPU::readPPU(unsigned int address) {
    switch(address) {
        case 0x2000:
            return readPPUCtrl();
        case 0x2001:
            return readPPUMask();
        case 0x2002:
            return readPPUStatus();
        case 0x2003:
            return readOAMAddr();
        case 0x2004:
            return readOAMData();
        case 0x2007:
            return readPPUData();
        case 0x4014:
            break;
    }
}

void nemus::core::PPU::writePPUCtrl(unsigned int data){
    m_ppuCtrl.nmi = (data & 0x80) != 0;
    m_ppuCtrl.master_slave = (data & 0x40) != 0;
    m_ppuCtrl.sprite_height = (data & 0x20) != 0;
    m_ppuCtrl.bg_tile_select = (data & 0x10) != 0;
    m_ppuCtrl.sprite_select = (data & 0x08) != 0;
    m_ppuCtrl.inc_mode = (data & 0x04) != 0;
    m_ppuCtrl.name_select = data & 0x03;
}

unsigned int nemus::core::PPU::readPPUCtrl() {
    unsigned int result = 0;

    if(m_ppuCtrl.nmi) result += 0x80;
    if(m_ppuCtrl.master_slave) result += 0x40;
    if(m_ppuCtrl.sprite_height) result += 0x20;
    if(m_ppuCtrl.bg_tile_select) result += 0x10;
    if(m_ppuCtrl.sprite_select) result += 0x08;
    if(m_ppuCtrl.inc_mode) result += 0x04;
    result += m_ppuCtrl.name_select;

    return result;
}

void nemus::core::PPU::writePPUMask(unsigned int data) {
    m_ppuMask.color_emph = (data >> 5) & 0xF;
    m_ppuMask.sprite_enable = (data & 0x10) != 0;
    m_ppuMask.bg_enable = (data & 0x08) != 0;
    m_ppuMask.slc_enable = (data & 0x04) != 0;
    m_ppuMask.blc_enable = (data & 0x02) != 0;
    m_ppuMask.greyscale = (data & 0x01) != 0;
}

unsigned int nemus::core::PPU::readPPUMask() {
    unsigned int result = 0;

    result += m_ppuMask.color_emph << 5;
    if(m_ppuMask.sprite_enable) result += 0x10;
    if(m_ppuMask.bg_enable) result += 0x08;
    if(m_ppuMask.slc_enable) result += 0x04;
    if(m_ppuMask.blc_enable) result += 0x02;
    if(m_ppuMask.greyscale) result += 0x01;

    return result;
}

void nemus::core::PPU::writePPUStatus(unsigned int data) {
    m_ppuStatus.vblank = (data & 0x80) != 0;
    m_ppuStatus.s0_hit = (data & 0x40) != 0;
    m_ppuStatus.sprite_overflow = (data & 0x20) != 0;
}

unsigned int nemus::core::PPU::readPPUStatus() {
    unsigned int result = 0;

    if(m_ppuStatus.vblank) result += 0x80;
    if(m_ppuStatus.s0_hit) result += 0x40;
    if(m_ppuStatus.sprite_overflow) result += 0x20;

    m_ppuStatus.vblank = false;
    m_addressLatch = false;

    return result;
}

void nemus::core::PPU::writePPUAddr(unsigned int data) {
    if(!m_addressLatch) {
        m_ppuTmpAddr = 0;
        m_ppuTmpAddr = (data << 8) & 0x3F00;

        m_addressLatch = true;
    } else {
        m_ppuAddr = (m_ppuTmpAddr & 0x3F00) | (data & 0xFF);

        m_addressLatch = false;
    }
}

void nemus::core::PPU::writePPUData(unsigned int data) {
    writeVRAM(data, m_ppuAddr);

    if(!m_ppuCtrl.inc_mode) {
        m_ppuAddr += 1;
    } else {
        m_ppuAddr += 32;
    }
}

unsigned int nemus::core::PPU::readPPUData() {
    unsigned int value = m_vram[m_ppuAddr];

    if(!m_ppuCtrl.inc_mode) {
        m_ppuAddr += 1;
    } else {
        m_ppuAddr += 32;
    }

    unsigned char ret = m_dataBuffer;
    m_dataBuffer = value;

    return ret;
}

void nemus::core::PPU::writePPUScroll(unsigned int data) {
    if(!m_addressLatch) {
        m_ppuTmpAddr &= ~0x1f;
        m_ppuTmpAddr |= (data >> 3) & 0x1f;

        m_ppuScrollX = data & 0x7;

        m_addressLatch = true;
    } else {
        m_ppuTmpAddr &= ~0x73E0;
        m_ppuTmpAddr |= ((data & 0x7) << 12) |
                        ((data & 0xF8) << 2);

        m_addressLatch = false;
    }
}

void nemus::core::PPU::writeVRAM(unsigned char data, int address) {
    m_vram[address] = data;

    if(m_memory->getMirroring()) {
        if(address >= 0x2000 && address < 0x2800) {
            m_vram[address + 0x800] = data;
        } else if(address >= 0x2800 && address < 0x3000) {
            m_vram[address - 0x800] = data;
        }
    } else {
        // TODO: Horizontal mirroring
    }
}

void nemus::core::PPU::dumpRam(std::string filename) {
    std::fstream output;
    output.open(filename, std::ios::out | std::ios::binary);

    for(int i = 0; i < 0x4000; i++) {
        output << m_vram[i];
    }

    output.close();
}

void nemus::core::PPU::dumpOAM(std::string filename) {
    std::fstream output;
    output.open(filename, std::ios::out | std::ios::binary);

    for(int i = 0; i < 0x0F; i++) {
        output << m_oam[i];
    }

    output.close();
}

void nemus::core::PPU::writeOAMDMA(unsigned int data) {
    unsigned int cpuAddress = data << 8;

    for(int i = 0; i < 0x10; i++) {
        m_oam[m_oamAddr + i] = (unsigned char)(m_memory->readByte(cpuAddress + i));
    }

    dumpOAM("oam.dat");
}