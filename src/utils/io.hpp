#pragma once

#include "utils/SDL.hpp"
#include "utils/flip_vertical.hpp"
#include "utils/primitives.hpp"
#include "utils/timer.hpp"
#include <fstream>
#include <glm/glm.hpp>
#include <vector>

auto load_file(const std::string &path) -> std::vector<char>;

auto load_image(const std::string &path) -> sdl2::unique_ptr<SDL_Surface>;