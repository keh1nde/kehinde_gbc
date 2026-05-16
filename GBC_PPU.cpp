//
// Created by Kehinde Adeoso on 2/27/26.
//

#include "GBC_PPU.h"


void GBC_PPU::tick(const BYTE cycles) {
	dot_ += cycles;

	while (dot_ >= get_duration_(mode_)) {
		dot_ -= get_duration_(mode_);

		const int prev_mode = mode_;

		// Do exit-work
		switch (mode_) {
			case OAMScan:
				scan_oam_();
				break;
			case Drawing:
				render_scanline_();
				break;
			case HBlank:
				/* TODO: STAT HBlank IRQ if enabled */
				break;
			case VBlank:
				/* TODO: STAT VBlank IRQ if enabled (per-line, not just entry)*/
				break;
			default:
				break;
		}

		if (mode_ == HBlank) {
			ly_++;
			if (ly_ >= 144) {
				mode_ = VBlank;
				frame_ready_ = true;
				if (bus_) bus_->requestInterrupt(Interrupt::VBlank);
			}
			else mode_ = get_next_step_(mode_);
		} else if (mode_ == VBlank) {
			if (ly_ >= 153) {
				ly_ = 0;
				window_line_counter_ = 0;
				mode_ = OAMScan;
			} else {
				ly_++;
				mode_ = get_next_step_(mode_);
			}
		} else {
			mode_ = get_next_step_(mode_);
		}

		// HDMA HBlank trigger — fires once on Drawing→HBlank entry.
		if (prev_mode == Drawing && mode_ == HBlank && bus_) {
			bus_->notify_hblank();
		}

		// STAT IRQ: edge-triggered on the OR of all enabled sources.
		// stat_ bits: 3=HBlank, 4=VBlank, 5=OAMScan, 6=LY=LYC.
		const bool new_stat_line =
			   ((stat_ & 0x08) && mode_ == HBlank)
			|| ((stat_ & 0x10) && mode_ == VBlank)
			|| ((stat_ & 0x20) && mode_ == OAMScan)
			|| ((stat_ & 0x40) && ly_ == lyc_);
		if (new_stat_line && !stat_line_ && bus_) {
			bus_->requestInterrupt(Interrupt::LCDStat);
		}
		stat_line_ = new_stat_line;
	}
}

void GBC_PPU::resetPostBoot() {
	lcdc_ = 0x91;
	stat_ = 0x81;
	scy_ = 0x00;
	scx_ = 0x00;
	ly_ = 0x00; lyc_ = 0x00;
	bgp_ = 0xFC;
	obp0_ = 0x00;
	obp1_ = 0x00;
	wy_ = 0x00;
	wx_ = 0x00;
	vbk_ = 0x00;
	bgpi_ = 0x00;
	obpi_ = 0x00;
	opri_ = 0x01; // CGB boot-ROM default for this test ROM: OAM-priority.

	const WORD dmg_pal[4] = {0x7FFF, 0x56B5, 0x294A, 0x0000};
	for (int c = 0; c < 4; c++) {
		bg_palette_ram[c * 2 + 0] = dmg_pal[c] & 0xFF;
		bg_palette_ram[c * 2 + 1] = (dmg_pal[c] >> 8) & 0xFF;
		obj_palette_ram[c * 2 + 0] = dmg_pal[c] & 0xFF;
		obj_palette_ram[c * 2 + 1] = (dmg_pal[c] >> 8) & 0xFF;

	}

	dot_ = 0;
	mode_ = 2;
	window_line_counter_ = 0;
	visible_sprite_count_ = 0;
	stat_line_ = false;
	frame_ready_ = false;
	framebuffer.fill(0);
}

