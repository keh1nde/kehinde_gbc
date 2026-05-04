//
// Created by Kehinde Adeoso on 2/27/26.
//

#pragma once

#include "GBC_Types.h"
#include <array>
#include <algorithm>

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

constexpr int HBlank = 0;
constexpr int VBlank = 1;
constexpr int OAMScan = 2;
constexpr int Drawing = 3;

/*
 * Good-to-knows:
 *
 * 456 dots = one scanline
 * 154 scanlines = one frame (144 visible and 10 VBlank)
 * 70,224 dots = one frame (around 59.7 fps)
 */


class GBC_PPU {
public:

	GBC_PPU() = default;

	void tick(BYTE cycles);
	BYTE read(WORD addr) const;
	void write(WORD addr, BYTE val);
	void resetPostBoot();



private:

	struct Sprite {
		BYTE x, y, tile, attribute; // raw OAM bytes for one sprite (y is byte 0, x byte 1, tile byte 2, attribute byte 3)
		int oam_index;             // 0..39, original OAM slot — used as a tiebreaker when two sprites share an X
	};

	std::array<std::array<BYTE, 0x2000>, 2> ppu_VRAM; // VRAM banks 0 and 1; CGB selects active bank via vbk_
	std::array<BYTE, 0xA0> ppu_OAM;                   // 40 sprites * 4 bytes = 160 bytes of OAM

	std::array<BYTE, 64> bg_palette_ram;  // CGB BG palette RAM: 8 palettes * 4 colors * 2 bytes (RGB555 little-endian)
	std::array<BYTE, 64> obj_palette_ram; // CGB OBJ palette RAM, same layout as bg_palette_ram
	std::array<WORD, 160 * 144> framebuffer; // One frame of output pixels (may widen to RGB555/888 later for CGB color)
	std::array<Sprite, 10> visible_sprites_; // All visible sprites for the current line
	BYTE visible_sprite_count_; // The number of visible sprites. Max 10
	BYTE window_line_counter_;
	std::array<BYTE, 160> bg_color_idx_line_;

	// Device Registers
	BYTE lcdc_; // 0xFF40 LCD Control: master enable, tile-map/tile-data selectors, sprite size, BG/window enable bits
	BYTE stat_; // 0xFF41 LCD Status: only bits 6..3 (IRQ enables) live here; mode (1..0) and LY=LYC (2) are computed on read
	BYTE scy_;  // 0xFF42 Background scroll Y
	BYTE scx_;  // 0xFF43 Background scroll X
	BYTE ly_;   // 0xFF44 Current scanline (0..153). Written by PPU each line; read-only to the CPU
	BYTE lyc_;  // 0xFF45 LY-compare target; equality drives the STAT coincidence bit and optional STAT IRQ
	BYTE bgp_; // 0xFF47 DMG BG palette (unused in pure CGB-mode rendering, but still readable/writable)
	BYTE obp0_; // 0xFF48 DMG OBJ palette 0
	BYTE obp1_; // 0xFF49 DMG OBJ palette 1
	BYTE wx_;   // 0xFF4B Window X position (on-screen X = wx_ - 7)
	BYTE wy_;   // 0xFF4A Window Y position
	BYTE vbk_;  // 0xFF4F VRAM bank select; only bit 0 is meaningful, indexes ppu_VRAM
	BYTE bgpi_; // 0xFF68 BG palette index: bits 0..5 = index into bg_palette_ram, bit 7 = auto-increment on BGPD write
	BYTE obpi_; // 0xFF6A OBJ palette index, same layout as bgpi_ but for obj_palette_ram
	BYTE mode_; // The present mode of the PPU (VBlank, HBlank, Drawing, OAMScan)
	WORD dot_; // Store for the amount of cycles passed.

	// Timing Methods
	int get_next_step_(int step) const;

	static int get_duration_(int step);

	// Render Methods
	void scan_oam_(); // exit-work for OAMScan
	void render_scanline_(); // exit-work for Drawing
	void render_sprites_();
};
