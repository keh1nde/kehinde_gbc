// GAMEBOYCOLOR.hpp
// Created by Kehinde Adeoso on January 5th, 2026.
// Stores the interface for the Game Boy Color CPU core.

#pragma once

#include <cstdint>
#include <string>

// Type Aliases
using BYTE = uint8_t;
using WORD = uint16_t;

constexpr int c_ZeroFlag      = 7;
constexpr int c_SubtractFlag  = 6;
constexpr int c_HalfCarryFlag = 5;
constexpr int c_CarryFlag     = 4;

class CPU {
	// Registers
	WORD c_AF; // Accumulator and Flags.
	WORD c_BC;
	WORD c_DE;
	WORD c_HL;
	WORD c_ProgramCounter;
	WORD c_StackPointer;

	// Class Methods
	explicit CPU(const std::string& cart); // aka the initializer/reset.
	WORD getNextOpcode();
	void execute();

	// ### Instructions ###

	// 8-bit load instructions
	// NOTE: Could the register load instructions be replaced with
	// a generic function?

	void LD_r_reg(const WORD& opcode);
	void LD_r_imm(const WORD& opcode);
	void LD_r_HL(const WORD& opcode);
	void LD_HL_reg(const WORD& opcode);
	void LD_HL_imm(const WORD& opcode);
	void LD_A_BC(const WORD& opcode);
	void LD_A_DE(const WORD& opcode);
	void LD_BC_A(const WORD& opcode);
	void LD_A_DIR(const WORD& opcode);
	void LD_DIR_A(const WORD& opcode);
	void LDH_A_C(const WORD& opcode);
	void LDH_C_A(const WORD& opcode);
	void LDH_A_imm(const WORD& opcode);
	void LDH_imm_A(const WORD& opcode);
	void LD_A_HLM(const WORD& opcode);
	void LD_HLM_A(const WORD& opcode);
	void LD_A_HLP(const WORD& opcode);
	void LD_HLP_A(const WORD& opcode);

	// 16-bit load instructions



};