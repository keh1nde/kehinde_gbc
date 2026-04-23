//
// Created by Kehinde Adeoso on 4/22/26.
//

#include "TestBus.h"




BYTE TestBus::read8(const WORD addr) {
	if (addr == 0xFF44) return 0x90;
	return mem_[addr];
}

void TestBus::write8(const WORD addr, const BYTE data) {
	if (addr == 0xFF02 && data == 0x81){
		std::cout << static_cast<char>(mem_[0xFF01]) << std::flush;
	}
	mem_[addr] = data;
}

WORD TestBus::read16(const WORD addr) {
	return static_cast<WORD>(mem_[addr]) | (static_cast<WORD>(mem_[addr + 1]) << 8);
}

void TestBus::write16(const WORD addr, const WORD data) {
	mem_[addr] = static_cast<BYTE>(data & 0xFF);
	mem_[addr + 1] = static_cast<BYTE>((data >> 8) & 0xFF);
}