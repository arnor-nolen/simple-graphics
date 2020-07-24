#pragma once

#include <chrono>
#include <iostream>

struct Timer {
  Timer() : Timer("") {}
  Timer(const std::string_view &str)
      : start_time_(std::chrono::high_resolution_clock::now()), str_(str) {}
  ~Timer() { stop(); }

  void stop() {
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        end_time - start_time_);
    std::cout << str_ << duration.count() * 0.001f << " ms.\n";
  }

private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
  std::string str_;
};