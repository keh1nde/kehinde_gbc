#include "GBC_BUS.h"
#include "GBC_CPU.h"
#include "GBC_PPU.h"
#include "GBC_CART.h"
#include "GBC_TIMER.h"
#include "GBC_DISPLAY.h"

int main(int argc, char* argv[]) {

	const std::string romPath = argv[1];

	GBC_PPU ppu{};
	GBC_CART cart(romPath);
	std::cerr << "header[0x143]=0x" << std::hex << static_cast<int>(cart.read_rom(0x0143)) <<
 "\n";

	GBC_TIMER timer{};
	GBC_BUS bus("", cart, ppu, timer);
	GBC_CPU cpu(bus);

	ppu.set_bus(&bus);
	timer.set_bus(&bus);
	cpu.resetPostBoot();
	ppu.resetPostBoot();
	timer.resetPostBoot();

	GBC_DISPLAY display;
	if (!display.init(4)) return 1;

	bool running = true;
	while (running) {
		try {
			const int cycles = cpu.execute();
			ppu.tick(cycles);
			timer.tick(cycles);
			const int irq_cycles = cpu.serviceInterrupts();
			if (irq_cycles) {
				ppu.tick(irq_cycles);
				timer.tick(irq_cycles);
			}

			if (ppu.consume_frame_ready()) {
				display.present(ppu.framebuffer_view());
				running = display.pump();
			}
		} catch (const std::exception& e) {
			std::cerr << "Caught the following error: " << e.what() << std::endl;
			break;
		}
	}
	return 0;
}