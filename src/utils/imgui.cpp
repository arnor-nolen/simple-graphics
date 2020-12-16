#include "utils/imgui.hpp"

namespace imgui {
Imgui::Imgui(const sdl2::unique_ptr<SDL_Window> &window,
             const sdl2::SDL_Context &context) {
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  io_ = ImGui::GetIO();

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup platform/renderer bindings
  ImGui_ImplSDL2_InitForOpenGL(window.get(), context.get());
  ImGui_ImplOpenGL3_Init("#version 410 core");
}
Imgui::~Imgui() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
}

void Imgui::create_frame(const sdl2::unique_ptr<SDL_Window> &window) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(window.get());
  ImGui::NewFrame();
}

void Imgui::render() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

} // namespace imgui