#pragma once

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
  SDL(Uint32 flags) { SDL_Init(flags); }
  ~SDL() { SDL_Quit(); }
};

struct SDL_Context {
  SDL_Context(const sdl2::unique_ptr<SDL_Window> &w)
      : context_(SDL_GL_CreateContext(w.get())) {}
  ~SDL_Context() { SDL_GL_DeleteContext(context_); }

private:
  SDL_GLContext context_;
};

struct SDL_image {
  SDL_image(int flags) {
    // Check if IMG_Init returns the flags we requested
    if (!(IMG_Init(flags) & flags)) {
      throw std::runtime_error(
          "SDL_image could not initialize! SDL_image Error: " +
          std::string(IMG_GetError()) + '\n');
    }
  }
  ~SDL_image() { IMG_Quit(); }
};

void gl_setAttributes(const std::vector<std::pair<SDL_GLattr, int>> &init) {
  for (const auto &i : init) {
    if (SDL_GL_SetAttribute(i.first, i.second)) {
      throw std::runtime_error("Can't set some GL attributes!" +
                               std::string(SDL_GetError()) + '\n');
    }
  }
}
} // namespace sdl2