//
// Created by Kehinde Adeoso on 2/14/26.
//

#include "GBC_Cart.h"

#include "GBC_MMU.h"

BYTE GBC_Cart::read_rom(WORD addr) const {
	return c_CartridgeROM[addr];
}
