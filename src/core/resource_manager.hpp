#pragma once

#include <core/model.hpp>
#include <vector>

struct ResourceManager {
  ResourceManager() {}
  ~ResourceManager() {}

  template <typename... Args> void load_model(Args &&... args) {
    models_.emplace_back(args...);
  }

  void render_all() {
    for (auto &model : models_) {
      model.render(program_.get_matrix_uniform());
    }
  }

  auto &get_models() { return models_; }
  auto &get_program_ptr() { return program_; }

private:
  std::vector<Model> models_;
  gl::Program program_;
};