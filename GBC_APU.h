//
// GBC_APU — 4-channel sound (SQ1+sweep, SQ2, WAVE, NOISE).
// Pragmatic-accuracy: covers length/envelope/sweep, frame sequencer at
// 512 Hz, 48 kHz S16 stereo output. Doesn't model wave-RAM access glitches
// or sub-step length-counter quirks (Mooneye APU tests will not all pass).
//

#ifndef KEHINDES_GAMEBOYCOLOR_EMULATOR_GBC_APU_H
#define KEHINDES_GAMEBOYCOLOR_EMULATOR_GBC_APU_H

#include "GBC_Types.h"
#include <array>
#include <vector>
#include <cstdint>

class GBC_APU {
public:
	void resetPostBoot();
	void tick(int cycles);                           // T-cycles
	BYTE read(WORD addr);
	void write(WORD addr, BYTE val);

	// Pop accumulated stereo S16 samples (interleaved L,R).
	void drain_samples(std::vector<int16_t>& out);

private:
	static constexpr int CPU_FREQ    = 4194304;
	static constexpr int SAMPLE_RATE = 48000;

	bool power_   = true;
	BYTE nr50_    = 0x77;
	BYTE nr51_    = 0xF3;

	// Frame sequencer: 8192 T-cycles per step, 8 steps.
	int  fs_counter_ = 0;
	int  fs_step_    = 0;

	// Sample emitter: emit when sample_acc_ rolls over CPU_FREQ.
	int  sample_acc_ = 0;
	std::vector<int16_t> samples_;

	// DC-blocking high-pass filter state (per side). Single-pole IIR:
	//   y[n] = x[n] - x[n-1] + alpha * y[n-1]
	// alpha ~ 0.9925 at 48 kHz gives ~60 Hz cutoff — kills the DC offset and
	// damps the transient pops on channel triggers / envelope steps without
	// audibly affecting bass.
	float hpf_x_prev_l_ = 0.0f, hpf_y_prev_l_ = 0.0f;
	float hpf_x_prev_r_ = 0.0f, hpf_y_prev_r_ = 0.0f;

	// ---- Channel 1 / 2 (square + envelope; ch1 also sweep)
	struct Square {
		bool enabled = false;
		bool dac_on  = false;
		BYTE duty    = 0;
		int  duty_pos = 0;
		int  freq     = 0;          // 11-bit
		int  freq_timer = 0;
		int  length    = 0;          // counts down; 64 max
		bool length_enable = false;
		// envelope
		BYTE env_initial = 0;
		bool env_add     = false;
		BYTE env_period  = 0;
		BYTE env_timer   = 0;
		BYTE volume      = 0;
		// register shadows (for read-back)
		BYTE nrx1_raw = 0;           // duty + length-load
		BYTE nrx2_raw = 0;           // envelope register
		BYTE nrx4_raw = 0;           // freq-hi + trigger + length-en
	};
	Square ch1_{}, ch2_{};

	// ch1 sweep
	BYTE sw_period_ = 0, sw_shift_ = 0;
	bool sw_negate_ = false;
	BYTE sw_timer_  = 0;
	bool sw_enabled_ = false;
	int  sw_shadow_  = 0;
	BYTE nr10_raw_   = 0;

	// ---- Channel 3 (wave)
	struct {
		bool enabled = false;
		bool dac_on  = false;
		int  freq    = 0;
		int  freq_timer = 0;
		int  pos     = 0;            // 0..31 nibble index
		int  length  = 0;            // 256 max
		bool length_enable = false;
		BYTE volume_code = 0;
		BYTE sample_buffer = 0;
		BYTE nr30_raw = 0;
		BYTE nr32_raw = 0;
		BYTE nr34_raw = 0;
	} ch3_{};
	std::array<BYTE, 16> wave_ram_{};

	// ---- Channel 4 (noise)
	struct {
		bool enabled = false;
		bool dac_on  = false;
		int  freq_timer = 0;
		WORD lfsr   = 0x7FFF;
		bool width_mode = false;
		BYTE divisor_code = 0;
		BYTE shift = 0;
		int  length = 0;             // 64 max
		bool length_enable = false;
		BYTE env_initial = 0;
		bool env_add     = false;
		BYTE env_period  = 0;
		BYTE env_timer   = 0;
		BYTE volume      = 0;
		BYTE nr42_raw = 0;
		BYTE nr43_raw = 0;
		BYTE nr44_raw = 0;
	} ch4_{};

	// helpers
	void step_length_();
	void step_sweep_();
	void step_envelope_();
	int  sweep_calc_();
	void trigger_ch1_();
	void trigger_ch2_();
	void trigger_ch3_();
	void trigger_ch4_();
	void emit_sample_();
	void clear_regs_();
};

#endif //KEHINDES_GAMEBOYCOLOR_EMULATOR_GBC_APU_H
