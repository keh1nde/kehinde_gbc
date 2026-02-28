//
// Created by Kehinde Adeoso on 1/27/26.
//

#ifndef KEHINDES_GAMEBOYCOLOR_EMULATOR_GB_MMU_H
#define KEHINDES_GAMEBOYCOLOR_EMULATOR_GB_MMU_H
#include "GBC_CPU.h"
#include "GBC_CART.h"
#include <array>


/*
 * Address banking:
 * 0x0000–0x3FFF: Cartridge ROM bank 0 (if boot ROM enabled and addr < 0x0100 (DMG): return boot ROM byte)
 * 0x4000–0x7FFF: Cartridge switchable ROM bank (MBC-controlled)
 * 0x8000–0x9FFF: VRAM (PPU)
 * 0xA000–0xBFFF: External RAM (cartridge/MBC-controlled enable/bank)
 * 0xC000–0xDFFF: WRAM (banked on CGB for 0xD000–0xDFFF)
 * 0xE000–0xFDFF: Echo RAM (mirror of 0xC000–0xDDFF)
 * 0xFE00–0xFE9F: OAM (sprite attribute table)
 * 0xFEA0–0xFEFF: “Not usable” (typically reads as 0xFF; writes ignored)
 * 0xFF00–0xFF7F: I/O registers
 * - Joypad, serial, timer, interrupt flag (IF), APU, PPU regs, etc.
 * - 0xFF46: OAM DMA trigger (write starts DMA)
 * - 0xFF50: Boot ROM disable latch
 * 0xFF80–0xFFFE: HRAM
 * 0xFFFF: IE (interrupt enable)
 */

#include "GBC_CPU.h"
using BYTE = uint8_t;
using WORD = uint16_t;

enum class Interrupt : BYTE {VBlank, LCDStat, Timer, Serial, Joypad};

struct IBus { // Interface for the CPU to access memory of components
							// and for component memory implementations.
	virtual ~IBus() = default;

	virtual BYTE read8(WORD addr) = 0;
	virtual void write8(WORD addr, BYTE val) = 0;

	virtual WORD read16(WORD addr) = 0;
	virtual void write16(WORD addr, WORD val) = 0;
};

class GBC_BUS final : public IBus {
public:
	struct CgbState { // May not be needed
		bool is_cgb = false;

		BYTE vbk = 0;
		BYTE svbk = 1;
		BYTE key1 = 0;
	};

	GBC_BUS(GBC_CART& cart); // Include all other components later.

	BYTE read8(WORD addr) override;
	void write8(WORD addr, BYTE val) override;

	WORD read16(WORD addr) override;
	void write16(WORD addr, WORD val) override;
	void tick(int cycles);

	void request_interrupt(Interrupt which);

	void reset();
	CgbState& cgb();
	const CgbState& cgb() const;

private:
	GBC_CART& cart_;
	// TODO: Include all other components later.

	// ---- Memory objects ----:
	std::array<BYTE, 0x8000> mmu_WorkRAM_{};
	std::array<BYTE, 0x007F> mmu_HighRAM_{};

	// Interrupt registers
	BYTE IF_ = 0xE1; // FF0F
	BYTE IE_ = 0x00; // FFFF

	// ---- Helpers
	BYTE read_io(WORD addr);
	void write_io(WORD addr, BYTE val);

	BYTE read_wram(WORD addr) const;
	void write_wram(WORD addr, BYTE val);

	BYTE read_hram(WORD addr) const;
	void write_hram(WORD addr, BYTE val);

	// Address decoding helper
	bool is_unusuable(WORD addr) const; // Between FEA0 - FEFF
};


#endif //KEHINDES_GAMEBOYCOLOR_EMULATOR_GB_MMU_H