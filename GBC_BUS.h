//
// Created by Kehinde Adeoso on 1/27/26.
//

#ifndef KEHINDES_GAMEBOYCOLOR_EMULATOR_GB_MMU_H
#define KEHINDES_GAMEBOYCOLOR_EMULATOR_GB_MMU_H

#include "GBC_Types.h"
#include "GBC_CART.h"
#include "GBC_PPU.h"
#include <array>
#include <iostream>
#include <stdexcept>

#include "GBC_TIMER.h"

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


class GBC_BUS final : public IBus {
public:

	GBC_BUS(const std::string& bootPath, GBC_CART& cart, GBC_PPU& ppu); // Include all other components later.

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
	// TODO: Include all other components later.

	// ---- Memory objects ----:
	std::array<BYTE, 0x8000> mmu_WorkRAM_{};
	std::array<BYTE, 0x007F> mmu_HighRAM_{};
	std::array<BYTE, 0x900> mmu_BootROM_{};
	std::array<BYTE, 0x80> mmu_IO_{}; // Public for testing

	// Interrupt registers
	BYTE IF_ = 0xE1; // FF0F
	BYTE IE_ = 0x00; // FFFF

	GBC_CART& cart_;
	GBC_PPU& ppu_;
	CgbState cgb_;

	// Other members
	bool bootROM_enabled_ = false;


	BYTE sb_ = 0;

	// ---- Helpers
	BYTE read_io(WORD addr);
	void write_io(WORD addr, BYTE val);

	BYTE read_wram(WORD addr) const;
	void write_wram(WORD addr, BYTE val);

	BYTE read_hram(WORD addr) const;
	void write_hram(WORD addr, BYTE val);

	// Address decoding helper
	// bool is_unusuable(WORD addr) const; // Between FEA0 - FEFF
};


#endif //KEHINDES_GAMEBOYCOLOR_EMULATOR_GB_MMU_H