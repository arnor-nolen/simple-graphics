#pragma once

#include "utils/GL.hpp"
#include <vector>

namespace parser {
void parse_model(const std::vector<char> &data,
                 std::vector<gl::Element> &elements,
                 std::vector<gl::Vertex> &vertices, std::string &texture_path);
void parse_model_fbx(const std::vector<char> &data,
                     std::vector<gl::Element> &elements,
                     std::vector<gl::Vertex> &vertices,
                     std::string &texture_path);
} // namespace parser