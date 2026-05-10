//
// Created by Kehinde Adeoso on 2/14/26.
//

#include "GBC_CART.h"

namespace {
	int rom_size_from_header(BYTE h148) {
		// Header byte 0x148: total ROM size = 32 KiB << h148 for h148 in 0..8.
		// (Values 0x52/0x53/0x54 are non-standard and rarely used; ignore.)
		return (32 * 1024) << h148;
	}

	int ram_size_from_header(BYTE h149) {
		switch (h149) {
			case 0x00: return 0;
			case 0x01: return 2 * 1024;     // legacy / unofficial
			case 0x02: return 8 * 1024;
			case 0x03: return 32 * 1024;
			case 0x04: return 128 * 1024;
			case 0x05: return 64 * 1024;
			default:   return 0;
		}
	}
}

GBC_CART::GBC_CART(const std::string &romPath) {
	if (!romPath.empty()) {
		FILE* file = fopen(romPath.c_str(), "rb");

		if (!file) {
			throw std::runtime_error("Could not open cartridge file");
		}

		fseek(file, 0, SEEK_END);

		const long size = ftell(file);
		// std::cout << size << std::endl;

		fseek(file, 0, SEEK_SET);
		if (size > c_CartridgeROM.size()) throw std::runtime_error("ROM size exceeds max size.");

		fread(c_CartridgeROM.data(), 1, size, file);

		fclose(file);
	}
}

BYTE GBC_CART::read_rom(const WORD addr) const {
	return c_CartridgeROM[addr];
}

void GBC_CART::write_rom(WORD addr, BYTE value) {

}


BYTE GBC_CART::read_ram(const WORD addr) {
	return addr;
}

void GBC_CART::write_ram(WORD addr, BYTE input) {

}

