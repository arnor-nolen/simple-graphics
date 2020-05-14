#pragma once

#include <array>

struct Element {
  std::array<GLuint, 3> vertices;
};

struct Vertex {
  glm::vec3 coord;
  glm::vec3 color;
  glm::vec2 uv;
};