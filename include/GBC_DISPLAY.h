#pragma once

#include "GBC_Types.h"
#include <SDL.h>
#include <array>
#include <optional>
#include <string>

class GBC_DISPLAY {
public:
    GBC_DISPLAY() = default;
    ~GBC_DISPLAY();

    GBC_DISPLAY(const GBC_DISPLAY&) = delete;
    GBC_DISPLAY& operator=(const GBC_DISPLAY&) = delete;

    bool init(int scale = 4);
    void present(const std::array<WORD, 160 * 144>& fb);
    // Renders the "drag a ROM onto this window" prompt image full-window.
    // No-ops (blank window) if the asset failed to load.
    void present_drop_prompt();
    bool pump();

    // Returns a bitmask of currently-pressed Game Boy buttons.
    // bit 0=A, 1=B, 2=Select, 3=Start, 4=Right, 5=Left, 6=Up, 7=Down. 1 = pressed.
    BYTE poll_input() const;

    // Returns the most recently dropped file's path, if any, and clears it.
    // Mirrors GBC_PPU's frame_ready_/consume_frame_ready() flag+consume idiom.
    std::optional<std::string> consume_dropped_path();

private:
    SDL_Window*   window_        = nullptr;
    SDL_Renderer* renderer_      = nullptr;
    SDL_Texture*  texture_       = nullptr;
    SDL_Texture*  prompt_texture_ = nullptr;
    std::optional<std::string> dropped_path_;
};