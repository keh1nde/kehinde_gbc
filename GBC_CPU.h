// GAMEBOYCOLOR.hpp
// Created by Kehinde Adeoso on January 5th, 2026.
// Stores the interface for the Game Boy Color CPU core.

#pragma once

#include <cstdint>
#include <string>
#include "GBC_BUS.h"


/*
 * For future reference:
 * Reading a bit: (c_F >> c_AnyFlag) & 1
 * Setting a bit: c_F |= (1 << c_AnyFlag)
 * Clearing a bit: c_F &= ~(1 << c_AnyFlag)
 * Assign computed truth value to bit: (c_F &= ~mask; if (cond) c_F |= mask;)
 */
// Type Aliases

constexpr int c_ZeroFlag      = 7;
constexpr int c_SubtractFlag  = 6;
constexpr int c_HalfCarryFlag = 5;
constexpr int c_CarryFlag     = 4;

constexpr int c_PairBC = 0;
constexpr int c_PairDE = 1;
constexpr int c_PairHL = 2;
constexpr int c_PairSP = 3;
// rp2 table: code 3 means AF instead of SP. Numerically collides with c_PairSP —
// callers must route through the RP2-flavored helpers (getPairByCode2 / storeImmediateWordRP2).
constexpr int c_PairAF = 3;

// Condition code table (cc, 2 bits): 0=NZ, 1=Z, 2=NC, 3=C. Used by JP cc/JR cc/CALL cc/RET cc.
constexpr int c_CondNZ = 0;
constexpr int c_CondZ  = 1;
constexpr int c_CondNC = 2;
constexpr int c_CondC  = 3;

constexpr int c_RegB = 0;
constexpr int c_RegC = 1;
constexpr int c_RegD = 2;
constexpr int c_RegE = 3;
constexpr int c_RegH = 4;
constexpr int c_RegL = 5;
constexpr int c_RegHL = 6;
constexpr int c_RegA = 7;

class GBC_CPU {
public:
	// Class Methods
	explicit GBC_CPU(const GBC_BUS& bus); // aka the initializer/reset.
	void execute();
	void executeCB();


private:
	WORD c_ProgramCounter;
	WORD c_StackPointer;

	// Registers
	BYTE c_A;
	BYTE c_B;
	BYTE c_C;
	BYTE c_D;
	BYTE c_E;
	BYTE c_H;
	BYTE c_F;
	BYTE c_L;

	// Interrupt master enable. DI clears it, EI sets it (delayed-by-one semantics not modeled yet),
	// RETI sets it. Not yet consulted by an interrupt-servicing loop — the bit is just maintained.
	bool c_IME = false;

	// HALT latch. HALT sets this; the fetch loop is expected to skip dispatch and idle the CPU
	// until an interrupt arrives. STOP also flips it on for now (full STOP semantics — LCD off,
	// joypad wake — are deferred).
	bool c_Halted = false;

	// RAM
	GBC_BUS bus_;


	// Helper Functions
	/**
	 * @brief Stores information from the source register to the destination register.
	 * @param dest Points to the destination register.
	 * @param source Points to the source register.
	 * @post The register pointed to by dest now holds information located at source.
	 */
	void storeByteByCode(BYTE dest, BYTE source);

	/**
	 * @brief Stores information from the source register pair to the destination register pair.
	 * @param dest Points to the destination register pair.
	 * @param source Points to the source register pair.
	 * @post The register pair pointed to by dest now holds information located at source.
	 */
	// void storeWordByCode(WORD dest, WORD source);

	/**
	 * @brief Stores immediate value within the destination register.
	 * @param dest The destination register.
	 * @param imm The immediate value.
	 * @post The destination register now holds the immediate value.
	 */
	void storeImmediateByte(BYTE dest, BYTE imm);

	/**
	 * @brief Stores a 16-bit immediate value within the destination register pair.
	 * @param dest The destination register pair.
	 * @param imm The 16-bit immediate value.
	 * @post The destination register pair now holds the immediate value.
	 */
	void storeImmediateWord(BYTE dest, WORD imm);

	/**
	 * @brief rp2-flavored variant of storeImmediateWord: code 3 routes to AF instead of SP.
	 * @param dest The destination register pair (rp2 table).
	 * @param imm The 16-bit immediate value.
	 * @post The destination register pair now holds the immediate value. On AF writes the low nibble of F is masked to zero.
	 */
	void storeImmediateWordRP2(BYTE dest, WORD imm);

