#include "GBC_BUS.h"
#include "GBC_CPU.h"
#include "GBC_PPU.h"
#include "GBC_CART.h"
#include "GBC_TIMER.h"
#include "GBC_APU.h"
#include "GBC_DISPLAY.h"
#include <vector>

int main(int argc, char* argv[]) {

	const std::string romPath = argv[1];

	GBC_PPU ppu{};
	GBC_CART cart(romPath);
	std::cerr << "header[0x143]=0x" << std::hex << static_cast<int>(cart.read_rom(0x0143)) <<
 "\n";

	GBC_TIMER timer{};
	GBC_APU apu{};
	GBC_BUS bus("", cart, ppu, timer, apu);
	GBC_CPU cpu(bus);

	ppu.set_bus(&bus);
	timer.set_bus(&bus);

	cpu.resetPostBoot();
	ppu.resetPostBoot();
	timer.resetPostBoot();
	apu.resetPostBoot();

	GBC_DISPLAY display;
	if (!display.init(4)) return 1;

	// SDL audio: 48 kHz S16 stereo. Pull from APU each frame.
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
		std::cerr << "SDL_InitSubSystem(AUDIO) failed: " << SDL_GetError() << std::endl;
		return 1;
	}
	SDL_AudioSpec want{}, have{};
	want.freq     = 48000;
	want.format   = AUDIO_S16SYS;
	want.channels = 2;
	want.samples  = 1024;
	want.callback = nullptr;
	const SDL_AudioDeviceID audio_dev = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
	if (audio_dev == 0) {
		std::cerr << "SDL_OpenAudioDevice failed: " << SDL_GetError() << std::endl;
		return 1;
	}
	SDL_PauseAudioDevice(audio_dev, 0);
	std::vector<int16_t> sample_buf;
	sample_buf.reserve(4096);

	// Frame pacing. Real GB frame: 70224 dots / 4194304 Hz ≈ 16.7427 ms.
	const Uint64 perf_freq   = SDL_GetPerformanceFrequency();
	const Uint64 frame_ticks = (perf_freq * 70224) / 4194304;
	Uint64 next_frame        = SDL_GetPerformanceCounter() + frame_ticks;

	bool running = true;
	while (running) {
		try {
			const int cycles = cpu.execute();
			ppu.tick(cycles);
			timer.tick(cycles);
			apu.tick(cycles);

			const int irq_cycles = cpu.serviceInterrupts();
			if (irq_cycles) {
				ppu.tick(irq_cycles);
				timer.tick(irq_cycles);
				apu.tick(irq_cycles);
			}

			if (ppu.consume_frame_ready()) {
				display.present(ppu.framebuffer_view());
				running = display.pump();
				bus.set_buttons(display.poll_input());

				// Drain APU samples and push to SDL. Cap queue so we don't drift.
				sample_buf.clear();
				apu.drain_samples(sample_buf);
				if (!sample_buf.empty()) {
					// If the queue is already huge (>100ms), drop this batch.
					constexpr Uint32 kMaxQueueBytes = 48000 * 2 * 2 / 10; // ~100ms stereo s16
					if (SDL_GetQueuedAudioSize(audio_dev) < kMaxQueueBytes) {
						SDL_QueueAudio(audio_dev, sample_buf.data(),
							static_cast<Uint32>(sample_buf.size() * sizeof(int16_t)));
					}
				}

				const Uint64 now = SDL_GetPerformanceCounter();
				if (now < next_frame) {
					const Uint64 wait_ms = ((next_frame - now) * 1000) / perf_freq;
					if (wait_ms > 0) SDL_Delay(static_cast<Uint32>(wait_ms));
					next_frame += frame_ticks;
				} else {
					// Fell behind — resync rather than burst-catch-up.
					next_frame = now + frame_ticks;
				}
			}
		} catch (const std::exception& e) {
			std::cerr << "Caught the following error: " << e.what() << std::endl;
			break;
		}
	}

	SDL_CloseAudioDevice(audio_dev);
	return 0;
}
