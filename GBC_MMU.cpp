//
// Created by Kehinde Adeoso on 1/27/26.
//

#include "GBC_MMU.h"

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

BYTE GBC_MMU::read8(WORD addr) {
	// This method must evaluate the 16 bit addr given as input and check it against the previous bounds.
	// If the address is within a certain bound, then we either read directly or call the device's read
	// method.

	if (addr < 0x8000) {
		// TODO: Properly implement cartridge.read_rom(addr)
		return read8(addr);
	}
	else if (addr < 0xA000) {
		// TODO: Implement ppu.read_vram(addr)
	}
	else if (addr < 0xC000) {
		// TODO: Figure out c_WorkRAM implementation
	}
	else if (addr < 0xE000) {
		// Echo RAM mirror, not directly accessible
	}
	else if (addr < 0xFEA0) {
		// TODO: Implement ppu.read_oam(addr)
	}
	else if (addr < 0xFF00) {
		// TODO: Implement exception
	}
	else if (addr < 0xFF80) {
		// TODO: Implement read_io(addr)
	}
	else if (addr < 0xFFFF) {
		// TODO: Implement hram[addr] access
	}
	else {
		// TODO: Implement interrupts
	}
}

void GBC_MMU::write8(WORD addr) {
	if (addr < 0x8000) {
		// TODO: Implement cartridge.write_rom(addr)
	}
	else if (addr < 0xA000) {
		// TODO: Implement ppu.write_vram(addr)
	}
	else if (addr < 0xC000) {
		// TODO: Figure out c_WorkRAM implementation
	}
	else if (addr < 0xE000) {
		// Echo RAM mirror, not directly accessible
	}
	else if (addr < 0xFEA0) {
		// TODO: Implement ppu.write_oam(addr)
	}
	else if (addr < 0xFF00) {
		// TODO: Implement exception
	}
	else if (addr < 0xFF80) {
		// TODO: Implement write_io(addr)
	}
	else if (addr < 0xFFFF) {
		// TODO: Implement hram[addr] access
	}
	else {
		// TODO: Implement interrupts
	}
}


// TODO: Extend implementation to read16 and write16