#pragma once

#include "utils/SDL.hpp"
#include <vector>

auto load_file(const std::string_view path) -> std::vector<char>;

auto load_image(const std::string_view path) -> sdl2::unique_ptr<SDL_Surface>;