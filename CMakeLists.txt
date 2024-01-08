# Set the minimum version of CMake that can be used
# To find the cmake version run
# $ cmake --version
cmake_minimum_required(VERSION 3.22)

# Set the project name
project(input_parser)

# Create a sources variable with a link to all cpp files to compile
set(SOURCES
  src/lib/parser.cpp
  src/lib/option/base_option.cpp
  src/lib/option/flag_option.cpp
  src/lib/option/compound_option.cpp
  src/lib/option/single_option.cpp
)

# Set the C++ standard we will use
set(CMAKE_CXX_STANDARD 23)

# Set the flags for the compiler
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wshadow -O3")

# Add an executable with the above sources
add_executable(${PROJECT_NAME} ${SOURCES})

# Set the directories that should be included in the build command for this target
# when running g++ these will be included as -I/directory/path/
target_include_directories(${PROJECT_NAME}
  PRIVATE
    ${PROJECT_SOURCE_DIR}/src/include
    ${PROJECT_SOURCE_DIR}/src/include/option
)