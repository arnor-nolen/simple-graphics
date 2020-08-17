#include "core/model.hpp"

Model::Model(const std::vector<gl::Element> &elements,
             const std::vector<gl::Vertex> &vertices,
             const std::string &texture_path)
    : ebo_(GL_ELEMENT_ARRAY_BUFFER, elements), vbo_(GL_ARRAY_BUFFER, vertices),
      texture_(texture_path) {}

Model::Model(const std::string &path) {
  auto file = load_file(path);
  auto elements = std::vector<gl::Element>();
  auto vertices = std::vector<gl::Vertex>();
  std::string texture_path;
  parser::parse_model_fbx(file, elements, vertices, texture_path);
  auto model = Model(elements, vertices, texture_path);
  this->swap(model);
}

Model::Model(Model &&other) noexcept { swap(other); };
auto Model::operator=(Model &&other) noexcept -> Model & {
  swap(other);
  return *this;
};

void Model::swap(Model &other) {
  this->ebo_.swap(other.ebo_);
  this->vbo_.swap(other.vbo_);
  std::swap(this->mvp_matrix_, other.mvp_matrix_);
  std::swap(this->texture_, other.texture_);
}

void Model::render(const GLuint &matrix_uniform) {
  glUniformMatrix4fv(matrix_uniform, 1, GL_FALSE, &mvp_matrix_[0][0]);
  bind_buffers();
  set_layout();
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(ebo_.get_data().size() * 3),
                 GL_UNSIGNED_INT, nullptr);
}

void Model::set_mvp_matrix(const glm::mat4 &mvp_matrix) {
  mvp_matrix_ = mvp_matrix;
}

auto Model::get_mvp_matrix() -> const glm::mat4 & { return mvp_matrix_; }

void Model::bind_buffers() {
  ebo_.bind();
  vbo_.bind();
  texture_.bind();
}

void Model::set_layout() {

  // Set constants according to our data structure
  constexpr int offset_vertex = 0;
  constexpr int offset_color = 3;
  constexpr int offset_uv = 6;

  constexpr int stride_vertex = 3;
  constexpr int stride_color = 3;
  constexpr int stride_uv = 2;

  constexpr int total_stride = stride_vertex + stride_color + stride_uv;

  glVertexAttribPointer(offset_vertex, stride_vertex, GL_FLOAT, GL_FALSE,
                        total_stride * sizeof(float), nullptr);
  glVertexAttribPointer(
      offset_color, stride_color, GL_FLOAT, GL_FALSE,
      total_stride * sizeof(float),
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      reinterpret_cast<void *>(offset_color * sizeof(float)));
  glVertexAttribPointer(
      offset_uv, stride_uv, GL_FLOAT, GL_FALSE, total_stride * sizeof(float),
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      reinterpret_cast<void *>(offset_uv * sizeof(float)));
}