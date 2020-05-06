#include "pch.h"

GLuint texture_id;

struct Element {
  GLuint vertices[3];
};

struct Vertex {
  glm::vec3 coord;
  glm::vec3 color;
  glm::vec2 uv;
};

struct Timer {
  Timer() : start_time_(std::chrono::high_resolution_clock::now()) {}
  ~Timer() { stop(); }

  void stop() {
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        end_time - start_time_);
    std::cout << duration.count() * 0.001f << " ms\n";
  }

private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
};

struct Model {
  Model(const std::vector<Vertex> &vertices,
        const std::vector<Element> elements)
      : vertices_(vertices), elements_(elements) {
    bind_buffers();
  }
  Model(const std::string &path) {
    std::ifstream file(path);
    if (!file) {
      std::cerr << "Can't load model file!\n";
      // TODO: Exception in constructor!
    }
    std::string buf;
    while (std::getline(file, buf)) {
      std::stringstream line(buf);
      std::string op;
      line >> op;
      if (op == "#")
        continue;
      else if (op == "v") {
        Vertex vertex;
        line >> vertex.coord.x >> vertex.coord.y >> vertex.coord.z;
        vertex.color = {1, 1, 1};
        vertex.uv = {1, 0};
        vertices_.push_back(vertex);
      } else if (op == "f") {
        Element e;
        for (int i = 0; i != 3; ++i) {
          std::string vf;
          line >> vf;
          std::replace(vf.begin(), vf.end(), '/', ' ');
          std::stringstream ss(vf);
          ss >> e.vertices[i];
          // OBJ vertex index starts from 1, not from 0
          e.vertices[i]--;
        }
        elements_.push_back(e);
      }
    }

    bind_buffers();
  }

  ~Model() {
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
  }

  void render() const {
    glDrawElements(GL_TRIANGLES, elements_.size() * 3, GL_UNSIGNED_INT, 0);
  }

  const auto &get_vertices() const { return vertices_; }
  const auto &get_elements() const { return elements_; }

private:
  void bind_buffers() {
    // Bind VBO and EBO objects
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex),
                 &vertices_.front(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements_.size() * sizeof(Element),
                 &elements_.front(), GL_STATIC_DRAW);
  }

  std::vector<Vertex> vertices_;
  std::vector<Element> elements_;
  GLuint vbo, ebo;
};

SDL_Surface *flip_vertical(SDL_Surface *sfc) {
  SDL_Surface *result = SDL_CreateRGBSurface(
      sfc->flags, sfc->w, sfc->h, sfc->format->BytesPerPixel * 8,
      sfc->format->Rmask, sfc->format->Gmask, sfc->format->Bmask,
      sfc->format->Amask);
  const auto pitch = sfc->pitch;
  const auto pxlength = pitch * (sfc->h - 1);
  auto pixels = static_cast<unsigned char *>(sfc->pixels) + pxlength;
  auto rpixels = static_cast<unsigned char *>(result->pixels);
  for (auto line = 0; line != sfc->h; ++line) {
    memcpy(rpixels, pixels, pitch);
    pixels -= pitch;
    rpixels += pitch;
  }
  return result;
}

GLuint load_shaders(std::string vert_path, std::string frag_path) {
  std::stringstream vert_shader_source, frag_shader_source;
  std::ifstream vert_shader(vert_path);
  if (!vert_shader) {
    std::cerr << "Can't load vertex shader file!\n";
    return 0;
  }
  std::ifstream frag_shader(frag_path);
  if (!frag_shader) {
    std::cerr << "Can't load fragment shader file!\n";
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
    std::cerr << "Error compiling vertex shader!\n";
    for (auto &i : error_message)
      std::cerr << i;
    std::cerr << "\n";
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
    std::cerr << "Error compiling fragment shader!\n";
    for (auto &i : error_message)
      std::cerr << i;
    std::cerr << "\n";
  }

  // Link the vertex and fragment shader into a shader program
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glBindFragDataLocation(shaderProgram, 0, "program_color");
  glLinkProgram(shaderProgram);

  // Check the program
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
  glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &info_log_length);
  if (info_log_length > 0) {
    std::vector<char> error_message(info_log_length + 1);
    glGetProgramInfoLog(shaderProgram, info_log_length, NULL,
                        &error_message[0]);
    std::cerr << "Error compiling shader program!\n";
    for (auto &i : error_message)
      std::cerr << i;
    std::cerr << "\n";
  }

  glDetachShader(shaderProgram, vertexShader);
  glDetachShader(shaderProgram, fragmentShader);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgram;
}

