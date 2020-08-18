#include "utils/io.hpp"

#include <SDL.h>

auto load_file(const std::string &path) -> std::vector<char> {
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

auto load_image(const std::string &path) -> sdl2::unique_ptr<SDL_Surface> {
  auto loaded_surface = sdl2::unique_ptr<SDL_Surface>(IMG_Load(path.c_str()));
  if (!loaded_surface) {
    throw std::runtime_error("Unable to load image " + path +
                             "!\nSDL_image error: " + IMG_GetError() + '\n');
  }
  // SDL and OpenGL have different coordinates, we have to flip the surface
  auto converted_surface =
      sdl2::unique_ptr<SDL_Surface>(SDL_ConvertSurfaceFormat(
          loaded_surface.get(), SDL_PIXELFORMAT_RGBA32, 0));
  auto flipped_surface = flip_vertical(converted_surface);
  return flipped_surface;
}
