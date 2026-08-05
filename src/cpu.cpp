#include "cpu.h"
#include "emu.h"
#include "const.h"
#include <iostream>

// NOTE: Change all flag checks into simpler statements. No need to bit shift, can generally just test expression for nonzero using mask

CPU::CPU(Emu& emu) : emu(emu) {
    opcodeDispatch.fill(&CPU::op_unknown);
    opcodeDispatch[0x00] = &CPU::op_nop;
    opcodeDispatch[0xC3] = &CPU::op_jp_a16;
    opcodeDispatch[0xAF] = &CPU::op_xor_a_a;
    opcodeDispatch[0x21] = &CPU::op_ld_hl_n16;
    opcodeDispatch[0x0E] = &CPU::op_ld_c_n8;
    opcodeDispatch[0x06] = &CPU::op_ld_b_n8;
    opcodeDispatch[0x32] = &CPU::op_ld_hlptrd_a;
    opcodeDispatch[0x05] = &CPU::op_dec_b;
    opcodeDispatch[0x20] = &CPU::op_jr_nz_e8;
    opcodeDispatch[0x0D] = &CPU::op_dec_c;
    opcodeDispatch[0x3E] = &CPU::op_ld_a_n8;
    opcodeDispatch[0xF2] = &CPU::op_ldh_a_c;
    opcodeDispatch[0x0F] = &CPU::op_rrca;
    opcodeDispatch[0xE0] = &CPU::op_ldh_a8_a;
    opcodeDispatch[0xF0] = &CPU::op_ldh_a_a8;
    opcodeDispatch[0xFE] = &CPU::op_cp_a_n8;
    opcodeDispatch[0xF3] = &CPU::op_di;
}

// prep CPU to start reading program
void CPU::init() {
    std::cout << "Prepping CPU for execution..." << std::endl;
    // half and carry flag only set if 0x00
    AF.value = (emu.headerChecksum == 0x00) ? 0x0180 : 0x01B0;
    BC.value = 0x0013;
    DE.value = 0x00D8;
    HL.value = 0x014D;
    PC.value = 0x0100;
    SP.value = 0xFFFE;
    ime = false;
    currentInterrupt = NONE;
    lastClockCyclesElapsed = 0;
    clockCyclesElapsed = 0;
    internalCounter = 0;
}

void CPU::fetch() {
    checkInterrupt();
    std::cout << "Fetching Instruction at address 0x" << std::hex << PC.value << "..." << std::endl;
    instructionReg = emu.busRead(PC.value);
    PC.value++;
    clockCyclesElapsed++;
}

void CPU::decode() {
    std::cout << "Decoding Opcode 0x" << std::hex << (unsigned int)instructionReg << "..." << std::endl;
    (this->*opcodeDispatch[instructionReg])();
    updateTimers();
}

void CPU::checkInterrupt() {
    if(ime) {
        std::cout << "Checking for interrupts..." << std::endl;
        uint8_t interruptEnable = emu.busRead(bus::addr::IE_REG);
        uint8_t interruptFlag = emu.busRead(bus::addr::io::IF_REG);

        for(int i = 0; i < 5; i++) {
            bool enable = ((interruptEnable & (0x01 << i)) >> i) == 0x01;
            bool flag = ((interruptFlag & (0x01 << i)) >> i) == 0x01;

            if(enable && flag) {
                std::cout << "Servicing interrupt!" << std::endl;
                currentInterrupt = static_cast<interrupt_type>(i);
                serviceInterrupt(interruptFlag);
                break;
            }

            currentInterrupt = NONE;
        }
    }
}

void CPU::serviceInterrupt(uint8_t interruptFlag) {
    ime = false;
    emu.busWrite(SP.value, PC.highByte);
    SP.value--;
    emu.busWrite(SP.value, PC.lowByte);
    SP.value--;

    emu.busWrite(bus::addr::io::IF_REG, interruptFlag & ~(1 << currentInterrupt));
    PC.value = INTERRUPT_VECTORS[currentInterrupt];
    clockCyclesElapsed += 20;
}

void CPU::updateTimers() { // eventually change this to busRead() calls for uniformity and control
    int clockDivider;
    uint16_t lastCounter = internalCounter;
    totalCyclesSinceLastExecution = clockCyclesElapsed - lastClockCyclesElapsed;
    internalCounter += totalCyclesSinceLastExecution;
    lastClockCyclesElapsed = clockCyclesElapsed;

    emu.timerDivRegs[reg::DIV] = internalCounter >> 8;

    if(emu.timerDivRegs[reg::TAC] & timer::flags::TAC_E) {
        int8_t clockSelect = emu.timerDivRegs[reg::TAC] &= ~(timer::flags::TAC_E);
        switch(clockSelect) {
            case timer::flags::TAC_CS_0:
                clockDivider = 1024;
                break;
            case timer::flags::TAC_CS_1:
                clockDivider = 16;
                break;
            case timer::flags::TAC_CS_2:
                clockDivider = 64;
                break;
            case timer::flags::TAC_CS_3:
                clockDivider = 256;
                break;
        }
        int ticks = (internalCounter / clockDivider) - (lastCounter / clockDivider);

        while (ticks > 0) {
            emu.timerDivRegs[reg::TIMA]++;

            if(emu.timerDivRegs[reg::TIMA] == 0) {
                emu.timerDivRegs[reg::TIMA] = emu.timerDivRegs[reg::TMA];
                emu.busWrite(bus::addr::io::IF_REG, emu.ifReg | interrupt::flags::TIMER);
            }

            ticks--;
        }


    }
}

void CPU::op_unknown() {
    std::cout << "Unrecognized opcode!" << std::endl;
    haltExecution = true;
}

