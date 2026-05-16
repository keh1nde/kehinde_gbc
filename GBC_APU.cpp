//
// GBC_APU implementation.
//

#include "GBC_APU.h"
#include <algorithm>

namespace {
	// 4 duty patterns, 8 phases each. 1 = high, 0 = low.
	constexpr BYTE kDuty[4][8] = {
		{0,0,0,0,0,0,0,1},  // 12.5%
		{1,0,0,0,0,0,0,1},  // 25%
		{1,0,0,0,0,1,1,1},  // 50%
		{0,1,1,1,1,1,1,0},  // 75%
	};

	// Noise LFSR divisor table.
	constexpr int kNoiseDivisor[8] = {8, 16, 32, 48, 64, 80, 96, 112};

	// Wave-channel volume right-shift per volume_code (0=mute via shift>=4).
	constexpr int kWaveShift[4] = {4, 0, 1, 2};
}

void GBC_APU::clear_regs_() {
	ch1_ = {};
	ch2_ = {};
	ch3_ = {};
	ch4_ = {};
	sw_period_ = sw_shift_ = sw_timer_ = 0;
	sw_negate_ = sw_enabled_ = false;
	sw_shadow_ = 0;
	nr10_raw_ = 0;
	nr50_ = 0;
	nr51_ = 0;
	fs_counter_ = 0;
	fs_step_ = 0;
}

void GBC_APU::resetPostBoot() {
	power_ = true;
	nr50_  = 0x77;
	nr51_  = 0xF3;
	fs_counter_ = 0;
	fs_step_    = 0;
	sample_acc_ = 0;
	samples_.clear();
	ch1_ = {}; ch2_ = {}; ch3_ = {}; ch4_ = {};
	wave_ram_.fill(0);
}

void GBC_APU::drain_samples(std::vector<int16_t>& out) {
	out.insert(out.end(), samples_.begin(), samples_.end());
	samples_.clear();
}

// ---- frame-sequencer steps ----

void GBC_APU::step_length_() {
	auto step_sq = [](Square& s){
		if (s.length_enable && s.length > 0) {
			if (--s.length == 0) s.enabled = false;
		}
	};
	step_sq(ch1_);
	step_sq(ch2_);
	if (ch3_.length_enable && ch3_.length > 0) {
		if (--ch3_.length == 0) ch3_.enabled = false;
	}
	if (ch4_.length_enable && ch4_.length > 0) {
		if (--ch4_.length == 0) ch4_.enabled = false;
	}
}

int GBC_APU::sweep_calc_() {
	int delta = sw_shadow_ >> sw_shift_;
	int next  = sw_negate_ ? sw_shadow_ - delta : sw_shadow_ + delta;
	if (next > 2047) ch1_.enabled = false;
	return next;
}

void GBC_APU::step_sweep_() {
	if (sw_timer_ > 0 && --sw_timer_ == 0) {
		sw_timer_ = sw_period_ ? sw_period_ : 8;
		if (sw_enabled_ && sw_period_ > 0) {
			int next = sweep_calc_();
			if (next <= 2047 && sw_shift_ > 0) {
				sw_shadow_ = next;
				ch1_.freq  = next;
				sweep_calc_(); // overflow re-check
			}
		}
	}
}

void GBC_APU::step_envelope_() {
	auto step = [](BYTE& timer, BYTE period, bool add, BYTE& vol){
		if (period == 0) return;
		if (timer > 0 && --timer == 0) {
			timer = period;
			if (add) { if (vol < 15) vol++; }
			else     { if (vol > 0)  vol--; }
		}
	};
	step(ch1_.env_timer, ch1_.env_period, ch1_.env_add, ch1_.volume);
	step(ch2_.env_timer, ch2_.env_period, ch2_.env_add, ch2_.volume);
	step(ch4_.env_timer, ch4_.env_period, ch4_.env_add, ch4_.volume);
}

// ---- triggers ----

void GBC_APU::trigger_ch1_() {
	ch1_.enabled    = ch1_.dac_on;
	if (ch1_.length == 0) ch1_.length = 64;
	ch1_.freq_timer = (2048 - ch1_.freq) * 4;
	ch1_.env_timer  = ch1_.env_period ? ch1_.env_period : 8;
	ch1_.volume     = ch1_.env_initial;

	sw_shadow_  = ch1_.freq;
	sw_timer_   = sw_period_ ? sw_period_ : 8;
	sw_enabled_ = (sw_period_ != 0) || (sw_shift_ != 0);
	if (sw_shift_ > 0) sweep_calc_(); // immediate overflow check
}

