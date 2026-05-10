#include "GBC_DISPLAY.h"
#include <iostream>

GBC_DISPLAY::~GBC_DISPLAY() {
    if (texture_)  SDL_DestroyTexture(texture_);
    if (renderer_) SDL_DestroyRenderer(renderer_);
    if (window_)   SDL_DestroyWindow(window_);
    SDL_Quit();
}

bool GBC_DISPLAY::init(const int scale) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }
    window_ = SDL_CreateWindow(
        "kehindes-gbc",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        160 * scale, 144 * scale,
        SDL_WINDOW_SHOWN);
    if (!window_) { std::cerr << "SDL_CreateWindow: " << SDL_GetError() << std::endl; return false; }

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_) { std::cerr << "SDL_CreateRenderer: " << SDL_GetError() << std::endl; return false; }

    // GBC framebuffer is RGB555 with R in low 5 bits => SDL_PIXELFORMAT_BGR555 (high-to-low: B G R).
    texture_ = SDL_CreateTexture(
        renderer_,
        SDL_PIXELFORMAT_BGR555,
        SDL_TEXTUREACCESS_STREAMING,
        160, 144);
    if (!texture_) { std::cerr << "SDL_CreateTexture: " << SDL_GetError() << std::endl; return false; }

    return true;
}

void GBC_DISPLAY::present(const std::array<WORD, 160 * 144>& fb) {
    SDL_UpdateTexture(texture_, nullptr, fb.data(), 160 * sizeof(WORD));
    SDL_RenderClear(renderer_);
    SDL_RenderCopy(renderer_, texture_, nullptr, nullptr);
    SDL_RenderPresent(renderer_);
}

bool GBC_DISPLAY::pump() {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) return false;
        if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) return false;
    }
    return true;
}