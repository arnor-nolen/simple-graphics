#pragma once

#include "utils/io.hpp"
#include <GL/glew.h>

namespace gl {
void enable(const std::vector<GLenum> &features) {
  for (const auto &feature : features) {
    glEnable(feature);
  }
}

void check_error_log(const GLuint &object,
                     void (*glGet)(GLuint, GLenum, GLint *),
                     void (*glGetLog)(GLuint, GLsizei, GLsizei *, GLchar *)) {
  auto result = GL_FALSE;
  int info_log_length = 0;

  glGet(object, GL_COMPILE_STATUS, &result);
  glGet(object, GL_INFO_LOG_LENGTH, &info_log_length);

  if (info_log_length > 0) {
    std::vector<char> error_message(info_log_length + 1);
    glGetLog(object, info_log_length, NULL, &error_message.front());
    std::cerr << "Error during shader compilation or program linkage!\n"
              << error_message.data() << '\n';
  }
}

struct Shader {
  Shader(GLenum shader_type) : shader_(glCreateShader(shader_type)) {}
  ~Shader() { glDeleteShader(shader_); }

  Shader(const Shader &) = delete;
  Shader(Shader &&other) { swap(other); }
  const Shader &operator=(const Shader &) = delete;
  const Shader &operator=(Shader &&other) {
    swap(other);
    return *this;
  }

  void swap(Shader &other) { std::swap(this->shader_, other.shader_); }

  void load(const std::string &path) {
    auto source = load_file(path);
    compile(source);
  }

  const auto &get() const { return shader_; }

private:
  void compile(const std::vector<char> &source) {
    const char *shader_ptr = &source.front();

    glShaderSource(shader_, 1, &shader_ptr, NULL);
    glCompileShader(shader_);
    check_error_log(shader_, glGetShaderiv, glGetShaderInfoLog);
  }

  GLuint shader_;
};

struct Program {
  Program() : shader_program_(glCreateProgram()) {}
  ~Program() { glDeleteProgram(shader_program_); }

  Program(const Program &) = delete;
  Program(Program &&other) { swap(other); }
  const Program &operator=(const Program &) = delete;
  const Program &operator=(Program &&other) {
    swap(other);
    return *this;
  };

  void swap(Program &other) {
    std::swap(this->shader_program_, other.shader_program_);
  }

  const auto &get() const { return shader_program_; }

  const auto &get_matrix_uniform() const { return matrix_uniform_; }

  void use() { glUseProgram(shader_program_); }

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

  void link() {
    glLinkProgram(shader_program_);
    check_error_log(shader_program_, glGetProgramiv, glGetProgramInfoLog);
  }

  void set_input() {
    // Enable shader attributes
    GLint posAttrib = glGetAttribLocation(shader_program_, "position");
    glEnableVertexAttribArray(posAttrib);

    GLint colAttrib = glGetAttribLocation(shader_program_, "vertex_color");
    glEnableVertexAttribArray(colAttrib);

    GLint uvAttrib = glGetAttribLocation(shader_program_, "vertex_uv");
    glEnableVertexAttribArray(uvAttrib);
  }

  void set_output() {
    glBindFragDataLocation(shader_program_, 0, "program_color");
  }

  GLuint shader_program_;
  GLuint matrix_uniform_;
};

template <typename T> struct Buffer {
  Buffer() : buf_(0), buffer_type_(0) {}
  Buffer(const GLenum &buffer_type, const std::vector<T> &data)
      : buffer_type_(buffer_type), data_(data) {
    glGenBuffers(1, &buf_);
    bind();
    set_layout();
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
      glNamedBufferData(buf_, data_.size() * sizeof(std::vector<T>),
                        &data_.front(), GL_STATIC_DRAW);
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

} // namespace gl