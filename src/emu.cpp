#include "emu.h"
#include "const.h"
#include <iostream>
#include <string>
#include <cstdint>
#include <fstream>
#include <exception> // Required for std::exception
Emu::Emu() : cpu(*this) {
	std::cout << "Creating emulator object..." << std::endl;
}

void Emu::emuLoop() {
	init();
	while(!cpu.haltExecution) {
		cpu.fetch();
		cpu.decode();
		cpu.printRegisters();
	}
}

bool Emu::readRomFile(char *romFile) {
	std::cout << "Attempting ROM file read..." << std::endl;
	std::ifstream file;

	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		file.open(romFile, std::ios::in | std::ios::binary);

		file.seekg(0, std::ios::end);
		std::streampos fileEnd = file.tellg();
		
		size_t fileSize = static_cast<size_t>(fileEnd); 

		romData = std::make_unique<uint8_t[]>(fileSize);
		
		file.seekg(0, std::ios::beg);

		file.read(reinterpret_cast<char*>(romData.get()), fileSize);

		// getHeader(); // to add later
		std::cout << "ROM File Read Successful!" << std::endl;

		return true; // Execution completed perfectly without throwing
	}
	catch (const std::ios_base::failure& e) {
		// Catches file I/O runtime errors (e.g., File not found, permission denied)
		std::cerr << "ROM IO Error: " << e.what() << std::endl;
		return false;
	}
	catch (const std::bad_alloc& e) {
		// Catches out-of-romDataory errors if the ROM file is corrupt/gigantic
		std::cerr << "romDataory Allocation Error: " << e.what() << std::endl;
		return false;
	}
	catch (const std::exception& e) {
		// Catch-all safety net for any other unexpected exceptions
		std::cerr << "Unexpected Error: " << e.what() << std::endl;
		return false;
	}
}

void Emu::getHeader() {
	readTitle();
	readLogo();
	readLicenseeCode();
	readNewLicenseeCode();
	readSgbFlag();
	readCartType();
	readRomSize();
	readRamSize();
	readDestinationCode();
	readHeaderChecksum();
	printHeader();
}

void Emu::printHeader() {
	std::cout << "= ROM HEADER INFO =" << std::endl;
	std::cout << "Title: " << title << std::endl;
	std::cout << "Licensee Code: 0x" << std::hex << (unsigned int)licenseeCode << std::endl;
	std::cout << "New Licensee Code: " << newLicenseeCode[0] << newLicenseeCode[1] << std::endl;
	std::cout << "SGB Flag: 0x" << std::hex << (unsigned int)sgbFlag << std::endl;
	std::cout << "Cartridge Type Code: 0x" << std::hex << (unsigned int)cartType << std::endl;
	std::cout << "Rom Size Code: 0x" << (unsigned int)romSize<< std::endl;
	std::cout << "Ram Size Code: 0x" << std::hex << (unsigned int)ramSize << std::endl;
	std::cout << "Destination Code: 0x" << std::hex << (unsigned int)destinationCode << std::endl;
	std::cout << "Header Checksum Code: 0x" << std::hex << (unsigned int)headerChecksum << std::endl;
}

void Emu::readTitle() {
	for(int i = rom::header::TITLE_ADDR_START; i <= rom::header::TITLE_ADDR_END; i++) {
		title += (char)romData[i];
	}
}

void Emu::readLogo() {
	for(int i = rom::header::LOGO_ADDR_START, logoIndex = 0; i <= rom::header::LOGO_ADDR_END; i++, logoIndex++) {
		logo[logoIndex] = romData[i];
	}
}

void Emu::readLicenseeCode() {
	licenseeCode = romData[rom::header::LICENSEE_ADDR];
}

void Emu::readNewLicenseeCode() {
	// note to self, this is in ASCII for some unknown reason
	newLicenseeCode[0] = (char)romData[rom::header::NEW_LICENSEE_ADDR];
	newLicenseeCode[1] = (char)romData[rom::header::NEW_LICENSEE_ADDR + 1];
}

void Emu::readSgbFlag() {
	sgbFlag = romData[rom::header::SGB_FLAG_ADDR];
}

