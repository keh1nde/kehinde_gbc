// GAMEBOYCOLOR.cpp
// Created by Kehinde Adeoso on January 5th, 2026.
// Stores the interface for the Game Boy Color CPU core.


#include "GameBoyColor.h"

CPU::CPU(const std::string& bootPath, const std::string &cartPath)
: c_ProgramCounter(0x0000), c_StackPointer(0xFFFE), c_AF(0x1100), c_BC(0x0000), c_DE(0x0000), c_HL(0x0000){
	// Load BootROM.
	FILE* file = fopen(bootPath.c_str(),"rb");
	int b_pos = 0x0000;
	while (fread(&c_WorkRAM[b_pos], 1, 1, file)) {
		b_pos++;
	}

	fclose(file);

	// Load Cartridge.
	file = fopen(cartPath.c_str(), "rb");
	int c_pos = 0x0100;
	while (fread(&c_WorkRAM[c_pos], 1, 1, file)) {
		c_pos++;
	}
	fclose(file);
	// Set program counter to start at 0x0000, stack pointer at 0xFFFE
}