void GBC_APU::trigger_ch2_() {
	ch2_.enabled = ch2_.dac_on;
	if (ch2_.length == 0) ch2_.length = 64;
	ch2_.freq_timer = (2048 - ch2_.freq) * 4;
	ch2_.env_timer  = ch2_.env_period ? ch2_.env_period : 8;
	ch2_.volume     = ch2_.env_initial;
}

void GBC_APU::trigger_ch3_() {
	ch3_.enabled = ch3_.dac_on;
	if (ch3_.length == 0) ch3_.length = 256;
	ch3_.freq_timer = (2048 - ch3_.freq) * 2;
	ch3_.pos = 0;
}

void GBC_APU::trigger_ch4_() {
	ch4_.enabled = ch4_.dac_on;
	if (ch4_.length == 0) ch4_.length = 64;
	ch4_.lfsr = 0x7FFF;
	ch4_.env_timer = ch4_.env_period ? ch4_.env_period : 8;
	ch4_.volume    = ch4_.env_initial;
	const int div = kNoiseDivisor[ch4_.divisor_code];
	ch4_.freq_timer = div << ch4_.shift;
	if (ch4_.freq_timer == 0) ch4_.freq_timer = 8;
}

// ---- per-cycle stepping ----

void GBC_APU::tick(int cycles) {
	if (!power_) {
		// even powered-off, we still need to emit (silent) samples to keep audio queue paced.
		sample_acc_ += SAMPLE_RATE * cycles;
		while (sample_acc_ >= CPU_FREQ) {
			sample_acc_ -= CPU_FREQ;
			samples_.push_back(0);
			samples_.push_back(0);
		}
		return;
	}

	for (int i = 0; i < cycles; ++i) {
		// frame sequencer @ 512 Hz (every 8192 cycles)
		if (++fs_counter_ >= 8192) {
			fs_counter_ = 0;
			switch (fs_step_) {
				case 0: step_length_(); break;
				case 2: step_length_(); step_sweep_(); break;
				case 4: step_length_(); break;
				case 6: step_length_(); step_sweep_(); break;
				case 7: step_envelope_(); break;
				default: break;
			}
			fs_step_ = (fs_step_ + 1) & 7;
		}

		// channel timers (1 T-cycle per loop iter)
		if (--ch1_.freq_timer <= 0) {
			ch1_.freq_timer = (2048 - ch1_.freq) * 4;
			ch1_.duty_pos = (ch1_.duty_pos + 1) & 7;
		}
		if (--ch2_.freq_timer <= 0) {
			ch2_.freq_timer = (2048 - ch2_.freq) * 4;
			ch2_.duty_pos = (ch2_.duty_pos + 1) & 7;
		}
		if (--ch3_.freq_timer <= 0) {
			ch3_.freq_timer = (2048 - ch3_.freq) * 2;
			ch3_.pos = (ch3_.pos + 1) & 31;
			BYTE byte = wave_ram_[ch3_.pos >> 1];
			ch3_.sample_buffer = (ch3_.pos & 1) ? (byte & 0x0F) : (byte >> 4);
		}
		if (--ch4_.freq_timer <= 0) {
			const int div = kNoiseDivisor[ch4_.divisor_code];
			ch4_.freq_timer = div << ch4_.shift;
			if (ch4_.freq_timer == 0) ch4_.freq_timer = 8;
			const WORD x = (ch4_.lfsr & 1) ^ ((ch4_.lfsr >> 1) & 1);
			ch4_.lfsr = (ch4_.lfsr >> 1) | (x << 14);
			if (ch4_.width_mode) ch4_.lfsr = (ch4_.lfsr & ~0x40) | (x << 6);
		}

		// sample emit (Bresenham)
		sample_acc_ += SAMPLE_RATE;
		if (sample_acc_ >= CPU_FREQ) {
			sample_acc_ -= CPU_FREQ;
			emit_sample_();
		}
	}
}

