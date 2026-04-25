// GAMEBOYCOLOR.cpp
// Created by Kehinde Adeoso on January 5th, 2026.
// Stores the interface for the Game Boy Color CPU core.


#include "GBC_CPU.h"

#include <ios>
#include <iostream>
#include <numbers>

GBC_CPU::GBC_CPU(IBus& bus)
: c_ProgramCounter(0x0000), c_StackPointer(0xFFFE), c_A(0x11), c_B(0x00), c_C(0x00), c_D(0xFF),
c_E(0x56), c_H(0x00), c_F(0x80), c_L(0x0D), bus_(bus) {}

void GBC_CPU::clearFlags() {
	c_F &= ~c_AllFlagsMask;
}

void GBC_CPU::setFlag(const BYTE bit, const bool on) {
	if (on) c_F |= (1 << bit);
	else c_F &= ~(1 << bit);
}

bool GBC_CPU::getFlag(const BYTE bit) const {
	return ((c_F >> bit) & 1) != 0;
}

BYTE GBC_CPU::getNextOpcode() {
	const BYTE next = bus_.read8(c_ProgramCounter);
	c_ProgramCounter++;

	return next;
}

void GBC_CPU::resetPostBoot() {
	storeImmediateWordRP2(c_PairAF, 0x01B0);
	storeImmediateWord(c_PairBC, 0x0013);
	storeImmediateWord(c_PairDE, 0x00D8);
	storeImmediateWord(c_PairHL, 0x014D);

	/* For the CGB:
	 *storeImmediateWordRP2(c_PairAF, 0x1180);
	storeImmediateWord(c_PairBC, 0x0000);
	storeImmediateWord(c_PairDE, 0xFF56);
	storeImmediateWord(c_PairHL, 0x000D);*/

	c_StackPointer = 0xFFFE;
	c_ProgramCounter = 0x0100;

	c_IME = false;
	c_Halted = false;
}

void GBC_CPU::resetPostBootARegister() {
	storeImmediateWordRP2(c_PairAF, 0x4480);
	storeImmediateWord(c_PairBC, 0x0000);
	storeImmediateWord(c_PairDE, 0xFF56);
	storeImmediateWord(c_PairHL, 0x000D);
	c_ProgramCounter = 0xC7B1;
}

void GBC_CPU::execute() {
	/*std::printf("A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X "
								"SP:%04X PC:%04X PCMEM:%02X,%02X,%02X,%02X\n",
				c_A, c_F, c_B, c_C, c_D, c_E, c_H, c_L,
				c_StackPointer,  // SP — replace with whatever your SP member is named
				c_ProgramCounter,
				bus_.read8(c_ProgramCounter),
				bus_.read8(c_ProgramCounter + 1),
				bus_.read8(c_ProgramCounter + 2),
				bus_.read8(c_ProgramCounter + 3));*/
	const BYTE opcode = getNextOpcode();
	if (opcode == 0xCB) {
		executeCB();
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
			HALT();
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
			PUSH_rr(c_PairAF);
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
			POP_rr(c_PairAF);
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
			SUB_r(c_RegA);
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
		case 0x0C:
			INC_r(c_RegC);
			break;
		case 0x1C:
			INC_r(c_RegE);
			// std::cout << "PC=" << c_ProgramCounter << " Register E value after INC_r(E): 0x" << std::hex << static_cast<int>(c_E) << std::endl;
			// std::cout << "PC=" << c_ProgramCounter << " Z flag after INC_r: " << getFlag(c_ZeroFlag) << std::endl;
			break;
		case 0x2C:
			INC_r(c_RegL);
			break;
		case 0x3C:
			INC_r(c_RegA);
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
			break;
		case 0xA6:
			AND_HL();
			break;
		case 0xA7:
			AND_r(c_RegA);
			break;
		case 0xE6:
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
		case 0x03:
			INC_rr(c_PairBC);
			break;
		case 0x13:
			INC_rr(c_PairDE);
			break;
		case 0x23:
			INC_rr(c_PairHL);
			break;
		case 0x33:
			INC_rr(c_PairSP);
			break;
		case 0x0B:
			DEC_rr(c_PairBC);
			break;
		case 0x1B:
			DEC_rr(c_PairDE);
			break;
		case 0x2B:
			DEC_rr(c_PairHL);
			break;
		case 0x3B:
			DEC_rr(c_PairSP);
			break;
		case 0x09:
			ADD_HL_rr(c_PairBC);
			break;
		case 0x19:
			ADD_HL_rr(c_PairDE);
			break;
		case 0x29:
			ADD_HL_rr(c_PairHL);
			break;
		case 0x39:
			ADD_HL_rr(c_PairSP);
			break;
		case 0xE8:
			ADD_SP_e();
			break;

		// Unprefixed rotate-accumulator ops. Distinct from CB-prefixed RLC A/etc. — Z forced to 0.
		case 0x07:
			RLCA();
			break;
		case 0x0F:
			RRCA();
			break;
		case 0x17:
			RLA();
			break;
		case 0x1F:
			RRA();
			break;

		// Miscellaneous
		case 0x00:
			NOP();
			break;
		case 0x10:
			STOP();
			break;
		case 0xF3:
			DI();
			break;
		case 0xFB:
			EI();
			break;

		// Jumps
		case 0xC3:
			JP_nn();
			break;
		case 0xE9:
			JP_HL();
			break;
		case 0xC2:
			JP_cc_nn(c_CondNZ);
			break;
		case 0xCA:
			JP_cc_nn(c_CondZ);
			break;
		case 0xD2:
			JP_cc_nn(c_CondNC);
			break;
		case 0xDA:
			JP_cc_nn(c_CondC);
			break;
		case 0x18:
			JR_e();
			break;
		case 0x20:
			JR_cc_e(c_CondNZ);
			break;
		case 0x28:
			JR_cc_e(c_CondZ);
			break;
		case 0x30:
			JR_cc_e(c_CondNC);
			break;
		case 0x38:
			JR_cc_e(c_CondC);
			break;

		// Calls / returns
		case 0xCD:
			CALL_nn();
			break;
		case 0xC4:
			CALL_cc_nn(c_CondNZ);
			break;
		case 0xCC:
			CALL_cc_nn(c_CondZ);
			break;
		case 0xD4:
			CALL_cc_nn(c_CondNC);
			break;
		case 0xDC:
			CALL_cc_nn(c_CondC);
			break;
		case 0xC9:
			RET();
			break;
		case 0xC0:
			RET_cc(c_CondNZ);
			break;
		case 0xC8:
			RET_cc(c_CondZ);
			break;
		case 0xD0:
			RET_cc(c_CondNC);
			break;
		case 0xD8:
			RET_cc(c_CondC);
			break;
		case 0xD9:
			RETI();
			break;

		// Restart vectors
		case 0xC7:
			RST(0x00);
			break;
		case 0xCF:
			RST(0x08);
			break;
		case 0xD7:
			RST(0x10);
			break;
		case 0xDF:
			RST(0x18);
			break;
		case 0xE7:
			RST(0x20);
			break;
		case 0xEF:
			RST(0x28);
			break;
		case 0xF7:
			RST(0x30);
			break;
		case 0xFF:
			RST(0x38);
			break;
		// Previous missing opcodes
		case 0x06:
			LD_r_n(c_RegB);
			break;
		case 0x16:
			LD_r_n(c_RegD);
			break;
		case 0x26:
			LD_r_n(c_RegH);
			break;
		case 0x3E:
			LD_r_n(c_RegA);
			break;
		case 0x0E:
			LD_r_n(c_RegC);
			break;
		case 0x1E:
			LD_r_n(c_RegE);
			break;
		case 0x2E:
			LD_r_n(c_RegL);
			break;
		case 0x40:
			LD_r_reg(c_RegB, c_RegB);
			break;
		case 0xD6:
			SUB_n();
			break;
		default:
			throw std::runtime_error("Unimplemented opcode: 0x" +
				(std::stringstream{} << std::hex << static_cast<int>(opcode)).str());
	}

}


