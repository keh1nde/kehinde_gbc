//
// Created by Kehinde Adeoso on 1/27/26.
//

#ifndef KEHINDES_GAMEBOYCOLOR_EMULATOR_GB_MMU_H
#define KEHINDES_GAMEBOYCOLOR_EMULATOR_GB_MMU_H
#include "GBC_CPU.h"


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

class GBC_MMU {

public:
	BYTE read8(BYTE addr);
	void write8(BYTE addr);

	BYTE read16(BYTE addr);
	void write16(BYTE addr);

	/*
	 * void tick(cycles);
	 * void dma_start(page);
	 * load_boot_rom(bytes);
	 * connect(bytes);
	 * reset();
	 */


};


#endif //KEHINDES_GAMEBOYCOLOR_EMULATOR_GB_MMU_H