void GBC_APU::emit_sample_() {
	// Per-channel DAC outputs: 0..15 (0 if disabled or DAC off).
	const int o1 = (ch1_.enabled && ch1_.dac_on)
		? (kDuty[ch1_.duty][ch1_.duty_pos] ? ch1_.volume : 0) : 0;
	const int o2 = (ch2_.enabled && ch2_.dac_on)
		? (kDuty[ch2_.duty][ch2_.duty_pos] ? ch2_.volume : 0) : 0;
	const int o3 = (ch3_.enabled && ch3_.dac_on)
		? (ch3_.sample_buffer >> kWaveShift[ch3_.volume_code]) : 0;
	const int o4 = (ch4_.enabled && ch4_.dac_on)
		? (((~ch4_.lfsr) & 1) ? ch4_.volume : 0) : 0;

	// NR51: bits 4-7 = left enables for ch1..ch4, bits 0-3 = right enables.
	int left  = 0, right = 0;
	if (nr51_ & 0x10) left  += o1;
	if (nr51_ & 0x20) left  += o2;
	if (nr51_ & 0x40) left  += o3;
	if (nr51_ & 0x80) left  += o4;
	if (nr51_ & 0x01) right += o1;
	if (nr51_ & 0x02) right += o2;
	if (nr51_ & 0x04) right += o3;
	if (nr51_ & 0x08) right += o4;

	// Per-side master volume: bits 4-6 left, 0-2 right (each 0..7, +1 → 1..8).
	const int vl = ((nr50_ >> 4) & 0x07) + 1;
	const int vr = ( nr50_       & 0x07) + 1;

	// left/right are 0..(15*4)=60. * volume (1..8) → 0..480. Scale into int16.
	// Headroom: max 480; multiply by ~60 to land near 28800. /4 channels would
	// be ~7200/side. Use a conservative gain.
	constexpr int kGain = 40;
	const float xl = static_cast<float>((left  - 30) * vl * kGain);
	const float xr = static_cast<float>((right - 30) * vr * kGain);

	// DC-blocking high-pass filter (see GBC_APU.h for derivation). The (left - 30)
	// pre-bias gets the filter near its steady-state quickly; the filter then handles
	// residual DC + the transient pops on channel triggers / envelope changes.
	constexpr float kAlpha = 0.9925f;
	const float yl = xl - hpf_x_prev_l_ + kAlpha * hpf_y_prev_l_;
	const float yr = xr - hpf_x_prev_r_ + kAlpha * hpf_y_prev_r_;
	hpf_x_prev_l_ = xl;
	hpf_y_prev_l_ = yl;
	hpf_x_prev_r_ = xr;
	hpf_y_prev_r_ = yr;

	const int sl = std::clamp(static_cast<int>(yl), -32768, 32767);
	const int sr = std::clamp(static_cast<int>(yr), -32768, 32767);

	samples_.push_back(static_cast<int16_t>(sl));
	samples_.push_back(static_cast<int16_t>(sr));
}

// ---- register I/O ----

BYTE GBC_APU::read(WORD addr) {
	if (addr >= 0xFF30 && addr <= 0xFF3F) {
		return wave_ram_[addr - 0xFF30];
	}

	switch (addr) {
		// Square 1
		case 0xFF10: return nr10_raw_      | 0x80;
		case 0xFF11: return ch1_.nrx1_raw  | 0x3F;
		case 0xFF12: return ch1_.nrx2_raw  | 0x00;
		case 0xFF13: return 0xFF;
		case 0xFF14: return ch1_.nrx4_raw  | 0xBF;

		// Square 2
		case 0xFF15: return 0xFF;
		case 0xFF16: return ch2_.nrx1_raw  | 0x3F;
		case 0xFF17: return ch2_.nrx2_raw  | 0x00;
		case 0xFF18: return 0xFF;
		case 0xFF19: return ch2_.nrx4_raw  | 0xBF;

		// Wave
		case 0xFF1A: return ch3_.nr30_raw  | 0x7F;
		case 0xFF1B: return 0xFF;
		case 0xFF1C: return ch3_.nr32_raw  | 0x9F;
		case 0xFF1D: return 0xFF;
		case 0xFF1E: return ch3_.nr34_raw  | 0xBF;

		// Noise
		case 0xFF1F: return 0xFF;
		case 0xFF20: return 0xFF;
		case 0xFF21: return ch4_.nr42_raw  | 0x00;
		case 0xFF22: return ch4_.nr43_raw  | 0x00;
		case 0xFF23: return ch4_.nr44_raw  | 0xBF;

		// Mixer
		case 0xFF24: return nr50_;
		case 0xFF25: return nr51_;
		case 0xFF26: {
			BYTE v = (power_ ? 0x80 : 0) | 0x70;
			if (ch1_.enabled) v |= 0x01;
			if (ch2_.enabled) v |= 0x02;
			if (ch3_.enabled) v |= 0x04;
			if (ch4_.enabled) v |= 0x08;
			return v;
		}

		default: return 0xFF;
	}
}