void GBC_PPU::scan_oam_() {
	// Walk OAM, fill visible_sprites with up to 10 sprites who's Y range covers ly_.
	visible_sprite_count_ = 0;

	const BYTE height = (lcdc_ & 0x04) ? 16 : 8;
	for (int i = 0; i < 40; i++) {
		const int base = i * 4;
		if (const BYTE y = ppu_OAM[base + 0]; (ly_ + 16) >= y && (ly_+ 16) < y + height) {
			if (visible_sprite_count_ < 10) {
				const Sprite object = {
					ppu_OAM[base + 1],
					y,
					ppu_OAM[base + 2],
					ppu_OAM[base + 3],
					i
				};
				visible_sprites_[visible_sprite_count_] = object;
				visible_sprite_count_++;
			}
		}
	}

	if (opri_ & 0x01) {
		// DMG / OAM-priority: lower OAM index wins.
		std::sort(visible_sprites_.begin(), visible_sprites_.begin() + visible_sprite_count_,
			[](const Sprite& a, const Sprite& b) {
				return a.oam_index < b.oam_index;
		});
	} else {
		// CGB / X-priority: lower X wins, OAM index tiebreak.
		std::sort(visible_sprites_.begin(), visible_sprites_.begin() + visible_sprite_count_,
			[](const Sprite& a, const Sprite& b) {
				if (a.x != b.x) return a.x < b.x;
				return a.oam_index < b.oam_index;
		});
	}

}

/*
* LCDC bit 5 — window enable. If clear, skip window entirely.
	LCDC bit 6 — window tile-map base: 0x9800 or 0x9C00 (separate selector from BG's bit 3).
	wy_ (0xFF4A) — window's top Y on-screen.
	wx_ (0xFF4B) — window's left X on-screen, but offset by 7: on-screen X = wx_ - 7.
	Tile-data base: same as BG (LCDC bit 4).
 *
* Per-pixel rule: for the current pixel (x, ly_), the window covers it if:
	- LCDC bit 5 is set, AND
	- ly_ >= wy_, AND
	- x + 7 >= wx_ (i.e. x >= wx_ - 7).

	Window coordinates:
	window_x = x - (wx_ - 7) — pure screen-relative, no SCX.
	window_y is not ly_ - wy_. It's an internal counter (window_line_counter)
*/

void GBC_PPU::render_scanline_() {
	// Render BG, window, sprites for line ly_ into framebuffer.
	// On CGB, LCDC bit 0 is BG-master-priority, not BG enable — BG/window always render.
	{
		bool drew_window = false;
		BYTE color_idx;
		for (int x = 0; x < 160; x++) {
			WORD rgb;

			if (const bool in_window = (lcdc_ & 0x20) && (ly_ >= wy_) && (x + 7 >= wx_); in_window) {
				drew_window = true;
				const BYTE window_x = x - (wx_ - 7);

				const BYTE window_tile_col = window_x / 8;
				const BYTE window_x_tile_offset = window_x % 8;

				const BYTE window_tile_row = window_line_counter_ / 8;
				BYTE window_y_tile_offset = window_line_counter_ % 8;

				const WORD surface_tile_map_base = (lcdc_ & 0x40) ? 0x9C00 : 0x9800;
				const WORD data_mode = (lcdc_ & 0x10);
				const WORD surface_tile_map_addr = surface_tile_map_base + window_tile_row * 32 + window_tile_col;

				const BYTE window_tile_idx = ppu_VRAM[0][surface_tile_map_addr - 0x8000];
				const BYTE window_pixel_data = ppu_VRAM[1][surface_tile_map_addr - 0x8000];

				if (window_pixel_data & 0x40) window_y_tile_offset = 7 - window_y_tile_offset;

				WORD window_tile_addr;
				if (data_mode) {
					// 0x8000 mode = unsigned index
					window_tile_addr = 0x8000 + window_tile_idx * 16;
				} else {
					window_tile_addr = 0x9000 + static_cast<int8_t>(window_tile_idx) * 16;
				}

				const WORD window_row_addr = window_tile_addr + window_y_tile_offset * 2;
				const BYTE bank = (window_pixel_data >> 3) & 1;
				const BYTE low = ppu_VRAM[bank][window_row_addr - 0x8000];
				const BYTE high = ppu_VRAM[bank][window_row_addr + 1 - 0x8000];

				int bit = 7 - window_x_tile_offset;
				if (window_pixel_data & 0x20) bit = window_x_tile_offset; // Mirror tile horizontally


				color_idx = ((low >> bit) & 1) | (((high >> bit) & 1) << 1);


				const BYTE window_pal = window_pixel_data & 0x07;
				const BYTE off = window_pal * 8 + color_idx * 2;

				bg_priority_line_[x] = (window_pixel_data & 0x80) != 0;
				rgb = bg_palette_ram[off] | (bg_palette_ram[off + 1] << 8);


			} else {
				// surface_xy = (SCX + x, SCY + ly_) mod 256
				const BYTE surface_x = (scx_ + x) & 0xFF;
				const BYTE surface_y = (scy_ + ly_) & 0xFF;

				const BYTE tile_col = surface_x / 8;
				const BYTE x_tile_offset = surface_x % 8;

				const BYTE tile_row = surface_y / 8;
				BYTE y_tile_offset = surface_y % 8;

				const WORD tile_map_base = (lcdc_ & 0x08) ? 0x9C00 : 0x9800;
				const WORD data_mode = (lcdc_ & 0x10);
				const WORD tile_map_addr = tile_map_base + tile_row * 32 + tile_col;

				const BYTE tile_idx = ppu_VRAM[0][tile_map_addr - 0x8000];
				const BYTE pixel_data = ppu_VRAM[1][tile_map_addr - 0x8000];

				if (pixel_data & 0x40) y_tile_offset = 7 - y_tile_offset;

				WORD tile_addr;
				if (data_mode) {
					// 0x8000 mode = unsigned index
					tile_addr = 0x8000 + tile_idx * 16;
				} else {
					tile_addr = 0x9000 + static_cast<int8_t>(tile_idx) * 16;
				}

				const WORD row_addr = tile_addr + y_tile_offset * 2;
				const BYTE bank = (pixel_data >> 3) & 1;
				const BYTE low = ppu_VRAM[bank][row_addr - 0x8000];
				const BYTE high = ppu_VRAM[bank][row_addr + 1 - 0x8000];

				int bit = 7 - x_tile_offset;
				if (pixel_data & 0x20) bit = x_tile_offset; // Mirror tile horizontally


				color_idx = ((low >> bit) & 1) | (((high >> bit) & 1) << 1);
				const BYTE pal = pixel_data & 0x07;
				const BYTE off = pal * 8 + color_idx * 2;


				bg_priority_line_[x] = (pixel_data & 0x80) != 0;
				rgb = bg_palette_ram[off] | (bg_palette_ram[off + 1] << 8);
			}


			bg_color_idx_line_[x] = color_idx;
			framebuffer[ly_*160 + x] = rgb;
		}
		if (drew_window) window_line_counter_++;
	}

	if (lcdc_ & 0x02) {
		render_sprites_();
	}
}

