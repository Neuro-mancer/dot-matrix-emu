#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include "cpu.h"

class Emu {
	public:
		Emu();
		bool readRomFile(char *romFile);
        void getHeader();
        void printHeader();
        void emuLoop();
		friend class CPU;
		CPU cpu;
	private:
		std::unique_ptr<uint8_t[]> romData;
		std::unique_ptr<uint8_t[]> wram; // work ram
		std::unique_ptr<uint8_t[]> oam; // object attribute memory
		std::unique_ptr<uint8_t[]> hram;
		std::unique_ptr<uint8_t[]> waveRam;
		uint8_t ieReg; // interrupt enable
		uint8_t ifReg; // interrupt flag
		uint8_t joypadReg; // input
		uint8_t serialTransRegs[2];
		uint8_t timerDivRegs[4];
		uint8_t audioRegs[21];
		uint8_t lcdRegs[12];
        uint8_t logo[0x30];
        std::string title;
        char newLicenseeCode[2]; // in ASCII for some weird reason
        uint8_t licenseeCode;
        uint8_t sgbFlag;
        uint8_t cartType;
        uint8_t romSize;
        uint8_t ramSize;
        uint8_t destinationCode;
        uint8_t headerChecksum;
        uint8_t globalChecksum;

		void init();
        void readTitle();
        void readLogo();
        void readLicenseeCode();
        void readSgbFlag();
        void readNewLicenseeCode();
        void readCartType();
        void readRomSize();
        void readRamSize();
        void readDestinationCode();
        void readHeaderChecksum();
		void busWrite(uint16_t addr, uint8_t data);
		uint8_t busRead(uint16_t addr);
};