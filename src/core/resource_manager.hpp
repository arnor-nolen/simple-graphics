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

  template <typename... Args> void load_model(Args &&... args);

  void render_all();

  auto get_models() -> std::vector<Model> &;
  auto get_program_ptr() -> gl::Program &;

private:
  std::vector<Model> models_;
  gl::Program program_;
};

#include "core/resource_manager_impl.hpp"