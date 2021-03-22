#pragma once

namespace settings {
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

constexpr int DEFAULT_WINDOW_WIDTH = 1024;
constexpr int DEFAULT_WINDOW_HEIGHT = 768;

struct Resolution {
  int w;
  int h;
};
static Resolution window_resolution = {DEFAULT_WINDOW_WIDTH,
                                       DEFAULT_WINDOW_HEIGHT};

static unsigned int fullscreen = 0;
static Resolution fullscreen_resolution;

constexpr size_t file_str_size = 50;

} // namespace settings