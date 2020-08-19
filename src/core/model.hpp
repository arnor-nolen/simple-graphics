#pragma once

#include "utils/GL.hpp"
#include "utils/primitives.hpp"
#include <GL/glew.h>
#include <string_view>

struct Model {
  Model() = default;

  Model(std::vector<gl::Element> &elements, std::vector<gl::Vertex> &vertices,
        gl::Texture &texture);

  explicit Model(std::string_view path);
  Model(std::string_view path, std::string_view texture_path);

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
  void static set_layout();

  gl::Buffer<gl::Element> ebo_;
  gl::Buffer<gl::Vertex> vbo_;
  glm::mat4 mvp_matrix_ = glm::mat4(1.0F);
  gl::Texture texture_;
};