void GBC_CPU::executeCB() {
	const BYTE opcode = getNextOpcode();
	const BYTE top = opcode >> 6;          // bits 7:6 — family
	const BYTE mid = (opcode >> 3) & 0x7;  // bits 5:3 — sub-op or bit index
	const BYTE low = opcode & 0x7;         // bits 2:0 — register code r

	switch (top) {
		case 0x0:
			switch (mid) {
			case 0x0: RLC(low);  break;
			case 0x1: RRC(low);  break;
			case 0x2: RL(low);   break;
			case 0x3: RR(low);   break;
			case 0x4: SLA(low);  break;
			case 0x5: SRA(low);  break;
			case 0x6: SWAP(low); break;
			case 0x7: SRL(low);  break;
			default: break; // unreachable; mid is 3 bits
			}
			break;
		case 0x1:
			BIT(mid, low);
			break;
		case 0x2:
			RES(mid, low);
			break;
		case 0x3:
			SET(mid, low);
			break;
		default:
			throw std::runtime_error("Unimplemented opcode: " + std::to_string(opcode));
	}
}

void GBC_CPU::LD_r_reg(const BYTE dest, const BYTE source) {
	storeByteByCode(dest, source);
}

void GBC_CPU::LD_r_n(const BYTE dest) {
	const BYTE immediate = getNextOpcode();
	storeImmediateByte(dest, immediate);
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
	const BYTE low = getNextOpcode();
	const BYTE high = getNextOpcode();
	const WORD immediate = (high << 8) | low;

	c_A = bus_.read8(immediate);
}

void GBC_CPU::LD_nn_A() {
	const BYTE low = getNextOpcode();
	const BYTE high = getNextOpcode();
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
	high = address >> 8;
	low = address & 0xFF;

	c_H = high;
	c_L = low;

}

void GBC_CPU::LD_HLdec_A() {
	BYTE high = c_H;
	BYTE low = c_L;
	WORD address = (high << 8) | low;

	bus_.write8(address, c_A);

	address--;
	high = address >> 8;
	low = address & 0xFF;

	c_H = high;
	c_L = low;
}

void GBC_CPU::LD_A_HLinc() {
	BYTE high = c_H;
	BYTE low = c_L;
	WORD address = (high << 8) | low;

	c_A = bus_.read8(address);

	address++;
	high = address >> 8;
	low = address & 0xFF;

	c_H = high;
	c_L = low;
}

