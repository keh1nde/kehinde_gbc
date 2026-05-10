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
#include "GBC_APU.h"

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

	GBC_BUS(const std::string& bootPath, GBC_CART& cart, GBC_PPU& ppu, GBC_TIMER& timer, GBC_APU& apu);

	BYTE read8(WORD addr) override;
	void write8(WORD addr, BYTE val) override;

	WORD read16(WORD addr) override;
	void write16(WORD addr, WORD val) override;
	void tick(int cycles);

	void requestInterrupt(Interrupt which) override;

	void reset();

	void notify_hblank() override;

	// Joypad state. Caller (main loop) polls input each frame and pushes the
	// bitmask here. Bits: 0=A, 1=B, 2=Select, 3=Start, 4=Right, 5=Left, 6=Up, 7=Down. 1 = pressed.
	void set_buttons(BYTE buttons);
  // CgbState& cgb();              // TODO: define CgbState before re-enabling
	// const CgbState& cgb() const;



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
	GBC_TIMER& timer_;
	GBC_APU& apu_;

	// CgbState cgb_;  // TODO: define CgbState before re-enabling

	// Other members
	bool bootROM_enabled_ = false;

	// HDMA related members. hdma1..4 store the last value written (with the
	// hardware bit-masks already applied) so source/dest can be reassembled at
	// 0xFF55 trigger-time. Reads of FF51..FF54 return 0xFF on real hardware.
	BYTE hdma1_ = 0, hdma2_ = 0, hdma3_ = 0, hdma4_ = 0;
	bool hdma_active_ = false;          // HBlank-DMA in progress
	WORD hdma_src_ = 0;                 // advances by 0x10 per block
	WORD hdma_dst_ = 0;                 // advances by 0x10 per block (VRAM)
	BYTE hdma_blocks_remaining_ = 0;    // # of 0x10-byte blocks left



	BYTE sb_ = 0;

	// Joypad. joyp_select_ holds bits 4-5 (P14/P15) as written by the ROM (1=not selected).
	// buttons_ is the live key state (1 = pressed): 0=A,1=B,2=Select,3=Start,4=Right,5=Left,6=Up,7=Down.
	BYTE joyp_select_ = 0x30;
	BYTE buttons_     = 0x00;

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