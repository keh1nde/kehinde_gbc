//
// Created by Kehinde Adeoso on 2/27/26.
//

#pragma once

#include "GBC_BUS.h"

// Type Aliases
constexpr int ppu_LCDC = 0xFF40; // LCD Control
constexpr int ppu_STAT = 0xFF41; // LCD Status
constexpr int ppu_SCY = 0xFF42; // Scroll Y
constexpr int ppu_SCX = 0xFF43; // Scroll X
constexpr int ppu_LY = 0xFF44; // Current scanline (read-only)
constexpr int ppu_LYC = 0xFF45; // LY Compare
constexpr int ppu_BGP = 0xFF47; // BG Palette (DMG)
constexpr int ppu_OBP0 = 0xFF48; // OBJ Palette 0 (DMG)
constexpr int ppu_OBP1 = 0xFF49; // OBJ Palette 1 (DMG)
constexpr int ppu_WY = 0xFF4A; // Window Y
constexpr int ppu_WX = 0xFF4B; // Window X

// GBC PPU registers
constexpr int ppu_BGPI =  0xFF68; // BG Palette Index
constexpr int ppu_BGPD = 0xFF69; // BG Palette Data
constexpr int ppu_OBPI = 0xFF6A; // OBJ Palette Index
constexpr int ppu_OBPD = 0xFF6B; // OBJ Palette Data
constexpr int ppu_VBK = 0xFF4F; // VRAM Bank Select (0 or 1)


class GBC_PPU {
private:
	BYTE vram[2][0x2000];
};