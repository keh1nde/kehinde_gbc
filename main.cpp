#include "GBC_BUS.h"
#include "GBC_CPU.h"
#include "GBC_PPU.h"
#include "GBC_CART.h"


int main(int argc, char* argv[]) {

	const std::string romPath = argv[1];

	GBC_PPU ppu{};
	GBC_CART cart(romPath);
	GBC_BUS bus("", cart, ppu);

	GBC_CPU cpu(bus);

	for (WORD a = 0x207; a <= 0x20C; ++a) {
		std::cerr << std::hex << a << ": " << static_cast<int>(cart.read_rom(a)) << "\n";
	}

	cpu.resetPostBoot();

	/*std::cerr << "ROM[0X7000]=" << std::hex <<
		static_cast<int>(cart.read_rom(0x7000)) << std::endl;
	std::cerr << "ROM(0x7FFF)=" << std::hex <<
		static_cast<int>(cart.read_rom(0x7FFF)) << std::endl;*/

	std::cerr << "Starting execution, PC=" << std::hex << cpu.c_ProgramCounter << std::endl;
	for (int i = 0; i < 49200; ++i) {
		try {
			// if (i % 100000 == 0) std::cout << "Current PC address: " << std::hex << cpu.c_ProgramCounter << std::endl;
			cpu.execute();
		} catch (const std::exception& e) {
			std::cerr << "[i=" << i << "] "<< "Caught the following error:" << e.what() << std::endl;
		}
	}
}