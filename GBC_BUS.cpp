//
// Created by Kehinde Adeoso on 1/27/26.
//

#include "GBC_BUS.h"

GBC_BUS::GBC_BUS(const std::string &bootPath, GBC_CART& cart, GBC_PPU& ppu, GBC_TIMER& timer) : cart_(cart), ppu_(ppu), timer_(timer) {
	// Load BootROM.
	if (!bootPath.empty()) {
		FILE* file = fopen(bootPath.c_str(),"rb");
		if (!file) {
			throw std::runtime_error("Could not open boot ROM file");
		}

		fseek(file, 0, SEEK_END);
		const long size = ftell(file);

		fseek(file, 0, SEEK_SET);
		if (size > mmu_BootROM_.size()) throw std::runtime_error("ROM size exceeds register size.");

		fread(mmu_BootROM_.data(), 1, size, file);
		fclose(file);

		bootROM_enabled_ = true;
	}
	return;
}


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

BYTE GBC_BUS::read8(const WORD addr) {
	// ROM Bank 00
	if (addr < 0x8000) {
		return cart_.read_rom(addr);
	}

	if (addr < 0xA000 && addr >= 0x8000) {
		return ppu_.read(addr);
	}

	// 8 KiB External RAM
	if (addr < 0xC000 && addr >= 0xA000) {
		return cart_.read_ram(addr);
	}

	// 8 KiB Work RAM (Bank 1 + Bank 2 collapsed — 0xC000-0xDFFF indexes into mmu_WorkRAM_[0..0x1FFF])
	if (addr < 0xE000 && addr >= 0xC000) {
		return mmu_WorkRAM_[addr - 0xC000];
	}

	// Echo RAM, inaccessible, but mirror WorkRAM
	if (addr < 0xFE00 && addr >= 0xE000) {
		return mmu_WorkRAM_[addr - 0xE000];
	}

	// Object Attribute Memory
	if (addr < 0xFEA0 && addr >= 0xFE00) {
		return ppu_.read(addr);
	}

	// Not usable
	if (addr < 0xFF00 && addr >= 0xFEA0) {
		return 0xFF;
	}

	// I/O Registers
	if (addr < 0xFF80 && addr >= 0xFF00) {
		return read_io(addr);
	}

	// High RAM
	if (addr < 0xFFFF && addr >= 0xFF80) {
		return read_hram(addr);
	} else return IE_;
}

void GBC_BUS::write8(const WORD addr, const BYTE val) {
	if (addr == 0xFF01) {
		sb_ = val;
	}
	// ROM Bank from 00 to NN
	if (addr < 0x8000) {
		cart_.write_rom(addr, val);
		return;
	}

	if (addr < 0xA000 && addr >= 0x8000) {
		ppu_.write(addr, val);
		return;
	}
	// 8 KiB External RAM
	if (addr < 0xC000 && addr >= 0xA000) {
		cart_.write_ram(addr, val);
		return;
	}

	// 8 KiB Work RAM (Bank 1 + Bank 2 collapsed)
	if (addr < 0xE000 && addr >= 0xC000) {
		mmu_WorkRAM_[addr - 0xC000] = val;
		return;
	}

	// Echo RAM, inaccessible
	if (addr < 0xFE00 && addr >= 0xE000) {
		mmu_WorkRAM_[addr - 0xE000] = val; // Mirror of WorkRAM
		return;
	}

	// Object Attribute Memory
	if (addr < 0xFEA0 && addr >= 0xFE00) {
		ppu_.write(addr, val);
		return;
	}

	// Not usable
	if (addr < 0xFF00 && addr >= 0xFEA0) {
		return;
	}

	// I/O Registers
	if (addr < 0xFF80 && addr >= 0xFF00) {
		write_io(addr, val);
		return;
	}

	// High RAM
	if (addr <= 0xFFFE && addr >= 0xFF80) {
		write_hram(addr, val); // May expect an offset.
		return;
	}

	// Interrupt Register
	if (addr == 0xFFFF) {
		IE_ = val;
	}
}

WORD GBC_BUS::read16(const WORD addr) {
	//Properly implement cartridge.read_rom(addr)
	const BYTE lo = read8(addr);
	const BYTE hi = read8(addr + 1);


	return (hi << 8) | lo;
}

void GBC_BUS::write16(const WORD addr, const WORD val) {
	write8(addr, val & 0xFF);
	write8(addr + 1, val >> 8);
}


// ———————— End Memory Management, begin Hardware Management ————————

void GBC_BUS::tick(int cycles) {

}

void GBC_BUS::requestInterrupt(Interrupt which) {
	IF_ |= (1 << static_cast<int>(which));
}

void GBC_BUS::reset() {

}
// GBC_BUS::CgbState& GBC_BUS::cgb() { return cgb_; }                     // TODO: define CgbState
// const GBC_BUS::CgbState& GBC_BUS::cgb() const { return cgb_; }

// ———————— End Hardware Management, begin Helpers ————————

BYTE GBC_BUS::read_io(const WORD addr) {
	if (addr <= 0xFF07 && addr >= 0xFF04) return timer_.read(addr);
	if (addr == 0xFF0F) return IF_ | 0xE0;

	// PPU-owned registers
	if (addr >= 0xFF40 && addr <= 0xFF4B && addr != 0xFF46) return ppu_.read(addr);
	if (addr == 0xFF4F) return ppu_.read(addr);
	if (addr >= 0xFF68 && addr <= 0xFF6B) return ppu_.read(addr);
	if (addr == 0xFF6C) return ppu_.read(addr);

	return mmu_IO_[addr - 0xFF00];
}
void GBC_BUS::write_io(const WORD addr, const BYTE val) {
	if (addr <= 0xFF07 && addr >= 0xFF04) { timer_.write(addr, val); return; }
	if (addr == 0xFF0F) {
		IF_ = val & 0x1F;
		return;
	}

	// PPU-owned registers
	if (addr >= 0xFF40 && addr <= 0xFF4B && addr != 0xFF46) { ppu_.write(addr, val); return; }
	if (addr == 0xFF4F)                                    { ppu_.write(addr, val); return; }
	if (addr >= 0xFF68 && addr <= 0xFF6B)                  { ppu_.write(addr, val); return; }
	if (addr == 0xFF6C)                                    { ppu_.write(addr, val); return; }

	mmu_IO_[addr - 0xFF00] = val;

	if (addr == 0xFF02 && (val & 0x80)) {
		std::cout << static_cast<char>(mmu_IO_[0x01]) << std::flush;
		mmu_IO_[0x02] = val & 0x7F;

	}
}

BYTE GBC_BUS::read_wram(const WORD addr) const {
	return mmu_WorkRAM_[addr];
}
void GBC_BUS::write_wram(const WORD addr, const BYTE val) {
	mmu_WorkRAM_[addr] = val;
}

BYTE GBC_BUS::read_hram(const WORD addr) const {
	return mmu_HighRAM_[addr];
}
void GBC_BUS::write_hram(const WORD addr, const BYTE val) {
	mmu_HighRAM_[addr] = val;
}

// Address decoding helper
// bool is_unusuable(WORD addr) const; // Between FEA0 - FEFF