#pragma once

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>

namespace sdl2 {
struct sdl2_deleter {
  void operator()(SDL_Surface *p) const { SDL_FreeSurface(p); }
  void operator()(SDL_Window *p) const { SDL_DestroyWindow(p); }
  void operator()(SDL_Renderer *p) const { SDL_DestroyRenderer(p); }
  void operator()(SDL_Texture *p) const { SDL_DestroyTexture(p); }
};

template <typename T> using unique_ptr = std::unique_ptr<T, sdl2_deleter>;

struct SDL {
  explicit SDL(Uint32 flags) { SDL_Init(flags); }
  ~SDL() { SDL_Quit(); }
};

struct SDL_Context {
  explicit SDL_Context(const sdl2::unique_ptr<SDL_Window> &w)
      : context_(SDL_GL_CreateContext(w.get())) {}
  ~SDL_Context() { SDL_GL_DeleteContext(context_); }

private:
  SDL_GLContext context_;
};

struct SDL_image {
  explicit SDL_image(int flags) {
    // Check if IMG_Init returns the flags we requested
    if (!(static_cast<bool>(static_cast<unsigned int>(IMG_Init(flags)) &
                            static_cast<unsigned int>(flags)))) {
      throw std::runtime_error(
          "SDL_image could not initialize! SDL_image Error: " +
          std::string(IMG_GetError()) + '\n');
    }
  }
  ~SDL_image() { IMG_Quit(); }
};

void gl_setAttributes(const std::vector<std::pair<SDL_GLattr, int>> &init);
} // namespace sdl2