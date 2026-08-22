//
// Created by Kehinde Adeoso on 4/22/26.
//

#ifndef KEHINDES_GAMEBOYCOLOR_EMULATOR_TESTCART_H
#define KEHINDES_GAMEBOYCOLOR_EMULATOR_TESTCART_H

#include "../GBC_Types.h"
#include <array>
#include <string>


class TestCart {
public:
	explicit TestCart(const std::string& romPath);
	BYTE read_rom(WORD addr) const;
	void write_rom(WORD addr, BYTE value);

	BYTE read_ram(WORD addr) const;
	void write_ram(WORD addr, BYTE value);



private:
	std::array<BYTE, 0x8000> rom_{};
	std::array<BYTE, 0x2000> ram_{};
};



#endif //KEHINDES_GAMEBOYCOLOR_EMULATOR_TESTCART_H
