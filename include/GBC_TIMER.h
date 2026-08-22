//
// Created by Kehinde Adeoso on 5/4/26.
//

#ifndef KEHINDES_GAMEBOYCOLOR_EMULATOR_GBC_TIMER_H
#define KEHINDES_GAMEBOYCOLOR_EMULATOR_GBC_TIMER_H
#include "GBC_Types.h"


class GBC_TIMER {
public:
	GBC_TIMER() = default;
	void resetPostBoot();
	void tick(BYTE cycles);
	BYTE read(WORD addr) const;
	void write(WORD addr, BYTE val);

	void set_bus(IBus* bus) { bus_ = bus; }
private:
	// Data members
	IBus* bus_ = nullptr;
	WORD div_counter_;
	WORD tima_sub_;
	BYTE tima_;
	BYTE tma_;
	BYTE tac_;
	// BYTE reload_delay_pending_; If I have time.

	// Helper functions
	WORD tima_period_() const;

};



#endif //KEHINDES_GAMEBOYCOLOR_EMULATOR_GBC_TIMER_H
