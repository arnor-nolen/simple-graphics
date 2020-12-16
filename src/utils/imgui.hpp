#pragma once

#include "bindings/imgui_impl_opengl3.h"
#include "bindings/imgui_impl_sdl.h"
#include "utils/SDL.hpp"
#include <imgui.h>


namespace imgui {
struct Imgui {
  Imgui(const sdl2::unique_ptr<SDL_Window> &, const sdl2::SDL_Context &);
  ~Imgui();

  Imgui(const Imgui &) = delete;
  Imgui(Imgui &&other) noexcept = delete;
  auto operator=(const Imgui &) -> Imgui & = delete;
  auto operator=(Imgui &&other) noexcept -> Imgui & = delete;

  void create_frame(const sdl2::unique_ptr<SDL_Window> &);
  void render();

private:
  ImGuiIO io_;
};
} // namespace imgui