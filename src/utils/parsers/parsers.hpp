#pragma once

#include "utils/GL.hpp"
#include "utils/primitives.hpp"
#include <vector>

namespace parser {
void parse_model(const std::vector<char> &data,
                 std::vector<gl::Element> &elements,
                 std::vector<gl::Vertex> &vertices, gl::Texture &texture);
void parse_model_assimp(const std::vector<char> &data,
                        std::vector<gl::Element> &elements,
                        std::vector<gl::Vertex> &vertices,
                        std::string_view file_type, gl::Texture &texture,
                        std::string_view texture_path);
} // namespace parser