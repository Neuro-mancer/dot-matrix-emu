#pragma once

class Emu;

class PPU {
    public:
        PPU(Emu& parent);
    private:
        Emu& parent;
};