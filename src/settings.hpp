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

struct {
  int w;
  int h;
} window_resolution = {800, 600};

} // namespace settings