void CPU::op_nop() {
    clockCyclesElapsed += 4;
}

void CPU::op_jp_a16() {
    uint8_t lowByte = emu.busRead(PC.value);
    PC.value++;
    uint8_t highByte = emu.busRead(PC.value);
    PC.value++;
    PC.value = (highByte << 8) + lowByte;
    clockCyclesElapsed += 16;
}

void CPU::op_xor_a_a() {
    AF.highByte ^= AF.highByte;
    AF.lowByte = accumulator::flags::Z;
    clockCyclesElapsed += 4;
}

void CPU::op_ld_hl_n16() {
    uint8_t lowByte = emu.busRead(PC.value);
    PC.value++;
    uint8_t highByte = emu.busRead(PC.value);
    PC.value++;
    HL.value = (highByte << 8) + lowByte;
    clockCyclesElapsed += 12;
}

void CPU::op_ld_c_n8() {
    BC.lowByte = emu.busRead(PC.value);
    PC.value++;
    clockCyclesElapsed += 8;
}

void CPU::op_ld_b_n8() {
    BC.highByte = emu.busRead(PC.value);
    PC.value++;
    clockCyclesElapsed += 8;
}

void CPU::op_ld_hlptrd_a() {
    emu.busWrite(HL.value, AF.highByte);
    HL.value--;
    clockCyclesElapsed += 8;
}

void CPU::op_dec_b() {
    uint8_t b = BC.highByte;
    BC.highByte--;
    AF.lowByte |= accumulator::flags::N;
    if(BC.highByte == 0) { 
        AF.lowByte |= accumulator::flags::Z;
    }
    else {
        AF.lowByte &= ~(accumulator::flags::Z);
    }
    if((int)(b & 0xf) - (int)(1 & 0xf) < 0) {
        AF.lowByte |= accumulator::flags::H;
    }
    else {
        AF.lowByte &= ~(accumulator::flags::H);
    }
    clockCyclesElapsed += 4;
}

void CPU::op_jr_nz_e8() {
    if(((AF.lowByte & accumulator::flags::Z) >> 7) == 0x00u) {
        int8_t offset = emu.busRead(PC.value);
        PC.value++;
        uint16_t temp = PC.value + offset;
        PC.value = temp;
        clockCyclesElapsed += 12;
    }
    else {
        clockCyclesElapsed += 8;
        PC.value++;
    }
}

void CPU::op_dec_c() {
    uint8_t c = BC.lowByte;
    BC.lowByte--;
    AF.lowByte |= accumulator::flags::N;
    if(BC.lowByte == 0) {
        AF.lowByte |= accumulator::flags::Z;
    }
    else {
        AF.lowByte &= ~(accumulator::flags::Z);
    }
    if((int)(c & 0xf) - (int)(1 & 0xf) < 0) {
        AF.lowByte |= accumulator::flags::H;
    }
    else {
        AF.lowByte &= ~(accumulator::flags::H);
    }
    clockCyclesElapsed += 4;
}

void CPU::op_ld_a_n8() {
    AF.highByte = emu.busRead(PC.value);
    PC.value++;
    clockCyclesElapsed += 8;
}

void CPU::op_ldh_a_c() {
    AF.highByte = emu.busRead(0xFF00 + BC.lowByte);
    PC.value++;
    clockCyclesElapsed += 8;
}

void CPU::op_rrca() {
    uint8_t carry = AF.highByte & 0x01;
    AF.highByte >>= 1;

    if(carry == 0x01) {
        AF.highByte += carry << 7;
        AF.lowByte |= accumulator::flags::C;
    }
    else {
        AF.lowByte &= ~(accumulator::flags::C);
    }

    clockCyclesElapsed += 4;
}

void CPU::op_ldh_a8_a() {
    uint16_t addr = emu.busRead(PC.value) + 0xFF00;
    PC.value++;
    emu.busWrite(addr, AF.highByte); 
    clockCyclesElapsed += 12;
}

void CPU::op_ldh_a_a8() {
    uint16_t addr = emu.busRead(PC.value) + 0xFF00;
    PC.value++;
    AF.highByte = emu.busRead(addr);
    clockCyclesElapsed += 12;
}

void CPU::op_cp_a_n8() {
    uint8_t n8 = emu.busRead(PC.value);
    PC.value++;
    uint8_t cmp = AF.highByte - n8;
    AF.lowByte |= accumulator::flags::N;
    if(cmp == 0) {
        AF.lowByte |= accumulator::flags::Z;
    }
    else {
        AF.lowByte &= ~(accumulator::flags::Z);
    }
    if((int)(AF.highByte & 0xf) - (int)(n8 & 0xf) < 0) {
        AF.lowByte |= accumulator::flags::H;
    }
    else {
        AF.lowByte &= ~(accumulator::flags::H);
    }
    if(n8 > AF.lowByte) {
        AF.lowByte |= accumulator::flags::C;
    }
    else {
        AF.lowByte &= ~(accumulator::flags::C);
    }

    clockCyclesElapsed += 8;
}

void CPU::op_di() {
    ime = false;
}

void CPU::printRegisters() {
    std::cout << "= CPU REGISTERS =" << std::endl;
    std::cout << "AF: 0x" << std::hex << AF.value << std::endl;
    std::cout << "BC: 0x" << std::hex << BC.value << std::endl;
    std::cout << "DE: 0x" << std::hex << DE.value << std::endl;
    std::cout << "HL: 0x" << std::hex << HL.value << std::endl;
    std::cout << "PC: 0x" << std::hex << PC.value << std::endl;
    std::cout << "SP: 0x" << std::hex << SP.value << std::endl;
    std::cout << "IR: 0x" << std::hex << (unsigned int)instructionReg << std::endl;
}