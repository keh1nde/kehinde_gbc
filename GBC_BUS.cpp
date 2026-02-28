//
// Created by Kehinde Adeoso on 1/27/26.
//

#include "GBC_BUS.h"

GBC_BUS::GBC_BUS(const std::string &bootPath, GBC_CART &cart, GBC_PPU& ppu) : cart_(cart),  ppu_(ppu) {
	// Load BootROM.
	FILE* file = fopen(bootPath.c_str(),"rb");
	int b_pos = 0x0000;
	while (fread(&mmu_WorkRAM_[b_pos], 1, 1, file)) {
		b_pos++;
	}

	fclose(file);
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
	// This method must evaluate the 16 bit addr given as input and check it against the previous bounds.
	// If the address is within a certain bound, then we either read directly or call the device's read
	// method.

	if (addr < 0x8000) {
		//Properly implement cartridge.read_rom(addr)
		return ppu_.read(addr);
	}
	else if (addr < 0xA000) {
		// Implement ppu.read_vram(addr)
		return ppu_.read(addr);
	}
	else if (addr < 0xC000) {
		return mmu_WorkRAM_[addr];
	}
	else if (addr < 0xE000) {
		// Echo RAM mirror, not directly accessible
	}
	else if (addr < 0xFEA0) {
		// Implement ppu.read_oam(addr)
		return ppu_.read(addr);
	}
	else if (addr < 0xFF00) {
		// Implement exception
	}
	else if (addr < 0xFF80) {
		// Implement read_io(addr)
		return read_io(addr);
	}
	else if (addr < 0xFFFF) {
		// Implement hram[addr] access
		return read_hram(addr);
	}
	else {
		// TODO: Implement interrupts
	}
}

void GBC_BUS::write8(const WORD addr, const BYTE val) {
	if (addr < 0x8000) {
		// Implement cartridge.write_ram(addr)
		ppu_.write(addr);
	}
	else if (addr < 0xA000) {
		// Implement ppu.write_vram(addr)
		ppu_.write(addr);
	}
	else if (addr < 0xC000) {
		// TODO: Figure out c_WorkRAM write implementation
		mmu_WorkRAM_[addr] = val;
	}
	else if (addr < 0xE000) {
		// Echo RAM mirror, not directly accessible
	}
	else if (addr < 0xFEA0) {
		// Implement ppu.write_oam(addr)
		ppu_.write(addr, val);
	}
	else if (addr < 0xFF00) {
		// TODO: Implement exception
	}
	else if (addr < 0xFF80) {
		// Implement write_io(addr)
		write_io(addr, val);
	}
	else if (addr < 0xFFFF) {
		// TODO: Implement hram[addr] access
		mmu_HighRAM_[addr] = val;
	}
	else {
		// TODO: Implement interrupts
	}
}


// TODO: Extend implementation to read16 and write16

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
