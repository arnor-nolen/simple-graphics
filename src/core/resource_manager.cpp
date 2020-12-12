#include "core/resource_manager.hpp"

void ResourceManager::render_all() {
  for (auto &model : models_) {
    model.render(program_.get_matrix_uniform());
  }
}

auto ResourceManager::get_models() -> std::vector<Model> & { return models_; }

void ResourceManager::load_shaders(std::string_view vert_path,
                                   std::string_view frag_path) {
  std::vector<gl::Shader> shaders;
  shaders.emplace_back(gl::Shader(GL_VERTEX_SHADER));
  shaders.emplace_back(gl::Shader(GL_FRAGMENT_SHADER));

  shaders.at(0).load(vert_path);
  shaders.at(1).load(frag_path);

  // Link the vertex and fragment shader into a shader program
  program_.compile(shaders);
  program_.use();

  // We don't need shaders anymore as the program is compiled
  shaders.clear();
}