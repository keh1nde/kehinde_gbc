//
// Created by Kehinde Adeoso on 2/14/26.
//

#include "GBC_CART.h"

#include <stdexcept>

namespace {
	int rom_size_from_header(BYTE h148) {
		// Header byte 0x148: total ROM size = 32 KiB << h148 for h148 in 0..8.
		// (Values 0x52/0x53/0x54 are non-standard and rarely used; ignore.)
		return (32 * 1024) << h148;
	}

	int ram_size_from_header(BYTE h149) {
		switch (h149) {
			case 0x00: return 0;
			case 0x01: return 2 * 1024;     // legacy / unofficial
			case 0x02: return 8 * 1024;
			case 0x03: return 32 * 1024;
			case 0x04: return 128 * 1024;
			case 0x05: return 64 * 1024;
			default:   return 0;
		}
	}
}

GBC_CART::GBC_CART(const std::string& romPath) {
	if (romPath.empty()) {
		// Empty-path mode for tests: leave with a 32 KiB no-MBC blank cart.
		rom_.assign(32 * 1024, 0);
		rom_bank_count_ = 2;
		mapper_ = Mapper::None;
		return;
	}

	FILE* file = fopen(romPath.c_str(), "rb");
	if (!file) throw std::runtime_error("Could not open cartridge file");

	fseek(file, 0, SEEK_END);
	const long size = ftell(file);
	fseek(file, 0, SEEK_SET);

	rom_.resize(size);
	fread(rom_.data(), 1, size, file);
	fclose(file);

	if (size < 0x150) throw std::runtime_error("ROM too small to contain a header");

	const BYTE cart_type = rom_[0x147];
	const BYTE rom_code  = rom_[0x148];
	const BYTE ram_code  = rom_[0x149];

	const int rom_bytes = rom_size_from_header(rom_code);
	int ram_bytes = ram_size_from_header(ram_code);

	rom_bank_count_ = rom_bytes / (16 * 1024);
	ram_bank_count_ = ram_bytes / (8 * 1024);

	switch (cart_type) {
		case 0x00:
			mapper_ = Mapper::None;
			break;
		case 0x01: case 0x02: case 0x03:
			mapper_ = Mapper::MBC1;
			break;
		case 0x05: case 0x06:
			mapper_ = Mapper::MBC2;
			// MBC2 has built-in 512x4-bit RAM (256 bytes addressable, mirrored across 0xA000-0xBFFF).
			ram_bytes = 512;
			ram_bank_count_ = 1;
			break;
		case 0x0F: case 0x10: case 0x11: case 0x12: case 0x13:
			mapper_ = Mapper::MBC3;
			break;
		case 0x19: case 0x1A: case 0x1B:
		case 0x1C: case 0x1D: case 0x1E:
			mapper_ = Mapper::MBC5;
			break;
		default:
			throw std::runtime_error("Unsupported cartridge type");
	}

	if (ram_bytes > 0) ram_.assign(ram_bytes, 0);
}

// ---- MBC1 helpers ----

int GBC_CART::mbc1_rom_bank_lo_region_() const {
	// 0x0000-0x3FFF read.
	// In MBC1 mode 0, always bank 0. In mode 1, upper bits affect bank selection
	// for ROMs >= 1 MiB: bank = (mbc1_upper_ << 5) & (rom_bank_count_ - 1).
	if (!mbc1_mode_) return 0;
	const int bank = (mbc1_upper_ << 5) & (rom_bank_count_ - 1);
	return bank;
}

int GBC_CART::mbc1_rom_bank_hi_region_() const {
	// 0x4000-0x7FFF read. (mbc1_upper_ << 5) | mbc1_rom_lo_ with 0->1 quirk on lo.
	BYTE lo = mbc1_rom_lo_ & 0x1F;
	if (lo == 0) lo = 1;
	const int bank = ((mbc1_upper_ << 5) | lo) & (rom_bank_count_ - 1);
	return bank;
}

int GBC_CART::mbc1_ram_bank_() const {
	// In mode 1, RAM bank = mbc1_upper_ (for 32 KiB RAM carts). In mode 0, bank 0.
	if (ram_bank_count_ <= 1) return 0;
	return mbc1_mode_ ? (mbc1_upper_ & (ram_bank_count_ - 1)) : 0;
}