void GBC_APU::write(WORD addr, BYTE val) {
	// Wave RAM is always writable.
	if (addr >= 0xFF30 && addr <= 0xFF3F) {
		wave_ram_[addr - 0xFF30] = val;
		return;
	}

	// NR52 controls power; bit 7 toggles APU. While powered off, all writes to
	// FF10–FF25 are ignored (length-load on DMG is a quirk we skip).
	if (addr == 0xFF26) {
		const bool new_power = (val & 0x80) != 0;
		if (!new_power && power_) {
			clear_regs_();
		}
		power_ = new_power;
		return;
	}

	if (!power_) return;

	switch (addr) {
		// ---- NR10 sweep
		case 0xFF10: {
			nr10_raw_  = val;
			sw_period_ = (val >> 4) & 0x07;
			sw_negate_ = (val & 0x08) != 0;
			sw_shift_  = val & 0x07;
			break;
		}
		// ---- NR11 duty + length-load
		case 0xFF11: {
			ch1_.nrx1_raw = val;
			ch1_.duty   = (val >> 6) & 0x03;
			ch1_.length = 64 - (val & 0x3F);
			break;
		}
		// ---- NR12 envelope
		case 0xFF12: {
			ch1_.nrx2_raw    = val;
			ch1_.env_initial = (val >> 4) & 0x0F;
			ch1_.env_add     = (val & 0x08) != 0;
			ch1_.env_period  = val & 0x07;
			ch1_.dac_on      = (val & 0xF8) != 0;
			if (!ch1_.dac_on) ch1_.enabled = false;
			break;
		}
		// ---- NR13 freq lo
		case 0xFF13: {
			ch1_.freq = (ch1_.freq & 0x700) | val;
			break;
		}
		// ---- NR14 trigger / freq hi / length-en
		case 0xFF14: {
			ch1_.nrx4_raw = val;
			ch1_.freq = (ch1_.freq & 0xFF) | ((val & 0x07) << 8);
			ch1_.length_enable = (val & 0x40) != 0;
			if (val & 0x80) trigger_ch1_();
			break;
		}

		// ---- Square 2 mirrors NR21..NR24 (no sweep) — FF16..FF19
		case 0xFF16: {
			ch2_.nrx1_raw = val;
			ch2_.duty   = (val >> 6) & 0x03;
			ch2_.length = 64 - (val & 0x3F);
			break;
		}
		case 0xFF17: {
			ch2_.nrx2_raw    = val;
			ch2_.env_initial = (val >> 4) & 0x0F;
			ch2_.env_add     = (val & 0x08) != 0;
			ch2_.env_period  = val & 0x07;
			ch2_.dac_on      = (val & 0xF8) != 0;
			if (!ch2_.dac_on) ch2_.enabled = false;
			break;
		}
		case 0xFF18: {
			ch2_.freq = (ch2_.freq & 0x700) | val;
			break;
		}
		case 0xFF19: {
			ch2_.nrx4_raw = val;
			ch2_.freq = (ch2_.freq & 0xFF) | ((val & 0x07) << 8);
			ch2_.length_enable = (val & 0x40) != 0;
			if (val & 0x80) trigger_ch2_();
			break;
		}

		// ---- Wave (FF1A..FF1E)
		case 0xFF1A: {
			ch3_.nr30_raw = val;
			ch3_.dac_on = (val & 0x80) != 0;
			if (!ch3_.dac_on) ch3_.enabled = false;
			break;
		}
		case 0xFF1B: {
			ch3_.length = 256 - val;
			break;
		}
		case 0xFF1C: {
			ch3_.nr32_raw = val;
			ch3_.volume_code = (val >> 5) & 0x03;
			break;
		}
		case 0xFF1D: {
			ch3_.freq = (ch3_.freq & 0x700) | val;
			break;
		}
		case 0xFF1E: {
			ch3_.nr34_raw = val;
			ch3_.freq = (ch3_.freq & 0xFF) | ((val & 0x07) << 8);
			ch3_.length_enable = (val & 0x40) != 0;
			if (val & 0x80) trigger_ch3_();
			break;
		}

		// ---- Noise (FF20..FF23)
		case 0xFF20: {
			ch4_.length = 64 - (val & 0x3F);
			break;
		}
		case 0xFF21: {
			ch4_.nr42_raw    = val;
			ch4_.env_initial = (val >> 4) & 0x0F;
			ch4_.env_add     = (val & 0x08) != 0;
			ch4_.env_period  = val & 0x07;
			ch4_.dac_on      = (val & 0xF8) != 0;
			if (!ch4_.dac_on) ch4_.enabled = false;
			break;
		}
		case 0xFF22: {
			ch4_.nr43_raw     = val;
			ch4_.shift        = (val >> 4) & 0x0F;
			ch4_.width_mode   = (val & 0x08) != 0;
			ch4_.divisor_code = val & 0x07;
			break;
		}
		case 0xFF23: {
			ch4_.nr44_raw = val;
			ch4_.length_enable = (val & 0x40) != 0;
			if (val & 0x80) trigger_ch4_();
			break;
		}

		// ---- Mixer
		case 0xFF24: nr50_ = val; break;
		case 0xFF25: nr51_ = val; break;

		// FF15, FF1F unused.
		default: break;
	}
}
