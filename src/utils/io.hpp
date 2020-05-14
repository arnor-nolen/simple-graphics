#pragma once

#include "utils/timer.hpp"
#include <fstream>

auto load_file(const std::string &path) {
  Timer timer;
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  const std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<char> buffer(size + 1);
  if (!file.read(buffer.data(), size)) {
    throw std::runtime_error("Can't read shader file! File location: " + path +
                             '\n');
  }
  buffer.back() = '\0';
  return buffer;
}