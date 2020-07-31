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
} // namespace gl