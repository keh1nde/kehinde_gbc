//
// Created by Kehinde Adeoso on 1/5/26.
//

#ifndef KEHINDES_GAMEBOYCOLOR_EMULATOR_GAMEBOYCOLOR_H
#define KEHINDES_GAMEBOYCOLOR_EMULATOR_GAMEBOYCOLOR_H

#include <algorithm>
#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <fstream>

struct GameBoyColor {
	using BYTE = std::uint8_t;
	using WORD = std::uint16_t;

	BYTE CartROMOffset = 0x0000;

 // RAM and ROM Arrays
	BYTE m_WorkRAM[0x8000];
	BYTE m_VideoRAM[0x4000];

	// Cartridge Information
	BYTE CartridgeROMCode = 0x00;
	BYTE CartridgeRAMCode = 0x00;

	BYTE CartridgeROMBytes = 0;
	BYTE CartridgeRAMBytes = 0;
	std::vector<BYTE> CartridgeROM;  // ROM size depends on cartridge, so we need logic to set the size.
	std::vector<BYTE> CartridgeRAM;

	// Registers
	WORD m_AF; // Accumulators and Flags
	WORD m_BC;
	WORD m_DE;
	WORD m_HL;
	WORD m_StackPointer;
	WORD m_ProgramCounter;

	void CPUReset(bool useBootROM);
	void execute();
	WORD getNextOpcode();

	// Memory Methods
	void read(int bitSize);
	void write();

// Begin Instructions.

};




#endif //KEHINDES_GAMEBOYCOLOR_EMULATOR_GAMEBOYCOLOR_H