// ---- read_rom ----

BYTE GBC_CART::read_rom(const WORD addr) const {
	switch (mapper_) {
		case Mapper::None:
			if (addr < rom_.size()) return rom_[addr];
			return 0xFF;

		case Mapper::MBC1: {
			if (addr < 0x4000) {
				const int bank = mbc1_rom_bank_lo_region_();
				const size_t offset = bank * 0x4000 + addr;
				return offset < rom_.size() ? rom_[offset] : 0xFF;
			}
			const int bank = mbc1_rom_bank_hi_region_();
			const size_t offset = bank * 0x4000 + (addr - 0x4000);
			return offset < rom_.size() ? rom_[offset] : 0xFF;
		}

		case Mapper::MBC2: {
			if (addr < 0x4000) return rom_[addr];
			BYTE bank = mbc1_rom_lo_ & 0x0F; // reuse mbc1_rom_lo_ as MBC2 bank reg
			if (bank == 0) bank = 1;
			bank &= (rom_bank_count_ - 1);
			const size_t offset = bank * 0x4000 + (addr - 0x4000);
			return offset < rom_.size() ? rom_[offset] : 0xFF;
		}

		case Mapper::MBC3: {
			if (addr < 0x4000) return rom_[addr];
			BYTE bank = mbc3_rom_bank_ & 0x7F;
			if (bank == 0) bank = 1;
			bank &= (rom_bank_count_ - 1);
			const size_t offset = bank * 0x4000 + (addr - 0x4000);
			return offset < rom_.size() ? rom_[offset] : 0xFF;
		}

		case Mapper::MBC5: {
			if (addr < 0x4000) return rom_[addr];
			const int bank = mbc5_rom_bank_ & (rom_bank_count_ - 1);
			const size_t offset = bank * 0x4000 + (addr - 0x4000);
			return offset < rom_.size() ? rom_[offset] : 0xFF;
		}
	}
	return 0xFF;
}

// ---- write_rom (MBC control registers) ----

void GBC_CART::write_rom(const WORD addr, const BYTE val) {
	switch (mapper_) {
		case Mapper::None:
			return; // ROM-only carts ignore writes.

		case Mapper::MBC1:
			if (addr < 0x2000) {
				ram_enable_ = (val & 0x0F) == 0x0A;
			} else if (addr < 0x4000) {
				mbc1_rom_lo_ = val & 0x1F;
				if (mbc1_rom_lo_ == 0) mbc1_rom_lo_ = 1; // 0->1 quirk applied at write time
			} else if (addr < 0x6000) {
				mbc1_upper_ = val & 0x03;
			} else if (addr < 0x8000) {
				mbc1_mode_ = (val & 0x01) != 0;
			}
			return;

		case Mapper::MBC2:
			// In MBC2, address bit 8 selects which register: 0 = RAM enable, 1 = ROM bank.
			if (addr < 0x4000) {
				if ((addr & 0x0100) == 0) {
					ram_enable_ = (val & 0x0F) == 0x0A;
				} else {
					BYTE bank = val & 0x0F;
					if (bank == 0) bank = 1;
					mbc1_rom_lo_ = bank; // reuse field as MBC2 bank
				}
			}
			return;

		case Mapper::MBC3:
			if (addr < 0x2000) {
				// RAM + RTC enable share this region.
				ram_enable_ = (val & 0x0F) == 0x0A;
			} else if (addr < 0x4000) {
				mbc3_rom_bank_ = val & 0x7F;
				if (mbc3_rom_bank_ == 0) mbc3_rom_bank_ = 1;
			} else if (addr < 0x6000) {
				mbc3_ram_rtc_select_ = val;
			} else if (addr < 0x8000) {
				// Latch: 0 → 1 transition snapshots the live RTC into the latched copy.
				if (mbc3_latch_prev_ == 0x00 && val == 0x01) {
					for (int i = 0; i < 5; ++i) mbc3_rtc_latched_[i] = mbc3_rtc_[i];
				}
				mbc3_latch_prev_ = val;
			}
			return;

		case Mapper::MBC5:
			if (addr < 0x2000) {
				ram_enable_ = (val & 0x0F) == 0x0A;
			} else if (addr < 0x3000) {
				mbc5_rom_bank_ = (mbc5_rom_bank_ & 0x100) | val;
			} else if (addr < 0x4000) {
				mbc5_rom_bank_ = (mbc5_rom_bank_ & 0xFF) | ((val & 0x01) << 8);
			} else if (addr < 0x6000) {
				mbc5_ram_bank_ = val & 0x0F;
			}
			// 0x6000-0x7FFF unused on MBC5.
			return;
	}
}

