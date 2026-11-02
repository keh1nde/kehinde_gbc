#include <filesystem>
#include <gtest/gtest-param-test.h>
#include <gtest/gtest.h>

#include "GBC_APU.h"
#include "test_utils.h"
#include "GBC_CPU.h"
#include "GBC_PPU.h"
#include "GBC_TIMER.h"
#include "GBC_CART.h"
#include "GBC_BUS.h"
#include "test-shims/TestBus.h"
namespace fs = std::filesystem;

// Get test utils (roms in this case)
std::vector<fs::path> mooneye_utils = get_utils(
	fs::path(GBC_REPO_ROOT) / "p-docs/p-test/mooneye-test-suite"
	, ".gb");

class MooneyeTest : public ::testing::TestWithParam<fs::path> {};

TEST_P(MooneyeTest, PassesROM) {
	const fs::path& rom = GetParam();
	GBC_PPU ppu{};
	GBC_CART cart(rom.string());
	GBC_TIMER timer{};
	GBC_APU apu{};
	GBC_BUS bus("", cart, ppu, timer, apu);
	GBC_CPU cpu(bus);
	for (int i = 0; i < 5000000; i++) {
		const int cycles = cpu.execute();
		ppu.tick(cycles);
		apu.tick(cycles);

		const int irq_cycles = cpu.serviceInterrupts();
		if (irq_cycles) {
			ppu.tick(irq_cycles);
			timer.tick(irq_cycles);
			apu.tick(irq_cycles);
		}
	}

	const std::string expected_pass{3, 5, 8, 13, 21, 34};
	EXPECT_NE(bus.serial_output().find(expected_pass), std::string::npos);
}

INSTANTIATE_TEST_SUITE_P(MoonEyeSuite, MooneyeTest, ::testing::ValuesIn(mooneye_utils));