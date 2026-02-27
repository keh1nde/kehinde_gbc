//
// Created by Kehinde Adeoso on 2/14/26.
//

#include "GBC_CART.h"

#include "GBC_BUS.h"

BYTE GBC_CART::read_rom(WORD addr) const {
	return c_CartridgeROM[addr];
}
