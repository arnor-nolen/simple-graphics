#include "core/model.hpp"
#include "core/resource_manager.hpp"

#include "utils/GL.hpp"
#include "utils/SDL.hpp"
#include "utils/timer.hpp"
#include <glm/gtx/transform.hpp>
#include <string_view>

auto main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) -> int try {
  auto sdl = sdl2::SDL(SDL_INIT_VIDEO);

  // Setting up constants
  constexpr struct {
    int major;
    int minor;
  } opengl_version = {4, 1};

  constexpr int stencil_size = 8;

  constexpr struct {
    int buffers;
    int samples;
  } multisample = {1, 4};

  constexpr struct {
    int x;
    int y;
  } window_position = {100, 100};

  constexpr struct {
    int w;
    int h;
  } window_resolution = {800, 600};

  // Set up OpenGL attributes
  sdl2::gl_setAttributes(
      {{SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE},
       {SDL_GL_CONTEXT_MAJOR_VERSION, opengl_version.major},
       {SDL_GL_CONTEXT_MINOR_VERSION, opengl_version.minor},
       {SDL_GL_STENCIL_SIZE, stencil_size}});

  // Enable 4x Antialiasing
  sdl2::gl_setAttributes({{SDL_GL_MULTISAMPLEBUFFERS, multisample.buffers},
                          {SDL_GL_MULTISAMPLESAMPLES, multisample.samples}});

  // Create window
  auto window = sdl2::unique_ptr<SDL_Window>(SDL_CreateWindow(
      "Simple graphics", window_position.x, window_position.y,
      window_resolution.w, window_resolution.h, SDL_WINDOW_OPENGL));
  auto context = sdl2::SDL_Context(window);

  // Enable GLEW and SDL_image
  glewExperimental = GL_TRUE;
  glewInit();
  auto sdl_image = sdl2::SDL_image(IMG_INIT_PNG);

  // Enable VSync
  if (SDL_GL_SetSwapInterval(1) == -1) {
    std::cerr << "Error enabling VSync!\n";
  };

  // Enable depth test and antialiasing
  gl::enable({GL_DEPTH_TEST, GL_MULTISAMPLE});
  glDepthFunc(GL_LESS);

  // Create Vertex Array Object
  // Single VAO for entire application
  auto vao = gl::VertexArrayObject();

  // Create resource manager
  auto resource_manager = ResourceManager();

  // Loading models
  resource_manager.load_model("./resources/AK-47.fbx",
                              "./resources/textures/Ak-47_Albedo.png");
  resource_manager.load_model("./resources/lowpoly_city_triangulated.obj");

  // Loading shaders
  resource_manager.load_shaders("./src/shaders/shader.vert",
                                "./src/shaders/shader.frag");

  // Setting up the demo scene
  constexpr auto fov = glm::radians(45.0F);
  constexpr float aspect_ratio = 4.0F / 3.0F;
  constexpr float z_near = 0.1F;
  constexpr float z_far = 100.0F;

  constexpr auto camera_position = glm::vec3(12, 9, 9);
  constexpr auto scene_center = glm::vec3(0);
  constexpr auto up_direction = glm::vec3(0, 1, 0);

  constexpr auto identity_matrix = glm::mat4(1.0F);
  constexpr float model1_scale = 10.0F;
  constexpr auto model1_scale_matrix =
      glm::vec3(model1_scale, model1_scale, model1_scale);
  constexpr float model2_scale = 0.001F;
  constexpr auto model2_scale_matrix =
      glm::vec3(model2_scale, model2_scale, model2_scale);

  constexpr auto model2_offset = glm::vec3(0.0F, 4.0F, 0.0F);

  // Calculate MVP matrix
  glm::mat4 projection_matrix =
      glm::perspective(fov, aspect_ratio, z_near, z_far);
  glm::mat4 view_matrix =
      glm::lookAt(camera_position, scene_center, up_direction);
  auto model1_matrix = glm::scale(identity_matrix, model1_scale_matrix);
  auto model2_matrix = glm::scale(identity_matrix, model2_scale_matrix);
  glm::mat4 mvp_matrix1 = projection_matrix * view_matrix * model1_matrix;
  glm::mat4 mvp_matrix2 = projection_matrix *
                          glm::translate(view_matrix, model2_offset) *
                          model2_matrix;

  auto &models = resource_manager.get_models();
  models.at(1).set_mvp_matrix(mvp_matrix2);

  // Game loop
  SDL_Event windowEvent;
  auto t_start = std::chrono::high_resolution_clock::now();
  while (true) {
    // Check for window events
    if (SDL_PollEvent(&windowEvent) != 0) {
      if (windowEvent.type == SDL_QUIT) {
        break;
      }
    }

    // Clear the screen to black
    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
    glClear(static_cast<unsigned int>(GL_COLOR_BUFFER_BIT) |
            static_cast<unsigned int>(GL_DEPTH_BUFFER_BIT));

    // Calculate new model mvp matrices
    auto t_now = std::chrono::high_resolution_clock::now();
    auto time =
        std::chrono::duration_cast<std::chrono::microseconds>(t_now - t_start);

    constexpr float time_delta = 0.001F * 0.001F * 0.1F;
    constexpr float pi_rad = 180.0F;
    constexpr auto axis = glm::vec3(0.0F, 1.0F, 0.0F);

    auto rotation_time = time.count() * time_delta;
    auto rotated_mvp1 =
        glm::rotate(mvp_matrix1, rotation_time * glm::radians(pi_rad), axis);
    models.at(0).set_mvp_matrix(rotated_mvp1);

    // Render all the models
    resource_manager.render_all();

    // Swap window
    SDL_GL_SwapWindow(window.get());
  }

  return 0;
} catch (const std::exception &e) {
  std::cerr << e.what() << '\n';
  return 1;
}