	/**
	 * @brief Retrieves the value held in the register specified by the given code.
	 * @param source The hex representation for the source register.
	 * @return The value stored within the source register.
	 */
	BYTE getByteByCode(BYTE source);

	/**
	 * @brief Retrieves the combined 16-bit value of the register pair specified by the given code (rp table: code 3 = SP).
	 * @param code The 2-bit pair code.
	 * @return The value stored within the register pair.
	 */
	WORD getPairByCode(BYTE code) const;

	/**
	 * @brief rp2-flavored variant of getPairByCode: code 3 returns AF instead of SP.
	 * @param code The 2-bit pair code (rp2 table).
	 * @return The value stored within the register pair.
	 */
	WORD getPairByCode2(BYTE code) const;

	/**
	 * @brief Shared body for ADD A, * instructions: adds source_val into the accumulator and updates flags.
	 * @param source_val The operand (register, memory, or immediate) to add to A.
	 * @post A holds A + source_val. Z is set if the result is zero, N is reset, H is set on carry from bit 3, and C is set on carry from bit 7.
	 */
	void generalAddInstruction(BYTE source_val);

	/**
	 * @brief Shared body for ADC A, * instructions: adds source_val plus the current carry flag into the accumulator and updates flags.
	 * @param source_val The operand (register, memory, or immediate) to add to A along with the carry-in.
	 * @post A holds A + source_val + carry-in. Z is set if the result is zero, N is reset, H is set on carry from bit 3 (including the carry-in), and C is set on carry from bit 7 (including the carry-in).
	 */
	void generalAdcInstruction(BYTE source_val);

	/**
	 * @brief Shared body for SUB * instructions: subtracts source_val from the accumulator and updates flags.
	 * @param source_val The operand (register, memory, or immediate) to subtract from A.
	 * @post A holds A - source_val. Z is set if the result is zero, N is set, H is set on borrow from bit 4, and C is set on borrow from bit 8.
	 */
	void generalSubInstruction(BYTE source_val);

	/**
	 * @brief Shared body for SBC A, * instructions: subtracts source_val plus the current carry flag from the accumulator and updates flags.
	 * @param source_val The operand (register, memory, or immediate) to subtract from A along with the carry-in.
	 * @post A holds A - source_val - carry-in. Z is set if the result is zero, N is set, H is set on borrow from bit 4 (including the carry-in), and C is set on borrow from bit 8 (including the carry-in).
	 */
	void generalSbcInstruction(BYTE source_val);

	/**
	 * @brief Shared body for CP * instructions: performs A - source_val purely to update flags, discarding the result.
	 * @param source_val The operand (register, memory, or immediate) to compare against A.
	 * @post A is unchanged. Z is set if A equals source_val, N is set, H is set on borrow from bit 4, and C is set on borrow from bit 8.
	 */
	void generalCprInstruction(BYTE source_val);

	/**
	 * @brief Shared body for INC * instructions: increments source_val by 1 and updates flags.
	 * @param source_val The current value to increment; callers are responsible for writing the returned result back to the register or memory location.
	 * @post Z is set if the incremented value is zero, N is reset, H is set on carry from bit 3, and C is unchanged.
	 */
	void generalIncInstruction(BYTE source_val);


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
	 * @brief LD r, n: Load to the 8-bit register r, immediate data n.
	 * @param dest The hex representation for the destination register r.
	 * @post The destination register now holds the 8-bit immediate value n located at PC+1.
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

	// 8-bit arithmetic and logical instructions

	/**
	 * @brief ADD A, r: Add to the 8-bit A register, data from the 8-bit register r.
	 * @param source The hex representation for the source register r.
	 * @post The accumulator register now holds the sum of its previous value and the value in the source register.
	 * The zero flag is set if the result is zero, the subtract flag is reset, the half-carry flag is set on carry from bit 3,
	 * and the carry flag is set on carry from bit 7.
	 */
	void ADD_A_r(BYTE source);

	/**
	 * @brief ADD A, n: Add to the 8-bit A register, 8-bit immediate data n.
	 * @post The accumulator register now holds the sum of its previous value and the immediate value n located at PC+1.
	 * The zero flag is set if the result is zero, the subtract flag is reset, the half-carry flag is set on carry from bit 3,
	 * and the carry flag is set on carry from bit 7.
	 */
	void ADD_A_n();