void GBC_PPU::render_sprites_() {
	sprite_claimed_line_.fill(false);

	// Iterate highest-priority first. visible_sprites_ is sorted ascending by priority key
	// (X-priority: lower X first; OAM-priority: lower OAM index first), so forward iteration
	// processes the highest-priority sprite at each pixel first.
	for (int i = 0; i < visible_sprite_count_; i++) {
		const Sprite& s = visible_sprites_[i];
		const BYTE height = (lcdc_ & 0x04) ? 16 : 8;
		BYTE tile_idx = s.tile;
		BYTE row_in_sprite = ly_ - (s.y - 16);
		if (s.attribute & 0x40) row_in_sprite = (height - 1) - row_in_sprite;

		if (height == 16) {
			tile_idx &= 0xFE;
			if (row_in_sprite >= 8) {
				tile_idx |= 0x01;
				row_in_sprite -= 8;
			}
		}

		for (int px = 0; px < 8; px++) {
			const int screen_x = (s.x - 8) + px;
			if (screen_x < 0 || screen_x >= 160) continue;

			// Higher-priority sprite already claimed this pixel — skip regardless of transparency.
			if (sprite_claimed_line_[screen_x]) continue;
			sprite_claimed_line_[screen_x] = true;

			int in_tile_x = px;
			if (s.attribute & 0x20) in_tile_x = 7 - in_tile_x;
			const int bit = 7 - in_tile_x;

			const BYTE bank = (s.attribute >> 3) & 1;
			const WORD tile_addr = 0x8000 + tile_idx * 16 + row_in_sprite * 2;
			const BYTE low  = ppu_VRAM[bank][tile_addr - 0x8000];
			const BYTE high = ppu_VRAM[bank][tile_addr + 1 - 0x8000];

			const WORD color_idx = ((low >> bit) & 1) | (((high >> bit) & 1) << 1);
			if (color_idx == 0) continue;
			if ((lcdc_ & 0x01) && bg_color_idx_line_[screen_x] != 0 &&
				(bg_priority_line_[screen_x] || (s.attribute & 0x80))) continue;

			const BYTE pal = s.attribute & 0x07;
			const int off = pal * 8 + color_idx * 2;
			const WORD rgb = obj_palette_ram[off] | (obj_palette_ram[off + 1] << 8);
			framebuffer[ly_ * 160 + screen_x] = rgb;
		}
	}
}