void load_image(std::string path) {
  SDL_Surface *loaded_surface = IMG_Load(path.c_str());
  if (loaded_surface == NULL) {
    std::cerr << "Unable to load image " << path << "!\n"
              << "SDL_image error: " << IMG_GetError() << "\n";
  } else {
    // SDL and OpenGL have different coordinates, we have to flip the surface
    SDL_Surface *flipped_surface = flip_vertical(loaded_surface);
    SDL_FreeSurface(loaded_surface);

    // Create texture
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    // Load image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, flipped_surface->w,
                 flipped_surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 flipped_surface->pixels);

    SDL_FreeSurface(flipped_surface);

    // Nice trilinear filtering with mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  return;
}

int main(int argc, char *argv[]) {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  // Enable 4x Antialiasing
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

  // Create window
  SDL_Window *window =
      SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
  SDL_GLContext context = SDL_GL_CreateContext(window);

  // Enable GLEW and SDL_image
  glewExperimental = GL_TRUE;
  glewInit();
  int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
  if (!(IMG_Init(imgFlags) & imgFlags)) {
    printf("SDL_image could not initialize! SDL_image Error: %s\n",
           IMG_GetError());
    return 1;
  }

  // Enable VSync
  int swap_interval_code = SDL_GL_SetSwapInterval(1);
  if (swap_interval_code == -1) {
    std::cerr << "Error enabling VSync!\n";
  }

  // Enable depth test and antialiasing
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
  glDepthFunc(GL_LESS);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // Load resources
  load_image("./resources/nazeeboPepega.png");

  // Create Vertex Array Object
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

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
  // auto model = Model(vertices, elements);
  // auto model = Model("./resources/cube.obj");
  auto model2 = Model("./resources/teapot.obj");

  // Reading shaders from files
  GLint shaderProgram =
      load_shaders("./src/shaders/shader.vert", "./src/shaders/shader.frag");
  glUseProgram(shaderProgram);

  // Specify the layout of the vertex data
  GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);

  GLint colAttrib = glGetAttribLocation(shaderProgram, "vertex_color");
  glEnableVertexAttribArray(colAttrib);
  glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  GLint uvAttrib = glGetAttribLocation(shaderProgram, "vertex_uv");
  glEnableVertexAttribArray(uvAttrib);
  glVertexAttribPointer(uvAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  GLuint texture_uniform = glGetUniformLocation(shaderProgram, "tex");
  glUniform1i(texture_uniform, 0);

  // Calculate MVP matrix
  glm::mat4 projection_matrix =
      glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
  glm::mat4 view_matrix =
      glm::lookAt(glm::vec3(12, 9, 9), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  glm::mat4 model_matrix = glm::mat4(1.0f);
  glm::mat4 mvp_matrix = projection_matrix * view_matrix * model_matrix;

  GLuint matrix_uniform = glGetUniformLocation(shaderProgram, "mvp_matrix");

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

    auto rotation_time = time.count() * 0.001f * 0.001f;
    auto rotated_mvp =
        glm::rotate(mvp_matrix, rotation_time * glm::radians(180.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(matrix_uniform, 1, GL_FALSE, &rotated_mvp[0][0]);

    // Clear the screen to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw triangles from the 3 vertices
    // model.render();
    model2.render();

    SDL_GL_SwapWindow(window);
  }
  glDeleteTextures(1, &texture_id);
  glDeleteProgram(shaderProgram);
  glDeleteVertexArrays(1, &vao);

  IMG_Quit();
  SDL_GL_DeleteContext(context);
  SDL_Quit();
  return 0;
}