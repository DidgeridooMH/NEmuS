#include <iostream>
#include <vector>
#include "PPU.h"
#include "Memory.h"
#include "../UI/Screen.h"

namespace nemus::core
{
  PPU::PPU()
      : m_frameBuffers({std::vector<uint32_t>(SCREEN_WIDTH * SCREEN_HEIGHT),
                        std::vector<uint32_t>(SCREEN_WIDTH * SCREEN_HEIGHT)}),
        m_activeBuffer(0)
  {
    reset();
  }

  void PPU::reset()
  {
    m_oamAddr = 0;

    m_v.addr = 0;
    m_t.addr = 0;

    for (int i = 0; i < 0x100; i++)
    {
      m_oam[i] = 0;
    }

    m_ppuCtrl.nmi = false;
    m_ppuCtrl.master_slave = false;
    m_ppuCtrl.sprite_height = false;
    m_ppuCtrl.bg_tile_select = false;
    m_ppuCtrl.sprite_select = false;
    m_ppuCtrl.inc_mode = false;
    m_ppuCtrl.name_select = 0;

    m_ppuMask.color_emph = 0;
    m_ppuMask.sprite_enable = false;
    m_ppuMask.bg_enable = false;
    m_ppuMask.slc_enable = false;
    m_ppuMask.blc_enable = false;
    m_ppuMask.greyscale = false;

    m_ppuStatus.vblank = true;
    m_ppuStatus.s0_hit = false;
    m_ppuStatus.sprite_overflow = true;

    m_cycle = MaxCycles();
    m_scanline = MaxScanlines();

    m_oamDMA = 2;
    m_oamTransfer = 0;
  }

  unsigned int *PPU::getPixels() { return m_frameBuffers[m_activeBuffer].data(); }

  // void PPU::renderPixel()
  // {
  //   unsigned int scanline = m_scanline + m_ppuScrollY;

  //   unsigned int cycle = m_cycle + m_ppuScrollX;

  //   int x = (cycle % 256) / 8;
  //   int y = (scanline % 240) / 8;
  //   int sliver = scanline % 8;
  //   int pixel = cycle % 8;
  //   int tileNum = x + (y * 32);

  //   int nameTableAddress = getNameTableAddress(cycle, scanline);

  //   int tileID = m_memory->readPPUByte(nameTableAddress + tileNum);

  //   int plane0 = 0, plane1 = 0;

  //   if (m_ppuMask.bg_enable)
  //   {
  //     if (m_ppuCtrl.bg_tile_select)
  //     {
  //       plane0 = m_memory->readPPUByte(PATTERN_TABLE_1 + (tileID * 0x10) + sliver);
  //       plane1 = m_memory->readPPUByte(PATTERN_TABLE_1 + (tileID * 0x10) + sliver + 0x08);
  //     }
  //     else
  //     {
  //       plane0 = m_memory->readPPUByte(PATTERN_TABLE_0 + (tileID * 0x10) + sliver);
  //       plane1 = m_memory->readPPUByte(PATTERN_TABLE_0 + (tileID * 0x10) + sliver + 0x08);
  //     }

  //     int color = 0;

  //     if (m_ppuMask.bg_enable)
  //     {
  //       color = (plane0 >> (7 - pixel)) & 0x1;

  //       if (pixel < 7)
  //       {
  //         color |= (plane1 >> (6 - pixel)) & 0x2;
  //       }
  //       else
  //       {
  //         color |= (plane1 << 1) & 0x2;
  //       }
  //     }

  //     if (m_ppuMask.sprite_enable)
  //     {
  //       if (m_spriteScanline[m_cycle] > 0)
  //       {
  //         for (unsigned int sprite0Pixel : m_sprite0Pixels)
  //         {
  //           if (m_cycle == sprite0Pixel)
  //           {
  //             m_hitNextLine = true;
  //           }
  //         }

  //         color = m_spriteScanline[m_cycle];
  //       }
  //     }

