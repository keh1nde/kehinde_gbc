// GAMEBOYCOLOR.cpp
// Created by Kehinde Adeoso on January 5th, 2026.
// Stores the interface for the Game Boy Color CPU core.


#include "GameBoyColor.h"

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

WORD CPU::getNextOpcode() {
	const WORD next = c_ProgramCounter;
	c_ProgramCounter += 1;
	return next;
}

void CPU::execute() {
	WORD opcode = (getNextOpcode() & 0xF000) & 0b1100;
	switch (opcode & 0xF0) {
		case 01:
			LD_r_reg(opcode);
			break;
		case 00:
			LD_r_imm(opcode);
		default:
			throw std::logic_error("OPCode is either invalid or not implemented");
	}

}

void CPU::LD_r_reg(const WORD &opcode) {
	const BYTE r = (opcode >> 3) & 0b111;
	const BYTE rp = opcode & 0b111;

	storeByCode(r, rp);
}

void CPU::LD_r_imm(const WORD &opcode) {
	BYTE dest = (opcode >> 3) & 0b111;
	BYTE imm = opcode & 0b111;

	storeImmediate(dest, imm);
}


void CPU::storeByCode(const BYTE& dest, const BYTE& source) {
	BYTE value;

	// set source r'
	switch (source) {
		case 0:
			value = c_B;
			break;
		case 1: value = c_C;
			break;
		case 2: value = c_D;
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
		case 7:
			value = c_A;
			break;
		default: throw std::logic_error("code 6 is (HL), not a register ref");
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
		case 7:
			c_A = value;

		default: throw std::logic_error("code 6 is (HL), not a register ref");
	}
}

void CPU::storeImmediate(const BYTE &dest, const BYTE &imm) {
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
		case 7:
			c_A = imm;

		default: throw std::logic_error("code 6 is (HL), not a register ref");
	}
}
