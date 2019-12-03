#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <fstream>
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
  glUseProgram(shaderProgram);

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
      -0.5f, 0.5f,  1.0f, 0.0f,
      0.0f, // Top-left
      0.5f,  0.5f,  0.0f, 1.0f,
      0.0f, // Top-right
      0.5f,  -0.5f, 0.0f, 0.0f,
      1.0f, // Bottom-right
      -0.5f, -0.5f, 1.0f, 1.0f,
      1.0f // Bottom-left
  };

  GLuint elements[] = {0, 1, 2, 2, 3, 0};

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements,
               GL_STATIC_DRAW);

  // Reading shaders from files
  GLint shaderProgram =
      load_shaders("./src/shaders/shader1.vert", "./src/shaders/shader2.frag");

  // Specify the layout of the vertex data
  GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

  GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
  glEnableVertexAttribArray(colAttrib);
  glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(2 * sizeof(float)));

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
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    SDL_GL_SwapWindow(window);
  }
  glDeleteProgram(shaderProgram);
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);

  SDL_GL_DeleteContext(context);
  SDL_Quit();
  return 0;
}