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
      model.render();
    }
  }

private:
  std::vector<Model> models_;
};