void Emu::readCartType() {
	cartType = romData[rom::header::CART_TYPE_ADDR];
}

void Emu::readRomSize() {
	romSize = romData[rom::header::ROM_SIZE_ADDR];
}

void Emu::readRamSize() {
	romSize = romData[rom::header::RAM_SIZE_ADDR];
}

void Emu::readDestinationCode() {
	destinationCode = romData[rom::header::DESTINATION_CODE_ADDR];
}

void Emu::readHeaderChecksum() {
	uint8_t checksum = 0;
	for(uint16_t addr = 0x0134; addr <= 0x014C; addr++) {
		checksum = checksum - romData[addr] - 1;
	}

	std::cout << "Header checksum 0x" << std::hex << (unsigned int)checksum;
	std::cout << (checksum == romData[rom::header::HEADER_CHECKSUM_ADDR] ? " passes check" : " fails check");
	headerChecksum = checksum;
	std::cout << std::endl;
}

void Emu::init() {
	std::cout << "Initializing virtual console..." << std::endl;
	cpu.init();
	wram = std::make_unique<uint8_t[]>((bus::addr::WRAM_END - bus::addr::WRAM_START) + 1);
	oam = std::make_unique<uint8_t[]>((bus::addr::OAM_END - bus::addr::OAM_START) + 1);
	hram = std::make_unique<uint8_t[]>((bus::addr::HRAM_END - bus::addr::HRAM_START) + 1);
	waveRam = std::make_unique<uint8_t[]>((bus::addr::io::WAVE_RAM_END - bus::addr::io::WAVE_RAM_START) + 1);
	ieReg = 0x00;
	joypadReg = 0xCF;
	serialTransRegs[reg::SERIAL_TRANS_REG_0] = 0x00;
	serialTransRegs[reg::SERIAL_TRANS_REG_1] = 0x7E;
	timerDivRegs[reg::DIV] = 0xAB;
	timerDivRegs[reg::TIMA] = 0x00;
	timerDivRegs[reg::TMA] = 0x00;
	timerDivRegs[reg::TAC] = 0xF8;
	ifReg = 0xE1;
	audioRegs[reg::NR10] = 0x80;
	audioRegs[reg::NR11] = 0xBF;
	audioRegs[reg::NR12] = 0xF3;
	audioRegs[reg::NR13] = 0xFF;
	audioRegs[reg::NR14] = 0xBF;
	audioRegs[reg::NR21] = 0x3F;
	audioRegs[reg::NR22] = 0x00;
	audioRegs[reg::NR23] = 0xFF;
	audioRegs[reg::NR24] = 0xBF;
	audioRegs[reg::NR30] = 0x7F;
	audioRegs[reg::NR31] = 0xFF;
	audioRegs[reg::NR32] = 0x9F;
	audioRegs[reg::NR33] = 0xFF;
	audioRegs[reg::NR34] = 0xBF;
	audioRegs[reg::NR41] = 0xFF;
	audioRegs[reg::NR42] = 0x00;
	audioRegs[reg::NR43] = 0x00;
	audioRegs[reg::NR44] = 0xBF;
	audioRegs[reg::NR50] = 0x77;
	audioRegs[reg::NR51] = 0xF3;
	audioRegs[reg::NR52] = 0xF1;
	lcdRegs[reg::LCDC] = 0x91;
	lcdRegs[reg::STAT] = 0x85;
	lcdRegs[reg::SCY] = 0x00;
	lcdRegs[reg::SCX] = 0x00;
	lcdRegs[reg::LY] = 0x00;
	lcdRegs[reg::LYC] = 0x00;
	lcdRegs[reg::DMA] = 0xFF;
	lcdRegs[reg::BGP] = 0xFC;
	lcdRegs[reg::OBP0] = 0x00;
	lcdRegs[reg::OBP1] = 0x00;
	lcdRegs[reg::WY] = 0x00;
	lcdRegs[reg::WX] = 0x00;
	ieReg = 0x00;
}

