#include "utils/SDL.hpp"

namespace sdl2 {

void gl_setAttributes(const std::vector<std::pair<SDL_GLattr, int>> &init) {
  for (const auto &i : init) {
    if (SDL_GL_SetAttribute(i.first, i.second) != 0) {
      throw std::runtime_error("Can't set some GL attributes!" +
                               std::string(SDL_GetError()) + '\n');
    }
  }
}

void sdl2_deleter::operator()(SDL_Surface *p) const { SDL_FreeSurface(p); }
void sdl2_deleter::operator()(SDL_Window *p) const { SDL_DestroyWindow(p); }
void sdl2_deleter::operator()(SDL_Renderer *p) const { SDL_DestroyRenderer(p); }
void sdl2_deleter::operator()(SDL_Texture *p) const { SDL_DestroyTexture(p); }

SDL::SDL(Uint32 flags) { SDL_Init(flags); }
SDL::~SDL() { SDL_Quit(); }

SDL_Context::SDL_Context(const sdl2::unique_ptr<SDL_Window> &w)
    : context_(SDL_GL_CreateContext(w.get())) {}
SDL_Context::~SDL_Context() { SDL_GL_DeleteContext(context_); }

SDL_image::SDL_image(int flags) {
  // Check if IMG_Init returns the flags we requested
  if ((static_cast<unsigned int>(IMG_Init(flags)) &
       static_cast<unsigned int>(flags)) == 0) {
    throw std::runtime_error(
        "SDL_image could not initialize! SDL_image Error: " +
        std::string(IMG_GetError()) + '\n');
  }
}
SDL_image::~SDL_image() { IMG_Quit(); }
} // namespace sdl2