void GBC_CPU::LD_HLinc_A() {
	BYTE high = c_H;
	BYTE low = c_L;
	WORD address = (high << 8) | low;

	bus_.write8(address, c_A);

	address++;
	high = address >> 8;
	low = address & 0xFF;

	c_H = high;
	c_L = low;
}

// ———————— End 8-bit load instructions, begin 16-bit load instructions ————————

void GBC_CPU::LD_rr_nn(const BYTE rr) {
	const BYTE low = getNextOpcode();
	const BYTE high = getNextOpcode();

	const WORD imm = (high << 8) | low;

	storeImmediateWord(rr, imm);
}

void GBC_CPU::LD_nn_SP() {
	const BYTE low = getNextOpcode();
	const BYTE high = getNextOpcode();
	const WORD address = (high << 8) | low;

	bus_.write16(address, c_StackPointer);
}

void GBC_CPU::LD_SP_HL() {
	const BYTE nn_msb = c_H;
	const BYTE nn_lsb = c_L;
	const WORD value = (nn_msb << 8) | nn_lsb;

	c_StackPointer = value;
}

void GBC_CPU::PUSH_rr(const BYTE rr) {
	c_StackPointer -= 2;
	bus_.write16(c_StackPointer, getPairByCode2(rr));
}

void GBC_CPU::POP_rr(const BYTE rr) {
	storeImmediateWordRP2(rr, bus_.read16(c_StackPointer));
	c_StackPointer += 2;
}

void GBC_CPU::LD_HL_SPe() {
	const int8_t e = static_cast<int8_t>(getNextOpcode());
	storeImmediateWord(c_PairHL, c_StackPointer + e); // I think?

	// For the following, e is converted back to unsigned 8-bit due to C++ convention. Works for
	// this operation, but won't work the same way if e is turned to BYTE.
	const bool half_carry_val = ((c_StackPointer & 0xF) + (e & 0xF) & 0x10) != 0;
	const bool carry_val = ((c_StackPointer & 0xFF) + (e & 0xFF) & 0x100) != 0;

	// Clear bits first:
	clearFlags();

	// Then, set:
	setFlag(c_HalfCarryFlag, half_carry_val);
	setFlag(c_CarryFlag, carry_val);
}

// ———————— End 16-bit LOAD instructions, begin 8-bit ADD instructions ————————

void GBC_CPU::ADD_A_r(const BYTE source) {
	/*Set the source val from either memory or the source register.
	const BYTE source_val = (source == c_RegHL) ? bus_.read8((c_H << 8) | c_L) : getByteByCode(source);*/
	const BYTE source_val = getByteByCode(source);
	generalAddInstruction(source_val);
}

void GBC_CPU::ADD_A_HL() {
	// Set the source val from either memory or the source register.
	const BYTE source_val = bus_.read8((c_H << 8) | c_L);
	generalAddInstruction(source_val);
}

void GBC_CPU::ADD_A_n() {
	const BYTE source_val = getNextOpcode();
	generalAddInstruction(source_val);
}

void GBC_CPU::ADC_A_r(const BYTE source) {
	const BYTE source_val = getByteByCode(source);
	generalAdcInstruction(source_val);
}

void GBC_CPU::ADC_A_HL() {
	// Set the source val from either memory or the source register.
	const BYTE source_val = bus_.read8((c_H << 8) | c_L);
	generalAdcInstruction(source_val);
}

void GBC_CPU::ADC_A_n() {
	const BYTE source_val = getNextOpcode();
	generalAdcInstruction(source_val);
}

void GBC_CPU::SUB_r(const BYTE source) {
	const BYTE source_val = getByteByCode(source);
	generalSubInstruction(source_val);
}

void GBC_CPU::SUB_HL() {
	const BYTE source_val = bus_.read8((c_H << 8) | c_L);
	generalSubInstruction(source_val);
}

void GBC_CPU::SUB_n() {
	const BYTE source_val = getNextOpcode();
	generalSubInstruction(source_val);
}

void GBC_CPU::SBC_A_r(const BYTE source) {
	const BYTE source_val = getByteByCode(source);
	generalSbcInstruction(source_val);
}

void GBC_CPU::SBC_A_HL() {
	const BYTE source_val = bus_.read8((c_H << 8) | c_L);
	generalSbcInstruction(source_val);
}

void GBC_CPU::SBC_A_n() {
	const BYTE source_val = getNextOpcode();
	generalSbcInstruction(source_val);
}

void GBC_CPU::CP_r(const BYTE source) {
	const BYTE source_val = getByteByCode(source);
	generalCprInstruction(source_val);
}

void GBC_CPU::CP_HL() {
	const BYTE source_val = bus_.read8((c_H << 8) | c_L);
	generalCprInstruction(source_val);
}

void GBC_CPU::CP_n() {
	const BYTE source_val = getNextOpcode();
	generalCprInstruction(source_val);
}

