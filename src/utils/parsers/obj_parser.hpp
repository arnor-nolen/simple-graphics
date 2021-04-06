#pragma once

#include "utils/primitives.hpp"
#include <vector>

namespace parser::obj {

template <typename Container> auto parse(const Container &data);
} // namespace parser::obj

#include "utils/parsers/obj_parser_impl.hpp"