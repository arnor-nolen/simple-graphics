#pragma once

#include "utils/GL.hpp"
#include "utils/io.hpp"
#include "utils/primitives.hpp"
#include <GL/glew.h>

struct Model {
  Model(const std::vector<Element> &elements,
        const std::vector<Vertex> &vertices)
      : ebo_(GL_ELEMENT_ARRAY_BUFFER, elements),
        vbo_(GL_ARRAY_BUFFER, vertices) {}
  // Model(const std::string &path) {
  //   auto file = load_file(path);
  //   auto stream = std::stringstream(file.data());
  //   std::vector<Element> elements;
  //   std::vector<Vertex> vertices;
  //   std::string buf;
  //   while (std::getline(stream, buf)) {
  //     std::stringstream line(buf);
  //     std::string op;
  //     line >> op;
  //     if (op == "#")
  //       continue;
  //     else if (op == "v") {
  //       Vertex vertex;
  //       line >> vertex.coord.x >> vertex.coord.y >> vertex.coord.z;
  //       vertex.color = {1, 1, 1};
  //       vertex.uv = {1, 0};
  //       vertices.push_back(vertex);
  //     } else if (op == "f") {
  //       Element e;
  //       for (int i = 0; i != 3; ++i) {
  //         std::string vf;
  //         line >> vf;
  //         std::replace(vf.begin(), vf.end(), '/', ' ');
  //         std::stringstream ss(vf);
  //         ss >> e.vertices[i];
  //         // OBJ vertex index starts from 1, not from 0
  //         e.vertices[i]--;
  //       }
  //       elements.push_back(e);
  //     }
  //   }
  //   ebo_ = gl::Buffer<Element>(elements);
  //   vbo_ = gl::Buffer<Vertex>(vertices);

  //   bind_buffers();
  // }

  ~Model() {}

  void render() {
    bind_buffers();
    set_layout();
    glDrawElements(GL_TRIANGLES,
                   static_cast<GLsizei>(ebo_.get_data().size() * 3),
                   GL_UNSIGNED_INT, 0);
  }

private:
  void bind_buffers() {
    ebo_.bind();
    vbo_.bind();
  }

  void set_layout() {
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(6 * sizeof(float)));
  }

  gl::Buffer<Element> ebo_;
  gl::Buffer<Vertex> vbo_;
};