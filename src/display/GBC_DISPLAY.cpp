#include "GBC_DISPLAY.h"
#include <SDL_image.h>
#include <filesystem>
#include <iostream>

namespace {
// Resolved relative to the running executable's own directory (via
// SDL_GetBasePath), not the process's working directory — a cwd-relative
// path only works when launched from repo root, which CLion's default run
// config doesn't do and a downloaded release binary has no notion of at all.
//
// Two possible layouts depending on how this was built:
//   - packaged .app bundle: executable sits in Contents/MacOS/, assets are
//     installed to Contents/Resources/assets/ -> one level up, into Resources.
//   - dev build (build-sys/build/ or build-sys/cmake-build-debug/): assets
//     live at repo root -> two levels up from the executable.
// Probe both and use whichever actually exists on disk; existence is checked
// against the already-resolved absolute candidate, never re-derived from cwd.
std::string resolve_asset_path(const std::string& relative_to_root) {
    char* base_c = SDL_GetBasePath();
    if (!base_c) return relative_to_root;
    const std::string base(base_c);
    SDL_free(base_c);

    const std::string candidates[] = {
        base + "../Resources/" + relative_to_root,  // .app bundle
        base + "../../" + relative_to_root,         // dev build
    };
    for (const auto& candidate : candidates) {
        if (std::filesystem::exists(candidate)) return candidate;
    }
    return candidates[0]; // none found; let IMG_Load's own error reporting take over
}
}

GBC_DISPLAY::~GBC_DISPLAY() {
    if (prompt_texture_) SDL_DestroyTexture(prompt_texture_);
    if (texture_)  SDL_DestroyTexture(texture_);
    if (renderer_) SDL_DestroyRenderer(renderer_);
    if (window_)   SDL_DestroyWindow(window_);
    IMG_Quit();
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

    // If image fails to load, default to a blank window
    const std::string prompt_path = resolve_asset_path("assets/drop_rom_prompt.png");
    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == 0) {
        std::cerr << "IMG_Init(PNG) failed: " << IMG_GetError() << std::endl;
    } else if (SDL_Surface* surface = IMG_Load(prompt_path.c_str())) {
        prompt_texture_ = SDL_CreateTextureFromSurface(renderer_, surface);
        SDL_FreeSurface(surface);
        if (!prompt_texture_) {
            std::cerr << "SDL_CreateTextureFromSurface: " << SDL_GetError() << std::endl;
        }
    } else {
        std::cerr << "IMG_Load(" << prompt_path << "): " << IMG_GetError() << std::endl;
    }

    return true;
}

void GBC_DISPLAY::present(const std::array<WORD, 160 * 144>& fb) {
    SDL_UpdateTexture(texture_, nullptr, fb.data(), 160 * sizeof(WORD));
    SDL_RenderClear(renderer_);
    SDL_RenderCopy(renderer_, texture_, nullptr, nullptr);
    SDL_RenderPresent(renderer_);
}

void GBC_DISPLAY::present_drop_prompt() {
    SDL_RenderClear(renderer_);
    if (prompt_texture_) SDL_RenderCopy(renderer_, prompt_texture_, nullptr, nullptr);
    SDL_RenderPresent(renderer_);
}

bool GBC_DISPLAY::pump() {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) return false;
        if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) return false;
        if (ev.type == SDL_DROPFILE) {
            dropped_path_ = std::string(ev.drop.file);
            SDL_free(ev.drop.file);
        }
    }
    return true;
}

std::optional<std::string> GBC_DISPLAY::consume_dropped_path() {
    std::optional<std::string> path = std::move(dropped_path_);
    dropped_path_.reset();
    return path;
}

BYTE GBC_DISPLAY::poll_input() const {
    const Uint8* keys = SDL_GetKeyboardState(nullptr);
    BYTE b = 0;
    if (keys[SDL_SCANCODE_N])      b |= 0x01; // A
    if (keys[SDL_SCANCODE_M])      b |= 0x02; // B
    if (keys[SDL_SCANCODE_RSHIFT]) b |= 0x04; // Select
    if (keys[SDL_SCANCODE_RETURN]) b |= 0x08; // Start
    if (keys[SDL_SCANCODE_D])      b |= 0x10; // Right
    if (keys[SDL_SCANCODE_A])      b |= 0x20; // Left
    if (keys[SDL_SCANCODE_W])      b |= 0x40; // Up
    if (keys[SDL_SCANCODE_S])      b |= 0x80; // Down
    return b;
}