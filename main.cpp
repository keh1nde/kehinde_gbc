#include "GBC_BUS.h"
#include "GBC_CPU.h"
#include "GBC_PPU.h"
#include "GBC_CART.h"
#include "TestBus.h"
#include "TestCart.h"

/*
 * Hypotheses:
 * 1. The read and write methods aren't declared in CPU, but in Bus. The CPU uses IBus but the
 *
 */

int main(int argc, char* argv[]) {

	const std::string romPath = argv[1];

	GBC_PPU ppu{};
	GBC_CART cart(romPath);
	GBC_BUS bus("", cart, ppu);

	GBC_CPU cpu(bus);


	cpu.resetPostBoot();
	for (int i = 0; i < 50000000; ++i) {
		try {
			cpu.execute();
		} catch (const std::exception& e) {
			std::cerr << "[i=" << i << "] "<< "Caught the following error:" << e.what() << std::endl;
			break;
		}
	}
}