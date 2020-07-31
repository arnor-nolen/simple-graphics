#pragma once

#include <array>
#include <glm/glm.hpp>

struct Point {
  double x;
  double y;
  double z;
};

struct Color {
  double r;
  double g;
  double b;
};

namespace gl {

struct Element {
  std::array<unsigned int, 3> vertices;
};

struct Vertex {
  glm::vec3 coord;
  glm::vec3 color;
  glm::vec2 uv;
};

} // namespace gl

namespace parser::obj {

struct Vertex {
  unsigned int vertex_id;
  unsigned int uv_id;
  unsigned int normal_id;
};

struct Face {
  std::array<Vertex, 3> vertices;
};

struct TextureCoords {
  double u;
  double v;
};

} // namespace parser::obj