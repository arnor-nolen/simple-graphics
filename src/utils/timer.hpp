#pragma once

#include <chrono>
#include <iostream>

struct Timer {
  Timer();
  explicit Timer(const std::string_view &str);
  ~Timer();

  Timer(const Timer &) = delete;
  Timer(Timer &&other) noexcept = delete;
  auto operator=(const Timer &) -> const Timer & = delete;
  auto operator=(Timer &&other) noexcept -> Timer & = delete;

  void stop() noexcept;

private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
  std::string str_;
};