//
// Created by Kehinde Adeoso on 1/5/26.
//

#ifndef KEHINDES_GAMEBOYCOLOR_EMULATOR_GAMEBOYCOLOR_H
#define KEHINDES_GAMEBOYCOLOR_EMULATOR_GAMEBOYCOLOR_H

#include <stack>


class GameBoyColor {
	using BYTE = uint8_t;
	using WORD = uint16_t;

 // Ram Arrays
	BYTE m_WorkRAM[0x8000];
	BYTE m_VideoRAM[0x4000];

	// Registers
	WORD m_AF; // Accumulators and Flags
	WORD m_BC;
	WORD m_DE;
	WORD m_HL;
	WORD m_StackPointer;
	WORD m_ProgramCounter;

	void CPUReset();
	void execute();
	WORD getNextOpcode();

// Begin Instructions.







};


#endif //KEHINDES_GAMEBOYCOLOR_EMULATOR_GAMEBOYCOLOR_H