uint8_t Emu::busRead(uint16_t addr) {
	if(addr >= bus::addr::ROM_0_START && addr <= bus::addr::ROM_0_END) {
		return romData[addr - bus::addr::ROM_0_START];
	}
	if(addr >= bus::addr::ROM_1_NN_START && addr <= bus::addr::ROM_1_NN_END) {
		// NOTE: add rom bank switching here
		return romData[addr - bus::addr::ROM_1_NN_START];
	}
	if(addr >= bus::addr::VRAM_START && addr <= bus::addr::VRAM_END) {
		// change once graphics implemented
		std::cerr << "Error: VRAM not implemented; returning 0x69" << std::endl;
		return 0x69;
	}
	if(addr >= bus::addr::ERAM_START && addr <= bus::addr::ERAM_END) {
		// external RAM bank (not implemented yet)
		std::cerr << "Error: External RAM not implemented; returning 0x69" << std::endl;
		return 0x69;

	}
	// NOTE: upper half of WRAM becomes switchable bank in CGB mode
	if((addr >= bus::addr::WRAM_START && addr <= bus::addr::WRAM_END) || (addr >= bus::addr::ECHO_RAM_START && addr <= bus::addr::ECHO_RAM_END)) {
		return wram[addr - bus::addr::WRAM_START];
	}
	if(addr >= bus::addr::OAM_START && addr <= bus::addr::OAM_END) {
		return oam[addr - bus::addr::OAM_START];
	}
	if(addr >= bus::addr::NO_USE_START && addr <= bus::addr::NO_USE_END) {
		// NOTE: reads during OAM block trigger OAM corruption
		// for now defaults to 0x00
		return 0x00;
	}
	if(addr >= bus::addr::IO_REG_START && addr <= bus::addr::IO_REG_END) {
		// add switch statement here for IO
		if(addr >= bus::addr::io::WAVE_RAM_START && addr <= bus::addr::io::WAVE_RAM_END) {
			return waveRam[addr - bus::addr::io::WAVE_RAM_START];
		}
		switch(addr) {
			case bus::addr::io::JOYPAD_INPUT_REG:
				return joypadReg;
			case bus::addr::io::SERIAL_TRANS_REG_0:
				return serialTransRegs[reg::SERIAL_TRANS_REG_0];
			case bus::addr::io::SERIAL_TRANS_REG_1:
				return serialTransRegs[reg::SERIAL_TRANS_REG_1];
			case bus::addr::io::DIV:
				return timerDivRegs[reg::DIV];
			case bus::addr::io::TIMA:
				return timerDivRegs[reg::TIMA];
			case bus::addr::io::TMA:
				return timerDivRegs[reg::TMA];
			case bus::addr::io::TAC:
				return timerDivRegs[reg::TAC];
			case bus::addr::io::IF_REG:
				return ifReg;
			case bus::addr::io::NR10:
				return audioRegs[reg::NR10];
			case bus::addr::io::NR11:
				return audioRegs[reg::NR11];
			case bus::addr::io::NR12:
				return audioRegs[reg::NR12];
			case bus::addr::io::NR13:
				return audioRegs[reg::NR13];
			case bus::addr::io::NR14:
				return audioRegs[reg::NR14];
			case bus::addr::io::NR21:
				return audioRegs[reg::NR21];
			case bus::addr::io::NR22:
				return audioRegs[reg::NR22];
			case bus::addr::io::NR23:
				return audioRegs[reg::NR23];
			case bus::addr::io::NR24:
				return audioRegs[reg::NR24];
			case bus::addr::io::NR30:
				return audioRegs[reg::NR30];
			case bus::addr::io::NR31:
				return audioRegs[reg::NR31];
			case bus::addr::io::NR32:
				return audioRegs[reg::NR32];
			case bus::addr::io::NR33:
				return audioRegs[reg::NR33];
			case bus::addr::io::NR34:
				return audioRegs[reg::NR34];
			case bus::addr::io::NR41:
				return audioRegs[reg::NR41];
			case bus::addr::io::NR42:
				return audioRegs[reg::NR42];
			case bus::addr::io::NR43:
				return audioRegs[reg::NR43];
			case bus::addr::io::NR44:
				return audioRegs[reg::NR44];
			case bus::addr::io::NR50:
				return audioRegs[reg::NR50];
			case bus::addr::io::NR51:
				return audioRegs[reg::NR51];
			case bus::addr::io::NR52:
				return audioRegs[reg::NR52];
			case bus::addr::io::LCDC:
				return lcdRegs[reg::LCDC];
			case bus::addr::io::STAT:
				return lcdRegs[reg::STAT];
			case bus::addr::io::SCY:
				return lcdRegs[reg::SCY];
			case bus::addr::io::SCX:
				return lcdRegs[reg::SCX];
			case bus::addr::io::LY:
				return lcdRegs[reg::LY];
			case bus::addr::io::LYC:
				return lcdRegs[reg::LYC];
			case bus::addr::io::DMA:
				return lcdRegs[reg::DMA];
			case bus::addr::io::BGP:
				return lcdRegs[reg::BGP];
			case bus::addr::io::OBP0:
				return lcdRegs[reg::OBP0];
			case bus::addr::io::OBP1:
				return lcdRegs[reg::OBP1];
			case bus::addr::io::WY:
				return lcdRegs[reg::WY];
			case bus::addr::io::WX:
				return lcdRegs[reg::WX];
			default:
				std::cerr << "Error: Couldn't recognize I/O register for read!" << std::endl;
				return 0x69;
		}
	}
	if(addr >= bus::addr::HRAM_START && addr <= bus::addr::HRAM_END) {
		return hram[addr - bus::addr::HRAM_START];
	}
	if(addr == bus::addr::IE_REG) {
		return ieReg;
	}

	// all else fails
	std::cerr << "Unable to find address for read!" << std::endl;
	return 0x69;

}