  //     unsigned int videoAddress = m_cycle + (m_scanline * SCREEN_WIDTH);
  //     videoAddress %= SCREEN_WIDTH * SCREEN_HEIGHT;

  //     switch (color)
  //     {
  //     case 0:
  //       m_backBuffer[videoAddress] = PPU_COLOR_BLACK;
  //       break;
  //     case 1:
  //       m_backBuffer[videoAddress] = PPU_COLOR_RED;
  //       break;
  //     case 2:
  //       m_backBuffer[videoAddress] = PPU_COLOR_BLUE;
  //       break;
  //     case 3:
  //       m_backBuffer[videoAddress] = PPU_COLOR_WHITE;
  //       break;
  //     }
  //   }
  // }

  // void PPU::tick()
  // {
  //     if (m_scanline < 240)
  //     {
  //         renderPixel();
  //     }
  //     else if (m_scanline < 261)
  //     {
  //         if (m_cycle == 1)
  //         {
  //             if (m_ppuCtrl.nmi)
  //             {
  //                 m_cpu->setInterrupt(comp::INT_NMI);
  //             }

  //             m_ppuStatus.vblank = true;
  //         }
  //     }
  //     else
  //     {
  //         m_scanline = 0;
  //         m_cycle = 0;
  //         m_ppuStatus.s0_hit = false;
  //         m_ppuStatus.vblank = false;

  //         m_sprite0Pixels.clear();

  //         unsigned int *tmp = m_backBuffer;
  //         m_backBuffer = m_frontBuffer;
  //         m_frontBuffer = tmp;

  //         return;
  //     }

  //     if (++m_cycle > 256)
  //     {
  //         if (m_hitNextLine)
  //         {
  //             m_ppuStatus.s0_hit = true;
  //             m_hitNextLine = false;
  //         }

  //         m_scanline++;
  //         m_cycle = 0;
  //         m_sprite0Pixels.clear();
  //         if (m_scanline < 240)
  //         {
  //             evaluateSprites();
  //         }
  //     }
  // }

