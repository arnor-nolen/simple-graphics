cmake_minimum_required (VERSION 3.16)
project(simple-graphics)

# Setting up Conan dependencies
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

# Set compiler-specific stuff
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

  # Suppress compiler warnings:
  # 1. Boost.Spirit.X3 global variable shadowing.
  add_compile_options(/W4 /wd4459)

  # Suppress linker warnings:
  # 1. external libraries are compiled without debug info.
  add_link_options(/IGNORE:4099)

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")

  # -fvisibility=hidden for external libraries
  add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -fvisibility=hidden)

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

  # -fvisibility=hidden for external libraries
  add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -fvisibility=hidden)

else()

  message(WARNING "Unknown compiler, proceeding without additional compiler options")

endif()

# Set targets
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
add_executable (${PROJECT_NAME} ${SOURCE_FILES})

# Use c++17 standard
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# Add src to the include path
target_include_directories(${PROJECT_NAME} PUBLIC src)

#Link libraries
target_link_libraries(${PROJECT_NAME} ${CONAN_TARGETS})