//
// Created by Kehinde Adeoso on 2/14/26.
//

#pragma once
#include "GBC_Types.h"
#include <string>
#include <array>
#include <iostream>

class GBC_CART {
public:
	// Initializers
	explicit GBC_CART(const std::string& romPath);

	BYTE read_rom(WORD addr) const;
	void write_rom(WORD addr, BYTE input);

	BYTE read_ram(WORD addr);
	void write_ram(WORD addr, BYTE input);


private:
	// Storage
	std::array<BYTE, 0xFFFF> c_CartridgeRAM;
	std::array<BYTE, 0xFFFF> c_CartridgeROM;
};

