#pragma once

#include "utils/primitives.hpp"
#include "utils/timer.hpp"
#include <fstream>

auto load_file(const std::string &path) {
  Timer timer("Loading file " + path + " took ");
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  const std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<char> buffer(size + 1);
  if (!file.read(buffer.data(), size)) {
    throw std::runtime_error("Can't read shader file! File location: " + path +
                             '\n');
  }
  buffer.back() = '\0';
  return buffer;
}

auto parse_obj(const std::vector<char> &obj, std::vector<Element> &elements,
               std::vector<Vertex> &vertices) {
  auto stream = std::stringstream(obj.data());
  std::string buf;
  while (std::getline(stream, buf)) {
    std::stringstream line(buf);
    std::string op;
    line >> op;
    if (op == "#")
      continue;
    else if (op == "v") {
      Vertex vertex;
      line >> vertex.coord.x >> vertex.coord.y >> vertex.coord.z;
      vertex.color = {1, 1, 1};
      vertex.uv = {1, 0};
      vertices.push_back(vertex);
    } else if (op == "f") {
      Element e;
      for (int i = 0; i != 3; ++i) {
        std::string vf;
        line >> vf;
        std::replace(vf.begin(), vf.end(), '/', ' ');
        std::stringstream ss(vf);
        ss >> e.vertices[i];
        // OBJ vertex index starts from 1, not from 0
        e.vertices[i]--;
      }
      elements.push_back(e);
    }
  }
}