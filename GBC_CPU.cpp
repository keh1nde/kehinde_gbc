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

	switch (opcode) {
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
		case 0x72:
			LD_HL_r(low);
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
		case 0x01:
			LD_rr_nn(c_PairBC);
			break;
		case 0x11:
			LD_rr_nn(c_PairDE);
			break;
		case 0x21:
			LD_rr_nn(c_PairHL);
			break;
		case 0x31:
			LD_rr_nn(c_PairSP);
			break;
		case 0x08:
			LD_nn_SP();
			break;
		case 0xF9:
			LD_SP_HL();
			break;
		case 0xC5:
			PUSH_rr(c_PairBC);
			break;
		case 0xD5:
			PUSH_rr(c_PairDE);
			break;
		case 0xE5:
			PUSH_rr(c_PairHL);
			break;
		case 0xF5:
			// TODO: Implement AF as an entry inside store modification functions including read and write.
			break;
		case 0xC1:
			POP_rr(c_PairBC);
			break;
		case 0xD1:
			POP_rr(c_PairDE);
			break;
		case 0xE1:
			POP_rr(c_PairHL);
			break;
		case 0xF1:
			// TODO: Implement AF as an entry inside store modification functions including read and write.
			break;
		case 0xF8:
			LD_HL_SPe();
			break;
		// The following are all ADD instructions:
		case 0x80:
			ADD_A_r(c_RegB);
			break;
		case 0x81:
			ADD_A_r(c_RegC);
			break;
		case 0x82:
			ADD_A_r(c_RegD);
			break;
		case 0x83:
			ADD_A_r(c_RegE);
			break;
		case 0x84:
			ADD_A_r(c_RegH);
			break;
		case 0x85:
			ADD_A_r(c_RegL);
			break;
		case 0x86:
			ADD_A_HL();
			break;
		case 0x87:
			ADD_A_r(c_RegA);
			break;
		case 0xC6:
			ADD_A_n();
			break;
		case 0x88:
			ADC_A_r(c_RegB);
			break;
		case 0x89:
			ADC_A_r(c_RegC);
			break;
		case 0x8A:
			ADC_A_r(c_RegD);
			break;
		case 0x8B:
			ADC_A_r(c_RegE);
			break;
		case 0x8C:
			ADC_A_r(c_RegH);
			break;
		case 0x8D:
			ADC_A_r(c_RegL);
			break;
		case 0x8E:
			ADC_A_HL();
			break;
		case 0x8F:
			ADC_A_r(c_RegA);
			break;
		case 0xCE:
			ADC_A_n();
			break;
		case 0x90:
			SUB_r(c_RegB);
			break;
		case 0x91:
			SUB_r(c_RegC);
			break;
		case 0x92:
			SUB_r(c_RegD);
			break;
		case 0x93:
			SUB_r(c_RegE);
			break;
		case 0x94:
			SUB_r(c_RegH);
			break;
		case 0x95:
			SUB_r(c_RegL);
			break;
		case 0x96:
			SUB_HL();
			break;
		case 0x97:
			SBC_A_r(c_RegA);
			break;
		case 0x98:
			SBC_A_r(c_RegB);
			break;
		case 0x99:
			SBC_A_r(c_RegC);
			break;
		case 0x9A:
			SBC_A_r(c_RegD);
			break;
		case 0x9B:
			SBC_A_r(c_RegE);
			break;
		case 0x9C:
			SBC_A_r(c_RegH);
			break;
		case 0x9D:
			SBC_A_r(c_RegL);
			break;
		case 0x9E:
			SBC_A_HL();
			break;
		case 0x9F:
			SBC_A_r(c_RegA);
			break;
		case 0xDE:
			SBC_A_n();
			break;
		case 0xB8:
			CP_r(c_RegB);
			break;
		case 0xB9:
			CP_r(c_RegC);
			break;
		case 0xBA:
			CP_r(c_RegD);
			break;
		case 0xBB:
			CP_r(c_RegE);
			break;
		case 0xBC:
			CP_r(c_RegH);
			break;
		case 0xBD:
			CP_r(c_RegL);
			break;
		case 0xBE:
			CP_HL();
			break;
		case 0xBF:
			CP_r(c_RegA);
			break;
		case 0xFE:
			CP_n();
			break;
		case 0x04:
			INC_r(c_RegB);
			break;
		case 0x14:
			INC_r(c_RegD);
			break;
		case 0x24:
			INC_r(c_RegH);
			break;
		case 0x34:
			INC_HL();
			break;
		case 0x1C:
			INC_r(c_RegE);
			break;
		case 0x2C:
			INC_r(c_RegL);
			break;
		case 0x3C:
			INC_r(c_RegA);
			break;
		case 0xC0:
			INC_r(c_RegC);
			break;
		case 0x05:
			DEC_r(c_RegB);
			break;
		case 0x15:
			DEC_r(c_RegD);
			break;
		case 0x25:
			DEC_r(c_RegH);
			break;
		case 0x35:
			DEC_HL();
			break;
		case 0x0D:
			DEC_r(c_RegC);
			break;
		case 0x1D:
			DEC_r(c_RegE);
			break;
		case 0x2D:
			DEC_r(c_RegL);
			break;
		case 0x3D:
			DEC_r(c_RegA);
			break;
		case 0xA0:
			AND_r(c_RegB);
			break;
		case 0xA1:
			AND_r(c_RegC);
			break;
		case 0xA2:
			AND_r(c_RegD);
			break;
		case 0xA3:
			AND_r(c_RegE);
			break;
		case 0xA4:
			AND_r(c_RegH);
			break;
		case 0xA5:
			AND_r(c_RegL);
		case 0xA6:
			AND_HL();
			break;
		case 0xA7:
			AND_r(c_RegA);
			break;
		case 0xE8:
			AND_n();
			break;
		case 0xB0:
			OR_r(c_RegB);
			break;
		case 0xB1:
			OR_r(c_RegC);
			break;
		case 0xB2:
			OR_r(c_RegD);
			break;
		case 0xB3:
			OR_r(c_RegE);
			break;
		case 0xB4:
			OR_r(c_RegH);
			break;
		case 0xB5:
			OR_r(c_RegL);
			break;
		case 0xB6:
			OR_HL();
			break;
		case 0xB7:
			OR_r(c_RegA);
			break;
		case 0xF6:
			OR_n();
			break;
		case 0xA8:
			XOR_r(c_RegB);
			break;
		case 0xA9:
			XOR_r(c_RegC);
			break;
		case 0xAA:
			XOR_r(c_RegD);
			break;
		case 0xAB:
			XOR_r(c_RegE);
			break;
		case 0xAC:
			XOR_r(c_RegH);
			break;
		case 0xAD:
			XOR_r(c_RegL);
			break;
		case 0xAE:
			XOR_HL();
			break;
		case 0xAF:
			XOR_r(c_RegA);
			break;
		case 0xEE:
			XOR_n();
			break;
		case 0x3F:
			CCF();
			break;
		case 0x37:
			SCF();
			break;
		case 0x27:
			DAA();
			break;
		case 0x2F:
			CPL();
			break;

		default: break; // TODO: Implement error handling.
	}
}

