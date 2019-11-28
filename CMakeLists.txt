cmake_minimum_required (VERSION 3.11)
project(simple-graphics)

# Set build output directories
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_HOME_DIRECTORY}/build")
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_HOME_DIRECTORY}/build")
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)

add_executable (simple-graphics src/main.cpp)