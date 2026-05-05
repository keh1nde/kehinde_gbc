//
// Created by Kehinde Adeoso on 4/22/26.
//

#ifndef KEHINDES_GAMEBOYCOLOR_EMULATOR_TESTBUS_H
#define KEHINDES_GAMEBOYCOLOR_EMULATOR_TESTBUS_H

#include "GBC_Types.h"
#include <array>
#include <iostream>


class TestBus final: public IBus{
public:
	TestBus() = default;
	std::array<BYTE, 0x10000> mem_{};

	BYTE read8(WORD addr) override;
	void write8(WORD addr, BYTE data) override;

	WORD read16(WORD addr) override;
	void write16(WORD addr, WORD data) override;

	void requestInterrupt(Interrupt) override {}

private:

};



#endif //KEHINDES_GAMEBOYCOLOR_EMULATOR_TESTBUS_H
