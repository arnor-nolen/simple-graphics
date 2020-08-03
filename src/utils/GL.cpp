#include "utils/GL.hpp"

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
    glGetLog(object, info_log_length, nullptr, error_message.data());
    std::cerr << "Error during shader compilation or program linkage!\n"
              << error_message.data() << '\n';
  }
}

Shader::Shader(GLenum shader_type) : shader_(glCreateShader(shader_type)) {}
Shader::~Shader() { glDeleteShader(shader_); }

Shader::Shader(Shader &&other) noexcept { swap(other); }
auto Shader::operator=(Shader &&other) noexcept -> Shader & {
  swap(other);
  return *this;
}

void Shader::swap(Shader &other) { std::swap(this->shader_, other.shader_); }
void Shader::load(const std::string &path) {
  auto source = load_file(path);
  compile(source);
}
auto Shader::get() const -> const GLuint & { return shader_; };
void Shader::compile(const std::vector<char> &source) const {
  const char *shader_ptr = source.data();

  glShaderSource(shader_, 1, &shader_ptr, nullptr);
  glCompileShader(shader_);
  check_error_log(shader_, glGetShaderiv, glGetShaderInfoLog);
}

Program::Program() : shader_program_(glCreateProgram()) {}
Program::~Program() { glDeleteProgram(shader_program_); }

Program::Program(Program &&other) noexcept { swap(other); }
auto Program::operator=(Program &&other) noexcept -> Program & {
  swap(other);
  return *this;
};

void Program::swap(Program &other) {
  std::swap(this->shader_program_, other.shader_program_);
}

auto Program::get() const -> const GLuint & { return shader_program_; }

auto Program::get_matrix_uniform() const -> const GLuint & {
  return matrix_uniform_;
}

void Program::use() const { glUseProgram(shader_program_); }

void Program::compile(const std::vector<Shader> &shaders) {
  attach(shaders);
  set_output();
  link();
  detach(shaders);
  set_input();
  matrix_uniform_ = glGetUniformLocation(shader_program_, "mvp_matrix");
}

void Program::attach(const Shader &shader) const {
  glAttachShader(shader_program_, shader.get());
}

void Program::attach(const std::vector<Shader> &list) {
  for (const auto &i : list) {
    attach(i);
  }
}

void Program::detach(const Shader &shader) const {
  glDetachShader(shader_program_, shader.get());
}

void Program::detach(const std::vector<Shader> &list) const {
  for (const auto &i : list) {
    detach(i);
  }
}

void Program::link() const {
  glLinkProgram(shader_program_);
  check_error_log(shader_program_, glGetProgramiv, glGetProgramInfoLog);
}

void Program::set_input() const {
  // Enable shader attributes
  GLint posAttrib = glGetAttribLocation(shader_program_, "position");
  glEnableVertexAttribArray(posAttrib);

  GLint colAttrib = glGetAttribLocation(shader_program_, "vertex_color");
  glEnableVertexAttribArray(colAttrib);

  GLint uvAttrib = glGetAttribLocation(shader_program_, "vertex_uv");
  glEnableVertexAttribArray(uvAttrib);
}

void Program::set_output() const {
  glBindFragDataLocation(shader_program_, 0, "program_color");
}

VertexArrayObject::VertexArrayObject() {
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);
}
VertexArrayObject::~VertexArrayObject() { glDeleteVertexArrays(1, &vao_); }

Texture::Texture(const std::string &path) {
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
Texture::~Texture() { glDeleteTextures(1, &texture_id_); }

Texture::Texture(Texture &&other) noexcept { swap(other); }
auto Texture::operator=(Texture &&other) noexcept -> Texture & {
  swap(other);
  return *this;
};

void Texture::swap(Texture &other) {
  std::swap(this->texture_id_, other.texture_id_);
}
void Texture::bind() const { glBindTexture(GL_TEXTURE_2D, texture_id_); }

} // namespace gl