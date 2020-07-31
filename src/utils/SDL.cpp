#include "utils/SDL.hpp"

namespace sdl2 {

void gl_setAttributes(const std::vector<std::pair<SDL_GLattr, int>> &init) {
  for (const auto &i : init) {
    if (static_cast<bool>(SDL_GL_SetAttribute(i.first, i.second))) {
      throw std::runtime_error("Can't set some GL attributes!" +
                               std::string(SDL_GetError()) + '\n');
    }
  }
}
} // namespace sdl2