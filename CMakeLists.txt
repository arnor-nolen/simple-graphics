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
  CONANFILE conanfile.txt
  BASIC_SETUP
  CMAKE_TARGETS
  NO_OUTPUT_DIRS
  BUILD
  missing)

# Set binary output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/$<0:>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/$<0:>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/$<0:>)

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

#-------------------------------------------------------------------
# Generate include paths for VSCode (CPP_TOOLS and clang-tidy)
#-------------------------------------------------------------------
if(GENERATE_INCLUDE_PATHS)
  #-------------------------------------------------------------
  # Generate c_cpp_properties.json for VSCode code highlighting
  #-------------------------------------------------------------
  message(STATUS "Generating include paths for VSCode CPP extension and clang-tidy plugin...")
  set(VSCODE_CPP_TOOLS_FILE_LOCATION ".vscode/c_cpp_properties.json")

  if(WIN32)
    set(CPP_TOOLS_CONFIG_NAME "Win32")
  elseif(APPLE)
    set(CPP_TOOLS_CONFIG_NAME "Mac")
  else()
    set(CPP_TOOLS_CONFIG_NAME "Linux")
  endif()

  file(READ ${VSCODE_CPP_TOOLS_FILE_LOCATION} VSCODE_CPP_TOOLS_FILE)

  # Combine all includePaths into one string
  set(REGEX_REPLACE_STRING "\"\${workspaceFolder}/src/**\",")

  foreach(INCLUDE ${CONAN_INCLUDE_DIRS})
    string(APPEND REGEX_REPLACE_STRING "\"${INCLUDE}\",")
  endforeach()

  string(REGEX REPLACE ".$" "" REGEX_REPLACE_STRING ${REGEX_REPLACE_STRING})

  # Replace c_cpp_properties.json
  string(REGEX REPLACE "(${CPP_TOOLS_CONFIG_NAME}[^}]*includePath[^\\[]*\\[)([^\]]*)(\])" "\\1${REGEX_REPLACE_STRING}\\3" VSCODE_CPP_TOOLS_FILE ${VSCODE_CPP_TOOLS_FILE})

  file(WRITE ${VSCODE_CPP_TOOLS_FILE_LOCATION} ${VSCODE_CPP_TOOLS_FILE})

  #-------------------------------------------------------------
  # Modify settings.json to include paths for clang-tidy syntax highlight 
  #-------------------------------------------------------------
  set(SETTINGS_JSON_FILE_LOCATION ".vscode/settings.json")

  # Combine all includePaths into one string
  set(REGEX_REPLACE_STRING "\"-std=c++17\",")
  string(APPEND REGEX_REPLACE_STRING "\"-I${PROJECT_SOURCE_DIR}/src/\",")

  foreach(INCLUDE ${CONAN_INCLUDE_DIRS})
    string(APPEND REGEX_REPLACE_STRING "\"-I${INCLUDE}\",")
  endforeach()

  string(REGEX REPLACE ".$" "" REGEX_REPLACE_STRING ${REGEX_REPLACE_STRING})

  if (EXISTS ".vscode/settings.json")
    file(READ ${SETTINGS_JSON_FILE_LOCATION} SETTINGS_JSON_FILE)
    # Replace settings.json
    string(REGEX REPLACE "(clang-tidy.compilerArgs[^\\[]*\\[)([^\]]*)(\])" "\\1${REGEX_REPLACE_STRING}\\3" SETTINGS_JSON_FILE ${SETTINGS_JSON_FILE})
  else()
    # Generate settings.json
    set(SETTINGS_JSON_FILE "{\"clang-tidy.compilerArgs\": [${REGEX_REPLACE_STRING}]}")
  endif()
  
  file(WRITE ${SETTINGS_JSON_FILE_LOCATION} ${SETTINGS_JSON_FILE})
endif()

#Link libraries
target_link_libraries(${PROJECT_NAME} ${CONAN_TARGETS})