#pragma once

#include "GBC_Types.h"
#include <SDL.h>
#include <array>

class GBC_DISPLAY {
public:
    GBC_DISPLAY() = default;
    ~GBC_DISPLAY();

    GBC_DISPLAY(const GBC_DISPLAY&) = delete;
    GBC_DISPLAY& operator=(const GBC_DISPLAY&) = delete;

    bool init(int scale = 4);
    void present(const std::array<WORD, 160 * 144>& fb);
    bool pump();

private:
    SDL_Window*   window_   = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    SDL_Texture*  texture_  = nullptr;
};