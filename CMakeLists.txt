cmake_minimum_required (VERSION 3.10)
project(simple-graphics)

# Include and library paths
include_directories(${CMAKE_INCLUDE_PATH})
link_directories(${CMAKE_LIBRARY_PATH})

# Find dependencies 
find_package(SDL2 REQUIRED)
find_package(SDL_image REQUIRED)
find_package(glew REQUIRED)

# Set binary output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# For debugging
add_compile_options(-g)

# Set platform-specific stuff
if (WIN32)
    # Add "-mwindows" flag for hiding the console
    set(EXTRA_LINKER_FLAGS mingw32 SDL2main opengl32 glew32)
elseif(APPLE)
    set(EXTRA_LINKER_FLAGS "-framework opengl" glew)
else()
    message(FATAL_ERROR "Currently, only WIN32 and APPLE architectures are officially supported. You need to link your libraries manually by editing CMakeLists.txt")
endif()

add_executable (${PROJECT_NAME} src/main.cpp)
# Use c++17 standard
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_link_libraries(${PROJECT_NAME} ${EXTRA_LINKER_FLAGS} SDL2 SDL2_image)