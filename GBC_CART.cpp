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

		// fread(c_CartridgeROM.data(), 1, size, file);

		const size_t bytes_read = fread(c_CartridgeROM.data(), 1, size, file);
		std::cerr << "bytes_read=" << bytes_read << "\n";
		std::cerr << "byte @ 0x20D = 0x" << std::hex << static_cast<int>(c_CartridgeROM[0x20D]) << "\n";
		std::cerr << "byte @ 0x7000 = 0x" << std::hex << static_cast<int>(c_CartridgeROM[0x7000]) << "\n";

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

