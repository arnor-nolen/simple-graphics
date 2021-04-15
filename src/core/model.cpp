#include "core/model.hpp"

#include "utils/parsers/parsers.hpp"
#include "utils/primitives.hpp"
#include <glm/gtx/transform.hpp>

Model::Model(std::vector<gl::Element> &elements,
             std::vector<gl::Vertex> &vertices, gl::Texture &texture)
    : ebo_(GL_ELEMENT_ARRAY_BUFFER, std::move(elements)),
      vbo_(GL_ARRAY_BUFFER, std::move(vertices)), texture_(std::move(texture)) {
}

Model::Model(loader_enum loader, const std::string_view path,
             const std::string_view texture_path) {
  auto file = load_file(path);

  gl::Texture texture;
  std::vector<gl::Element> elements;
  std::vector<gl::Vertex> vertices;

  switch (loader) {
  case loader_enum::LOADER_OBJ:
    std::tie(elements, vertices, texture) = parser::parse_model(file);
    break;
  case loader_enum::LOADER_ASSIMP:
    std::tie(elements, vertices, texture) =
        parser::parse_model_assimp(file, "fbx", texture_path);
    break;
  default:
    elements = std::vector<gl::Element>();
    vertices = std::vector<gl::Vertex>();
    break;
  }
  ebo_ = gl::Buffer<gl::Element>(GL_ELEMENT_ARRAY_BUFFER, std::move(elements));
  vbo_ = gl::Buffer<gl::Vertex>(GL_ARRAY_BUFFER, std::move(vertices));
  texture_ = gl::Texture(std::move(texture));
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
  std::swap(this->scale_, other.scale_);
  std::swap(this->offset_, other.offset_);
  std::swap(this->settings, other.settings);
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

void Model::set_offset(const glm::dvec3 &offset) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
  settings.offset = {offset.x, offset.y, offset.z};
  offset_ = offset;
}
auto Model::get_offset() -> const glm::dvec3 & { return offset_; }
void Model::set_scale(const glm::dvec3 &scale) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
  settings.scale = {scale.x, scale.y, scale.z};
  scale_ = scale;
}
auto Model::get_scale() -> const glm::dvec3 & { return scale_; }

void Model::calculate_mvp_matrix(const glm::dmat4 &projection_matrix,
                                 const glm::dmat4 &view_matrix) {
  constexpr auto identity_matrix = glm::dmat4(1.0);
  auto model_matrix = glm::scale(identity_matrix, scale_);
  mvp_matrix_ =
      projection_matrix * glm::translate(view_matrix, offset_) * model_matrix;
}

void Model::rotate(double angle, const glm::dvec3 &axis) {
  mvp_matrix_ = glm::rotate(glm::dmat4(mvp_matrix_), angle, axis);
};