  // TODO: memory operations might use a buffer.
  // TODO: Skipped cycle on (0, 0) with background rendering on on odd frame.
  void PPU::tick()
  {
    if (m_ppuMask.bg_enable || m_ppuMask.sprite_enable)
    { // Visible scanline.
      if (m_scanline < 240 || m_scanline == MaxScanlines())
      {
        // Pixels 257-320 are idle frames.
        if (m_cycle <= 256 || m_cycle > 320)
        {
          if (m_cycle < 256)
          {
            m_tileData >>= 2;
          }
          switch (m_cycle % 8)
          {
          case 0:
            // TODO: Maybe need X scrolling?
            for (auto i = 0; i < 8; i++)
            {
              m_tileData <<= 1;
              m_tileData |= (m_patternHighBuffer >> i) & 1;
              m_tileData <<= 1;
              m_tileData |= (m_patternLowBuffer >> i) & 1;
            }
            IncrementX();
            break;
          case 1:
            /*
             * Name table fetch only uses the lower 12 bits of v
             * as an offset into 0x2000 (the name table locations).
             */
            m_nameTableBuffer = m_memory->readPPUByte((m_v.addr & 0xFFF) | 0x2000);
            break;
          case 3:
            /*
             * Attribute table works with 4x4 chunks so shift the coarse scroll
             * components by 2.
             */
            if (m_cycle != 339)
            {
              // 10 NN11 11YY YXXX
              uint16_t x = m_v.r.coarseXScroll >> 2;
              uint16_t y = m_v.r.coarseYScroll >> 2;
              uint16_t attributeAddress = 0x23C0 | m_v.r.nameTableSelect << 10 | (y << 3) | x;
              m_attributeBuffer = m_memory->readPPUByte(attributeAddress);
            }
            break;
          case 5:
          {
            /*
             * 0Hnnnnnnnn0yyy
             * H: pattern table selection.
             * n: nametable byte
             * y: fine y scroll
             */
            auto patternTableAddress =
                (static_cast<uint16_t>(m_ppuCtrl.bg_tile_select) << 12) |
                (m_nameTableBuffer << 4) |
                m_v.r.fineYScroll;
            m_patternLowBuffer = m_memory->readPPUByte(patternTableAddress);
            break;
          }
          case 7:
          {
            // Same as above but the bit plane at bit 3 is high.
            auto patternTableAddress = (static_cast<uint16_t>(m_ppuCtrl.bg_tile_select) << 12) |
                                       (m_nameTableBuffer << 4) |
                                       0x8 | m_v.r.fineYScroll;
            m_patternHighBuffer = m_memory->readPPUByte(patternTableAddress);
            break;
          }
          default:
            break;
          }
        }

        if (m_cycle == 256)
        {
          IncrementY();
        }
        else if (m_cycle == 257)
        {
          // Reload the X scroll at the end of the scanline.
          m_v.r.coarseXScroll = m_t.r.coarseXScroll;
          m_v.r.nameTableSelect = (m_t.r.nameTableSelect & 1) | (m_v.r.nameTableSelect & 0b10);
        }
      }

      if (m_cycle > 0 && m_cycle <= 256 && m_scanline < 240)
      {
        uint32_t pixel = 0U;
        if (m_ppuMask.bg_enable)
        {
          pixel = FetchBackgroundPixel();
        }

        // TODO: SCREEN_WIDTH should be constexpr
        m_frameBuffers[m_activeBuffer][m_cycle - 1 + m_scanline * SCREEN_WIDTH] = pixel;
      }
    }

    // Reload Y scroll at the end of the frame.
    if (m_scanline == MaxScanlines() && m_cycle >= 280 && m_cycle <= 304)
    {
      m_v.r.coarseYScroll = m_t.r.coarseYScroll;
      m_v.r.fineYScroll = m_t.r.fineYScroll;
      m_v.r.nameTableSelect = (m_t.r.nameTableSelect & 0b10) | (m_v.r.nameTableSelect & 1);
    }

    // Flags for VBlank, etc. are made on Cycle 1 instead of 0.
    if (m_cycle == 1)
    {
      if (m_scanline == 241)
      {
        if (m_ppuCtrl.nmi)
        {
          m_cpu->setInterrupt(comp::INT_NMI);
        }

        m_ppuStatus.vblank = true;
      }
      else if (m_scanline == 261)
      {
        m_ppuStatus.s0_hit = false;
        m_ppuStatus.vblank = false;
        m_ppuStatus.sprite_overflow = false;
      }
    }

    // Increment the coordinates that we're emulating.
    if (++m_cycle > MaxCycles())
    {
      m_cycle = 0;
      if (++m_scanline > MaxScanlines())
      {
        m_scanline = 0;

        // Display the current back buffer.
        m_activeBuffer = (m_activeBuffer + 1) % m_frameBuffers.size();
      }
    }
  }

  void PPU::IncrementX()
  {
    if (++m_v.r.coarseXScroll == 0)
    {
      m_v.r.nameTableSelect ^= 0b01;
    }
  }

  void PPU::IncrementY()
  {
    if (++m_v.r.fineYScroll == 0)
    {
      if (++m_v.r.coarseYScroll == 0)
      {
        m_v.r.nameTableSelect ^= 0b10;
      }
    }
  }

  uint32_t PPU::FetchBackgroundPixel()
  {
    switch (m_tileData & 0x3)
    {
    case 0:
      return PPU_COLOR_BLACK;
    case 1:
      return PPU_COLOR_RED;
    case 2:
      return PPU_COLOR_BLUE;
    default:
      return PPU_COLOR_WHITE;
    }
  }

