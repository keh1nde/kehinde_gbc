//
// Created by Kehinde Adeoso on 2/27/26.
//

#pragma once

#include "GBC_Types.h"
#include "array"

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
public:

	GBC_PPU() = default;
	BYTE read(WORD addr);
	void write(WORD addr, BYTE val);


private:
	std::array<std::array<BYTE, 0x2000>, 2> ppu_VRAM;
	std::array<BYTE, 0x2000> ppu_OAM;

	std::array<BYTE, 64> bg_palette_ram;
	std::array<BYTE, 64> obj_palette_ram;

	std::array<BYTE, 160 * 144> framebuffer;

	// State Tracking
	int cycles; // Dot clock counter. TODO: Timer schema still needed.
	BYTE mode; // 0 = HBlank, 1 = VBlank, 2 = OAM Scan, 3 = Drawing
	BYTE window_line; // Internal window line counter (NOT the same as LY)
	bool frame_ready; // Signal to host that a frame is complete

	// Mode Handling
	void tick_oam_scan();
	void tick_drawing();
	void tick_hblank();
	void tick_vblank();

	void render_scanline();
	void render_bg_scanline();
	void render_window_scanline();
	void render_sprites_scanline();

	// Helpers
	void check_lyc();
	void set_mode(BYTE mode);
	BYTE get_title_data();

};