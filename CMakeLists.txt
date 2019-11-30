cmake_minimum_required (VERSION 3.10)
project(simple-graphics)

# Include and library paths
include_directories(${CMAKE_INCLUDE_PATH})
link_directories(${CMAKE_LIBRARY_PATH})

# Set binary output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# For debugging
add_compile_options(-g)

# Set platform-specific stuff
IF (WIN32)
    set(WIN_LINKER_FLAGS mingw32 SDL2main)
ENDIF()

add_executable (${PROJECT_NAME} src/main.cpp)
# Use c++17 standard
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_link_libraries(${PROJECT_NAME} ${WIN_LINKER_FLAGS} SDL2 opengl32 glew32)