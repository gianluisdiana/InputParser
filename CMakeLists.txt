# Set the minimum version of CMake that can be used
# To find the cmake version run
# $ cmake --version
cmake_minimum_required(VERSION 3.22)

# Set the project name
project(input_parser
  VERSION 0.4.0
  DESCRIPTION "An argument parser for C++23"
  HOMEPAGE_URL "https://github.com/gianluisdiana/InputParser"
  LANGUAGES CXX
)

# ----------------------------- Extra CMake Modules ---------------------------- #

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# ---------------------------------- Sources ----------------------------------- #

# Create a sources variable with a link to all cpp files to compile
set(SOURCE
  src/parser.cpp
  src/option/base_option.cpp
  src/option/flag_option.cpp
  src/option/compound_option.cpp
  src/option/single_option.cpp
)

# Add all the sources to the library
add_library(${PROJECT_NAME} STATIC
  ${SOURCE}
)

# -------------------------------- Header files -------------------------------- #

# Set the directories that should be included in the build command for this target
# when running g++ these will be included as -I/directory/path/
target_include_directories(${PROJECT_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}/include
)

# ----------------------------- Compile options ----------------------------- #

# Set the C++ standard we will use to compile the library
set(CMAKE_CXX_STANDARD 23)

# Add flags to the compiler
target_compile_options(${PROJECT_NAME} PRIVATE
  -Wall
  -Wextra
  -Wshadow
  -O3
)

# ---------------------------------- Tests ---------------------------------- #

# Only add the tests directory if the BUILD_INPUT_PARSER_TESTS option is turned on
# cmake -DBUILD_INPUT_PARSER_TESTS=ON ..
if(BUILD_INPUT_PARSER_TESTS)
  add_subdirectory(test)
endif()