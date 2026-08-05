#include "ppu.h"
#include "emu.h"
#include <iostream>

PPU::PPU(Emu& emu) : emu(emu) {
}

void PPU::requestStatInterrupt() {
    std::cout << "Requested Stat Interrupt" << std::endl;
    uint8_t ifReg = emu.busRead(bus::addr::io::IF_REG);
    emu.busWrite(bus::addr::io::IF_REG, ifReg | interrupt::flags::LCD);
}

void PPU::requestVBlankInterrupt() {
    std::cout << "Requested VBlank Interrupt" << std::endl;
    uint8_t ifReg = emu.busRead(bus::addr::io::IF_REG);
    emu.busWrite(bus::addr::io::IF_REG, ifReg | interrupt::flags::VBLANK);
}

void PPU::getLCDCFlags(uint8_t lcdcVal) {
    bgWinEnable = (lcdcVal & graphics::flags::BG_WIN_ENABLE);
    objEnable = (lcdcVal & graphics::flags::OBJ_ENABLE);
    objSizeLarge = (lcdcVal & graphics::flags::OBJ_SIZE);
    winEnable = (lcdcVal & graphics::flags::WIN_ENABLE);
    ppuEnable = (lcdcVal & graphics::flags::LCD_PPU_ENABLE);
    BGTileMapAddrStart = (lcdcVal & graphics::flags::BG_TILEMAP) ? graphics::BG_TILEMAP_ADDR_1 : graphics::BG_TILEMAP_ADDR_0;
    BGTileDataAddrStart = (lcdcVal & graphics::flags::BG_WIN_TILES) ? graphics::TILE_DATA_ADDR_1 : graphics::TILE_DATA_ADDR_0;
    winTileMapAddrStart = (lcdcVal & graphics::flags::WIN_TILEMAP) ? graphics::WIN_TILEMAP_ADDR_1 : graphics::WIN_TILEMAP_ADDR_0;
}

void PPU::getLCDCReg() {
    uint8_t lcdcVal = emu.busRead(bus::addr::io::LCDC);
    getLCDCFlags(lcdcVal);
}

void PPU::init() {
    frameBuffer = std::make_unique<uint32_t[]>(graphics::lcdHeight * graphics::lcdWidth);
    mode = HBLANK;
}

void PPU::getLCDStatusReg() {
    uint8_t lcdStatus = emu.busRead(bus::addr::io::STAT);
    getLCDStatusFlags(lcdStatus);
}

void PPU::getLCDStatusFlags(uint8_t lcdStatus) {
    lycIntSelect = (lcdStatus & graphics::flags::LYC_INT_SELECT);
    mode2IntSelect = (lcdStatus & graphics::flags::MODE_2_INT_SELECT);
    mode1IntSelect = (lcdStatus & graphics::flags::MODE_1_INT_SELECT);
    mode0IntSelect = (lcdStatus & graphics::flags::MODE_0_INT_SELECT);
    lycLyEqual = (lcdStatus & graphics::flags::LYC_LY_CMP);
}

void PPU::setPPUMode(Mode currentMode) {
    uint8_t lcdStatus = emu.busRead(bus::addr::io::STAT);
    emu.busWrite(bus::addr::io::STAT, (lcdStatus & ~(graphics::flags::PPU_MODE)) | currentMode);
    std::cout << "Mode: " << mode << std::endl;
}

void PPU::lyCompare() {
    uint8_t ly = emu.busRead(bus::addr::io::LY);
    uint8_t lyc = emu.busRead(bus::addr::io::LYC);
    if(ly == lyc) {
        emu.lcdRegs[reg::STAT] |= graphics::flags::LYC_LY_CMP;
        lycLyEqual = true;

        // request interrupt
        if(lycIntSelect) {
            requestStatInterrupt();
        }
    }
}

void PPU::render() {
    bool renderLine = true;
    while(renderLine){

        switch(mode) {
            case OAM_SCAN:
                oamScan();
                break;
            case DRAW:
                draw();
                break;
            case HBLANK:
                hblank();
                renderLine = false;
                break;
            case VBLANK:
                vblank();
                renderLine = false;
                break;
        }
    }

    lyCompare();
}

void PPU::oamScan() {
    mode = DRAW;
    setPPUMode(mode);
}

void PPU::draw() {
    unsigned int yBackground = emu.lcdRegs[reg::LY];
    uint16_t x;
    uint16_t y;
    uint16_t mapAddr;
    uint16_t tileAddr;
    uint8_t byte0;
    uint8_t byte1;
    uint8_t shift;
    uint8_t bit0;
    uint8_t bit1;

    for(unsigned int xBackground = 0; xBackground < graphics::lcdWidth; xBackground++) {
        // read tilemap ptr
        if(bgWinEnable) {
            x = ((emu.lcdRegs[reg::SCX] + xBackground) % 256) / 8;
            y = (((emu.lcdRegs[reg::SCY] + yBackground) % 256) / 8) * 32;
            mapAddr = x + y + BGTileMapAddrStart;
            tileAddr = BGTileDataAddrStart + (emu.busRead(mapAddr) * 16) + ((emu.lcdRegs[reg::SCY] + yBackground) * 2);
            // read tile data
            byte0 = emu.busRead(tileAddr);
            byte1 = emu.busRead(tileAddr + 1);
            shift = 7 - ((xBackground + emu.lcdRegs[reg::SCX]) % 8);
            // determine color
            bit0 = (byte0 >> shift) & 1;
            bit1 = (byte1 >> (shift - 1)) & 2;
            // set frame buffer pixel color
        }
        else {
            bit0 = 0;
            bit1 = 0;
        }
        frameBuffer[(yBackground * graphics::lcdWidth) + xBackground] = graphics::COLORS[bit0 | bit1];
    }

    mode = HBLANK;
    setPPUMode(mode);
    if(mode0IntSelect) {
        requestStatInterrupt();
    }
}

void PPU::hblank() {
    emu.lcdRegs[reg::LY]++;
    if(emu.lcdRegs[reg::LY] >= 144) {
        mode = VBLANK;
        setPPUMode(mode);
        if(mode1IntSelect) {
            requestVBlankInterrupt();
        }
    }
    else {
        mode = OAM_SCAN;
        setPPUMode(mode);
    }
}

void PPU::vblank() {
    emu.lcdRegs[reg::LY]++;
    if(emu.lcdRegs[reg::LY] > 153) {
        emu.lcdRegs[reg::LY] = 0;
        mode = OAM_SCAN;
        setPPUMode(mode);
        if(mode2IntSelect) {
            requestStatInterrupt();
        }
    }

}

void PPU::runPPU() {
    getLCDCReg();
    getLCDStatusReg();
    if(ppuEnable) {
        counter += emu.cpu.totalCyclesSinceLastExecution;
        // update PPU counter and render if applicable
        if(counter >= 456) {
            counter -= 456;
            render();
        }
    }
    else {
        // while PPU is off indicate mode 0 to the lcd status register.
        setPPUMode(HBLANK);
    }
}