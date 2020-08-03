#include "utils/parsers/parsers.hpp"

#include "utils/parsers/obj_parser.hpp"

namespace parser {
void parse_model(const std::vector<char> &obj,
                 std::vector<gl::Element> &elements,
                 std::vector<gl::Vertex> &vertices, std::string &texture_path) {
  Timer timer("Parsing both OBJ and MTL files took ");

  Color color = {1.0, 1.0, 1.0};
  std::vector<Point> points;
  std::vector<parser::obj::TextureCoords> uvs;
  std::vector<parser::obj::Face> faces;
  parser::obj::parse(obj, points, uvs, faces, color, texture_path);

  for (const auto &face : faces) {
    auto v = std::array<gl::Vertex, 3>();
    for (int i = 0; i != 3; ++i) {
      unsigned int vertex_id = face.vertices.at(i).vertex_id;
      unsigned int uv_id = face.vertices.at(i).uv_id;

      auto vertex = points[vertex_id];
      auto uv = uvs[uv_id];

      v.at(i).coord = {vertex.x, vertex.y, vertex.z};
      v.at(i).color = {color.r, color.g, color.b};
      v.at(i).uv = {uv.u, uv.v};
    }
    vertices.insert(vertices.end(), v.begin(), v.end());
    auto e = gl::Element();
    for (int i = 0; i != 3; ++i) {
      e.vertices.at(i) = static_cast<unsigned int>(vertices.size() - i - 1);
    }
    elements.push_back(e);
  }
}
} // namespace parser