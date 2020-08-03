#include "core/resource_manager.hpp"

void ResourceManager::render_all() {
  for (auto &model : models_) {
    model.render(program_.get_matrix_uniform());
  }
}

auto ResourceManager::get_models() -> std::vector<Model> & { return models_; }
auto ResourceManager::get_program_ptr() -> gl::Program & { return program_; }