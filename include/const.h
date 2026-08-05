#pragma once
#include <cstdint>

namespace timing {
    inline constexpr uint64_t FRAME_RATE = 60;
    inline constexpr uint64_t CYCLES_PER_FRAME = 70224;
}

namespace interrupt::vectors {
    inline constexpr uint16_t VBLANK = 0x0040;
    inline constexpr uint16_t LCD = 0x0048;
    inline constexpr uint16_t TIMER = 0x0050;
    inline constexpr uint16_t SERIAL = 0x0058;
    inline constexpr uint16_t JOYPAD = 0x0060;
}

namespace interrupt::flags {
    inline constexpr uint8_t VBLANK = 0x01;
    inline constexpr uint8_t LCD = 0x02;
    inline constexpr uint8_t TIMER = 0x04;
    inline constexpr uint8_t SERIAL = 0x08;
    inline constexpr uint8_t JOYPAD = 0x10;
}

namespace accumulator::flags {
    inline constexpr uint8_t Z = 0x80;
    inline constexpr uint8_t N = 0x40;
    inline constexpr uint8_t H = 0x20;
    inline constexpr uint8_t C = 0x10;
}

namespace timer::flags {
    inline constexpr uint8_t TAC_E = 0x04;
    inline constexpr uint8_t TAC_CS_0 = 0x00;
    inline constexpr uint8_t TAC_CS_1 = 0x01;
    inline constexpr uint8_t TAC_CS_2 = 0x02;
    inline constexpr uint8_t TAC_CS_3 = 0x03;
}

// Cartridge ROM Header addresses
namespace rom::header {

    // ROM Title bounds
    inline constexpr uint16_t TITLE_ADDR_START       = 0x134;
    inline constexpr uint16_t TITLE_ADDR_END         = 0x143;

    // Nintendo Logo bounds
    inline constexpr uint16_t LOGO_ADDR_START        = 0x104;
    inline constexpr uint16_t LOGO_ADDR_END          = 0x133;

    // Developer / Licensee details
    inline constexpr uint16_t LICENSEE_ADDR          = 0x14B; // Old licensee code
    inline constexpr uint16_t NEW_LICENSEE_ADDR      = 0x144; // New licensee code (2 bytes)

    // Hardware and Cartridge capabilities
    inline constexpr uint16_t SGB_FLAG_ADDR          = 0x146; // Super Game Boy support
    inline constexpr uint16_t CART_TYPE_ADDR         = 0x147; // MBC type (e.g., MBC1, MBC5)
    inline constexpr uint16_t ROM_SIZE_ADDR          = 0x148; // Size of the ROM chip
    inline constexpr uint16_t RAM_SIZE_ADDR          = 0x149; // On-board RAM size (if any)
    inline constexpr uint16_t DESTINATION_CODE_ADDR  = 0x14A; // Japanese vs. International

    // Validation
    inline constexpr uint16_t HEADER_CHECKSUM_ADDR   = 0x14D; // 8-bit checksum
}

namespace graphics {
    inline constexpr unsigned int lcdWidth = 160;
    inline constexpr unsigned int lcdHeight = 144;
    inline constexpr uint32_t COLORS[4] = { 
        0XFFFFFFFF, 
        0XAAAAAAFF, 
        0X555555FF, 
        0X000000FF,
    };
    inline constexpr uint16_t BG_TILEMAP_ADDR_0 = 0x9800;
    inline constexpr uint16_t BG_TILEMAP_ADDR_1 = 0x9C00;
    inline constexpr uint16_t WIN_TILEMAP_ADDR_0 = BG_TILEMAP_ADDR_0;
    inline constexpr uint16_t WIN_TILEMAP_ADDR_1 = BG_TILEMAP_ADDR_1;
    inline constexpr uint16_t TILE_DATA_ADDR_0 = 0x8000;
    inline constexpr uint16_t TILE_DATA_ADDR_1 = 0x9000;
}

namespace graphics::flags {
    // lcdc
    inline constexpr uint8_t BG_WIN_ENABLE = 0x01;
    inline constexpr uint8_t OBJ_ENABLE = 0x02;
    inline constexpr uint8_t OBJ_SIZE = 0x04;
    inline constexpr uint8_t BG_TILEMAP = 0x08;
    inline constexpr uint8_t BG_WIN_TILES = 0x10;
    inline constexpr uint8_t WIN_ENABLE = 0x20;
    inline constexpr uint8_t WIN_TILEMAP = 0x40;
    inline constexpr uint8_t LCD_PPU_ENABLE = 0x80;

    // lcd status
    inline constexpr uint8_t LYC_INT_SELECT = 0x40;
    inline constexpr uint8_t MODE_2_INT_SELECT = 0x20;
    inline constexpr uint8_t MODE_1_INT_SELECT = 0x10;
    inline constexpr uint8_t MODE_0_INT_SELECT = 0x08;
    inline constexpr uint8_t LYC_LY_CMP = 0x04;
    inline constexpr uint8_t PPU_MODE = 0x03;
}