void GBC_CPU::LD_r_reg(const BYTE dest, const BYTE source) {
	storeByteByCode(dest, source);
}

void GBC_CPU::LD_r_n(const BYTE dest) {
	const int immediate = getNextOpcode();
	storeByteByCode(dest, immediate);
}

void GBC_CPU::LD_r_HL(const BYTE dest) {
	const BYTE high = c_H;
	const BYTE low = c_L;

	const WORD address = (high << 8) | low;
	storeImmediateByte(dest, bus_.read8(address));
}

void GBC_CPU::LD_HL_r(const BYTE source) {
	const BYTE high = c_H;
	const BYTE low  = c_L;

	const WORD address = (high << 8) | low;

	bus_.write8(address, getByteByCode(source));

}


void GBC_CPU::LD_HL_n() {
	const BYTE high = c_H;
	const BYTE low  = c_L;

	const WORD address = (high << 8) | low;
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

// ———————— End 8-bit load instructions, begin 16-bit load instructions ————————

void GBC_CPU::LD_rr_nn(const BYTE rr) {
	const BYTE nn_lsb = getNextOpcode();
	const BYTE nn_msb = getNextOpcode();

	const WORD imm = (nn_msb << 8) | nn_lsb;

	storeImmediate(high, nn_lsb);
	storeImmediate(low, nn_msb);
}

void GBC_CPU::LD_nn_SP() {
	const BYTE nn_lsb = getNextOpcode();
	const BYTE nn_msb = getNextOpcode();
	const WORD address = (nn_msb << 8) | nn_lsb;

	bus_.write16(address, bus_.read16(c_StackPointer));
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


