#pragma once

#include "utils/primitives.hpp"

namespace parser::mtl {
template <typename Container>
auto parse(const Container &data, Color &color, std::string &texture_path)
    -> bool;
} // namespace parser::mtl

#include "utils/parsers/mtl_parser_impl.hpp"