namespace bus::addr {
    inline constexpr uint16_t ROM_0_START = 0x0000;
    inline constexpr uint16_t ROM_0_END = 0x3FFF;
    inline constexpr uint16_t ROM_1_NN_START = 0x4000;
    inline constexpr uint16_t ROM_1_NN_END = 0x7FFF;
    inline constexpr uint16_t VRAM_START = 0x8000;
    inline constexpr uint16_t VRAM_END = 0x9FFF;
    inline constexpr uint16_t ERAM_START = 0XA000;
    inline constexpr uint16_t ERAM_END = 0xBFFF;
    inline constexpr uint16_t WRAM_START = 0xC000;
    inline constexpr uint16_t WRAM_END = 0xDFFF;
    inline constexpr uint16_t ECHO_RAM_START = 0xE000;
    inline constexpr uint16_t ECHO_RAM_END = 0xFDFF;
    inline constexpr uint16_t OAM_START = 0xFE00;
    inline constexpr uint16_t OAM_END = 0xFE9F;
    inline constexpr uint16_t NO_USE_START = 0xFEA0;
    inline constexpr uint16_t NO_USE_END = 0xFEFF;
    inline constexpr uint16_t IO_REG_START = 0xFF00;
    inline constexpr uint16_t IO_REG_END = 0xFF75;
    inline constexpr uint16_t HRAM_START = 0xFF80;
    inline constexpr uint16_t HRAM_END = 0xFFFE;
    inline constexpr uint16_t IE_REG = 0xFFFF;
}

namespace bus::addr::io {
    inline constexpr uint16_t JOYPAD_INPUT_REG = 0xFF00;
    inline constexpr uint16_t SERIAL_TRANS_REG_0 = 0xFF01;
    inline constexpr uint16_t SERIAL_TRANS_REG_1 = 0xFF02;
    inline constexpr uint16_t DIV = 0xFF04;
    inline constexpr uint16_t TIMA = 0xFF05;
    inline constexpr uint16_t TMA = 0xFF06;
    inline constexpr uint16_t TAC = 0xFF07;
    inline constexpr uint16_t IF_REG = 0xFF0F;
    inline constexpr uint16_t NR10 = 0xFF10;
    inline constexpr uint16_t NR11 = 0xFF11;
    inline constexpr uint16_t NR12 = 0xFF12;
    inline constexpr uint16_t NR13 = 0xFF13;
    inline constexpr uint16_t NR14 = 0xFF14;
    inline constexpr uint16_t NR21 = 0xFF16;
    inline constexpr uint16_t NR22 = 0xFF17;
    inline constexpr uint16_t NR23 = 0xFF18;
    inline constexpr uint16_t NR24 = 0xFF19;
    inline constexpr uint16_t NR30 = 0xFF1A;
    inline constexpr uint16_t NR31 = 0xFF1B;
    inline constexpr uint16_t NR32 = 0xFF1C;
    inline constexpr uint16_t NR33 = 0xFF1D;
    inline constexpr uint16_t NR34 = 0xFF1E;
    inline constexpr uint16_t NR41 = 0xFF20;
    inline constexpr uint16_t NR42 = 0xFF21;
    inline constexpr uint16_t NR43 = 0xFF22;
    inline constexpr uint16_t NR44 = 0xFF23;
    inline constexpr uint16_t NR50 = 0xFF24;
    inline constexpr uint16_t NR51 = 0xFF25;
    inline constexpr uint16_t NR52 = 0xFF26;
    inline constexpr uint16_t WAVE_RAM_START = 0xFF30;
    inline constexpr uint16_t WAVE_RAM_END = 0xFF3F;
    inline constexpr uint16_t LCDC = 0xFF40;
    inline constexpr uint16_t STAT = 0xFF41;
    inline constexpr uint16_t SCY = 0xFF42;
    inline constexpr uint16_t SCX = 0xFF43;
    inline constexpr uint16_t LY = 0xFF44;
    inline constexpr uint16_t LYC = 0xFF45;
    inline constexpr uint16_t DMA = 0xFF46;
    inline constexpr uint16_t BGP = 0xFF47;
    inline constexpr uint16_t OBP0 = 0xFF48;
    inline constexpr uint16_t OBP1 = 0xFF49;
    inline constexpr uint16_t WY = 0xFF4A;
    inline constexpr uint16_t WX = 0xFF4B;
}

namespace reg {
    inline constexpr uint16_t SERIAL_TRANS_REG_0 = 0;
    inline constexpr uint16_t SERIAL_TRANS_REG_1 = 1;
    inline constexpr uint16_t DIV = 0;
    inline constexpr uint16_t TIMA = 1;
    inline constexpr uint16_t TMA = 2;
    inline constexpr uint16_t TAC = 3;
    inline constexpr uint16_t NR10 = 0;
    inline constexpr uint16_t NR11 = 1;
    inline constexpr uint16_t NR12 = 2;
    inline constexpr uint16_t NR13 = 3;
    inline constexpr uint16_t NR14 = 4;
    inline constexpr uint16_t NR21 = 5;
    inline constexpr uint16_t NR22 = 6;
    inline constexpr uint16_t NR23 = 7;
    inline constexpr uint16_t NR24 = 8;
    inline constexpr uint16_t NR30 = 9;
    inline constexpr uint16_t NR31 = 10;
    inline constexpr uint16_t NR32 = 11;
    inline constexpr uint16_t NR33 = 12;
    inline constexpr uint16_t NR34 = 13;
    inline constexpr uint16_t NR41 = 14;
    inline constexpr uint16_t NR42 = 15;
    inline constexpr uint16_t NR43 = 16;
    inline constexpr uint16_t NR44 = 17;
    inline constexpr uint16_t NR50 = 18;
    inline constexpr uint16_t NR51 = 19;
    inline constexpr uint16_t NR52 = 20;
    inline constexpr uint16_t LCDC = 0;
    inline constexpr uint16_t STAT = 1;
    inline constexpr uint16_t SCY = 2;
    inline constexpr uint16_t SCX = 3;
    inline constexpr uint16_t LY = 4;
    inline constexpr uint16_t LYC = 5;
    inline constexpr uint16_t DMA = 6;
    inline constexpr uint16_t BGP = 7;
    inline constexpr uint16_t OBP0 = 8;
    inline constexpr uint16_t OBP1 = 9;
    inline constexpr uint16_t WY = 10;
    inline constexpr uint16_t WX = 11;
}