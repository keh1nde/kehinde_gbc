//
// Created by Kehinde Adeoso on 4/20/26.
//

#ifndef KEHINDES_GAMEBOYCOLOR_EMULATOR_TYPEDEFS_H
#define KEHINDES_GAMEBOYCOLOR_EMULATOR_TYPEDEFS_H

#include <cstdint>
using BYTE = uint8_t;
using WORD = uint16_t;
using DWORD = uint32_t;

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

constexpr int c_OAM_DMA = 0xFF46;

enum class Interrupt : BYTE {VBlank, LCDStat, Timer, Serial, Joypad};



struct IBus { // Interface for the CPU to access memory of components
	// and for component memory implementations.
	virtual ~IBus() = default;

	virtual BYTE read8(WORD addr) = 0;
	virtual void write8(WORD addr, BYTE val) = 0;

	virtual WORD read16(WORD addr) = 0;
	virtual void write16(WORD addr, WORD val) = 0;

	virtual void requestInterrupt(Interrupt which) = 0;
};


#endif //KEHINDES_GAMEBOYCOLOR_EMULATOR_TYPEDEFS_H
