#pragma once

#include "utils/parsers/obj_parser.hpp"

#include "utils/io.hpp"
#include "utils/parsers/mtl_parser.hpp"
#include <boost/spirit/home/x3.hpp>

namespace parser::obj {

template <typename Container>
auto parse(const Container &data, std::vector<Point> &points,
           std::vector<TextureCoords> &uvs, std::vector<Face> &faces,
           Color &color, std::string &texture_path) -> bool {

  namespace x3 = boost::spirit::x3;

  using boost::fusion::at_c;
  using x3::char_;
  using x3::double_;
  using x3::uint_;

  const auto lex_string_no_eol = x3::lexeme[+(char_ - x3::ascii::space)];
  const auto lex_vertex = x3::lit('v') >> double_ >> double_ >> double_;
  const auto lex_face = uint_ >> '/' >> uint_ >> '/' >> uint_;
  const auto lex_faces = x3::lit('f') >> lex_face >> lex_face >> lex_face;
  const auto lex_mtl = x3::lit("mtllib") >> lex_string_no_eol;
  const auto lex_uv = x3::lit("vt") >> double_ >> double_;

  auto lambda_mtl = [&](auto &ctx) {
    std::string mtl_path = x3::_attr(ctx);
    auto mtl_file = load_file(mtl_path);
    parser::mtl::parse(mtl_file, color, texture_path);
  };

  auto lambda_vertex = [&](auto &ctx) {
    Point p = {at_c<0>(x3::_attr(ctx)), at_c<1>(x3::_attr(ctx)),
               at_c<2>(x3::_attr(ctx))};
    points.push_back(std::move(p));
  };

  auto lambda_faces = [&](auto &ctx) {
    Vertex v1 = {at_c<0>(x3::_attr(ctx)) - 1, at_c<1>(x3::_attr(ctx)) - 1,
                 at_c<2>(x3::_attr(ctx)) - 1};
    Vertex v2 = {at_c<3>(x3::_attr(ctx)) - 1, at_c<4>(x3::_attr(ctx)) - 1,
                 at_c<5>(x3::_attr(ctx)) - 1};
    Vertex v3 = {at_c<6>(x3::_attr(ctx)) - 1, at_c<7>(x3::_attr(ctx)) - 1,
                 at_c<8>(x3::_attr(ctx)) - 1};

    Face face = {std::move(v1), std::move(v2), std::move(v3)};
    faces.push_back(std::move(face));
  };

  auto lambda_uv = [&](auto &ctx) {
    TextureCoords tc = {at_c<0>(x3::_attr(ctx)), at_c<1>(x3::_attr(ctx))};
    uvs.push_back(std::move(tc));
  };

  auto first = data.begin();
  auto last = data.end();

  bool r = x3::phrase_parse(first, last,
                            (lex_vertex[lambda_vertex] | lex_uv[lambda_uv] |
                             lex_faces[lambda_faces] | lex_mtl[lambda_mtl] |
                             *lex_string_no_eol) %
                                x3::eol,
                            x3::ascii::blank);

  // We didn't get a match
  if (first != last) {
    return false;
  }

  return r;
}
} // namespace parser::obj