	/**
	 * @brief ADD A, (HL): Add to the 8-bit A register, data from the absolute address specified by the 16-bit register HL.
	 * @post The accumulator register now holds the sum of its previous value and the value held in memory located at the address in HL.
	 * The zero flag is set if the result is zero, the subtract flag is reset, the half-carry flag is set on carry from bit 3,
	 * and the carry flag is set on carry from bit 7.
	 */
	void ADD_A_HL();

	/**
	 * @brief ADC A, r: Add to the 8-bit A register, data from the 8-bit register r and the value of the carry flag.
	 * @param source The hex representation for the source register r.
	 * @post The accumulator register now holds the sum of its previous value, the value in the source register, and the carry flag.
	 * The zero flag is set if the result is zero, the subtract flag is reset, the half-carry flag is set on carry from bit 3,
	 * and the carry flag is set on carry from bit 7.
	 */
	void ADC_A_r(BYTE source);

	/**
	 * @brief ADC A, n: Add to the 8-bit A register, 8-bit immediate data n and the value of the carry flag.
	 * @post The accumulator register now holds the sum of its previous value, the immediate value n located at PC+1, and the carry flag.
	 * The zero flag is set if the result is zero, the subtract flag is reset, the half-carry flag is set on carry from bit 3,
	 * and the carry flag is set on carry from bit 7.
	 */
	void ADC_A_n();

	/**
	 * @brief ADC A, (HL): Add to the 8-bit A register, data from the absolute address specified by the 16-bit register HL and the carry flag.
	 * @post The accumulator register now holds the sum of its previous value, the value held in memory at the address in HL, and the carry flag.
	 * The zero flag is set if the result is zero, the subtract flag is reset, the half-carry flag is set on carry from bit 3,
	 * and the carry flag is set on carry from bit 7.
	 */
	void ADC_A_HL();

	/**
	 * @brief SUB r: Subtract from the 8-bit A register, data from the 8-bit register r.
	 * @param source The hex representation for the source register r.
	 * @post The accumulator register now holds the difference of its previous value and the value in the source register.
	 * The zero flag is set if the result is zero, the subtract flag is set, the half-carry flag is set on borrow from bit 4,
	 * and the carry flag is set on borrow from bit 8.
	 */
	void SUB_r(BYTE source);

	/**
	 * @brief SUB n: Subtract from the 8-bit A register, 8-bit immediate data n.
	 * @post The accumulator register now holds the difference of its previous value and the immediate value n located at PC+1.
	 * The zero flag is set if the result is zero, the subtract flag is set, the half-carry flag is set on borrow from bit 4,
	 * and the carry flag is set on borrow from bit 8.
	 */
	void SUB_n();

	/**
	 * @brief SUB (HL): Subtract from the 8-bit A register, data from the absolute address specified by the 16-bit register HL.
	 * @post The accumulator register now holds the difference of its previous value and the value held in memory at the address in HL.
	 * The zero flag is set if the result is zero, the subtract flag is set, the half-carry flag is set on borrow from bit 4,
	 * and the carry flag is set on borrow from bit 8.
	 */
	void SUB_HL();

	/**
	 * @brief SBC A, r: Subtract from the 8-bit A register, data from the 8-bit register r and the value of the carry flag.
	 * @param source The hex representation for the source register r.
	 * @post The accumulator register now holds the difference of its previous value and the sum of the source register and the carry flag.
	 * The zero flag is set if the result is zero, the subtract flag is set, the half-carry flag is set on borrow from bit 4,
	 * and the carry flag is set on borrow from bit 8.
	 */
	void SBC_A_r(BYTE source);

	/**
	 * @brief SBC A, n: Subtract from the 8-bit A register, 8-bit immediate data n and the value of the carry flag.
	 * @post The accumulator register now holds the difference of its previous value and the sum of the immediate value n located at PC+1 and the carry flag.
	 * The zero flag is set if the result is zero, the subtract flag is set, the half-carry flag is set on borrow from bit 4,
	 * and the carry flag is set on borrow from bit 8.
	 */
	void SBC_A_n();

	/**
	 * @brief SBC A, (HL): Subtract from the 8-bit A register, data from the absolute address specified by the 16-bit register HL and the carry flag.
	 * @post The accumulator register now holds the difference of its previous value and the sum of the value held in memory at the address in HL and the carry flag.
	 * The zero flag is set if the result is zero, the subtract flag is set, the half-carry flag is set on borrow from bit 4,
	 * and the carry flag is set on borrow from bit 8.
	 */
	void SBC_A_HL();

