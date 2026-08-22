//
// Created by Kehinde Adeoso on 4/22/26.
//

#include "test-shims/TestCart.h"


TestCart::TestCart(const std::string &romPath) {
	if (!romPath.empty()) {
		FILE* file = fopen(romPath.c_str(), "rb");

		if (!file) {
			throw std::runtime_error("Could not open cartridge file");
		}

		fseek(file, 0, SEEK_END);

		const long size = ftell(file);
		// std::cout << size << std::endl;

		fseek(file, 0, SEEK_SET);
		if (size > rom_.size()) throw std::runtime_error("ROM size exceeds max size.");

		fread(rom_.data(), 1, size, file);

		/*const size_t bytes_read = fread(c_CartridgeROM.data(), 1, size, file);
		std::cerr << "bytes_read=" << bytes_read << "\n";
		std::cerr << "byte @ 0x20D = 0x" << std::hex << static_cast<int>(c_CartridgeROM[0x20D]) << "\n";
		std::cerr << "byte @ 0x7000 = 0x" << std::hex << static_cast<int>(c_CartridgeROM[0x7000]) << "\n";*/

		fclose(file);
	}
}

BYTE TestCart::read_ram(const WORD addr) const {
	return ram_[addr];
}

void TestCart::write_ram(const WORD addr, const BYTE value) {
	ram_[addr - 0xA000] = value;
}

BYTE TestCart::read_rom(const WORD addr) const {
	return rom_[addr];
}

void TestCart::write_rom(const WORD addr, const BYTE value) {
	// NO-OP
}