  void PPU::evaluateSprites()
  {
    // Clear list of sprites
    for (int i = 0; i < 8; i++)
    {
      m_oamEntries[i] = {0xFF, 0, 0, 0, 0};
    }

    m_spriteCount = 0;

    for (int i = 0; i < 0x100; i++)
    {
      m_spriteScanline[i] = 0;
    }

    m_ppuStatus.sprite_overflow = false;

    // Read through OAM finding first 8 sprites on scanline
    for (int i = 0xFC; i >= 0; i -= 4)
    {
      const int ycheck = m_scanline - m_oam[i];
      if (ycheck >= 0 && ycheck < 8 + (8 * m_ppuCtrl.sprite_height))
      {
        if (m_spriteCount < 8)
        {
          m_oamEntries[m_spriteCount] = {static_cast<unsigned int>(i) % 4, m_oam[i], m_oam[i + 1], m_oam[i + 2], m_oam[i + 3]};
          m_spriteCount++;
        }
        else
        {
          m_ppuStatus.sprite_overflow = true;
        }
      }
    }

    // Setup scanline buffer
    for (unsigned int i = 0; i < m_spriteCount; i++)
    {
      unsigned int spriteAddr = 0;

      if (m_ppuCtrl.sprite_height)
      {
        spriteAddr = (m_oamEntries[i].index & 1) * 0x1000;

        spriteAddr += (m_oamEntries[i].index * 0x10);

        spriteAddr += m_scanline - m_oamEntries[i].y;

        if (m_scanline - m_oamEntries[i].y >= 8)
        {
          spriteAddr += 8;
        }
      }
      else
      {
        spriteAddr = 0x1000 * m_ppuCtrl.sprite_select;
        spriteAddr += m_oamEntries[i].index * 0x10;
        spriteAddr += m_scanline - m_oamEntries[i].y;
      }

      unsigned int plane0 = m_memory->readPPUByte(spriteAddr);
      unsigned int plane1 = m_memory->readPPUByte(spriteAddr + 8);

      for (int bit = 7; bit >= 0; bit--)
      {
        unsigned int bit0 = (plane0 >> bit) & 1;
        unsigned int bit1 = (plane1 >> bit) & 1;

        unsigned int color = bit0 | (bit1 << 1);

        unsigned int scanlineAddr = 0;

        if (m_oamEntries[i].attributes & 0x40)
        {
          scanlineAddr = m_oamEntries[i].x + bit;
        }
        else
        {
          scanlineAddr = m_oamEntries[i].x + (-bit + 7);
        }

        if (scanlineAddr < 256)
        {
          if (color != 0)
          {
            if (m_oamEntries[i].id == 0)
            {
              m_sprite0Pixels.push_back(scanlineAddr);
            }
            m_spriteScanline[scanlineAddr] = color;
          }
        }
      }
    }
  }

