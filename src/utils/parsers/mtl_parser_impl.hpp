#pragma once

#include "utils/parsers/mtl_parser.hpp"

#include "utils/timer.hpp"
#include <boost/spirit/home/x3.hpp>
#include <vector>

namespace parser::mtl {
template <typename Container>
auto parse(const Container &data, Color &color, std::string &texture_path)
    -> bool {
  Timer timer("Parsing MTL file took ");

  namespace x3 = boost::spirit::x3;

  using boost::fusion::at_c;
  using x3::char_;
  using x3::double_;

  color = {1.0, 1.0, 1.0};

  const auto lex_string_no_eol = x3::lexeme[+(char_ - x3::ascii::space)];
  const auto lex_diffuse = x3::lit("Kd") >> double_ >> double_ >> double_;
  const auto lex_map_diffuse = x3::lit("map_Kd") >> lex_string_no_eol;

  auto lambda_diffuse = [&](auto &ctx) {
    color = {at_c<0>(x3::_attr(ctx)), at_c<1>(x3::_attr(ctx)),
             at_c<2>(x3::_attr(ctx))};
  };

  auto lambda_map_diffuse = [&](auto &ctx) { texture_path = x3::_attr(ctx); };

  auto first = data.begin();
  auto last = data.end();

  bool r = x3::phrase_parse(first, last,
                            (lex_diffuse[lambda_diffuse] |
                             lex_map_diffuse[lambda_map_diffuse] |
                             *lex_string_no_eol) %
                                x3::eol,
                            x3::ascii::blank);

  // We didn't get a match
  if (first != last) {
    return false;
  }

  return r;
}
} // namespace parser::mtl