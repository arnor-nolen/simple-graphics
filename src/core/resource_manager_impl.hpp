#pragma once

#include "core/resource_manager.hpp"

template <typename... Args>
auto ResourceManager::load_model(Args &&... args) -> Model & {
  return models_.emplace_back(args...);
}