void GBC_CPU::INC_r(const BYTE dest) {
	const BYTE pre = getByteByCode(dest);
	const BYTE post = pre + 1;
	const bool h = (((pre & 0x0F) + 1) & 0x10) != 0;

	const bool c = getFlag(c_CarryFlag);
	clearFlags();
	setFlag(c_CarryFlag, c);
	setFlag(c_HalfCarryFlag, h);
	setFlag(c_ZeroFlag, post == 0);

	storeImmediateByte(dest, post);
}

void GBC_CPU::INC_HL() {
	const WORD addr = (c_H << 8) | c_L;
	const BYTE pre = bus_.read8(addr);
	const BYTE post = pre + 1;
	const bool h = (((pre & 0x0F) + 1) & 0x10) != 0;

	const bool c = getFlag(c_CarryFlag);
	clearFlags();
	setFlag(c_CarryFlag, c);
	setFlag(c_HalfCarryFlag, h);
	setFlag(c_ZeroFlag, post == 0);

	bus_.write8(addr, post);
}

void GBC_CPU::DEC_r(const BYTE dest) {
	const BYTE pre = getByteByCode(dest);
	const BYTE post = pre - 1;
	const bool h = (pre & 0x0F) == 0;

	const bool c = getFlag(c_CarryFlag);
	clearFlags();
	setFlag(c_CarryFlag, c);
	setFlag(c_HalfCarryFlag, h);
	setFlag(c_ZeroFlag, post == 0);
	setFlag(c_SubtractFlag, true);

	storeImmediateByte(dest, post);
}

void GBC_CPU::DEC_HL() {
	const WORD addr = (c_H << 8) | c_L;
	const BYTE pre = bus_.read8(addr);
	const BYTE post = pre - 1;
	const bool h = (pre & 0x0F) == 0;

	const bool c = getFlag(c_CarryFlag);
	clearFlags();
	setFlag(c_CarryFlag, c);
	setFlag(c_HalfCarryFlag, h);
	setFlag(c_ZeroFlag, post == 0);
	setFlag(c_SubtractFlag, true);

	bus_.write8(addr, post);
}

void GBC_CPU::AND_r(const BYTE source) {
	// Clear all bits
	clearFlags();

	const BYTE val = getByteByCode(source);
	c_A &= val;

	// Set zero flag
	setFlag(c_ZeroFlag, c_A == 0);

	// Set half carry flag
	setFlag(c_HalfCarryFlag, true);
}

void GBC_CPU::AND_HL() {
	// Clear all bits
	clearFlags();

	const BYTE val = bus_.read8((c_H << 8) | c_L);
	c_A &= val;

	setFlag(c_ZeroFlag, c_A == 0);

	// Set half carry flag
	setFlag(c_HalfCarryFlag, true);
}

void GBC_CPU::AND_n() {
	// Clear all bits
	clearFlags();

	const BYTE val = getNextOpcode();
	c_A &= val;

	setFlag(c_ZeroFlag, c_A == 0);

	setFlag(c_HalfCarryFlag, true);
}

void GBC_CPU::OR_r(const BYTE source) {
	// Clear all bits
	clearFlags();

	const BYTE val = getByteByCode(source);
	c_A |= val;

	// Set zero flag
	setFlag(c_ZeroFlag, c_A == 0);
}

void GBC_CPU::OR_HL() {
	// Clear all bits
	clearFlags();

	const BYTE val = bus_.read8((c_H << 8) | c_L);
	c_A |= val;

	// Set zero flag
	setFlag(c_ZeroFlag, c_A == 0);
}

void GBC_CPU::OR_n() {
	// Clear all bits
	clearFlags();

	const BYTE val = getNextOpcode();
	c_A |= val;

	// Set zero flag
	setFlag(c_ZeroFlag, c_A == 0);
}

void GBC_CPU::XOR_r(const BYTE source) {
	// Clear all bits
	clearFlags();

	const BYTE val = getByteByCode(source);
	c_A ^= val;

	// Set zero flag
	setFlag(c_ZeroFlag, c_A == 0);
}

void GBC_CPU::XOR_n() {
	// Clear all bits
	clearFlags();

	const BYTE val = getNextOpcode();
	c_A ^= val;

	// Set zero flag
	setFlag(c_ZeroFlag, c_A == 0);
}

void GBC_CPU::XOR_HL() {
	// Clear all bits
	clearFlags();

	const BYTE val = bus_.read8((c_H << 8) | c_L);
	c_A ^= val;

	// Set zero flag
	setFlag(c_ZeroFlag, c_A == 0);
}

void GBC_CPU::CCF() {
	setFlag(c_SubtractFlag, false);
	setFlag(c_HalfCarryFlag, false);
	setFlag(c_CarryFlag, !getFlag(c_CarryFlag));
}

void GBC_CPU::SCF() {
	setFlag(c_SubtractFlag, false);
	setFlag(c_HalfCarryFlag, false);
	setFlag(c_CarryFlag, true);
}

void GBC_CPU::DAA() {
	if (!getFlag(c_SubtractFlag)) {
		if (getFlag(c_CarryFlag) || c_A > 0x99) {
			c_A += 0x60;
			setFlag(c_CarryFlag, true);
		}

		if (getFlag(c_HalfCarryFlag) || (c_A & 0x0F) > 0x09) {
			c_A += 0x06;
		}
	} else {
		if (getFlag(c_CarryFlag)) {
			c_A -= 0x60;
		}

		if (getFlag(c_HalfCarryFlag)) {
			c_A -= 0x06;
		}
	}

	setFlag(c_ZeroFlag, c_A == 0);
	setFlag(c_HalfCarryFlag, false);
}

