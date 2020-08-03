#pragma once

#include "utils/SDL.hpp"

auto flip_vertical(const sdl2::unique_ptr<SDL_Surface> &sfc)
    -> sdl2::unique_ptr<SDL_Surface>;