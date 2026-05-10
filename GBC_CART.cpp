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

GBC_CART::GBC_CART(const std::string& romPath) {
	if (romPath.empty()) {
		// Empty-path mode for tests: leave with a 32 KiB no-MBC blank cart.
		rom_.assign(32 * 1024, 0);
		rom_bank_count_ = 2;
		mapper_ = Mapper::None;
		return;
	}

	FILE* file = fopen(romPath.c_str(), "rb");
	if (!file) throw std::runtime_error("Could not open cartridge file");

	fseek(file, 0, SEEK_END);
	const long size = ftell(file);
	fseek(file, 0, SEEK_SET);

	rom_.resize(size);
	fread(rom_.data(), 1, size, file);
	fclose(file);

	if (size < 0x150) throw std::runtime_error("ROM too small to contain a header");

	const BYTE cart_type = rom_[0x147];
	const BYTE rom_code  = rom_[0x148];
	const BYTE ram_code  = rom_[0x149];

	const int rom_bytes = rom_size_from_header(rom_code);
	int ram_bytes = ram_size_from_header(ram_code);

	rom_bank_count_ = rom_bytes / (16 * 1024);
	ram_bank_count_ = ram_bytes / (8 * 1024);

	switch (cart_type) {
		case 0x00:
			mapper_ = Mapper::None;
			break;
		case 0x01: case 0x02: case 0x03:
			mapper_ = Mapper::MBC1;
			break;
		case 0x05: case 0x06:
			mapper_ = Mapper::MBC2;
			// MBC2 has built-in 512x4-bit RAM (256 bytes addressable, mirrored across 0xA000-0xBFFF).
			ram_bytes = 512;
			ram_bank_count_ = 1;
			break;
		case 0x19: case 0x1A: case 0x1B:
		case 0x1C: case 0x1D: case 0x1E:
			mapper_ = Mapper::MBC5;
			break;
		default:
			throw std::runtime_error("Unsupported cartridge type");
	}

	if (ram_bytes > 0) ram_.assign(ram_bytes, 0);
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

