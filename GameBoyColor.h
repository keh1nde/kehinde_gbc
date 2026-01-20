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

	// Instructions




};