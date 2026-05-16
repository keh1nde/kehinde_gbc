// Emscripten entry point. Mirrors main.cpp but lets the browser drive frames
// via emscripten_set_main_loop instead of an SDL_Delay-paced while loop.
//
// The ROM is bundled into the WASM at build time via `--preload-file`, mounted
// at /rom.gbc inside the Emscripten virtual FS.

#include "GBC_BUS.h"
#include "GBC_CPU.h"
#include "GBC_PPU.h"
#include "GBC_CART.h"
#include "GBC_TIMER.h"
#include "GBC_APU.h"
#include "GBC_DISPLAY.h"

#include <vector>
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace {

struct WebState {
	GBC_PPU* ppu = nullptr;
	GBC_CART* cart = nullptr;
	GBC_TIMER* timer = nullptr;
	GBC_APU* apu = nullptr;
	GBC_BUS* bus = nullptr;
	GBC_CPU* cpu = nullptr;
	GBC_DISPLAY* display = nullptr;
	SDL_AudioDeviceID audio_dev = 0;
	std::vector<int16_t> sample_buf;
	bool running = true;
};

WebState g;

// One browser RAF tick: advance the emulator until a single frame is ready,
// present, drain audio, return. The browser handles pacing — no SDL_Delay.
void web_frame() {
	if (!g.running) {
#ifdef __EMSCRIPTEN__
		emscripten_cancel_main_loop();
#endif
		return;
	}

	try {
		// 70224 T-cycles per frame; ~17.5k instructions worst case. 1M is
		// orders-of-magnitude safety margin against a runaway no-frame state.
		for (int safety = 0; safety < 1'000'000; ++safety) {
			const int cycles = g.cpu->execute();
			g.ppu->tick(cycles);
			g.timer->tick(cycles);
			g.apu->tick(cycles);

			const int irq_cycles = g.cpu->serviceInterrupts();
			if (irq_cycles) {
				g.ppu->tick(irq_cycles);
				g.timer->tick(irq_cycles);
				g.apu->tick(irq_cycles);
			}

			if (g.ppu->consume_frame_ready()) {
				g.display->present(g.ppu->framebuffer_view());
				g.running = g.display->pump();
				g.bus->set_buttons(g.display->poll_input());

				g.sample_buf.clear();
				g.apu->drain_samples(g.sample_buf);
				if (!g.sample_buf.empty() && g.audio_dev != 0) {
					constexpr Uint32 kMaxQueueBytes = 48000 * 2 * 2 / 10; // ~100ms
					if (SDL_GetQueuedAudioSize(g.audio_dev) < kMaxQueueBytes) {
						SDL_QueueAudio(g.audio_dev, g.sample_buf.data(),
							static_cast<Uint32>(g.sample_buf.size() * sizeof(int16_t)));
					}
				}
				return;
			}
		}
		std::cerr << "web_frame: hit safety cap without frame_ready\n";
	} catch (const std::exception& e) {
		std::cerr << "Caught: " << e.what() << "\n";
		g.running = false;
	}
}

} // namespace

int main(int /*argc*/, char* /*argv*/[]) {
	const std::string romPath = "/rom.gbc";

	static GBC_PPU ppu{};
	static GBC_CART cart(romPath);
	static GBC_TIMER timer{};
	static GBC_APU apu{};
	static GBC_BUS bus("", cart, ppu, timer, apu);
	static GBC_CPU cpu(bus);

	ppu.set_bus(&bus);
	timer.set_bus(&bus);

	cpu.resetPostBoot();
	ppu.resetPostBoot();
	timer.resetPostBoot();
	apu.resetPostBoot();

	static GBC_DISPLAY display;
	if (!display.init(4)) {
		std::cerr << "display.init failed\n";
		return 1;
	}

	SDL_AudioDeviceID audio_dev = 0;
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
		std::cerr << "SDL_InitSubSystem(AUDIO) failed: " << SDL_GetError() << "\n";
	} else {
		SDL_AudioSpec want{}, have{};
		want.freq = 48000;
		want.format = AUDIO_S16SYS;
		want.channels = 2;
		want.samples = 1024;
		want.callback = nullptr;
		audio_dev = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
		if (audio_dev == 0) {
			std::cerr << "SDL_OpenAudioDevice failed: " << SDL_GetError() << "\n";
		} else {
			SDL_PauseAudioDevice(audio_dev, 0);
		}
	}

	g.ppu = &ppu;
	g.cart = &cart;
	g.timer = &timer;
	g.apu = &apu;
	g.bus = &bus;
	g.cpu = &cpu;
	g.display = &display;
	g.audio_dev = audio_dev;
	g.running = true;
	g.sample_buf.reserve(4096);

#ifdef __EMSCRIPTEN__
	// 0 fps → use requestAnimationFrame (~display refresh, typically 60Hz).
	// simulate_infinite_loop=1 → never returns from main, keeps stack alive.
	emscripten_set_main_loop(web_frame, 0, 1);
#else
	while (g.running) web_frame();
#endif

	if (audio_dev != 0) SDL_CloseAudioDevice(audio_dev);
	return 0;
}