BYTE GBC_PPU::read(const WORD addr) const {
	switch (addr) {
		case ppu_LCDC: return lcdc_;
		case ppu_STAT: return (stat_ & 0x78) | (ly_ == lyc_ ? 0x04 : 0) | mode_;
		case ppu_SCY: return scy_;
		case ppu_SCX: return scx_;
		case ppu_LY: return ly_;
		case ppu_LYC: return lyc_;
		case ppu_BGP: return bgp_;
		case ppu_OBP0: return obp0_;
		case ppu_OBP1: return obp1_;
		case ppu_WY: return wy_;
		case ppu_WX: return wx_;
		case ppu_BGPI: return bgpi_;
		case ppu_BGPD: return bg_palette_ram[bgpi_ & 0x3F];
		case ppu_OBPI: return obpi_;
		case ppu_OBPD: return obj_palette_ram[obpi_ & 0x3F];
		case ppu_OPRI:
			return opri_ | 0xFE;
		case ppu_VBK: return vbk_ | 0xFE;
		default: break;
	}

	if (addr < 0xA000 && addr >= 0x8000) {
		return ppu_VRAM[vbk_ & 1][addr-0x8000];
	} else if (addr < 0xFEA0 && addr >= 0xFE00) {
		return ppu_OAM[addr-0xFE00];
	} else return 0;
}

void GBC_PPU::write(const WORD addr, const BYTE val) {
	switch (addr) {
		case ppu_LCDC:
			lcdc_ = val;
			return;
		case ppu_STAT:
			stat_ = (stat_ & 0x07) | (val & 0x78);
			return;
		case ppu_SCY:
			scy_ = val;
			return;
		case ppu_SCX:
			scx_ = val;
			return;
		case ppu_LY:
			return;
		case ppu_LYC:
			lyc_ = val;
			return;
		case ppu_BGP:
			bgp_ = val;
			return;
		case ppu_OBP0:
			obp0_ = val;
			return;
		case ppu_OBP1:
			obp1_ = val;
			return;
		case ppu_WY:
			wy_ = val;
			return;
		case ppu_WX:
			wx_ = val;
			return;
		case ppu_BGPI:
			bgpi_ = val;
			return;
		case ppu_BGPD:
			bg_palette_ram[bgpi_ & 0x3F] = val;
			if (bgpi_ & 0x80) bgpi_ = (bgpi_ & 0x80) | ((bgpi_ + 1) & 0x3F);
			return;
		case ppu_OBPI:
			obpi_ = val;
			return;
		case ppu_OBPD:
			obj_palette_ram[obpi_ & 0x3F] = val;
			if (obpi_ & 0x80) obpi_ = (obpi_ & 0x80) | ((obpi_ + 1) & 0x3F);
			return;
		case ppu_OPRI:
			opri_ = val;
			return;
		case ppu_VBK:
			vbk_ = val & 1;
			return;
		default: break;
	}

	if (addr < 0xA000 && addr >= 0x8000) ppu_VRAM[vbk_ & 1][addr-0x8000] = val;
	if (addr < 0xFEA0 && addr >= 0xFE00) ppu_OAM[addr-0xFE00] = val;
}

/*
constexpr int HBlank = 0;
constexpr int VBlank = 1;
constexpr int OAMScan = 2;
constexpr int Drawing = 3;
*/

int GBC_PPU::get_next_step_(const int step) const {
	switch (step) {
		case OAMScan: return Drawing;
		case Drawing: return HBlank;
		case HBlank: return OAMScan;
		case VBlank: return VBlank;
		default: return mode_; // Undefined
	}
}

int GBC_PPU::get_duration_(const int step) {
	switch (step) {
		case OAMScan: return 80;
		case Drawing: return 172;
		case HBlank: return 204;
		case VBlank: return 456;
		default: return 0;
	}
}