void Emu::busWrite(uint16_t addr, uint8_t data) {
	if(addr >= bus::addr::ROM_0_START && addr <= bus::addr::ROM_0_END) {
		romData[addr - bus::addr::ROM_0_START] = data;
	}
	else if(addr >= bus::addr::ROM_1_NN_START && addr <= bus::addr::ROM_1_NN_END) {
		// NOTE: add rom bank switching here
		romData[addr - bus::addr::ROM_1_NN_START] = data;
	}
	else if(addr >= bus::addr::VRAM_START && addr <= bus::addr::VRAM_END) {
		// change once graphics implemented
		std::cerr << "Error: VRAM not implemented; not writing!" << std::endl;
	}
	else if(addr >= bus::addr::ERAM_START && addr <= bus::addr::ERAM_END) {
		// external RAM bank (not implemented yet)
		std::cerr << "Error: External RAM not implemented; not writing!" << std::endl;
	}
	// NOTE: upper half of WRAM becomes switchable bank in CGB mode
	else if((addr >= bus::addr::WRAM_START && addr <= bus::addr::WRAM_END) || (addr >= bus::addr::ECHO_RAM_START && addr <= bus::addr::ECHO_RAM_END)) {
		wram[addr - bus::addr::WRAM_START] = data;
	}
	else if(addr >= bus::addr::OAM_START && addr <= bus::addr::OAM_END) {
		oam[addr - bus::addr::OAM_START] = data;
	}
	else if(addr >= bus::addr::NO_USE_START && addr <= bus::addr::NO_USE_END) {
		// NOTE: reads during OAM block trigger OAM corruption
		// for now defaults to 0x00
		std::cerr << "Error: Attempting to write to unusable memory!" << std::endl;
	}
	else if(addr >= bus::addr::IO_REG_START && addr <= bus::addr::IO_REG_END) {
		// add switch statement here for IO
		if(addr >= bus::addr::io::WAVE_RAM_START && addr <= bus::addr::io::WAVE_RAM_END) {
			waveRam[addr - bus::addr::io::WAVE_RAM_START] = data;
		}
		else {
			switch(addr) {
				case bus::addr::io::JOYPAD_INPUT_REG:
					joypadReg = data;
					break;
				case bus::addr::io::SERIAL_TRANS_REG_0:
					serialTransRegs[reg::SERIAL_TRANS_REG_0] = data;
					break;
				case bus::addr::io::SERIAL_TRANS_REG_1:
					serialTransRegs[reg::SERIAL_TRANS_REG_1] = data;
					break;
				case bus::addr::io::DIV:
					timerDivRegs[reg::DIV] = 0x00;
					break;
				case bus::addr::io::TIMA:
					timerDivRegs[reg::TIMA] = data;
					break;
				case bus::addr::io::TMA:
					timerDivRegs[reg::TMA] = data;
					break;
				case bus::addr::io::TAC:
					timerDivRegs[reg::TAC] = data;
					break;
				case bus::addr::io::IF_REG:
					ifReg = data;
					break;
				case bus::addr::io::NR10:
					audioRegs[reg::NR10] = data;
					break;
				case bus::addr::io::NR11:
					audioRegs[reg::NR11] = data;
					break;
				case bus::addr::io::NR12:
					audioRegs[reg::NR12] = data;
					break;
				case bus::addr::io::NR13:
					audioRegs[reg::NR13] = data;
					break;
				case bus::addr::io::NR14:
					audioRegs[reg::NR14] = data;
					break;
				case bus::addr::io::NR21:
					audioRegs[reg::NR21] = data;
					break;
				case bus::addr::io::NR22:
					audioRegs[reg::NR22] = data;
					break;
				case bus::addr::io::NR23:
					audioRegs[reg::NR23] = data;
					break;
				case bus::addr::io::NR24:
					audioRegs[reg::NR24] = data;
					break;
				case bus::addr::io::NR30:
					audioRegs[reg::NR30] = data;
					break;
				case bus::addr::io::NR31:
					audioRegs[reg::NR31] = data;
					break;
				case bus::addr::io::NR32:
					audioRegs[reg::NR32] = data;
					break;
				case bus::addr::io::NR33:
					audioRegs[reg::NR33] = data;
					break;
				case bus::addr::io::NR34:
					audioRegs[reg::NR34] = data;
					break;
				case bus::addr::io::NR41:
					audioRegs[reg::NR41] = data;
					break;
				case bus::addr::io::NR42:
					audioRegs[reg::NR42] = data;
					break;
				case bus::addr::io::NR43:
					audioRegs[reg::NR43] = data;
					break;
				case bus::addr::io::NR44:
					audioRegs[reg::NR44] = data;
					break;
				case bus::addr::io::NR50:
					audioRegs[reg::NR50] = data;
					break;
				case bus::addr::io::NR51:
					audioRegs[reg::NR51] = data;
					break;
				case bus::addr::io::NR52:
					audioRegs[reg::NR52] = data;
					break;
				case bus::addr::io::LCDC:
					lcdRegs[reg::LCDC] = data;
					break;
				case bus::addr::io::STAT:
					lcdRegs[reg::STAT] = data;
					break;
				case bus::addr::io::SCY:
					lcdRegs[reg::SCY] = data;
					break;
				case bus::addr::io::SCX:
					lcdRegs[reg::SCX] = data;
					break;
				case bus::addr::io::LY:
					lcdRegs[reg::LY] = data;
					break;
				case bus::addr::io::LYC:
					lcdRegs[reg::LYC] = data;
					break;
				case bus::addr::io::DMA:
					lcdRegs[reg::DMA] = data;
					break;
				case bus::addr::io::BGP:
					lcdRegs[reg::BGP] = data;
					break;
				case bus::addr::io::OBP0:
					lcdRegs[reg::OBP0] = data;
					break;
				case bus::addr::io::OBP1:
					lcdRegs[reg::OBP1] = data;
					break;
				case bus::addr::io::WY:
					lcdRegs[reg::WY] = data;
					break;
				case bus::addr::io::WX:
					lcdRegs[reg::WX] = data;
					break;
				default:
					std::cerr << "Attempted to write I/O address: 0x" << std::hex << addr << std::endl;
					std::cerr << "ERROR: Couldn't recognize I/O register for write!" << std::endl;
					cpu.haltExecution = true;
					break;
			}
		}
	}
	else if(addr >= bus::addr::HRAM_START && addr <= bus::addr::HRAM_END) {
		hram[addr - bus::addr::HRAM_START] = data;
	}
	else if(addr == bus::addr::IE_REG) {
		ieReg = data;
	}
	else {
		std::cerr << "Unable to find address for write!" << std::endl;
		cpu.haltExecution = true;
	}
}