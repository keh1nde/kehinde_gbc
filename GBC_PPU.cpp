//
// Created by Kehinde Adeoso on 2/27/26.
//

#include "GBC_PPU.h"

BYTE GBC_PPU::read(const WORD addr) {
	if (addr < 0xA000 && addr >= 0x8000) return ppu_VRAM[0][addr-0x8000];
	if (addr < 0xFEA0 && addr >= 0xFE00) return ppu_OAM[addr-0xFE00];
}

void GBC_PPU::write(const WORD addr, const BYTE val) {
	if (addr < 0xA000 && addr >= 0x8000) {
		ppu_VRAM[0][addr-0x8000] = val;
	}

	if (addr < 0xFEA0 && addr >= 0xFE00) {
		ppu_OAM[addr-0xFE00] = val;
	}
}
