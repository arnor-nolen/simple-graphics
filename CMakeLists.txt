cmake_minimum_required (VERSION 3.16)
project(simple-graphics)

include_directories("C:/MinGW/SDLx64/include/SDL2")
link_directories("C:/MinGW/SDLx64/lib/")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-lmingw32 -lSDL2main")

# Set binary output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)


add_executable (${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} SDL2)