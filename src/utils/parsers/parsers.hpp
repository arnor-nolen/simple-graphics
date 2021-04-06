#pragma once

#include "utils/GL.hpp"
#include "utils/primitives.hpp"
#include <vector>

namespace parser {
auto parse_model(const std::vector<char> &data)
    -> std::tuple<std::vector<gl::Element>, std::vector<gl::Vertex>,
                  gl::Texture>;
auto parse_model_assimp(const std::vector<char> &data,
                        std::string_view file_type,
                        std::string_view texture_path)
    -> std::tuple<std::vector<gl::Element>, std::vector<gl::Vertex>,
                  gl::Texture>;
} // namespace parser