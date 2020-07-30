#pragma once

#include "utils/SDL.hpp"
#include "utils/flip_vertical.hpp"
#include "utils/primitives.hpp"
#include "utils/timer.hpp"
#include <fstream>
#include <glm/glm.hpp>
#include <vector>

auto load_file(const std::string &path) {
  Timer timer("Loading file " + path + " took ");
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  const std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<char> buffer(size + 1);
  if (!file.read(buffer.data(), size)) {
    throw std::runtime_error("Can't read from file! File location: " + path +
                             '\n');
  }
  buffer.back() = '\0';
  return buffer;
}

auto load_image(const std::string &path) {
  auto loaded_surface = sdl2::unique_ptr<SDL_Surface>(IMG_Load(path.c_str()));
  if (!loaded_surface) {
    throw std::runtime_error("Unable to load image " + path +
                             "!\nSDL_image error: " + IMG_GetError() + '\n');
  } else {
    // SDL and OpenGL have different coordinates, we have to flip the surface
    auto flipped_surface = flip_vertical(loaded_surface);
    return flipped_surface;
  }
}