// ---- read_ram / write_ram ----

BYTE GBC_CART::read_ram(const WORD addr) const {
	// MBC3 with RTC selected can read even when no RAM exists; gate that path differently.
	if (mapper_ == Mapper::MBC3 && ram_enable_ && mbc3_ram_rtc_select_ >= 0x08 && mbc3_ram_rtc_select_ <= 0x0C) {
		return mbc3_rtc_latched_[mbc3_ram_rtc_select_ - 0x08];
	}

	if (!ram_enable_ || ram_.empty()) return 0xFF;

	const WORD off = addr - 0xA000;

	switch (mapper_) {
		case Mapper::None:
			return off < ram_.size() ? ram_[off] : 0xFF;

		case Mapper::MBC1: {
			const int bank = mbc1_ram_bank_();
			const size_t idx = bank * 0x2000 + off;
			return idx < ram_.size() ? ram_[idx] : 0xFF;
		}

		case Mapper::MBC2: {
			// 512 nibbles, mirrored every 0x200. High nibble reads as 1.
			const size_t idx = off & 0x1FF;
			return ram_[idx] | 0xF0;
		}

		case Mapper::MBC3: {
			const int bank = ram_bank_count_ > 0 ? (mbc3_ram_rtc_select_ & 0x03 & (ram_bank_count_ - 1)) : 0;
			const size_t idx = bank * 0x2000 + off;
			return idx < ram_.size() ? ram_[idx] : 0xFF;
		}

		case Mapper::MBC5: {
			const int bank = ram_bank_count_ > 0 ? (mbc5_ram_bank_ & (ram_bank_count_ - 1)) : 0;
			const size_t idx = bank * 0x2000 + off;
			return idx < ram_.size() ? ram_[idx] : 0xFF;
		}
	}
	return 0xFF;
}

void GBC_CART::write_ram(const WORD addr, const BYTE val) {
	// MBC3 RTC writes don't depend on cart RAM.
	if (mapper_ == Mapper::MBC3 && ram_enable_ && mbc3_ram_rtc_select_ >= 0x08 && mbc3_ram_rtc_select_ <= 0x0C) {
		mbc3_rtc_[mbc3_ram_rtc_select_ - 0x08] = val;
		return;
	}

	if (!ram_enable_ || ram_.empty()) return;

	const WORD off = addr - 0xA000;

	switch (mapper_) {
		case Mapper::None:
			if (off < ram_.size()) ram_[off] = val;
			return;

		case Mapper::MBC1: {
			const int bank = mbc1_ram_bank_();
			const size_t idx = bank * 0x2000 + off;
			if (idx < ram_.size()) ram_[idx] = val;
			return;
		}

		case Mapper::MBC2: {
			const size_t idx = off & 0x1FF;
			ram_[idx] = val & 0x0F; // only low nibble retained
			return;
		}

		case Mapper::MBC3: {
			const int bank = ram_bank_count_ > 0 ? (mbc3_ram_rtc_select_ & 0x03 & (ram_bank_count_ - 1)) : 0;
			const size_t idx = bank * 0x2000 + off;
			if (idx < ram_.size()) ram_[idx] = val;
			return;
		}

		case Mapper::MBC5: {
			const int bank = ram_bank_count_ > 0 ? (mbc5_ram_bank_ & (ram_bank_count_ - 1)) : 0;
			const size_t idx = bank * 0x2000 + off;
			if (idx < ram_.size()) ram_[idx] = val;
			return;
		}
	}
}