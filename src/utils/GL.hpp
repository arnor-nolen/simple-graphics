#pragma once

#include "utils/io.hpp"
#include <GL/glew.h>

namespace gl {
void enable(const std::vector<GLenum> &features) {
  for (const auto &feature : features) {
    glEnable(feature);
  }
}

struct Shader {
  Shader(GLenum shader_type) : shader_(glCreateShader(shader_type)) {}
  ~Shader() { glDeleteShader(shader_); }

  Shader(const Shader &) = delete;
  Shader(Shader &&other) { swap(other); }
  const Shader &Shader::operator=(const Shader &) = delete;
  const Shader &Shader::operator=(Shader &&other) {
    swap(other);
    return *this;
  }

  void swap(Shader &other) { std::swap(this->shader_, other.shader_); }

  void load(const std::string &path) {
    auto source = load_file(path);
    compile(source);
  }

  const GLuint &get() const { return shader_; }

private:
  void compile(const std::vector<char> &source) {
    const char *shader_ptr = &source.front();

    glShaderSource(shader_, 1, &shader_ptr, NULL);
    glCompileShader(shader_);

    GLint result = GL_FALSE;
    int info_log_length = 0;

    glGetShaderiv(shader_, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shader_, GL_INFO_LOG_LENGTH, &info_log_length);

    if (info_log_length > 0) {
      std::vector<char> error_message(info_log_length + 1);
      glGetShaderInfoLog(shader_, info_log_length, NULL,
                         &error_message.front());
      std::cerr << "Error compiling vertex shader!\n";
      for (const auto &i : error_message)
        std::cerr << i;
      std::cerr << '\n';
    }
  }

  GLuint shader_;
};

struct Program {
  Program() : shader_program_(glCreateProgram()) {}
  ~Program() { glDeleteProgram(shader_program_); }

  Program(const Program &) = delete;
  Program(Program &&other) { swap(other); }
  const Program &Program::operator=(const Program &) = delete;
  const Program &Program::operator=(Program &&other) {
    swap(other);
    return *this;
  };

  void swap(Program &other) {
    std::swap(this->shader_program_, other.shader_program_);
  }

  const GLuint &get() const { return shader_program_; }

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

    // Check the program
    GLint result = GL_FALSE;
    int info_log_length = 0;

    glGetProgramiv(shader_program_, GL_LINK_STATUS, &result);
    glGetProgramiv(shader_program_, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
      std::vector<char> error_message(info_log_length + 1);
      glGetProgramInfoLog(shader_program_, info_log_length, NULL,
                          &error_message.front());
      std::cerr << "Error compiling shader program!\n";
      for (const auto &i : error_message)
        std::cerr << i;
      std::cerr << '\n';
    }
  }

  void use() { glUseProgram(shader_program_); }

private:
  GLuint shader_program_;
};
} // namespace gl