// GAMEBOYCOLOR.hpp
// Created by Kehinde Adeoso on January 5th, 2026.
// Stores the interface for the Game Boy Color CPU core.

#pragma once

#include <cstdint>
#include <string>
#include "GBC_BUS.h"

// Type Aliases
constexpr int c_ZeroFlag      = 7;
constexpr int c_SubtractFlag  = 6;
constexpr int c_HalfCarryFlag = 5;
constexpr int c_CarryFlag     = 4;

class GBC_CPU {
	// Registers


public:
	// Class Methods
	explicit GBC_CPU(const GBC_BUS& bus); // aka the initializer/reset.
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
	GBC_BUS bus_;


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

	BYTE getByCode(BYTE source);

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
	void LD_HL_r(BYTE source);

	/**
	 * @brief LD A, (HL-): Load accumulator (indirect HL, decrement).
	 * @post The accumulator register now holds the value located at the memory address in HL,
	 * and the HL register pair is decremented by 1.
	 */
	void LD_A_HLdec();

	/**
	 * @brief LD (HL-), A: Load from accumulator (indirect HL, decrement).
	 * @post The memory address pointed to by HL now holds the value in the accumulator register,
	 * and the HL register pair is decremented by 1.
	 */
	void LD_HLdec_A();

	/**
	 * @brief LD A, (HL+): Load accumulator (indirect HL, increment).
	 * @post The accumulator register now holds the value located at the memory address in HL,
	 * and the HL register pair is incremented by 1.
	 */
	void LD_A_HLinc();

	/**
	 * @brief LD (HL+), A: Load from accumulator (indirect HL, increment).
	 * @post The memory address pointed to by HL now holds the value in the accumulator register,
	 * and the HL register pair is incremented by 1.
	 */
	void LD_HLinc_A();

	/**
	 * @brief LD (HL), n: Load from immediate data (indirect HL).
	 * @post The memory address pointed to by HL now holds the immediate value n located at PC+1.
	 */
	void LD_HL_n();

	/**
	 * @brief Load to the 8-bit A register, data from the absolute address specified by the 16-bit register BC.
	 * @post The accumulator register now holds the data located in the memory address held by BC.
	 */
	void LD_A_BC();

	/**
	 * @brief Load to the 8-bit A register, data from the absolute address specified by the 16-bit register DE.
	 * @post The accumulator register now holds the data located in the memory address held by DE.
	 */
	void LD_A_DE();

	/**
	 * @brief Load to the absolute address specified by the 16-bit register BC, data from the 8-bit A register.
	 * @post The value stored in the accumulator register is now held in the memory address in BC.
	 */
	void LD_BC_A();

	/**
	 * @brief Load to the absolute address specified by the 16-bit register DE, data from the 8-bit A register.
	 * @post The value stored in the accumulator register is now held in the memory address in DE.
	 */
	void LD_DE_A();

	/**
	 * @brief Load to the 8-bit A register, data from the absolute address specified by the 16-bit operand nn.
	 * @post The value stored in the memory address given as immediate is now stored in the accumulator register.
	 */
	void LD_A_nn();

	/**
	 * @brief Load to the absolute address specified by the 16-bit operand nn, data from the 8-bit A register.
	 * @post The value stored in the accumulator register is now stored in the memory address given as immediate.
	 */
	void LD_nn_A();

	/**
	* @brief Load to the 8-bit A register, data from the address specified by the 8-bit C register.
	* The full 16-bit absolute address is obtained by setting the most significant byte to 0xFF and
	* the least significant byte to the value of C, so the possible range is `0xFF00-0xFFFF`.
	* @post The accumulator register now holds data from a memory address 0xFF + C.
	*/
	void LDH_A_C();

	/**
	* @brief Load to the address specified by the 8-bit C register, data from the 8-bit A register. The full
	* 16-bit absolute address is obtained by setting the most significant byte to 0xFF and the least
	* significant byte to the value of C, so the possible range is 0xFF00-0xFFFF.
	* @post The C register now holds data from a memory address 0xFF + A.
	 */
	void LDH_C_A();

	/**
	* @brief Load to the 8-bit A register, data from the address specified by the 8-bit immediate data n. The
	* full 16-bit absolute address is obtained by setting the most significant byte to 0xFF and the
	* least significant byte to the value of n, so the possible range is 0xFF00-0xFFFF.
	* @post The C register now holds data from a memory address 0xFF + N
	*/
	void LDH_A_n();

	/**
	* @brief Load to the address specified by the 8-bit immediate data n, data from the 8-bit A register. The
	* full 16-bit absolute address is obtained by setting the most significant byte to 0xFF and the
	* least significant byte to the value of n, so the possible range is 0xFF00-0xFFFF.
	* @post The memory address given as immediate now holds data from the accumulator register.
	*/
	void LDH_n_A();

	// 16-bit load instructions

	/**
	 * @brief LD rr, nn: Load 16-bit immediate data into register pair.
	 * @param rr The hex representation for the destination register pair.
	 * @post The register pair rr now holds the 16-bit immediate value nn fetched from PC+1 and PC+2.
	 */
	void LD_rr_nn(BYTE rr);

	/**
	 * @brief LD (nn), SP: Load from stack pointer to immediate address.
	 * @post The two memory addresses beginning at the 16-bit immediate nn now hold the lower and upper bytes of the stack pointer.
	 */
	void LD_nn_SP();

	/**
	 * @brief LD SP, HL: Load stack pointer from HL.
	 * @post The stack pointer now holds the value in the HL register pair.
	 */
	void LD_SP_HL();

	/**
	 * @brief PUSH rr: Push register pair onto stack.
	 * @param rr The hex representation for the source register pair.
	 * @post The value of register pair rr is written to the stack at the address pointed to by SP, and SP is decremented by 2.
	 */
	void PUSH_rr(BYTE rr);

	/**
	 * @brief POP rr: Pop from stack into register pair.
	 * @param rr The hex representation for the destination register pair.
	 * @post The register pair rr now holds the 16-bit value read from the top of the stack, and SP is incremented by 2.
	 */
	void POP_rr(BYTE rr);

	/**
	 * @brief LD HL, SP+e: Load HL from adjusted stack pointer.
	 * @post The HL register pair now holds the value of SP plus the signed 8-bit immediate offset e,
	 * the half-carry and carry flags are set based on the addition, and the zero and subtract flags are reset.
	 */
	void LD_HL_SPe();

};