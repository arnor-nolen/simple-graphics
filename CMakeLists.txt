cmake_minimum_required (VERSION 3.16)
project(simple-graphics)

# Downloading cmake-conan script 
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_add_remote(NAME bincrafters URL https://api.bintray.com/conan/bincrafters/public-conan)

# Installing dependencies
conan_cmake_run(
  REQUIRES
  sdl2/2.0.12@bincrafters/stable
  sdl2_image/2.0.5@bincrafters/stable
  glew/2.1.0@bincrafters/stable
  glm/0.9.9.8
  boost/1.73.0
  BASIC_SETUP
  CMAKE_TARGETS
  NO_OUTPUT_DIRS
  BUILD
  missing)

# Set binary output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/$<0:>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/$<0:>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/$<0:>)


# Set platform-specific stuff
if (MSVC)
  # Strip out /RTC1 for production builds
  # string(REGEX REPLACE "[/|-]RTC[^ ]*" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})

  # Suppress compiler warnings:
  # 1. Boost.Spirit.X3 global variable shadowing.
  add_compile_options(/W4 /Od /wd4459)

  # Suppress linker warnings:
  # 1. external libraries are compiled without debug info.
  add_link_options(/IGNORE:4099)
elseif(APPLE)
  add_compile_options(-g -O0 -Wall -Wextra -Wpedantic -Wshadow -fvisibility=hidden)
else()
  message(FATAL_ERROR "Currently, only MSVC and APPLE architectures are officially supported. You need to link your libraries manually by editing CMakeLists.txt")
endif()

# Set targets
add_executable (${PROJECT_NAME} src/main.cpp)

# Precompiled headers for faster compilation times
target_precompile_headers(${PROJECT_NAME} PUBLIC src/pch.h)

# Use c++17 standard
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# Add src to the include path
target_include_directories(${PROJECT_NAME} PUBLIC src)

#Link libraries
target_link_libraries(${PROJECT_NAME} CONAN_PKG::glew CONAN_PKG::glm CONAN_PKG::sdl2 CONAN_PKG::sdl2_image CONAN_PKG::boost)