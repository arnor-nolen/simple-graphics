#pragma once

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>

namespace sdl2 {

void gl_setAttributes(const std::vector<std::pair<SDL_GLattr, int>> &init);

struct sdl2_deleter {
  void operator()(SDL_Surface *p) const;
  void operator()(SDL_Window *p) const;
  void operator()(SDL_Renderer *p) const;
  void operator()(SDL_Texture *p) const;
};

template <typename T> using unique_ptr = std::unique_ptr<T, sdl2_deleter>;

struct SDL {
  explicit SDL(Uint32 flags);
  ~SDL();

  SDL(const SDL &other) = delete;
  SDL(SDL &&other) = delete;
  auto operator=(const SDL &) -> SDL & = delete;
  auto operator=(SDL &&other) noexcept -> SDL & = delete;
}; // namespace sdl2

struct SDL_Context {
  explicit SDL_Context(const sdl2::unique_ptr<SDL_Window> &w);
  ~SDL_Context();

  SDL_Context(const SDL_Context &other) = delete;
  SDL_Context(SDL_Context &&other) = delete;
  auto operator=(const SDL_Context &) -> SDL_Context & = delete;
  auto operator=(SDL_Context &&other) noexcept -> SDL_Context & = delete;

  [[nodiscard]] auto get() const -> const SDL_GLContext &;

private:
  SDL_GLContext context_;
};

struct SDL_image {
  explicit SDL_image(int flags);
  ~SDL_image();

  SDL_image(const SDL_image &other) = delete;
  SDL_image(SDL_image &&other) = delete;
  auto operator=(const SDL_image &) -> SDL_image & = delete;
  auto operator=(SDL_image &&other) noexcept -> SDL_image & = delete;
};

} // namespace sdl2