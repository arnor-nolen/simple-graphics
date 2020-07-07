#pragma once

#include "utils/GL.hpp"
#include "utils/io.hpp"
#include "utils/primitives.hpp"
#include <GL/glew.h>

struct Model {
  Model() {}

  Model(const std::vector<Element> &elements,
        const std::vector<Vertex> &vertices)
      : ebo_(GL_ELEMENT_ARRAY_BUFFER, elements),
        vbo_(GL_ARRAY_BUFFER, vertices) {}

  Model(const std::string &path) {
    auto file = load_file(path);
    std::vector<Element> elements;
    std::vector<Vertex> vertices;
    parse_obj(file, elements, vertices);
    auto model = Model(elements, vertices);
    this->swap(model);
  }

  ~Model() {}

  Model(const Model &) = delete;
  Model(Model &&other) { swap(other); };
  const Model &operator=(const Model &) = delete;
  const Model &operator=(Model &&other) {
    swap(other);
    return *this;
  };

  void swap(Model &other) {
    this->ebo_.swap(other.ebo_);
    this->vbo_.swap(other.vbo_);
    std::swap(this->mvp_matrix_, other.mvp_matrix_);
  }

  void render(const GLuint &matrix_uniform) {
    glUniformMatrix4fv(matrix_uniform, 1, GL_FALSE, &mvp_matrix_[0][0]);
    bind_buffers();
    set_layout();
    glDrawElements(GL_TRIANGLES,
                   static_cast<GLsizei>(ebo_.get_data().size() * 3),
                   GL_UNSIGNED_INT, 0);
  }

  void set_mvp_matrix(const glm::mat4 &mvp_matrix) { mvp_matrix_ = mvp_matrix; }

  const auto &get_mvp_matrix() { return mvp_matrix_; }

private:
  void bind_buffers() {
    ebo_.bind();
    vbo_.bind();
  }

  void set_layout() {
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(6 * sizeof(float)));
  }

  gl::Buffer<Element> ebo_;
  gl::Buffer<Vertex> vbo_;
  glm::mat4 mvp_matrix_ = glm::mat4(1.0f);
};