	/**
	 * @brief AND r: Bitwise AND between the 8-bit A register and the 8-bit register r.
	 * @param source The hex representation for the source register r.
	 * @post The accumulator register now holds the result of a bitwise AND of its previous value and the value in the source register.
	 * The zero flag is set if the result is zero, the subtract flag is reset, the half-carry flag is set, and the carry flag is reset.
	 */
	void AND_r(BYTE source);

	/**
	 * @brief AND n: Bitwise AND between the 8-bit A register and 8-bit immediate data n.
	 * @post The accumulator register now holds the result of a bitwise AND of its previous value and the immediate value n located at PC+1.
	 * The zero flag is set if the result is zero, the subtract flag is reset, the half-carry flag is set, and the carry flag is reset.
	 */
	void AND_n();

	/**
	 * @brief AND (HL): Bitwise AND between the 8-bit A register and data from the absolute address specified by the 16-bit register HL.
	 * @post The accumulator register now holds the result of a bitwise AND of its previous value and the value held in memory at the address in HL.
	 * The zero flag is set if the result is zero, the subtract flag is reset, the half-carry flag is set, and the carry flag is reset.
	 */
	void AND_HL();

	/**
	 * @brief XOR r: Bitwise XOR between the 8-bit A register and the 8-bit register r.
	 * @param source The hex representation for the source register r.
	 * @post The accumulator register now holds the result of a bitwise XOR of its previous value and the value in the source register.
	 * The zero flag is set if the result is zero, and the subtract, half-carry, and carry flags are reset.
	 */
	void XOR_r(BYTE source);

	/**
	 * @brief XOR n: Bitwise XOR between the 8-bit A register and 8-bit immediate data n.
	 * @post The accumulator register now holds the result of a bitwise XOR of its previous value and the immediate value n located at PC+1.
	 * The zero flag is set if the result is zero, and the subtract, half-carry, and carry flags are reset.
	 */
	void XOR_n();

	/**
	 * @brief XOR (HL): Bitwise XOR between the 8-bit A register and data from the absolute address specified by the 16-bit register HL.
	 * @post The accumulator register now holds the result of a bitwise XOR of its previous value and the value held in memory at the address in HL.
	 * The zero flag is set if the result is zero, and the subtract, half-carry, and carry flags are reset.
	 */
	void XOR_HL();

	/**
	 * @brief OR r: Bitwise OR between the 8-bit A register and the 8-bit register r.
	 * @param source The hex representation for the source register r.
	 * @post The accumulator register now holds the result of a bitwise OR of its previous value and the value in the source register.
	 * The zero flag is set if the result is zero, and the subtract, half-carry, and carry flags are reset.
	 */
	void OR_r(BYTE source);

	/**
	 * @brief OR n: Bitwise OR between the 8-bit A register and 8-bit immediate data n.
	 * @post The accumulator register now holds the result of a bitwise OR of its previous value and the immediate value n located at PC+1.
	 * The zero flag is set if the result is zero, and the subtract, half-carry, and carry flags are reset.
	 */
	void OR_n();

	/**
	 * @brief OR (HL): Bitwise OR between the 8-bit A register and data from the absolute address specified by the 16-bit register HL.
	 * @post The accumulator register now holds the result of a bitwise OR of its previous value and the value held in memory at the address in HL.
	 * The zero flag is set if the result is zero, and the subtract, half-carry, and carry flags are reset.
	 */
	void OR_HL();

	/**
	 * @brief CP r: Compare the 8-bit A register with the 8-bit register r by subtraction, discarding the result.
	 * @param source The hex representation for the source register r.
	 * @post Flags are set as if the value in the source register had been subtracted from A, but the accumulator register is unchanged.
	 * The zero flag is set if A equals the source register, the subtract flag is set, the half-carry flag is set on borrow from bit 4,
	 * and the carry flag is set on borrow from bit 8.
	 */
	void CP_r(BYTE source);

	/**
	 * @brief CP n: Compare the 8-bit A register with 8-bit immediate data n by subtraction, discarding the result.
	 * @post Flags are set as if the immediate value n located at PC+1 had been subtracted from A, but the accumulator register is unchanged.
	 * The zero flag is set if A equals n, the subtract flag is set, the half-carry flag is set on borrow from bit 4,
	 * and the carry flag is set on borrow from bit 8.
	 */
	void CP_n();

	/**
	 * @brief CP (HL): Compare the 8-bit A register with data from the absolute address specified by the 16-bit register HL by subtraction, discarding the result.
	 * @post Flags are set as if the value held in memory at the address in HL had been subtracted from A, but the accumulator register is unchanged.
	 * The zero flag is set if the values are equal, the subtract flag is set, the half-carry flag is set on borrow from bit 4,
	 * and the carry flag is set on borrow from bit 8.
	 */
	void CP_HL();

