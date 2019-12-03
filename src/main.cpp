#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

GLuint load_shaders(std::string vert_path, std::string frag_path) {
  std::stringstream vert_shader_source, frag_shader_source;
  std::ifstream vert_shader(vert_path);
  if (!vert_shader) {
    std::cout << "Can't load vertex shader file!" << std::endl;
    return 0;
  }
  std::ifstream frag_shader(frag_path);
  if (!frag_shader) {
    std::cout << "Can't load fragment shader file!" << std::endl;
    return 0;
  }

  vert_shader_source << vert_shader.rdbuf();
  frag_shader_source << frag_shader.rdbuf();
  std::string vert_shader_str = vert_shader_source.str();
  std::string frag_shader_str = frag_shader_source.str();
  const char *vert_shader_ptr = vert_shader_str.c_str();
  const char *frag_shader_ptr = frag_shader_str.c_str();

  // Create and compile the vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vert_shader_ptr, NULL);
  glCompileShader(vertexShader);

  GLint result = GL_FALSE;
  int info_log_length = 0;

  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
  glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &info_log_length);
  if (info_log_length > 0) {
    std::vector<char> error_message(info_log_length + 1);
    glGetShaderInfoLog(vertexShader, info_log_length, NULL, &error_message[0]);
    std::cout << "Error compiling vertex shader!" << std::endl;
    for (auto &i : error_message)
      std::cout << i;
    std::cout << std::endl;
  }

  // Create and compile the fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &frag_shader_ptr, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
  glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &info_log_length);
  if (info_log_length > 0) {
    std::vector<char> error_message(info_log_length + 1);
    glGetShaderInfoLog(fragmentShader, info_log_length, NULL,
                       &error_message[0]);
    std::cout << "Error compiling fragment shader!" << std::endl;
    for (auto &i : error_message)
      std::cout << i;
    std::cout << std::endl;
  }

  // Link the vertex and fragment shader into a shader program
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glBindFragDataLocation(shaderProgram, 0, "outColor");
  glLinkProgram(shaderProgram);

  // Check the program
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
  glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &info_log_length);
  if (info_log_length > 0) {
    std::vector<char> error_message(info_log_length + 1);
    glGetProgramInfoLog(shaderProgram, info_log_length, NULL,
                        &error_message[0]);
    std::cout << "Error compiling shader program!" << std::endl;
    for (auto &i : error_message)
      std::cout << i;
    std::cout << std::endl;
  }

  glDetachShader(shaderProgram, vertexShader);
  glDetachShader(shaderProgram, fragmentShader);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgram;
}

int main(int argc, char *argv[]) {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_Window *window =
      SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
  SDL_GLContext context = SDL_GL_CreateContext(window);

  glewExperimental = GL_TRUE;
  glewInit();

  // Create Vertex Array Object
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Create a Vertex Buffer Object and copy the vertex data to it
  GLuint vbo;
  glGenBuffers(1, &vbo);

  GLuint ebo;
  glGenBuffers(1, &ebo);

  float vertices[] = {
      -0.5f, -0.5f, 0.5f,  1.0f, 1.0f, 1.0f, // Forward bottom-left
      -0.5f, 0.5f,  0.5f,  1.0f, 1.0f, 1.0f, // Forward top-left
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, // Forward top-right
      0.5f,  -0.5f, 0.5f,  1.0f, 1.0f, 1.0f, // Forward bottom-right
      -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, // Back bottom-left
      -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f, 1.0f, // Back top-left
      0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 1.0f, // Back top-right
      0.5f,  -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, // Back bottom-right
  };

  GLuint elements[] = {
      0, 3, 1, 1, 3, 2, // Front
      5, 7, 4, 5, 6, 7, // Back
      0, 1, 4, 5, 4, 1, // Left
      2, 3, 7, 2, 7, 6, // Right
      5, 1, 2, 6, 5, 2, // Top
      4, 1, 0, 4, 7, 1  // Bottom

  };

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements,
               GL_STATIC_DRAW);

  // Reading shaders from files
  GLint shaderProgram =
      load_shaders("./src/shaders/shader.vert", "./src/shaders/shader.frag");
  glUseProgram(shaderProgram);

  // Specify the layout of the vertex data
  GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);

  GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
  glEnableVertexAttribArray(colAttrib);
  glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));

  // Calculate MVP matrix
  glm::mat4 projection_matrix =
      glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
  glm::mat4 view_matrix =
      glm::lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  glm::mat4 model_matrix = glm::mat4(1.0f);
  glm::mat4 mvp_matrix = projection_matrix * view_matrix * model_matrix;

  GLuint matrix_uniform = glGetUniformLocation(shaderProgram, "mvp_matrix");
  glUniformMatrix4fv(matrix_uniform, 1, GL_FALSE, &mvp_matrix[0][0]);

  SDL_Event windowEvent;
  while (true) {
    if (SDL_PollEvent(&windowEvent)) {
      if (windowEvent.type == SDL_QUIT)
        break;
    }
    // Clear the screen to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw a triangle from the 3 vertices
    glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, 0);

    SDL_GL_SwapWindow(window);
  }
  glDeleteProgram(shaderProgram);
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);

  SDL_GL_DeleteContext(context);
  SDL_Quit();
  return 0;
}