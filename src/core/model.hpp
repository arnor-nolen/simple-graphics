#pragma once

#include "utils/GL.hpp"
#include "utils/parsers/parsers.hpp"
#include "utils/primitives.hpp"
#include <GL/glew.h>

struct Model {
  Model() = default;

  Model(const std::vector<gl::Element> &elements,
        const std::vector<gl::Vertex> &vertices,
        const std::string &texture_path);

  explicit Model(const std::string &path);

  ~Model() = default;

  Model(const Model &) = delete;
  Model(Model &&other) noexcept;
  auto operator=(const Model &) -> Model & = delete;
  auto operator=(Model &&other) noexcept -> Model &;

  void swap(Model &other);
  void render(const GLuint &matrix_uniform);
  void set_mvp_matrix(const glm::mat4 &mvp_matrix);
  auto get_mvp_matrix() -> const glm::mat4 &;

private:
  void bind_buffers();
  void set_layout();

  gl::Buffer<gl::Element> ebo_;
  gl::Buffer<gl::Vertex> vbo_;
  glm::mat4 mvp_matrix_ = glm::mat4(1.0F);
  gl::Texture texture_;
};