void GBC_CPU::CPL() {
	c_F ^=  (1 << c_SubtractFlag) | (1 << c_HalfCarryFlag);

	// Set Subtract and Half-Carry flags
	setFlag(c_SubtractFlag, true);
	setFlag(c_HalfCarryFlag, true);
}
// ———————— End 8-bit ALU instructions, begin 16-bit ALU instructions ————————

void GBC_CPU::INC_rr(const BYTE rr) {
	const WORD value = getPairByCode(rr) + 1;
	storeImmediateWord(rr, value);
}

void GBC_CPU::DEC_rr(const BYTE rr) {
	const WORD value = getPairByCode(rr) - 1;
	storeImmediateWord(rr, value);
}

void GBC_CPU::ADD_HL_rr(const BYTE rr) {
	const WORD source_val = getPairByCode(rr);
	const WORD HL_value = getPairByCode(c_PairHL);
	// Z unchanged; clear N/H/C so the half_carry/carry assignments below can just write into them.
	setFlag(c_SubtractFlag, false);
	setFlag(c_HalfCarryFlag, false);
	setFlag(c_CarryFlag, false);

	// Check half_carry and carry status, set bits if true.
	const bool half_carry_val = (((source_val & 0xFFF) + (HL_value & 0xFFF)) & 0x1000) != 0;
	const bool carry_val = ((static_cast<uint32_t>(source_val) + (HL_value & 0xFFFF)) & 0x10000) != 0;

	setFlag(c_HalfCarryFlag, half_carry_val);
	setFlag(c_CarryFlag, carry_val);

	const WORD sum = source_val + HL_value;
	storeImmediateWord(c_PairHL, sum);
}

void GBC_CPU::ADD_SP_e() {
	const int8_t e = static_cast<int8_t>(getNextOpcode());

	// Clear all bits
	clearFlags();

	// Check half_carry and carry status, set bits if true.
	const bool half_carry_val = ((c_StackPointer & 0xF) + (e & 0xF) & 0x10) != 0;
	const bool carry_val = ((c_StackPointer & 0xFF) + (e & 0xFF) & 0x100) != 0;

	setFlag(c_HalfCarryFlag, half_carry_val);
	setFlag(c_CarryFlag, carry_val);

	c_StackPointer += e;
}



// ———————— End 16-bit ALU instructions, begin Rotate, shift, and bit operation instructions ————————

void GBC_CPU::RLC(const BYTE r) {
	// Clear all bits
	clearFlags();

	BYTE modified_byte = getByteByCode(r);
	const BYTE old_bit7 = (modified_byte >> 7) & 1;

	modified_byte = (modified_byte << 1) | old_bit7;

	setFlag(c_CarryFlag, old_bit7);
	setFlag(c_ZeroFlag, modified_byte == 0);
	storeImmediateByte(r, modified_byte);
}

void GBC_CPU::RRC(const BYTE r) {
	clearFlags();

	BYTE modified_byte = getByteByCode(r);
	const BYTE old_bit0 = modified_byte & 1;

	modified_byte = (modified_byte >> 1) | (old_bit0 << 7);

	setFlag(c_CarryFlag, old_bit0);
	setFlag(c_ZeroFlag, modified_byte == 0);
	storeImmediateByte(r, modified_byte);
}

void GBC_CPU::RL(const BYTE r) {
	// Snapshot old carry before clearing F (mirrors the ADC/SBC `cy` idiom).
	const bool old_carry = getFlag(c_CarryFlag);
	clearFlags();

	BYTE modified_byte = getByteByCode(r);
	const BYTE old_bit7 = (modified_byte >> 7) & 1;

	modified_byte = (modified_byte << 1) | old_carry;

	setFlag(c_CarryFlag, old_bit7);
	setFlag(c_ZeroFlag, modified_byte == 0);
	storeImmediateByte(r, modified_byte);
}

void GBC_CPU::RR(const BYTE r) {
	const bool old_carry = getFlag(c_CarryFlag);
	clearFlags();

	BYTE modified_byte = getByteByCode(r);
	const BYTE old_bit0 = modified_byte & 1;

	modified_byte = (modified_byte >> 1) | (old_carry << 7);

	setFlag(c_CarryFlag, old_bit0);
	setFlag(c_ZeroFlag, modified_byte == 0);
	storeImmediateByte(r, modified_byte);
}

void GBC_CPU::SLA(const BYTE r) {
	clearFlags();

	BYTE modified_byte = getByteByCode(r);
	const BYTE old_bit7 = (modified_byte >> 7) & 1;

	// Shift left; bit 0 falls in as 0 naturally.
	modified_byte <<= 1;

	setFlag(c_CarryFlag, old_bit7);
	setFlag(c_ZeroFlag, modified_byte == 0);
	storeImmediateByte(r, modified_byte);
}

