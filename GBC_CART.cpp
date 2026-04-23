//
// Created by Kehinde Adeoso on 2/14/26.
//

#include "GBC_CART.h"


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

