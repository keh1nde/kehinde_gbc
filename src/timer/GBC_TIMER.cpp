//
// Created by Kehinde Adeoso on 5/4/26.
//

#include "GBC_TIMER.h"

void GBC_TIMER::resetPostBoot() {
	div_counter_ = 0xABCC;
	tima_ = 0;
	tma_ = 0;
	tac_ = 0xF8;
	tima_sub_ = 0;
}

void GBC_TIMER::tick(const BYTE cycles) {
	div_counter_ += cycles;

	if (tac_ & 0x04) {
		tima_sub_ += cycles;
		const WORD period = tima_period_();
		while (tima_sub_ >= period) {
			tima_sub_ -= period;
			if (tima_ == 0xFF) {
				tima_ = tma_;
				bus_->requestInterrupt(Interrupt::Timer);
			} else {
				tima_++;
			}
		}
	}

}

BYTE GBC_TIMER::read(const WORD addr) const {
	switch (addr) {
		case 0xFF04: return (div_counter_ >> 8) & 0xFF;
		case 0xFF05: return tima_;
		case 0xFF06: return tma_;
		case 0xFF07: return tac_ | 0xF8;
		default: return 0xFF;

	}
}

void GBC_TIMER::write(const WORD addr, const BYTE val) {
	switch (addr) {
		case 0xFF04: div_counter_ = 0; tima_sub_ = 0; break; // Any write makes counters zero
		case 0xFF05: tima_ = val; break;
		case 0xFF06: tma_ = val; break;
		case 0xFF07: tac_ = val & 0x07; break;
		default: return;
	}
}

WORD GBC_TIMER::tima_period_() const {
	switch (tac_ & 0x03) {
		case 0b00: return 1024;
		case 0b01: return 16;
		case 0b10: return 64;
		case 0b11: return 256;
		default: return 1024;
	}

}