void GBC_CPU::SRA(const BYTE r) {
	clearFlags();

	BYTE modified_byte = getByteByCode(r);
	const BYTE old_bit0 = modified_byte & 1;
	const BYTE preserved_bit7 = modified_byte & 0x80; // arithmetic shift retains sign

	modified_byte = (modified_byte >> 1) | preserved_bit7;

	setFlag(c_CarryFlag, old_bit0);
	setFlag(c_ZeroFlag, modified_byte == 0);
	storeImmediateByte(r, modified_byte);
}

void GBC_CPU::SWAP(const BYTE r) {
	// All four flags reset; C is reset (not preserved) — distinguishing it from the rotates/shifts.
	clearFlags();

	BYTE modified_byte = getByteByCode(r);
	modified_byte = (modified_byte << 4) | (modified_byte >> 4);

	setFlag(c_ZeroFlag, modified_byte == 0);
	storeImmediateByte(r, modified_byte);
}

void GBC_CPU::SRL(const BYTE r) {
	clearFlags();

	BYTE modified_byte = getByteByCode(r);
	const BYTE old_bit0 = modified_byte & 1;

	// Logical shift right: bit 7 becomes 0 naturally.
	modified_byte >>= 1;

	setFlag(c_CarryFlag, old_bit0);
	setFlag(c_ZeroFlag, modified_byte == 0);
	storeImmediateByte(r, modified_byte);
}

void GBC_CPU::BIT(const BYTE b, const BYTE r) {
	// Z = !bit, N = 0, H = 1, C = unchanged. Do not touch the carry flag.
	setFlag(c_SubtractFlag, false);
	setFlag(c_HalfCarryFlag, true);

	const BYTE val = getByteByCode(r);
	setFlag(c_ZeroFlag, ((val >> b) & 1) == 0);
}

void GBC_CPU::RES(const BYTE b, const BYTE r) {
	// No flags affected.
	BYTE val = getByteByCode(r);
	val &= ~(1 << b);
	storeImmediateByte(r, val);
}

void GBC_CPU::SET(const BYTE b, const BYTE r) {
	// No flags affected.
	BYTE val = getByteByCode(r);
	val |= (1 << b);
	storeImmediateByte(r, val);
}

// ———————— End CB-prefixed rotate/shift/bit ops, begin unprefixed rotate-accumulator ops ————————

void GBC_CPU::RLCA() {
	const BYTE old_bit7 = (c_A >> 7) & 1;
	clearFlags();

	c_A = (c_A << 1) | old_bit7;
	setFlag(c_CarryFlag, old_bit7);
	// Z forced to 0 — already cleared above.
}

void GBC_CPU::RRCA() {
	const BYTE old_bit0 = c_A & 1;
	clearFlags();

	c_A = (c_A >> 1) | (old_bit0 << 7);
	setFlag(c_CarryFlag, old_bit0);
}

void GBC_CPU::RLA() {
	const bool old_carry = getFlag(c_CarryFlag);
	const BYTE old_bit7 = (c_A >> 7) & 1;
	clearFlags();

	c_A = (c_A << 1) | old_carry;
	setFlag(c_CarryFlag, old_bit7);
}

void GBC_CPU::RRA() {
	const bool old_carry = getFlag(c_CarryFlag);
	const BYTE old_bit0 = c_A & 1;
	clearFlags();

	c_A = (c_A >> 1) | (old_carry << 7);
	setFlag(c_CarryFlag, old_bit0);
}

// ———————— End rotate-accumulator ops, begin control-flow and miscellaneous instructions ————————

void GBC_CPU::NOP() {
	// Intentionally empty. PC was already advanced past the opcode by the fetch.
}

void GBC_CPU::HALT() {
	c_Halted = true;
}

void GBC_CPU::STOP() {
	// STOP is encoded as 0x10 0x00 — consume the trailing byte.
	getNextOpcode();
	c_Halted = true;
}

void GBC_CPU::DI() {
	c_IME = false;
}

void GBC_CPU::EI() {
	// Real hardware delays the enable by one instruction; not modeled here.
	c_IME = true;
}

void GBC_CPU::JP_nn() {
	const BYTE lsb = getNextOpcode();
	const BYTE msb = getNextOpcode();
	c_ProgramCounter = (msb << 8) | lsb;
}

void GBC_CPU::JP_HL() {
	c_ProgramCounter = (c_H << 8) | c_L;
}

void GBC_CPU::JP_cc_nn(const BYTE cc) {
	// Immediate is always consumed regardless of branch outcome.
	const BYTE lsb = getNextOpcode();
	const BYTE msb = getNextOpcode();
	if (checkCondition(cc)) {
		c_ProgramCounter = (msb << 8) | lsb;
	}
}

void GBC_CPU::JR_e() {
	const int8_t e = static_cast<int8_t>(getNextOpcode());
	c_ProgramCounter += e;
}

void GBC_CPU::JR_cc_e(const BYTE cc) {
	const int8_t e = static_cast<int8_t>(getNextOpcode());
	if (checkCondition(cc)) {
		c_ProgramCounter += e;
	}

	if (c_ProgramCounter == 0x206 || c_ProgramCounter == 0x20F) {
		/*if (e == -9) std::cout << "OUTER JR..." << std::endl;

		std::cout
	<< "JR @PC=" << std::hex << c_ProgramCounter
	<< " JR cc=" << static_cast<int>(cc) << " Z=" << getFlag(c_ZeroFlag)
	<< " taken=" << checkCondition(cc) << " e=" << static_cast<int>(e) << std::endl;*/
	}

}

