#include <iostream>
#include "emu.h"
#include "const.h"

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
		emu.emuLoop();
	}

	return 0;
}
