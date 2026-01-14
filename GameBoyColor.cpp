#include "GameBoyColor.h"

/**
 * For development reference:
 * Set all registers to required values.
 * Load the game data into cartridge memory.
 *
 * Later: Allow the user to choose between generic setup or Boot ROM setup.
 */
void GameBoyColor::CPUReset(const bool useBootROM) {
	if (!useBootROM) {
		// Initialize all registers.
		m_AF = 0x1180;
		m_BC = 0x0000;
		m_DE = 0xFF56;
		m_HL = 0x000D;

		// Initialize RAM values.
		std::fill(std::begin(m_WorkRAM) + 0xC000, std::end(m_WorkRAM) - 0x0001, 0x00);
		std::ranges::fill(m_VideoRAM, 0x00);

		// Initialize Stack Pointer and Program Counter.
		m_ProgramCounter = 0x0100;
		m_StackPointer = 0xFFFE;
	} else {
		// TODO: Read in BootROM, and initialize all registers.
		m_AF = m_BC = m_DE = m_HL = 0x0000;

	}
	// TODO: Read in GameBoy file and push it to the Cartridge ROM.
	std::string path;
	std::cout << "Please enter the path to the desired .gb or .gbc file." << std::endl;
	std::cin >> path;

	FILE* cart = std::fopen(path.c_str(), "rb");
	while (!cart) {
		std::cout << "Please try again." << std::endl;
		std::cin >> path;
		cart = std::fopen(path.c_str(), "rb");
	}

	// Read in and set ROM size.
	/*switch (std::fseek(cart, 0x0149, SEEK_SET)) {
		case 0x02:
			CartridgeRAMBytes = 0x2000;
		case 0x03:
			CartridgeRAMBytes = 0x8000;
		case 0x04:
			CartridgeRAMBytes = 0x20000;
		case 0x05:
			CartridgeRAMBytes = 0x10000;
		default:
			CartridgeRAMBytes = 0;
	}*/
	initAndLoadCartridge(path);

	/*const BYTE ROMCode = std::fseek(cart, 0x0148, SEEK_SET);
	CartridgeROMBytes = 0x8000 * (1 << ROMCode);*/

	/*
	// Read in and set RAM size.
	switch (std::fseek(cart, 0x0149, SEEK_SET)) {
		case 0x02:
			CartridgeRAMBytes = 0x2000;
		case 0x03:
			CartridgeRAMBytes = 0x8000;
		case 0x04:
			CartridgeRAMBytes = 0x20000;
		case 0x05:
			CartridgeRAMBytes = 0x10000;
		default:
			CartridgeRAMBytes = 0;
	}*/
	initCartridgeRAM(path);

	// Once done, quit

	fclose(cart);
}


GameBoyColor::WORD GameBoyColor::getNextOpcode() {

}

void GameBoyColor::execute() {

}

void GameBoyColor::initAndLoadCartridge(const std::string &romPath) {
	std::ifstream cart(romPath, std::ios::in | std::ios::binary);

	// Fetch ROM code (CartridgeROMCode)
	cart.seekg(0x0148);
	cart.read(reinterpret_cast<char*>(CartridgeROMCode), 2); // Is the number right?

	// Dump file contents into CartridgeROM
	/*cart.seekg(std::ios::end);
	const int size = cart.tellg();
	CartridgeROM.resize(size);
	cart.read(reinterpret_cast<char*>(CartridgeROM.data()), size);*/

	cart.close();
}

void GameBoyColor::initCartridgeRAM(const std::string &romPath) {
	std::ifstream cart(romPath, std::ios::in | std::ios::binary);

	// Fetch RAM code
	cart.seekg(0x0149);
	cart.read(reinterpret_cast<char*>(CartridgeRAMCode), 2);
	switch (CartridgeRAMCode) {
		case 0x02:
			CartridgeRAMBytes = static_cast<BYTE>(0x2000);
			CartridgeRAM.resize(0x2000);
			return;
		case 0x03:
			CartridgeRAMBytes = static_cast<BYTE>(0x8000);
			CartridgeRAM.resize(0x8000);
			return;
		case 0x04:
			CartridgeRAMBytes = static_cast<BYTE>(0x20000);
			CartridgeRAM.resize(0x20000);
			return;
		case 0x05:
			CartridgeRAMBytes = static_cast<BYTE>(0x10000);
			CartridgeRAM.resize(0x10000);
			return;
		default:
			CartridgeRAM.resize(0);
	}
}
