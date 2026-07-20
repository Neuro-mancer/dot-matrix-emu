#pragma once
#include <cstdint>
#include <array>
#include "const.h"

class Emu;

// look into memeber initializations and passing in a reference object of emu to CPU
class CPU {
    public:
        void init();
        CPU(Emu& parent);
        union Register {
            uint16_t value;
            struct {
                uint8_t lowByte;
                uint8_t highByte;
            };
        };

        bool haltExecution = false;
        void fetch();
        void decode();
        uint64_t clockCyclesElapsed;
        void printRegisters();
        enum interrupt_type {VBLANK, LCD, TIMER, SERIAL, JOYPAD, NONE};
        const uint8_t INTERRUPT_VECTORS[5] = {interrupt::vectors::VBLANK, 
            interrupt::vectors::LCD, interrupt::vectors::TIMER, 
            interrupt::vectors::SERIAL, interrupt::vectors::JOYPAD};

    private:
        Emu& parent;
        interrupt_type currentInterrupt;
        Register AF, BC, DE, HL, SP, PC;
        uint8_t instructionReg;
        bool ime; // interrupt master enable
        std::array<void (CPU::*)(), 512> opcodeDispatch;
        void checkInterrupt();
        void serviceInterrupt(uint8_t interruptFlag);
        void op_unknown();
        void op_nop();
        void op_jp_a16();
        void op_xor_a_a();
        void op_ld_hl_n16();
        void op_ld_c_n8();
        void op_ld_b_n8();
        void op_ld_hlptrd_a();
        void op_dec_b();
        void op_jr_nz_e8();
        void op_dec_c();
        void op_ld_a_n8();
        void op_ldh_a_c();
        void op_rrca();
        void op_ldh_a8_a();
        void op_ldh_a_a8();
        void op_cp_a_n8();
        void op_di();
};