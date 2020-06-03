#include "pch.h"

#include "core/model.hpp"
#include "core/resource_manager.hpp"

#include "utils/GL.hpp"
#include "utils/SDL.hpp"
#include "utils/flip_vertical.hpp"
#include "utils/io.hpp"
#include "utils/timer.hpp"

// GLuint texture_id;

// void load_image(std::string path) {
//   auto loaded_surface =
//   sdl2::unique_ptr<SDL_Surface>(IMG_Load(path.c_str())); if (!loaded_surface)
//   {
//     std::cerr << "Unable to load image " << path << "!\n"
//               << "SDL_image error: " << IMG_GetError() << "\n";
//   } else {
//     // SDL and OpenGL have different coordinates, we have to flip the surface
//     auto flipped_surface = flip_vertical(loaded_surface);

//     // Create texture
//     glGenTextures(1, &texture_id);
//     glBindTexture(GL_TEXTURE_2D, texture_id);

//     // Load image
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, flipped_surface->w,
//                  flipped_surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
//                  flipped_surface->pixels);

//     // Nice trilinear filtering with mipmaps
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
//                     GL_LINEAR_MIPMAP_LINEAR);
//     glGenerateMipmap(GL_TEXTURE_2D);
//   }
//   return;
// }

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) try {
  auto sdl = sdl2::SDL(SDL_INIT_VIDEO);

  // TODO: CHANGE TO VARIADIC TEMPLATES!
  sdl2::gl_setAttributes(
      {{SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE},
       {SDL_GL_CONTEXT_MAJOR_VERSION, 4},
       {SDL_GL_CONTEXT_MINOR_VERSION, 1},
       {SDL_GL_STENCIL_SIZE, 8}});

  // Enable 4x Antialiasing
  sdl2::gl_setAttributes(
      {{SDL_GL_MULTISAMPLEBUFFERS, 1}, {SDL_GL_MULTISAMPLESAMPLES, 4}});

  // Create window
  auto window = sdl2::unique_ptr<SDL_Window>(
      SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL));
  auto context = sdl2::SDL_Context(window);

  // Enable GLEW and SDL_image
  glewExperimental = GL_TRUE;
  glewInit();
  auto sdl_image = sdl2::SDL_image(IMG_INIT_PNG | IMG_INIT_JPG);

  // Enable VSync
  if (SDL_GL_SetSwapInterval(1) == -1) {
    std::cerr << "Error enabling VSync!\n";
  };

  // Enable depth test and antialiasing
  // TODO: CHANGE TO VARIADIC TEMPLATES!
  gl::enable({GL_DEPTH_TEST, GL_MULTISAMPLE});
  glDepthFunc(GL_LESS);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // Load resources
  // load_image("./resources/nazeeboPepega.png");

  // Loading shaders
  std::vector<gl::Shader> shaders;
  shaders.emplace_back(gl::Shader(GL_VERTEX_SHADER));
  shaders.emplace_back(gl::Shader(GL_FRAGMENT_SHADER));

  shaders[0].load("./src/shaders/shader.vert");
  shaders[1].load("./src/shaders/shader.frag");

  // Link the vertex and fragment shader into a shader program
  auto program = gl::Program();
  program.attach(shaders);

  glBindFragDataLocation(program.get(), 0, "program_color");

  program.link();
  program.detach(shaders);
  shaders.clear();
  program.use();

  // Create Vertex Array Object
  // Single VAO for entire application
  auto vao = sdl2::VertexArrayObject();

  // Enable shader attributes
  GLint posAttrib = glGetAttribLocation(program.get(), "position");
  glEnableVertexAttribArray(posAttrib);

  GLint colAttrib = glGetAttribLocation(program.get(), "vertex_color");
  glEnableVertexAttribArray(colAttrib);

  GLint uvAttrib = glGetAttribLocation(program.get(), "vertex_uv");
  glEnableVertexAttribArray(uvAttrib);

  std::vector<Vertex> vertices = {
      // Position, Color, UV
      {{-0.5f, -0.5f, 0.5f},
       {0.583f, 0.771f, 0.014f},
       {0.0f, 0.0f}}, // Front bottom-left

      {{-0.5f, 0.5f, 0.5f},
       {0.609f, 0.115f, 0.436f},
       {0.0f, 1.0f}}, // Front top-left

      {{0.5f, 0.5f, 0.5f},
       {0.327f, 0.483f, 0.844f},
       {1.0f, 1.0f}}, // Front top-right

      {{0.5f, -0.5f, 0.5f},
       {0.822f, 0.569f, 0.201f},
       {1.0f, 0.0f}}, // Front bottom-right

      {{-0.5f, -0.5f, -0.5f},
       {0.602f, 0.223f, 0.310f},
       {0.0f, 0.0f}}, // Back bottom-left

      {{-0.5f, 0.5f, -0.5f},
       {0.747f, 0.185f, 0.597f},
       {0.0f, 0.0f}}, // Back top-left

      {{0.5f, 0.5f, -0.5f},
       {0.770f, 0.761f, 0.559f},
       {0.0f, 0.0f}}, // Back top-right

      {{0.5f, -0.5f, -0.5f},
       {0.971f, 0.572f, 0.833f},
       {0.0f, 0.0f}}, // Back bottom-right
  };

  std::vector<Element> elements = {
      {0, 3, 1}, {1, 3, 2}, // Front
      {5, 7, 4}, {5, 6, 7}, // Back
      {0, 1, 4}, {5, 4, 1}, // Left
      {2, 3, 7}, {2, 7, 6}, // Right
      {5, 1, 2}, {6, 5, 2}, // Top
      {4, 1, 0}, {4, 7, 1}  // Bottom

  };

  auto resource_manager = ResourceManager();
  resource_manager.load_model(elements, vertices);
  resource_manager.load_model("./resources/teapot.obj");

  // GLuint texture_uniform = glGetUniformLocation(program.get(), "tex");
  // glUniform1i(texture_uniform, 0);

  // Calculate MVP matrix
  glm::mat4 projection_matrix =
      glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
  glm::mat4 view_matrix =
      glm::lookAt(glm::vec3(12, 9, 9), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  glm::mat4 model_matrix = glm::mat4(1.0f);
  glm::mat4 mvp_matrix = projection_matrix * view_matrix * model_matrix;

  GLuint matrix_uniform = glGetUniformLocation(program.get(), "mvp_matrix");

  // Game loop
  SDL_Event windowEvent;
  auto t_start = std::chrono::high_resolution_clock::now();
  while (true) {
    // Timer timer;
    if (SDL_PollEvent(&windowEvent)) {
      if (windowEvent.type == SDL_QUIT)
        break;
    }
    auto t_now = std::chrono::high_resolution_clock::now();
    auto time =
        std::chrono::duration_cast<std::chrono::microseconds>(t_now - t_start);

    auto rotation_time = time.count() * 0.001f * 0.001f * 0.1f;
    auto rotated_mvp =
        glm::rotate(mvp_matrix, rotation_time * glm::radians(180.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(matrix_uniform, 1, GL_FALSE, &rotated_mvp[0][0]);

    // Clear the screen to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render all the models
    resource_manager.render_all();

    SDL_GL_SwapWindow(window.get());
  }
  // glDeleteTextures(1, &texture_id);

  return 0;
} catch (const std::exception &e) {
  std::cerr << e.what() << '\n';
  return 1;
}
