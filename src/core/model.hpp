#pragma once

#include "utils/GL.hpp"
#include "utils/primitives.hpp"
#include <GL/glew.h>
#include <string_view>

enum loader_enum { LOADER_OBJ, LOADER_ASSIMP };

struct Model {
  Model() = default;

  Model(std::vector<gl::Element> &elements, std::vector<gl::Vertex> &vertices,
        gl::Texture &texture);

  Model(loader_enum loader, const std::string_view path,
        const std::string_view texture_path = nullptr);

  ~Model() = default;

  Model(const Model &) = delete;
  Model(Model &&other) noexcept;
  auto operator=(const Model &) -> Model & = delete;
  auto operator=(Model &&other) noexcept -> Model &;

  void swap(Model &other);
  void render(const GLuint &matrix_uniform);
  void set_mvp_matrix(const glm::mat4 &mvp_matrix);
  auto get_mvp_matrix() -> const glm::mat4 &;
  void set_offset(const glm::dvec3 &offset);
  auto get_offset() -> const glm::dvec3 &;
  void set_scale(const glm::dvec3 &scale);
  auto get_scale() -> const glm::dvec3 &;

  void calculate_mvp_matrix(const glm::dmat4 &projection_matrix,
                            const glm::dmat4 &view_matrix);

  void rotate(double angle, const glm::dvec3 &axis);

  struct ModelSettings {
    std::array<double, 3> scale = {1.0, 1.0, 1.0};
    std::array<double, 3> offset = {0.0, 0.0, 0.0};
    std::string name;
    bool is_open = true;
    bool is_rotating = false;
    bool delete_me = false;
  };
  ModelSettings settings;

private:
  void bind_buffers();
  void static set_layout();

  gl::Buffer<gl::Element> ebo_;
  gl::Buffer<gl::Vertex> vbo_;
  glm::dvec3 scale_ = glm::dvec3(1.0, 1.0, 1.0);
  glm::dvec3 offset_ = glm::dvec3(0.0, 0.0, 0.0);
  glm::mat4 mvp_matrix_ = glm::mat4(1.0);
  gl::Texture texture_;
};