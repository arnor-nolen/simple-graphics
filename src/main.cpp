#include "pch.h"

#include "core/model.hpp"
#include "core/resource_manager.hpp"

#include "utils/GL.hpp"
#include "utils/SDL.hpp"
#include "utils/flip_vertical.hpp"
#include "utils/io.hpp"
#include "utils/timer.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) try {
  auto sdl = sdl2::SDL(SDL_INIT_VIDEO);

  // TODO: CHANGE TO VARIADIC ARGUMENTS!
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
  // TODO: CHANGE TO VARIADIC ARGUMENTS!
  gl::enable({GL_DEPTH_TEST, GL_MULTISAMPLE});
  glDepthFunc(GL_LESS);
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // Create Vertex Array Object
  // Single VAO for entire application
  auto vao = gl::VertexArrayObject();

  auto resource_manager = ResourceManager();
  // Loading models
  resource_manager.load_model("./resources/lowpoly_city_triangulated.obj");
  resource_manager.load_model(
      "./resources/lowpoly_helicopter_triangulated.obj");

  // Loading shaders
  std::vector<gl::Shader> shaders;
  shaders.emplace_back(gl::Shader(GL_VERTEX_SHADER));
  shaders.emplace_back(gl::Shader(GL_FRAGMENT_SHADER));

  shaders[0].load("./src/shaders/shader.vert");
  shaders[1].load("./src/shaders/shader.frag");

  // Link the vertex and fragment shader into a shader program
  auto &program = resource_manager.get_program_ptr();
  program.compile(shaders);
  program.use();

  // We don't need shaders anymore as the program is compiled
  shaders.clear();

  // Calculate MVP matrix
  glm::mat4 projection_matrix =
      glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
  glm::mat4 view_matrix =
      glm::lookAt(glm::vec3(12, 9, 9), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  auto model_matrix1 = glm::mat4(1.0f);
  auto model_matrix2 = glm::translate(glm::mat4(1.0f), glm::vec3(0, 5, 0));
  glm::mat4 mvp_matrix1 = projection_matrix * view_matrix * model_matrix1;
  glm::mat4 mvp_matrix2 = projection_matrix * view_matrix * model_matrix2;

  auto &models = resource_manager.get_models();
  models[1].set_mvp_matrix(mvp_matrix2);

  // Game loop
  SDL_Event windowEvent;
  auto t_start = std::chrono::high_resolution_clock::now();
  while (true) {
    // Timer timer;
    if (SDL_PollEvent(&windowEvent)) {
      if (windowEvent.type == SDL_QUIT)
        break;
    }
    // Clear the screen to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto t_now = std::chrono::high_resolution_clock::now();
    auto time =
        std::chrono::duration_cast<std::chrono::microseconds>(t_now - t_start);

    auto rotation_time = time.count() * 0.001f * 0.001f * 0.1f;
    auto rotated_mvp1 =
        glm::rotate(mvp_matrix1, rotation_time * glm::radians(180.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f));
    models[0].set_mvp_matrix(rotated_mvp1);

    // Render all the models
    resource_manager.render_all();

    SDL_GL_SwapWindow(window.get());
  }

  return 0;
} catch (const std::exception &e) {
  std::cerr << e.what() << '\n';
  return 1;
}
