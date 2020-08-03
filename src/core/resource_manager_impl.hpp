#pragma once

#include "core/resource_manager.hpp"

template <typename... Args> void ResourceManager::load_model(Args &&... args) {
  models_.emplace_back(args...);
}