void GBC_CPU::CALL_nn() {
	const BYTE lsb = getNextOpcode();
	const BYTE msb = getNextOpcode();
	pushWord(c_ProgramCounter); // PC has already advanced past the 3-byte CALL — return address.
	c_ProgramCounter = (msb << 8) | lsb;
}

void GBC_CPU::CALL_cc_nn(const BYTE cc) {
	const BYTE lsb = getNextOpcode();
	const BYTE msb = getNextOpcode();
	if (checkCondition(cc)) {
		pushWord(c_ProgramCounter);
		c_ProgramCounter = (msb << 8) | lsb;
	}
}

void GBC_CPU::RET() {
	c_ProgramCounter = popWord();
}

void GBC_CPU::RET_cc(const BYTE cc) {
	if (checkCondition(cc)) {
		c_ProgramCounter = popWord();
	}
}

void GBC_CPU::RETI() {
	c_ProgramCounter = popWord();
	c_IME = true;
}

void GBC_CPU::RST(const BYTE n) {
	pushWord(c_ProgramCounter);
	c_ProgramCounter = n;
}

// ———————— End Rotate, shift, and bit operation instructions, begin helper functions ————————

void GBC_CPU::storeByteByCode(const BYTE dest, const BYTE source) {
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
			value = bus_.read8((c_H << 8) | c_L);
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
			bus_.write8((c_H << 8) | c_L, value);
			break;
		case 7:
			c_A = value;
			break;

		default: break;
	}
}

/*void GBC_CPU::storeWordByCode(const WORD dest, const WORD source) {
	BYTE value;
	BYTE val_msb;
	BYTE val_lsb;
	switch (source) {
		case 0:
			val_msb = c_B;
			val_lsb = c_C;
			value = (val_msb << 8) | val_lsb;
			break;
		case 1:
			val_msb = c_D;
			val_lsb = c_E;
			value = (val_msb << 8) | val_lsb;
			break;
		case 2:
			val_msb = c_H;
			val_lsb = c_L;
			value = (val_msb << 8) | val_lsb;
			break;
		case 3:
			value = c_StackPointer;
			break;
		default:
			return;
	}
	switch (dest) {
		case 0:
			storeByteByCode(c_B, value & 0xFF);
			storeByteByCode(c_C, value >> 8);
			break;
		case 1:
			storeByteByCode(c_D, value & 0xFF);
			storeByteByCode(c_E, value >> 8);
			break;
		case 2:
			storeByteByCode(c_H, value & 0xFF);
			storeByteByCode(c_L, value >> 8);
			break;
			case 3:
			c_StackPointer = value;
			break;
		default:
			return;
	}
}*/


// 0b10 000 110 4 + 2 + 0 = 6
void GBC_CPU::storeImmediateByte(const BYTE dest, const BYTE imm) {
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
		case 6:
			bus_.write8((c_H << 8) | c_L, imm);
			break;
		case 7:
			c_A = imm;
			break;

		default: break;
	}
}

void GBC_CPU::storeImmediateWord(const BYTE dest, const WORD imm) {
	const BYTE imm_lsb = imm & 0xFF;
	const BYTE imm_msb = imm >> 8;
	switch (dest) {
		case 0:
			c_B = imm_msb;
			c_C = imm_lsb;
			break;
		case 1:
			c_D = imm_msb;
			c_E = imm_lsb;
			break;
		case 2:
			c_H = imm_msb;
			c_L = imm_lsb;
			break;
		case 3:
			c_StackPointer = imm;
			break;
		default:
			return;
	}
}

void GBC_CPU::storeImmediateWordRP2(const BYTE dest, const WORD imm) {
	const BYTE imm_lsb = imm & 0xFF;
	const BYTE imm_msb = imm >> 8;
	switch (dest) {
		case 0:
			c_B = imm_msb;
			c_C = imm_lsb;
			break;
		case 1:
			c_D = imm_msb;
			c_E = imm_lsb;
			break;
		case 2:
			c_H = imm_msb;
			c_L = imm_lsb;
			break;
		case 3:
			// AF write. Low nibble of F must stay zero — real hardware cannot store there.
			c_A = imm_msb;
			c_F = imm_lsb & 0xF0;
			break;
		default:
			return;
	}
}

WORD GBC_CPU::getPairByCode(const BYTE code) const {
	switch (code) {
		case 0: return (c_B << 8) | c_C;
		case 1: return (c_D << 8) | c_E;
		case 2: return (c_H << 8) | c_L;
		case 3: return c_StackPointer;
		default: return 0;
	}
}

WORD GBC_CPU::getPairByCode2(const BYTE code) const {
	switch (code) {
		case 0: return (c_B << 8) | c_C;
		case 1: return (c_D << 8) | c_E;
		case 2: return (c_H << 8) | c_L;
		case 3: return (c_A << 8) | c_F;
		default: return 0;
	}
}

