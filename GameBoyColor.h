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


public:
	// Class Methods
	explicit CPU(const std::string& bootPath, const std::string& cartPath); // aka the initializer/reset.
	void execute();


private:
	WORD c_ProgramCounter;
	WORD c_StackPointer;

	BYTE c_A;
	BYTE c_B;
	BYTE c_C;
	BYTE c_D;
	BYTE c_E;
	BYTE c_H;
	BYTE c_F;
	BYTE c_L;

	// RAM
	BYTE c_WorkRAM[0x4000];
	// Helper Functions
	void storeByCode(BYTE dest, BYTE source);
	void storeImmediate(BYTE dest, BYTE imm);

	BYTE getNextOpcode();

	// ### Instructions ###

	// 8-bit load instructions
	// NOTE: Could the register load instructions be replaced with
	// a generic function?

	void LD_r_reg(BYTE dest, BYTE source);

	// 16-bit load instructions



};