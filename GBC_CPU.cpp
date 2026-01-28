// GAMEBOYCOLOR.cpp
// Created by Kehinde Adeoso on January 5th, 2026.
// Stores the interface for the Game Boy Color CPU core.


#include "GBC_CPU.h"

CPU::CPU(const std::string& bootPath, const std::string &cartPath)
: c_ProgramCounter(0x0000), c_StackPointer(0xFFFE), c_WorkRAM(0x0000), c_A(0x11), c_B(0x00), c_C(0x00), c_D(0x00),
c_E(0x00), c_F(0x00), c_H(0x00), c_L(0x00){
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
	// Done above.
}

BYTE CPU::getNextOpcode() {
	const BYTE next = c_WorkRAM[c_ProgramCounter];
	c_ProgramCounter += 1;
	return next;
}

void CPU::execute() {
	const BYTE opcode = getNextOpcode();

	if (opcode == 0xCB) {
		// TODO: Implement executeCB().
		return;
	}

	const BYTE top = opcode >> 6; // Top two bits
	const BYTE mid = (opcode >> 3) & 0x7; // Middle 3 bits.
	const BYTE low = opcode & 0x7; // Low 3 bits.

	switch (opcode & 0xC0) {
		// Note that all opcodes 0x40 - 0x7F are register LD instructions,
		// except 0x76 which is HALT. Some instructions fetch from memory which requires the MMU.
		case 0x41:
			LD_r_reg(mid, low);
			break;
		case 0x42:
			LD_r_reg(mid, low);
			break;
		case 0x43:
			LD_r_reg(mid, low);
			break;
		case 0x44:
			LD_r_reg(mid, low);
			break;
		case 0x45:
			LD_r_reg(mid, low);
			break;
		case 0x46:
			// TODO: Implement the MMU.
			break;
		case 0x47:
			LD_r_reg(mid, low);
			break;
		case 0x48:
			LD_r_reg(mid, low);
			break;
		case 0x49:
			LD_r_reg(mid, low);
			break;
		case 0x4A:
			LD_r_reg(mid, low);
			break;
		case 0x4B:
			LD_r_reg(mid, low);
			break;
		case 0x4C:
			LD_r_reg(mid, low);
			break;
		case 0x4D:
			LD_r_reg(mid, low);
			break;
		case 0x4E:
			// TODO: Implement the MMU.
			break;
		case 0x4F:
			LD_r_reg(mid, low);
			break;
		case 0x50:
			LD_r_reg(mid, low);
			break;
		case 0x51:
			LD_r_reg(mid, low);
			break;
		case 0x52:
			LD_r_reg(mid, low);
			break;
		case 0x53:
			LD_r_reg(mid, low);
			break;
		case 0x54:
			LD_r_reg(mid, low);
			break;
		case 0x55:
			LD_r_reg(mid, low);
			break;
		case 0x56:
			// TODO: Implement the MMU.
			break;
		case 0x57:
			LD_r_reg(mid, low);
			break;
		case 0x58:
			LD_r_reg(mid, low);
			break;
		case 0x59:
			LD_r_reg(mid, low);
			break;
		case 0x5A:
			LD_r_reg(mid, low);
			break;
		case 0x5B:
			LD_r_reg(mid, low);
			break;
		case 0x5C:
			LD_r_reg(mid, low);
			break;
		case 0x5D:
			LD_r_reg(mid, low);
			break;
		case 0x5E:
			// TODO: Implement the MMU.
			break;
		case 0x5F:
			LD_r_reg(mid, low);
			break;
		case 0x60:
			LD_r_reg(mid, low);
			break;
		case 0x61:
			LD_r_reg(mid, low);
			break;
		case 0x62:
			LD_r_reg(mid, low);
			break;
		case 0x63:
			LD_r_reg(mid, low);
			break;
		case 0x64:
			LD_r_reg(mid, low);
			break;
		case 0x65:
			LD_r_reg(mid, low);
			break;
		case 0x66:
			// TODO: Implement the MMU.
			break;
		case 0x67:
			LD_r_reg(mid, low);
			break;
		case 0x68:
			LD_r_reg(mid, low);
			break;
		case 0x69:
			LD_r_reg(mid, low);
			break;
		case 0x6A:
			LD_r_reg(mid, low);
			break;
		case 0x6B:
			LD_r_reg(mid, low);
			break;
		case 0x6C:
			LD_r_reg(mid, low);
			break;
		case 0x6D:
			LD_r_reg(mid, low);
			break;
		case 0x6E:
			// TODO: Implement the MMU.
			break;
		case 0x6F:
			LD_r_reg(mid, low);
			break;
		case 0x70:
			// TODO: Implement the MMU
			break;
		case 0x71:
			// TODO: Implement the MMU.
			break;
		case 0x73:
			// TODO: Implement the MMU.
			break;
		case 0x74:
			// TODO: Implement the MMU.
			break;
		case 0x75:
			// TODO: Implement the MMU.
			break;
		case 0x76:
			// TODO: Implement HALT()
			break;
		case 0x77:
			// TODO: Implement the MMU.
			break;
		case 0x78:
			LD_r_reg(mid, low);
			break;
		case 0x79:
			LD_r_reg(mid, low);
			break;
		case 0x7A:
			LD_r_reg(mid, low);
			break;
		case 0x7B:
			LD_r_reg(mid, low);
			break;
		case 0x7C:
			LD_r_reg(mid, low);
			break;
		case 0x7D:
			LD_r_reg(mid, low);
			break;
		case 0x7E:
			// TODO: Implement the MMU.
			break;
		case 0x7F:
			LD_r_reg(mid, low);
			break;


		default: break; // TODO: Implement error handling.
	}


}

void CPU::LD_r_reg(const BYTE dest, const BYTE source) {
	storeByCode(dest, source);
}

void CPU::LD_r_HL(const BYTE H_dest, const BYTE L_dest, const BYTE val) {
	const BYTE low_val = (val & 0xFF00);
	const BYTE high_val = (val >> 8) & 0x00FF;

	storeImmediate(H_dest, high_val);
	storeImmediate(L_dest, low_val);
}


void CPU::storeByCode(const BYTE dest, const BYTE source) {
	BYTE value;

	// set source r'
	switch (source) {
		case 0:
			value = c_B;
			break;
		case 1:
			value = c_C;
			break;
		case 2:
			value = c_D;
			break;
		case 3:
			value = c_E;
			break;
		case 4:
			value = c_H;
			break;
		case 5:
			value = c_L;
			break;
		case 6:
			value = c_H;
			break;
		case 7:
			value = c_A;
			break;
		default: return;
	}

	// set dest r to value
	switch (dest) {
		case 0:
			c_B = value;
			break;
		case 1:
			c_C = value;
			break;
		case 2:
			c_D = value;
			break;
		case 3:
			c_E = value;
			break;
		case 4:
			c_H = value;
			break;
		case 5:
			c_L = value;
			break;
		case 6: // May need to be removed.
			c_H = value;
			break;
		case 7:
			c_A = value;
			break;

		default: break;
	}
}


// 0b10 000 110 4 + 2 + 0 = 6
void CPU::storeImmediate(const BYTE dest, const BYTE imm) {
	switch (dest) {
		case 0:
			c_B = imm;
			break;
		case 1:
			c_C = imm;
			break;
		case 2:
			c_D = imm;
			break;
		case 3:
			c_E = imm;
			break;
		case 4:
			c_H = imm;
			break;
		case 5:
			c_L = imm;
			break;
		case 6: // May need to be removed.
			c_H = imm;
			break;
		case 7:
			c_A = imm;
			break;

		default: break;
	}
}