BYTE GBC_CPU::getByteByCode(const BYTE source) {
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
			return bus_.read8((c_H << 8) | c_L);
		case 7:
			return c_A;
		default: // Throw an error
			throw;
	}
}

void GBC_CPU::generalAddInstruction(const BYTE source_val) {
	// Clear all bits
	clearFlags();

	// Check half_carry and carry status, set bits if true.
	const bool half_carry_val = ((c_A & 0xF) + (source_val & 0xF) & 0x10) != 0;
	const bool carry_val = ((c_A & 0xFF) + (source_val & 0xFF) & 0x100) != 0;

	setFlag(c_HalfCarryFlag, half_carry_val);
	setFlag(c_CarryFlag, carry_val);

	// Compute the sum
	c_A = c_A + source_val;

	// Check if the result is zero. If so, set.
	setFlag(c_ZeroFlag, c_A == 0);
}

void GBC_CPU::generalAdcInstruction(const BYTE source_val) {
	// Save the carry bit before clearing.
	const BYTE cy = getFlag(c_CarryFlag);

	// Clear all bits
	clearFlags();

	// Check half_carry and carry status, set bits if true.
	const bool half_carry_val = ((c_A & 0xF) + (source_val & 0xF) + cy & 0x10) != 0;
	const bool carry_val = ((c_A & 0xFF) + (source_val & 0xFF)  + cy & 0x100) != 0;

	setFlag(c_HalfCarryFlag, half_carry_val);
	setFlag(c_CarryFlag, carry_val);

	// Compute the sum with carry.
	c_A = c_A + source_val + cy;

	// Check if the result is zero. If so, set.
	setFlag(c_ZeroFlag, c_A == 0);
}

void GBC_CPU::generalSubInstruction(const BYTE source_val) {
	// Clear all bits
	clearFlags();

	// Check half_carry and carry status, set bits if true.
	const bool half_carry_val = ((c_A & 0xF) - (source_val & 0xF) & 0x10) != 0;
	const bool carry_val = ((c_A & 0xFF) - (source_val & 0xFF) & 0x100) != 0;

	setFlag(c_HalfCarryFlag, half_carry_val);
	setFlag(c_CarryFlag, carry_val);

	// Compute the difference.
	c_A = c_A - source_val;

	// Check if the result is zero. If so, set.
	setFlag(c_ZeroFlag, c_A == 0);

	// Set SubtractFlag to 1
	setFlag(c_SubtractFlag, true);
}

void GBC_CPU::generalSbcInstruction(const BYTE source_val) {
	const BYTE cy = getFlag(c_CarryFlag);
	// Clear all bits
	clearFlags();

	// Check half_carry and carry status, set bits if true.
	const bool half_carry_val = ((c_A & 0xF) - (source_val & 0xF) - cy & 0x10) != 0;
	const bool carry_val = ((c_A & 0xFF) - (source_val & 0xFF)  - cy & 0x100) != 0;

	setFlag(c_HalfCarryFlag, half_carry_val);
	setFlag(c_CarryFlag, carry_val);

	// Complete the sum.
	c_A = c_A - source_val - cy;

	// Check if the result is zero. If so, set.
	setFlag(c_ZeroFlag, c_A == 0);

	// Set SubtractFlag to 1
	setFlag(c_SubtractFlag, true);
}

void GBC_CPU::generalCprInstruction(const BYTE source_val) {
	// Clear all bits
	clearFlags();

	// Check half_carry and carry status, set bits if true.
	const bool half_carry_val = ((c_A & 0xF) - (source_val & 0xF) & 0x10) != 0;
	const bool carry_val = ((c_A & 0xFF) - (source_val & 0xFF) & 0x100) != 0;

	setFlag(c_HalfCarryFlag, half_carry_val);
	setFlag(c_CarryFlag, carry_val);

	// Compute difference and check if the result is zero. If so, set.
	setFlag(c_ZeroFlag, (c_A - source_val) == 0);

	// Set SubtractFlag to 1
	setFlag(c_SubtractFlag, true);
}

// ### CB-prefixed dispatch ###
// The CB opcode space decomposes cleanly by the top two bits of the opcode:
//   top=00 → 8-way op selected by bits 5:3 (RLC/RRC/RL/RR/SLA/SRA/SWAP/SRL), operand = bits 2:0 (r)
//   top=01 → BIT b, r      (b = bits 5:3, r = bits 2:0)
//   top=10 → RES b, r
//   top=11 → SET b, r
// So rather than enumerating 256 cases we decode fields and dispatch.

void GBC_CPU::pushWord(const WORD val) {
	c_StackPointer -= 2; // Changed independently for testing, was 2 previously.
	bus_.write16(c_StackPointer, val);
}

WORD GBC_CPU::popWord() {
	const WORD value = bus_.read16(c_StackPointer);
	c_StackPointer += 2;
	return value;
}

bool GBC_CPU::checkCondition(const BYTE cc) const {
	switch (cc) {
		case c_CondNZ: return !getFlag(c_ZeroFlag);
		case c_CondZ:  return  getFlag(c_ZeroFlag);
		case c_CondNC: return !getFlag(c_CarryFlag);
		case c_CondC:  return  getFlag(c_CarryFlag);
		default: return false;
	}
}

