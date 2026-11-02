#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <filesystem>
#include <gtest/gtest-param-test.h>
#include <gtest/gtest.h>
#include "GBC_APU.h"
#include "GBC_BUS.h"
#include "GBC_CART.h"
#include "test_utils.h"
#include "GBC_CPU.h"
#include "GBC_PPU.h"
#include "GBC_TIMER.h"
#include "test-shims/TestBus.h"
namespace fs = std::filesystem;

TEST(CgbAcid2Test, MatchesReference) {
	fs::path rom = fs::path(GBC_REPO_ROOT) / "p-docs/p-test/cgb-acid2/build/cgb-acid2.gbc";
	fs::path reference = fs::path(GBC_REPO_ROOT) / "p-docs/p-test/cgb-acid2/img/reference.png";

	GBC_PPU ppu{};
	GBC_CART cart(rom.string());
	GBC_TIMER timer{};
	GBC_APU apu{};
	GBC_BUS bus("", cart, ppu, timer, apu);
	GBC_CPU cpu(bus);
	for (int i = 0; i < 1000000; i++) {
		const int cycles = cpu.execute();
		ppu.tick(cycles);
		apu.tick(cycles);
		timer.tick(cycles);

		const int irq_cycles = cpu.serviceInterrupts();
		if (irq_cycles) {
			ppu.tick(irq_cycles);
			timer.tick(irq_cycles);
			apu.tick(irq_cycles);
		}
	}

	int width, height, channels;
	unsigned char* pixels = stbi_load(
		reference.string().c_str(),
		&width,
		&height,
		&channels,
		3);

	ASSERT_NE(pixels, nullptr) << stbi_failure_reason();

	bool match = true;
	auto framebuffer = ppu.framebuffer_view();
	for (int i = 0; i < framebuffer.size(); i++) {
		// get channels
		const int r5 = framebuffer[i] & 0x1F;
		const int g5 = (framebuffer[i] >> 5) & 0x1F;
		const int b5 = (framebuffer[i] >> 10) & 0x1F;

		// convert 5-bit channels to 8-bit
		// see cgb-acid2 README
		const int r8 = (r5 << 3) | (r5 >> 2);
		const int g8 = (g5 << 3) | (g5 >> 2);
		const int b8 = (b5 << 3) | (b5 >> 2);

		// compare channels
		// if they differ, match=false && break
		if (r8 != pixels[i*3+0] || g8 != pixels[i*3+1] || b8 != pixels[i*3+2]) {
			match = false;
			break;
		}
	}
	stbi_image_free(pixels);
	EXPECT_TRUE(match);
}

