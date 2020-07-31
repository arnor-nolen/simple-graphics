#pragma once

#include "utils/io.hpp"
#include <GL/glew.h>

namespace gl {

void enable(const std::vector<GLenum> &features);

void check_error_log(const GLuint &object,
                     void (*glGet)(GLuint, GLenum, GLint *),
                     void (*glGetLog)(GLuint, GLsizei, GLsizei *, GLchar *));

struct Shader {
  explicit Shader(GLenum shader_type) : shader_(glCreateShader(shader_type)) {}
  ~Shader() { glDeleteShader(shader_); }

  Shader(const Shader &) = delete;
  Shader(Shader &&other) noexcept { swap(other); }
  auto operator=(const Shader &) -> const Shader & = delete;
  auto operator=(Shader &&other) noexcept -> Shader & {
    swap(other);
    return *this;
  }

  void swap(Shader &other) { std::swap(this->shader_, other.shader_); }

  void load(const std::string &path) {
    auto source = load_file(path);
    compile(source);
  }

  [[nodiscard]] auto get() const -> const auto & { return shader_; }

private:
  void compile(const std::vector<char> &source) const {
    const char *shader_ptr = source.data();

    glShaderSource(shader_, 1, &shader_ptr, nullptr);
    glCompileShader(shader_);
    check_error_log(shader_, glGetShaderiv, glGetShaderInfoLog);
  }

  GLuint shader_ = 0;
};

struct Program {
  Program() : shader_program_(glCreateProgram()) {}
  ~Program() { glDeleteProgram(shader_program_); }

  Program(const Program &) = delete;
  Program(Program &&other) noexcept { swap(other); }
  auto operator=(const Program &) -> const Program & = delete;
  auto operator=(Program &&other) noexcept -> Program & {
    swap(other);
    return *this;
  };

  void swap(Program &other) {
    std::swap(this->shader_program_, other.shader_program_);
  }

  [[nodiscard]] auto get() const -> const auto & { return shader_program_; }

  [[nodiscard]] auto get_matrix_uniform() const -> const auto & {
    return matrix_uniform_;
  }

  void use() const { glUseProgram(shader_program_); }

  void compile(const std::vector<Shader> &shaders) {
    attach(shaders);
    set_output();
    link();
    detach(shaders);
    set_input();
    matrix_uniform_ = glGetUniformLocation(shader_program_, "mvp_matrix");
  }

private:
  void attach(const Shader &shader) const {
    glAttachShader(shader_program_, shader.get());
  }

  void attach(const std::vector<Shader> &list) {
    for (const auto &i : list) {
      attach(i);
    }
  }

  void detach(const Shader &shader) const {
    glDetachShader(shader_program_, shader.get());
  }

  void detach(const std::vector<Shader> &list) const {
    for (const auto &i : list) {
      detach(i);
    }
  }

  void link() const {
    glLinkProgram(shader_program_);
    check_error_log(shader_program_, glGetProgramiv, glGetProgramInfoLog);
  }

  void set_input() const {
    // Enable shader attributes
    GLint posAttrib = glGetAttribLocation(shader_program_, "position");
    glEnableVertexAttribArray(posAttrib);

    GLint colAttrib = glGetAttribLocation(shader_program_, "vertex_color");
    glEnableVertexAttribArray(colAttrib);

    GLint uvAttrib = glGetAttribLocation(shader_program_, "vertex_uv");
    glEnableVertexAttribArray(uvAttrib);
  }

  void set_output() const {
    glBindFragDataLocation(shader_program_, 0, "program_color");
  }

  GLuint shader_program_ = 0;
  GLuint matrix_uniform_ = 0;
};

template <typename T> struct Buffer {
  Buffer() : buf_(0), buffer_type_(0) {}
  Buffer(const GLenum &buffer_type, const std::vector<T> &data)
      : buffer_type_(buffer_type), data_(data) {
    glGenBuffers(1, &buf_);
  }
  ~Buffer() { glDeleteBuffers(1, &buf_); }

  Buffer(const Buffer &) = delete;
  Buffer(Buffer &&other) { swap(other); }
  const Buffer &operator=(const Buffer &) = delete;
  const Buffer &operator=(Buffer &&other) {
    swap(other);
    return *this;
  };

  void swap(Buffer &other) {
    std::swap(this->buf_, other.buf_);
    std::swap(this->data_, other.data_);
    std::swap(this->buffer_type_, other.buffer_type_);
    this->bind();
    this->set_layout();
    other.bind();
    other.set_layout();
  }

  const auto &get_data() const { return data_; }

  void bind() { glBindBuffer(buffer_type_, buf_); }

private:
  void set_layout() {
    if (data_.size() > 0) {
      glBufferData(buffer_type_, data_.size() * sizeof(T), data_.data(),
                   GL_STATIC_DRAW);
    }
  }

  GLuint buf_;
  GLenum buffer_type_;
  std::vector<T> data_;
};

struct VertexArrayObject {
  VertexArrayObject() {
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
  }
  ~VertexArrayObject() { glDeleteVertexArrays(1, &vao_); }

private:
  GLuint vao_;
};

struct Texture {
  Texture() : texture_id_(0) {}
  Texture(const std::string &path) {
    // Load SDL_image surface from file
    auto surface = load_image(path);
    // Create texture
    glGenTextures(1, &texture_id_);
    bind();

    // Load image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, surface->pixels);

    // Nice trilinear filtering with mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  ~Texture() { glDeleteTextures(1, &texture_id_); }

  Texture(const Texture &) = delete;
  Texture(Texture &&other) { swap(other); }
  const Texture &operator=(const Texture &) = delete;
  const Texture &operator=(Texture &&other) {
    swap(other);
    return *this;
  };

  void swap(Texture &other) { std::swap(this->texture_id_, other.texture_id_); }

  void bind() { glBindTexture(GL_TEXTURE_2D, texture_id_); }

private:
  GLuint texture_id_;
};

} // namespace gl