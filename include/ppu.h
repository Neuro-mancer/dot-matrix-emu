#pragma once
#include <cstdint>
#include "const.h"
#include <memory>

class Emu;

class PPU {
    public:
        PPU(Emu& emu);
        enum Mode {HBLANK, VBLANK, OAM_SCAN, DRAW};
        std::unique_ptr<uint32_t[]> frameBuffer;
        void init();
        void runPPU();
    private:
        Mode mode;
        uint16_t BGTileDataAddrStart;
        uint16_t BGTileMapAddrStart;
        uint16_t winTileMapAddrStart;
        bool ppuEnable;
        bool winEnable;
        bool objEnable;
        bool objSizeLarge; // false = 8x8, true = 8x16
        bool bgWinEnable;
        bool lycIntSelect;
        bool mode2IntSelect;
        bool mode1IntSelect;
        bool mode0IntSelect;
        bool lycLyEqual;
        uint64_t counter;
        Emu& emu;
        void getLCDCFlags(uint8_t lcdcVal);
        void getLCDCReg();
        void getLCDStatusFlags(uint8_t lcdStatus);
        void getLCDStatusReg();
        void setPPUMode(Mode currentMode); // sets this in the lcdstatus register
        void lyCompare();
        void render();
        void oamScan();
        void draw();
        void hblank();
        void vblank();
        void requestStatInterrupt();
        void requestVBlankInterrupt();
};