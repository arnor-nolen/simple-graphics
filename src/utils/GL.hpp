#pragma once

#include "utils/io.hpp"
#include <GL/glew.h>

namespace gl {

void enable(const std::vector<GLenum> &features);

void check_error_log(const GLuint &object,
                     void (*glGet)(GLuint, GLenum, GLint *),
                     void (*glGetLog)(GLuint, GLsizei, GLsizei *, GLchar *));

struct Shader {
  explicit Shader(GLenum shader_type);
  ~Shader();

  Shader(const Shader &) = delete;
  Shader(Shader &&other) noexcept;
  auto operator=(const Shader &) -> Shader & = delete;
  auto operator=(Shader &&other) noexcept -> Shader &;

  void swap(Shader &other);

  void load(std::string_view path);

  [[nodiscard]] auto get() const -> const GLuint &;

private:
  void compile(const std::vector<char> &source) const;

  GLuint shader_ = 0;
};

struct Program {
  Program();
  ~Program();

  Program(const Program &) = delete;
  Program(Program &&other) noexcept;
  auto operator=(const Program &) -> Program & = delete;
  auto operator=(Program &&other) noexcept -> Program &;

  void swap(Program &other);
  [[nodiscard]] auto get() const -> const GLuint &;
  [[nodiscard]] auto get_matrix_uniform() const -> const GLuint &;

  void use() const;

  void compile(const std::vector<Shader> &shaders);

private:
  void attach(const Shader &shader) const;
  void attach(const std::vector<Shader> &list);

  void detach(const Shader &shader) const;
  void detach(const std::vector<Shader> &list) const;

  void link() const;

  void set_input() const;
  void set_output() const;

  GLuint shader_program_ = 0;
  GLuint matrix_uniform_ = 0;
};

template <typename T> struct Buffer {
  Buffer() = default;
  Buffer(const GLenum &buffer_type, std::vector<T> &&data);
  ~Buffer();

  Buffer(const Buffer &) = delete;
  Buffer(Buffer &&other) noexcept;
  auto operator=(const Buffer &) -> Buffer & = delete;
  auto operator=(Buffer &&other) noexcept -> Buffer &;

  void swap(Buffer &other);
  auto get_data() const -> const std::vector<T> &;
  void bind();

private:
  void set_layout();

  GLuint buf_ = 0;
  GLenum buffer_type_ = 0;
  std::vector<T> data_;
};

struct VertexArrayObject {
  VertexArrayObject();
  ~VertexArrayObject();

  VertexArrayObject(const VertexArrayObject &) = delete;
  VertexArrayObject(VertexArrayObject &&other) noexcept = delete;
  auto operator=(const VertexArrayObject &) -> VertexArrayObject & = delete;
  auto operator=(VertexArrayObject &&other) noexcept
      -> VertexArrayObject & = delete;

private:
  GLuint vao_ = 0;
};

struct Texture {
  Texture() = default;
  Texture(std::string_view path);
  Texture(size_t width, size_t height, void *pixels);
  ~Texture();

  Texture(const Texture &) = delete;
  Texture(Texture &&other) noexcept;
  auto operator=(const Texture &) -> Texture & = delete;
  auto operator=(Texture &&other) noexcept -> Texture &;

  void swap(Texture &other);
  void bind() const;

private:
  void create(size_t width, size_t height, void *pixels);

  GLuint texture_id_ = 0;
};

} // namespace gl

// Include all template implementations
#include "utils/GL_impl.hpp"