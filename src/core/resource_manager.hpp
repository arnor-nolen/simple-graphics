#pragma once

#include "core/model.hpp"
#include <vector>

struct ResourceManager {
  ResourceManager() = default;
  ~ResourceManager() = default;

  ResourceManager(const ResourceManager &) = delete;
  ResourceManager(ResourceManager &&other) noexcept = delete;
  auto operator=(const ResourceManager &) -> ResourceManager & = delete;
  auto operator=(ResourceManager &&other) noexcept
      -> ResourceManager & = delete;

  template <typename... Args> auto load_model(Args &&... args) -> Model &;
  void load_shaders(std::string_view, std::string_view);

  void render_all();

  auto get_models() -> std::vector<Model> &;

private:
  gl::Program program_;
  std::vector<Model> models_;
};

#include "core/resource_manager_impl.hpp"