  void PPU::writePPU(unsigned int data, unsigned int address)
  {
    m_ppuRegister = data;

    switch (address)
    {
    case 0x2000:
      writePPUCtrl(data);
      break;
    case 0x2001:
      writePPUMask(data);
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

  unsigned int PPU::readPPU(unsigned int address)
  {
    switch (address)
    {
    case 0x2000:
      return m_ppuRegister;
    case 0x2001:
      return m_ppuRegister;
    case 0x2002:
      return readPPUStatus();
    case 0x2003:
      return readOAMAddr();
    case 0x2004:
      return readOAMData();
    case 0x2007:
      return ReadPPUData();
    case 0x4014:
      break;
    }

    return 0;
  }

  void PPU::writePPUCtrl(unsigned int data)
  {
    m_ppuCtrl.nmi = (data & 0x80) != 0;
    m_ppuCtrl.master_slave = (data & 0x40) != 0;
    m_ppuCtrl.sprite_height = (data & 0x20) != 0;
    m_ppuCtrl.bg_tile_select = (data & 0x10) != 0;
    m_ppuCtrl.sprite_select = (data & 0x08) != 0;
    m_ppuCtrl.inc_mode = (data & 0x04) != 0;
    m_ppuCtrl.name_select = data & 0x03;
  }

  void PPU::writePPUMask(unsigned int data)
  {
    m_ppuMask.color_emph = (data >> 5) & 0xF;
    m_ppuMask.sprite_enable = (data & 0x10) != 0;
    m_ppuMask.bg_enable = (data & 0x08) != 0;
    m_ppuMask.slc_enable = (data & 0x04) != 0;
    m_ppuMask.blc_enable = (data & 0x02) != 0;
    m_ppuMask.greyscale = (data & 0x01) != 0;
  }

  unsigned int PPU::readPPUMask()
  {
    unsigned int result = 0;

    result += m_ppuMask.color_emph << 5;
    if (m_ppuMask.sprite_enable)
      result += 0x10;
    if (m_ppuMask.bg_enable)
      result += 0x08;
    if (m_ppuMask.slc_enable)
      result += 0x04;
    if (m_ppuMask.blc_enable)
      result += 0x02;
    if (m_ppuMask.greyscale)
      result += 0x01;

    return result;
  }

  unsigned int PPU::readPPUStatus()
  {
    unsigned int result = m_ppuRegister & 0x1F;

    if (m_ppuStatus.vblank)
      result += 0x80;
    if (m_ppuStatus.s0_hit)
      result += 0x40;
    if (m_ppuStatus.sprite_overflow)
      result += 0x20;

    m_ppuStatus.vblank = false;
    m_writeLatch = false;

    return result;
  }

  void PPU::writePPUAddr(uint8_t data)
  {
    if (!m_writeLatch)
    {
      m_t.addr = (m_t.addr & 0x80FF) | ((data & 0x3F) << 8);
      m_writeLatch = true;
    }
    else
    {
      m_t.addr = (m_t.addr & 0xFF00) | data;
      m_v.addr = m_t.addr;
      m_writeLatch = false;
    }
  }

  void PPU::writePPUData(unsigned int data)
  {
    writeVRAM(data, m_v.addr);

    if (!m_ppuCtrl.inc_mode)
    {
      m_v.addr += 1;
    }
    else
    {
      m_v.addr += 32;
    }
  }

  uint8_t PPU::ReadPPUData()
  {
    uint8_t value = m_memory->readPPUByte(m_v.addr);

    if (m_v.addr % 0x4000 < 0x3F00)
    {
      std::swap(m_dataBuffer, value);
    }
    else
    {
      m_dataBuffer = m_memory->readPPUByte(m_v.addr - 0x1000);
    }

    if (!m_ppuCtrl.inc_mode)
    {
      m_v.addr += 1;
    }
    else
    {
      m_v.addr += 32;
    }

    return value;
  }

  void PPU::writePPUScroll(uint8_t data)
  {
    if (!m_writeLatch)
    {
      m_t.r.coarseXScroll = data >> 3;
      m_fineXScroll = data & 3;

      m_writeLatch = true;
    }
    else
    {
      m_t.r.coarseYScroll = data >> 3;
      m_t.r.fineYScroll = data & 3;

      m_writeLatch = false;
    }
  }

  void PPU::writeVRAM(unsigned char data, int address)
  {
    m_memory->writePPUByte(data, address);
  }

  void PPU::dumpRam(std::string filename)
  {
    std::ofstream output;
    output.open(filename, std::ios::out | std::ios::binary);

    for (int i = 0; i < 0x4000; i++)
    {
      output << static_cast<uint8_t>(m_memory->readPPUByte(i));
    }

    output.close();
  }

  void PPU::writeOAMDMA(unsigned int data)
  {
    unsigned int cpuAddress = data << 8;

    for (int i = 0; i < 0x100; i++)
    {
      m_oam[(m_oamAddr + i) % 0x100] = (unsigned char)(m_memory->readByte(cpuAddress + i));
    }
  }

  void PPU::writeOAMData(unsigned int data)
  {
    m_oam[m_oamAddr] = data;
    m_oamAddr++;
    m_oamAddr &= 0xFF;
  }

  unsigned int PPU::readOAMData()
  {
    return m_oam[m_oamAddr];
  }
} // namespace nemus::core
