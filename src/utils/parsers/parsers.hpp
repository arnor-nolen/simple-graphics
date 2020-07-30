#pragma once

#include "utils/parsers/obj_parser.hpp"

namespace parser {
void parse_model(const std::vector<char> &obj,
                 std::vector<gl::Element> &elements,
                 std::vector<gl::Vertex> &vertices, std::string &texture_path) {
  Timer timer("Parsing both OBJ and MTL files took ");

  Color color;
  std::vector<Point> points;
  std::vector<parser::obj::TextureCoords> uvs;
  std::vector<parser::obj::Face> faces;
  parser::obj::parse(obj, points, uvs, faces, color, texture_path);

  for (const auto &face : faces) {
    std::array<gl::Vertex, 3> v;
    for (int i = 0; i != 3; ++i) {
      unsigned int vertex_id = face.vertices[i].vertex_id;
      unsigned int uv_id = face.vertices[i].uv_id;

      auto vertex = points[vertex_id];
      auto uv = uvs[uv_id];

      v[i].coord = {vertex.x, vertex.y, vertex.z};
      v[i].color = {color.r, color.g, color.b};
      v[i].uv = {uv.u, uv.v};
    }
    vertices.insert(vertices.end(), v.begin(), v.end());
    gl::Element e;
    for (int i = 0; i != 3; ++i) {
      e.vertices[i] = static_cast<unsigned int>(vertices.size() - i - 1);
    }
    elements.push_back(e);
  }
}
} // namespace parser