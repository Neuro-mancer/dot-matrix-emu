#include <iostream>
#include "emu.h"
#include "const.h"
#include <chrono>
#include <thread>

int main(int argc, char **argv) {
	Emu emu;

	std::cout << "Starting DMEmu..." << std::endl;
	std::cout << "There is (are) " << argc << " argument(s):" << std::endl;
	for(int i = 0; i < argc; i++) {
		std::cout << "Argument " << i << ": '" << argv[i] << "', ";
	}
	std::cout << std::endl;

	// assume second argument is rom file to be read

	bool fileSuccess = emu.readRomFile(argv[1]);
	if(fileSuccess) {
		emu.getHeader();
		emu.init();

		auto lastTime = std::chrono::steady_clock::now();

		while(!emu.cpu.haltExecution)
		{
			emu.cpu.fetch();
			emu.cpu.decode();
			auto currentTime = std::chrono::steady_clock::now();
			std::chrono::duration<float,std::milli> deltaMs = currentTime - lastTime;
			float deltaS = deltaMs.count() / 1000.0f;
			float sleepFor = ((1.0f / timing::FRAME_RATE) - deltaS) * 1000.0f;
			lastTime = currentTime;
			emu.cpu.printRegisters();
			std::this_thread::sleep_for(std::chrono::milliseconds(uint64_t(sleepFor)));
		}
	}

	return 0;
}