	/**
	 * @brief INC r: Increment the 8-bit register r by 1.
	 * @param dest The hex representation for the destination register r.
	 * @post The destination register now holds its previous value plus 1.
	 * The zero flag is set if the result is zero, the subtract flag is reset, the half-carry flag is set on carry from bit 3,
	 * and the carry flag is unchanged.
	 */
	void INC_r(BYTE dest);

	/**
	 * @brief INC (HL): Increment the value held at the absolute address specified by the 16-bit register HL by 1.
	 * @post The memory location pointed to by the address in HL now holds its previous value plus 1.
	 * The zero flag is set if the result is zero, the subtract flag is reset, the half-carry flag is set on carry from bit 3,
	 * and the carry flag is unchanged.
	 */
	void INC_HL();

	/**
	 * @brief DEC r: Decrement the 8-bit register r by 1.
	 * @param dest The hex representation for the destination register r.
	 * @post The destination register now holds its previous value minus 1.
	 * The zero flag is set if the result is zero, the subtract flag is set, the half-carry flag is set on borrow from bit 4,
	 * and the carry flag is unchanged.
	 */
	void DEC_r(BYTE dest);

	/**
	 * @brief DEC (HL): Decrement the value held at the absolute address specified by the 16-bit register HL by 1.
	 * @post The memory location pointed to by the address in HL now holds its previous value minus 1.
	 * The zero flag is set if the result is zero, the subtract flag is set, the half-carry flag is set on borrow from bit 4,
	 * and the carry flag is unchanged.
	 */
	void DEC_HL();

	/**
	 * @brief DAA: Decimal adjust the accumulator after an addition or subtraction between two BCD-encoded values.
	 * @post The accumulator register is adjusted so that the result of the preceding addition or subtraction is a valid BCD value.
	 * The zero flag is set if the result is zero, the half-carry flag is reset, the carry flag is set if the adjustment produced a carry,
	 * and the subtract flag is unchanged.
	 */
	void DAA();

	/**
	 * @brief CPL: Complement the 8-bit A register (bitwise NOT).
	 * @post The accumulator register now holds the bitwise complement of its previous value.
	 * The subtract and half-carry flags are set; the zero and carry flags are unchanged.
	 */
	void CPL();

	/**
	 * @brief SCF: Set the carry flag.
	 * @post The carry flag is set; the subtract and half-carry flags are reset; the zero flag is unchanged.
	 */
	void SCF();

	/**
	 * @brief CCF: Complement (toggle) the carry flag.
	 * @post The carry flag is toggled; the subtract and half-carry flags are reset; the zero flag is unchanged.
	 */
	void CCF();

	// 16-bit arithmetic instructions

	/**
	 * @brief ADD HL, rr: Add to the 16-bit HL register pair, data from the 16-bit register pair rr (rp table: code 3 = SP).
	 * @param rr The 2-bit pair code for the source register pair.
	 * @post The HL register pair now holds the sum of its previous value and the value in the source register pair.
	 * The subtract flag is reset, the half-carry flag is set on carry from bit 11, the carry flag is set on carry from bit 15,
	 * and the zero flag is unchanged.
	 */
	void ADD_HL_rr(BYTE rr);

	/**
	 * @brief ADD SP, e: Add to the 16-bit stack pointer, the signed 8-bit immediate value e.
	 * @post The stack pointer now holds the sum of its previous value and the signed 8-bit immediate e located at PC+1.
	 * The zero and subtract flags are reset, and the half-carry and carry flags are set based on the low byte of SP plus e
	 * (half-carry from bit 3, carry from bit 7).
	 */
	void ADD_SP_e();

	/**
	 * @brief INC rr: Increment the 16-bit register pair rr by 1 (rp table: code 3 = SP).
	 * @param rr The 2-bit pair code for the destination register pair.
	 * @post The destination register pair now holds its previous value plus 1. No flags are affected.
	 */
	void INC_rr(BYTE rr);

	/**
	 * @brief DEC rr: Decrement the 16-bit register pair rr by 1 (rp table: code 3 = SP).
	 * @param rr The 2-bit pair code for the destination register pair.
	 * @post The destination register pair now holds its previous value minus 1. No flags are affected.
	 */
	void DEC_rr(BYTE rr);

};