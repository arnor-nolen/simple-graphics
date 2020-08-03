#pragma once

#include "utils/primitives.hpp"

namespace parser::obj {

template <typename Container>
auto parse(const Container &data, std::vector<Point> &points,
           std::vector<TextureCoords> &uvs, std::vector<Face> &faces,
           Color &color, std::string &texture_path) -> bool;
} // namespace parser::obj

#include "utils/parsers/obj_parser_impl.hpp"