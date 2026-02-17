// GAMEBOYCOLOR.hpp
// Created by Kehinde Adeoso on January 5th, 2026.
// Stores the interface for the Game Boy Color CPU core.

#pragma once

#include <cstdint>
#include <string>
#include "GBC_MMU.h"

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
	explicit CPU(const std::string& bootPath); // aka the initializer/reset.
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
	/**
	 * @brief Stores information from the source register to the destination register.
	 * @param dest Points to the destination register.
	 * @param source Points to the source register.
	 * @post The register pointed to by dest now holds information located at source.
	 */
	void storeByCode(BYTE dest, BYTE source);

	/**
	 * @brief Stores immediate value within the destination register.
	 * @param dest The destination register.
	 * @param imm The immediate value.
	 * @post The destination register now holds the immediate value.
	 */
	void storeImmediate(BYTE dest, BYTE imm);

	/*
	void splitBinary(BYTE binary, BYTE dest_one, BYTE dest_two);
	*/

	/**
	 * @brief Fetches the next opcode
	 * @return The instruction located at the address pointed to by the program counter.
	 * @post The program counter increments by 1.
	 */
	BYTE getNextOpcode();

	// ### Instructions ###

	// 8-bit load instructions
	// NOTE: Could the register load instructions be replaced with
	// a generic function?

	/**
	 * @brief Load to the 8-bit register r, data from the 8-bit register r'.
	 * @param dest The hex representation for the destination register r.
	 * @param source The hex representation for the source register r'.
	 * @post the destination register now holds information from the source register.
	 */
	void LD_r_reg(BYTE dest, BYTE source);

	/**
	 *
	 */
	void LD_r_n(BYTE dest);

	/**
	 * @brief LD r, (HL): Load register (indirect HL).
	 * @param dest The hex representation for the destination register r.
	 * @post The destination register now holds the value held in memory located at the address in HL.
	 */
	void LD_r_HL(BYTE dest);

	/**
	 * @brief Load to the absolute address specified by the 16-bit register HL, data from the 8-bit register r.
	 * @param source The hex representation for the source register r.
	 * @post The memory location pointed to by the address in HL now holds the value within the source register r.
	 */
	void LD_HL_r(BYTE source) const;

	/**
	 * @brief LD (HL), n: Load from immediate data (indirect HL).
	 * @post The memory address pointed to by HL now holds the immediate value n located at PC+1.
	 */
	void LD_HL_n();

	/**
	 * @brief Load to the 8-bit A register, data from the absolute address specified by the 16-bit register BC.
	 * @post Register A now holds the data located in the memory address held by BC.
	 */
	void LD_A_BC();

	// 16-bit load instructions



};