// GAMEBOYCOLOR.cpp
// Created by Kehinde Adeoso on January 5th, 2026.
// Stores the interface for the Game Boy Color CPU core.


#include "GBC_CPU.h"

GBC_CPU::GBC_CPU(const GBC_BUS& bus)
: c_ProgramCounter(0x0000), c_StackPointer(0xFFFE), c_A(0x11), c_B(0x00), c_C(0x00), c_D(0x00),
c_E(0x00), c_H(0x00), c_F(0x00), c_L(0x00), bus_(bus) {}

BYTE GBC_CPU::getNextOpcode() {
	const BYTE next = bus_.read8(c_ProgramCounter);
	c_ProgramCounter++;

	return next;
}

void GBC_CPU::execute() {
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
			LD_r_HL(mid); // HL_r op
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
			LD_r_HL(mid); // HL_r op
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
			LD_r_HL(mid); // HL_r op
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
			LD_r_HL(mid); // HL_r op
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
			LD_r_HL(mid); // HL_r op
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
			LD_r_HL(mid); // HL_r op
			break;
		case 0x6F:
			LD_r_reg(mid, low);
			break;
		case 0x70:
			LD_HL_r(low); // HL_w op
			break;
		case 0x71:
			LD_HL_r(low); // HL_w op
			break;
		case 0x73:
			LD_HL_r(low); // HL_w op
			break;
		case 0x74:
			LD_HL_r(low); // HL_w op
			break;
		case 0x75:
			LD_HL_r(low); // HL_w op
			break;
		case 0x76:
			// TODO: Implement HALT()
			break;
		case 0x77:
			LD_HL_r(low); // HL_w op
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
			LD_r_HL(mid); // HL_r op
			break;
		case 0x7F:
			LD_r_reg(mid, low);
			break;
		case 0x36:
			LD_HL_n();
			break;
		case 0x0A:
			LD_A_BC();
			break;
		case 0x1A:
			LD_A_DE();
			break;
		case 0x02:
			LD_BC_A();
			break;
		case 0x12:
			LD_DE_A();
			break;
		case 0xFA:
			LD_A_nn();
			break;
		case 0xEA:
			LD_nn_A();
			break;
		case 0xF2:
			LDH_A_C();
			break;
		case 0xE2:
			LDH_C_A();
			break;
		case 0xF0:
			LDH_A_n();
			break;
		case 0xE0:
			LDH_n_A();
			break;
		case 0x3A:
			LD_A_HLdec();
			break;
		case 0x32:
			LD_HLdec_A();
			break;
		case 0x2A:
			LD_A_HLinc();
			break;
		case 0x22:
			LD_HLinc_A();
			break;

		// Begin 16-bit load instructions


		default: break; // TODO: Implement error handling.
	}


}

void GBC_CPU::LD_r_reg(const BYTE dest, const BYTE source) {
	storeByCode(dest, source);
}

void GBC_CPU::LD_r_n(const BYTE dest) {
	const int immediate = getNextOpcode();
	storeByCode(dest, immediate);
}

void GBC_CPU::LD_r_HL(BYTE dest) {
	const BYTE high = c_H;
	const BYTE low = c_L;

	const WORD address = (high << 8) | low;
	storeImmediate(dest, bus_.read8(address));
}

void GBC_CPU::LD_HL_r(const BYTE source) {
	const BYTE high = c_H;
	const BYTE low  = c_L;

	const WORD address = (high << 8) | low;

	bus_.write8(address, getByCode(source));

}


void GBC_CPU::LD_HL_n() {
	const BYTE high = c_H;
	const BYTE low  = c_L;

	const WORD address = (high << 8) | low; // TODO: Verify this is the correct implementation
	const BYTE imm = getNextOpcode();

	bus_.write8(address, imm);
}

void GBC_CPU::LD_A_BC() {
	const BYTE high = c_B;
	const BYTE low  = c_C;
	const WORD address = (high << 8) | low;

	c_A = bus_.read8(address);
}

void GBC_CPU::LD_A_DE() {
	const BYTE high = c_D;
	const BYTE low  = c_E;
	const WORD address = (high << 8) | low;

	c_A = bus_.read8(address);
}

void GBC_CPU::LD_BC_A() {
	const BYTE high = c_B;
	const BYTE low = c_C;
	const WORD address = (high << 8) | low;

	bus_.write8(address, c_A);
}

void GBC_CPU::LD_DE_A() {
	const BYTE high = c_D;
	const BYTE low = c_E;
	const WORD address = (high << 8) | low;

	bus_.write8(address, c_A);
}

void GBC_CPU::LD_A_nn() {
	const BYTE high = getNextOpcode();
	const BYTE low = getNextOpcode();
	const WORD immediate = (high << 8) | low;

	c_A = bus_.read8(immediate);
}

void GBC_CPU::LD_nn_A() {
	const BYTE high = getNextOpcode();
	const BYTE low = getNextOpcode();
	const WORD address = (high << 8) | low;

	bus_.write8(address, c_A);
}

void GBC_CPU::LDH_A_C() {
	constexpr BYTE high = 0xFF;
	const BYTE low = c_C;
	const WORD address = (high << 8) | low;

	c_A = bus_.read8(address);
}

void GBC_CPU::LDH_C_A() {
	constexpr BYTE high = 0xFF;
	const BYTE low = c_C;
	const WORD address = (high << 8) | low;

	bus_.write8(address, c_A);
}

void GBC_CPU::LDH_A_n() {
	constexpr BYTE high = 0xFF;
	const BYTE low = getNextOpcode();
	const WORD address = (high << 8) | low;

	c_A = bus_.read8(address);
}

void GBC_CPU::LDH_n_A() {
	constexpr BYTE high = 0xFF;
	const BYTE low = getNextOpcode();
	const WORD address = (high << 8) | low;

	bus_.write8(address, c_A);
}

void GBC_CPU::LD_A_HLdec() {
	BYTE high = c_H;
	BYTE low = c_L;
	WORD address = (high << 8) | low;

	c_A = bus_.read8(address);

	address--;
	high = address & 0xFF;
	low = address >> 8;

	c_H = high;
	c_L = low;

}

void GBC_CPU::LD_HLdec_A() {
	BYTE high = c_H;
	BYTE low = c_L;
	WORD address = (high << 8) | low;

	bus_.write8(address, c_A);

	address--;
	high = address & 0xFF;
	low = address >> 8;

	c_H = high;
	c_L = low;
}

void GBC_CPU::LD_A_HLinc() {
	BYTE high = c_H;
	BYTE low = c_L;
	WORD address = (high << 8) | low;

	c_A = bus_.read8(address);


	address++;
	high = address & 0xFF;
	low = address >> 8;

	c_H = high;
	c_L = low;
}

void GBC_CPU::LD_HLinc_A() {
	BYTE high = c_H;
	BYTE low = c_L;
	WORD address = (high << 8) | low;

	bus_.write8(address, c_A);

	address++;
	high = address & 0xFF;
	low = address >> 8;

	c_H = high;
	c_L = low;
}



void GBC_CPU::storeByCode(const BYTE dest, const BYTE source) {
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
void GBC_CPU::storeImmediate(const BYTE dest, const BYTE imm) {
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

BYTE GBC_CPU::getByCode(const BYTE source) {
	switch (source) {
		case 0:
			return c_B;
		case 1:
			return c_C;
		case 2:
			return c_D;
		case 3:
			return c_E;
		case 4:
			return c_H;
		case 5:
			return c_L;
		case 6:
			return c_H;
		case 7:
			return c_A;
		default: // Throw an error
	}
}


