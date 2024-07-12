# Set the minimum version of CMake that can be used
# To find the cmake version run
# $ cmake --version
cmake_minimum_required(VERSION 3.22)

# Set the project name
project(input_parser
  VERSION 0.4.1
  DESCRIPTION "An argument parser for C++23"
  HOMEPAGE_URL "https://github.com/gianluisdiana/InputParser"
  LANGUAGES CXX
)

# ------------------------------- C++ Standard ------------------------------ #

# Set the C++ standard we will use to compile the library
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------- Clang-Tidy Integration ------------------------- #

find_program(CLANG_TIDY_COMMAND NAMES "clang-tidy")
if(CLANG_TIDY_COMMAND AND (NOT BUILD_INPUT_PARSER_TESTS))
  set(CMAKE_CXX_CLANG_TIDY
    ${CLANG_TIDY_COMMAND}-19;
    -config-file=${CMAKE_SOURCE_DIR}/.clang-tidy;
  )
endif()

# --------------------------- Extra CMake Modules --------------------------- #

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# --------------------------------- Sources --------------------------------- #

# Add the library as an interface
add_library(${PROJECT_NAME} INTERFACE)

# ------------------------------- Header files ------------------------------ #

# Set the directories that should be included in the build command for this
# target when running g++ these will be included as -I/directory/path/
target_include_directories(${PROJECT_NAME} INTERFACE
  ${PROJECT_SOURCE_DIR}/include
)

# ---------------------------------- Tests ---------------------------------- #

# Only add the tests directory if the BUILD_INPUT_PARSER_TESTS flag is turned on
# $ cmake -DBUILD_INPUT_PARSER_TESTS=ON ..
if(BUILD_INPUT_PARSER_TESTS)
  add_subdirectory(test)
endif()