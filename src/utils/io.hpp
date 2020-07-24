#pragma once

#include "utils/SDL.hpp"
#include "utils/flip_vertical.hpp"
#include "utils/primitives.hpp"
#include "utils/timer.hpp"
#include <fstream>
#include <glm/glm.hpp>

auto load_file(const std::string &path) {
  Timer timer("Loading file " + path + " took ");
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  const std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<char> buffer(size + 1);
  if (!file.read(buffer.data(), size)) {
    throw std::runtime_error("Can't read from file! File location: " + path +
                             '\n');
  }
  buffer.back() = '\0';
  return buffer;
}

auto load_image(const std::string &path) {
  auto loaded_surface = sdl2::unique_ptr<SDL_Surface>(IMG_Load(path.c_str()));
  if (!loaded_surface) {
    throw std::runtime_error("Unable to load image " + path +
                             "!\nSDL_image error: " + IMG_GetError() + '\n');
  } else {
    // SDL and OpenGL have different coordinates, we have to flip the surface
    auto flipped_surface = flip_vertical(loaded_surface);
    return flipped_surface;
  }
}

void parse_mtl(const std::vector<char> &mtl, glm::vec3 &color,
               std::string &texture_path) {
  // Timer timer("Parsing MTL file took ");
  auto stream = std::stringstream(mtl.data());
  std::string buf;
  while (std::getline(stream, buf)) {
    std::stringstream line(buf);
    std::string op;
    line >> op;
    if (op == "newmtl") {
      std::string mtl_name;
      line >> mtl_name;
    } else if (op == "Kd") {
      line >> color.r >> color.g >> color.b;
    } else if (op == "map_Kd") {
      line >> texture_path;
    }
  }
}

void parse_obj(const std::vector<char> &obj, std::vector<Element> &elements,
               std::vector<Vertex> &vertices, std::string &texture_path) {
  Timer timer("Parsing both OBJ and MTL files took ");
  auto stream = std::stringstream(obj.data());
  std::string buf;
  glm::vec3 color = {1.0, 1.0, 1.0};
  std::vector<glm::vec3> pure_vertices;
  std::vector<glm::vec2> uvs;
  while (std::getline(stream, buf)) {
    std::stringstream line(buf);
    std::string op;
    line >> op;
    if (op == "mtllib") {
      std::string mtl_path;
      line >> mtl_path;
      auto mtl_file = load_file(mtl_path);
      parse_mtl(mtl_file, color, texture_path);
    } else if (op == "v") {
      glm::vec3 pure_vertex;
      line >> pure_vertex.x >> pure_vertex.y >> pure_vertex.z;
      pure_vertex /= 100;
      pure_vertices.push_back(pure_vertex);
    } else if (op == "vt") {
      glm::vec2 uv;
      line >> uv.x >> uv.y;
      uvs.push_back(uv);
    } else if (op == "f") {
      Element e;
      for (int i = 0; i != 3; ++i) {
        Vertex vertex;
        unsigned int pure_vertex_id, uv_id;
        std::string vf;

        line >> vf;
        std::replace(vf.begin(), vf.end(), '/', ' ');
        std::stringstream ss(vf);
        ss >> pure_vertex_id >> uv_id;
        // OBJ indices start from 1, not from 0
        pure_vertex_id--;
        uv_id--;

        vertex.coord = pure_vertices[pure_vertex_id];
        vertex.color = color;
        vertex.uv = uvs[uv_id];

        vertices.push_back(vertex);

        e.vertices[i] = (unsigned int)vertices.size() - 1;
      }
      elements.push_back(e);
    }
  }
}