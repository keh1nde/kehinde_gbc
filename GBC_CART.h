//
// Created by Kehinde Adeoso on 2/14/26.
//

#pragma once
#include "GBC_Types.h"
#include <string>
#include <vector>
#include <cstdio>
#include <stdexcept>

class GBC_CART {
public:
	enum class Mapper { None, MBC1, MBC2, MBC5 };

	explicit GBC_CART(const std::string& romPath);

	BYTE read_rom(WORD addr) const;
	void write_rom(WORD addr, BYTE val);

	BYTE read_ram(WORD addr) const;
	void write_ram(WORD addr, BYTE val);

private:
	std::vector<BYTE> rom_;
	std::vector<BYTE> ram_;
	Mapper mapper_ = Mapper::None;

	int rom_bank_count_ = 2;   // total 16 KB banks
	int ram_bank_count_ = 0;   // total 8 KB banks

	// Common
	bool ram_enable_ = false;

	// MBC1
	BYTE mbc1_rom_lo_ = 1;     // 5 bits, 0x2000-0x3FFF (lower bank). 0 → 1 quirk on read.
	BYTE mbc1_upper_  = 0;     // 2 bits, 0x4000-0x5FFF (upper ROM bits or RAM bank).
	bool mbc1_mode_   = false; // 0x6000-0x7FFF: 0 = ROM mode, 1 = RAM/upper-ROM mode.

	// MBC5
	WORD mbc5_rom_bank_ = 1;   // 9 bits across 0x2000-0x2FFF (low 8) + 0x3000-0x3FFF (bit 8).
	BYTE mbc5_ram_bank_ = 0;   // 4 bits, 0x4000-0x5FFF.

	// Helpers
	int mbc1_rom_bank_lo_region_() const;  // bank number for 0x0000-0x3FFF reads
	int mbc1_rom_bank_hi_region_() const;  // bank number for 0x4000-0x7FFF reads
	int mbc1_ram_bank_() const;
};