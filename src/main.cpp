#include "core/model.hpp"
#include "core/resource_manager.hpp"

#include "settings.hpp"
#include "utils/GL.hpp"
#include "utils/SDL.hpp"
#include "utils/imgui.hpp"
#include "utils/timer.hpp"
#include <glm/gtx/transform.hpp>
#include <string_view>

void toggle_fullscreen(const sdl2::unique_ptr<SDL_Window> &window) {
  switch (settings::fullscreen) {
  case 0:
    settings::fullscreen = SDL_WINDOW_FULLSCREEN;
    SDL_DisplayMode dm;
    SDL_GetDesktopDisplayMode(0, &dm);
    settings::fullscreen_resolution = {dm.w, dm.h};
    SDL_SetWindowSize(window.get(), dm.w, dm.h);
    SDL_SetWindowFullscreen(window.get(), settings::fullscreen);
    break;
  case SDL_WINDOW_FULLSCREEN:
  case SDL_WINDOW_FULLSCREEN_DESKTOP:
    settings::fullscreen = 0;
    SDL_SetWindowFullscreen(window.get(), settings::fullscreen);
    SDL_SetWindowSize(window.get(), settings::window_resolution.w,
                      settings::window_resolution.h);
    break;
  default:
    break;
  }
}

auto main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) -> int try {
  auto sdl = sdl2::SDL(SDL_INIT_VIDEO);

  // Set up OpenGL attributes
  sdl2::gl_setAttributes(
      {{SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE},
       {SDL_GL_CONTEXT_MAJOR_VERSION, settings::opengl_version.major},
       {SDL_GL_CONTEXT_MINOR_VERSION, settings::opengl_version.minor},
       {SDL_GL_STENCIL_SIZE, settings::stencil_size}});

  // Enable 4x Antialiasing
  sdl2::gl_setAttributes(
      {{SDL_GL_MULTISAMPLEBUFFERS, settings::multisample.buffers},
       {SDL_GL_MULTISAMPLESAMPLES, settings::multisample.samples}});

  // Create window
  auto window_flags = static_cast<unsigned int>(SDL_WINDOW_OPENGL) |
                      static_cast<unsigned int>(SDL_WINDOW_RESIZABLE);
  window_flags |= settings::fullscreen;
  auto window = sdl2::unique_ptr<SDL_Window>(SDL_CreateWindow(
      "Simple graphics engine", settings::window_position.x,
      settings::window_position.y, settings::window_resolution.w,
      settings::window_resolution.h, window_flags));
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

  // Initialize ImGui
  auto imgui = imgui::Imgui(window, context);

  // Create Vertex Array Object
  // Single VAO for entire application
  auto vao = gl::VertexArrayObject();

  // Create resource manager
  auto resource_manager = ResourceManager();

  // Loading resources
  resource_manager.load_shaders("./src/shaders/shader.vert",
                                "./src/shaders/shader.frag");
  auto &model1 =
      resource_manager.load_model(LOADER_ASSIMP, "./resources/AK-47.fbx",
                                  "./resources/textures/Ak-47_Albedo.png");
  // resource_manager.load_model("./resources/lowpoly_city_triangulated.obj");

  auto &models = resource_manager.get_models();

  constexpr double model1_scale = 10.0;
  model1.set_scale(glm::dvec3(model1_scale, model1_scale, model1_scale));
  model1.set_offset(glm::dvec3(0.0, 0.0, 0.0));
  model1.settings.name = "AK-47";

  // Setting up the demo scene
  constexpr auto fov = glm::radians(45.0);
  constexpr double z_near = 0.1;
  constexpr double z_far = 100.0;

  constexpr auto camera_position = glm::dvec3(12, 9, 9);
  constexpr auto scene_center = glm::dvec3(0);
  constexpr auto up_direction = glm::dvec3(0, 1, 0);

  constexpr auto identity_matrix = glm::dmat4(1.0);

  // Calculate View matrix
  glm::dmat4 view_matrix =
      glm::lookAt(camera_position, scene_center, up_direction);

  auto aspect_ratio = settings::window_resolution.w /
                      static_cast<double>(settings::window_resolution.h);

  // Game loop
  SDL_Event e;
  bool should_quit = false;
  auto t_start = std::chrono::steady_clock::now();

  std::array<char, settings::file_str_size> file_str{};
  std::array<char, settings::file_str_size> albedo_str{};

  std::array<char, settings::file_str_size> model_name{};

  loader_enum loader = LOADER_OBJ;
  bool preserve_scale_ratio = true;

  bool show_open_dialogue = false;

  // Render model window at a fixed position
  constexpr float model_window_width = 300.0;
  constexpr float main_menu_bar_height = 20.0;
  ImVec2 model_window_pos = ImVec2(
      static_cast<float>(settings::window_resolution.w) - model_window_width,
      main_menu_bar_height);
  ImVec2 model_window_size = ImVec2(
      model_window_width,
      static_cast<float>(settings::window_resolution.h) - main_menu_bar_height);

  while (!should_quit) {
    // Check for window events
    if (SDL_PollEvent(&e) != 0) {
      ImGui_ImplSDL2_ProcessEvent(&e);
      switch (e.type) {
      case SDL_QUIT:
        // Handle app quit
        should_quit = true;
        break;
      case SDL_WINDOWEVENT:
        switch (e.window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
          // Handle window resize
          glViewport(0, 0, e.window.data1, e.window.data2);
          if (!static_cast<bool>(settings::fullscreen)) {
            settings::window_resolution = {e.window.data1, e.window.data2};
          }
          aspect_ratio = e.window.data1 / static_cast<double>(e.window.data2);

          // Set model window position
          if (static_cast<bool>(settings::fullscreen)) {
            model_window_pos =
                ImVec2(static_cast<float>(settings::fullscreen_resolution.w) -
                           model_window_width,
                       main_menu_bar_height);
            model_window_size =
                ImVec2(model_window_width,
                       static_cast<float>(settings::fullscreen_resolution.h) -
                           main_menu_bar_height);
          } else {
            model_window_pos =
                ImVec2(static_cast<float>(settings::window_resolution.w) -
                           model_window_width,
                       main_menu_bar_height);
            model_window_size =
                ImVec2(model_window_width,
                       static_cast<float>(settings::window_resolution.h) -
                           main_menu_bar_height);
          }
          break;
        }
      case SDL_KEYDOWN:
        switch (e.key.keysym.sym) {
        case SDLK_RETURN:
          if (static_cast<bool>(e.key.keysym.mod &
                                (static_cast<unsigned int>(KMOD_LALT) |
                                 static_cast<unsigned int>(KMOD_RALT)))) {
            // Handle fullscreen toggle
            toggle_fullscreen(window);
          }
          break;
        }
        break;
      }
    }

    // Clear the screen to black
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(static_cast<unsigned int>(GL_COLOR_BUFFER_BIT) |
            static_cast<unsigned int>(GL_DEPTH_BUFFER_BIT));

    // Delete models marked for deletion using erase-remove idiom
    models.erase(
        std::remove_if(models.begin(), models.end(),
                       [](const Model &m) { return m.settings.delete_me; }),
        models.end());

    // Calculate new projection matrix to match aspect ratio
    glm::dmat4 projection_matrix =
        glm::perspective(fov, aspect_ratio, z_near, z_far);

    // Calculate new model mvp matrices
    for (auto &&model : models) {
      model.calculate_mvp_matrix(projection_matrix, view_matrix);
    }

    // Clock to rotate models
    auto t_now = std::chrono::steady_clock::now();
    auto time =
        std::chrono::duration_cast<std::chrono::microseconds>(t_now - t_start);

    constexpr double time_delta = 0.001 * 0.001 * 0.1;
    constexpr double pi_rad = 180.0;
    constexpr auto axis = glm::dvec3(0.0, 1.0, 0.0);

    auto rotation_time = time.count() * time_delta;

    for (auto &&model : models) {
      if (model.settings.is_rotating) {
        model.rotate(rotation_time * glm::radians(pi_rad), axis);
      }
    }

    // Render all the models
    resource_manager.render_all();

    // Start the Dear ImGui frame
    imgui.create_frame(window);

    // Open model window
    if (show_open_dialogue) {
      ImGui::Begin("Open model", &show_open_dialogue);

      ImGui::InputTextWithHint("Model name", "Enter a model name...",
                               model_name.data(), model_name.size());

      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg, hicpp-vararg)
      ImGui::Text("Choose a loader");

      if (ImGui::RadioButton("OBJ loader", loader == LOADER_OBJ)) {
        loader = LOADER_OBJ;
      }
      ImGui::SameLine();
      if (ImGui::RadioButton("Assimp loader (fbx only)",
                             loader == LOADER_ASSIMP)) {
        loader = LOADER_ASSIMP;
      }

      ImGui::InputTextWithHint("Model location", "Enter file location...",
                               file_str.data(), file_str.size());
      if (loader == LOADER_ASSIMP) {
        ImGui::InputTextWithHint("Albedo map location",
                                 "Enter file location...", albedo_str.data(),
                                 albedo_str.size());
      }

      if (ImGui::Button("Open")) {
        auto &model = resource_manager.load_model(loader, file_str.data(),
                                                  albedo_str.data());
        model.settings.name = std::string(model_name.data());
        show_open_dialogue = false;
      }

      ImGui::End();
    }

    // Render model window at a fixed position
    ImGui::SetNextWindowPos(model_window_pos);
    ImGui::SetNextWindowSize(model_window_size);

    // Models view
    constexpr unsigned int STATIC_WINDOW_FLAGS =
        static_cast<unsigned int>(ImGuiWindowFlags_NoResize) |
        static_cast<unsigned int>(ImGuiWindowFlags_NoCollapse) |
        static_cast<unsigned int>(ImGuiWindowFlags_NoMove);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0F);
    ImGui::Begin("Models", nullptr, STATIC_WINDOW_FLAGS);
    ImGui::PopStyleVar();
    for (auto &&model : models) {
      auto &scale = model.settings.scale;
      auto &offset = model.settings.offset;
      auto &name = model.settings.name;
      auto &is_open = model.settings.is_open;
      auto &is_rotating = model.settings.is_rotating;

      ImGui::PushID(&model);
      ImGui::SetNextItemOpen(is_open);
      is_open =
          ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth);

      if (is_open) {
        constexpr double MIN_SCALE = 0.001;
        constexpr double MAX_SCALE = 1000.0;
        if (preserve_scale_ratio) {
          if (ImGui::SliderScalar("Scale", ImGuiDataType_Double, scale.data(),
                                  &MIN_SCALE, &MAX_SCALE, "%.4f",
                                  ImGuiSliderFlags_Logarithmic)) {
            scale[1] = scale[0];
            scale[2] = scale[0];
            model.set_scale(glm::dvec3(scale[0], scale[1], scale[2]));
          }
        } else {
          if (ImGui::SliderScalarN("Scale", ImGuiDataType_Double, scale.data(),
                                   3, &MIN_SCALE, &MAX_SCALE, "%.4f",
                                   ImGuiSliderFlags_Logarithmic)) {
            model.set_scale(glm::dvec3(scale[0], scale[1], scale[2]));
          }
        }

        if (ImGui::Checkbox("Preserve scale ratio", &preserve_scale_ratio)) {
          scale[1] = scale[0];
          scale[2] = scale[0];
          model.set_scale(glm::dvec3(scale[0], scale[1], scale[2]));
        }

        constexpr double MIN_OFFSET = -10.0;
        constexpr double MAX_OFFSET = 10.0;
        if (ImGui::SliderScalarN("Offset", ImGuiDataType_Double, offset.data(),
                                 3, &MIN_OFFSET, &MAX_OFFSET, "%.4f")) {
          model.set_offset(glm::dvec3(offset[0], offset[1], offset[2]));
        }
        ImGui::Checkbox("Rotate", &is_rotating);
        if (ImGui::Button("Delete")) {
          model.settings.delete_me = true;
        }
        ImGui::TreePop();
      }
      ImGui::PopID();
    }
    ImGui::End();

    if (ImGui::BeginMainMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Open", "Ctrl+O")) {
          show_open_dialogue = true;
        }

        if (ImGui::MenuItem("Fullscreen", "Alt-Enter",
                            static_cast<bool>(settings::fullscreen))) {
          toggle_fullscreen(window);
        }

        if (ImGui::MenuItem("Quit")) {
          should_quit = true;
        }
        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }

    // Render ImGui
    imgui.render();

    // Swap window
    SDL_GL_SwapWindow(window.get());
  }
  return 0;
} catch (const std::exception &e) {
  std::cerr << e.what() << '\n';
  return 1;
}
