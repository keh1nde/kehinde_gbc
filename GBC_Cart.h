//
// Created by Kehinde Adeoso on 2/14/26.
//

/*
 * Cartridge.h
 * Contains the data members, methods, and others for the cartridge
 * of a Game Boy Color.
 *
 * A Game Boy Color cartridge should include:
 * Data Structures:
 * An array of various size for RAM and ROM respectively
 * Initializers
 * Memory methods for both reading and writing
 */

#pragma once
#include "GBC_CPU.h"


class GBC_Cart {
public:
	// Initializers
	GBC_Cart(int c_CartridgeRAM, int c_CartridgeROM);
	BYTE read_rom(WORD addr) const;
	void write_rom(WORD addr, BYTE input);

	BYTE read_ram(WORD addr);
	BYTE write_ram(WORD addr, BYTE input);


private:
	// Storage
	BYTE c_CartridgeRAM[0xFFFF];
	BYTE c_CartridgeROM[0xFFFF];
};

