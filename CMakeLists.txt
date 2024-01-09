# Set the minimum version of CMake that can be used
# To find the cmake version run
# $ cmake --version
cmake_minimum_required(VERSION 3.22)

# Set the project name
project(input_parser)

# ---------------------------------- Sources ----------------------------------- #

# Create a sources variable with a link to all cpp files to compile
add_library(${PROJECT_NAME} STATIC
  src/lib/parser.cpp
  src/lib/option/base_option.cpp
  src/lib/option/flag_option.cpp
  src/lib/option/compound_option.cpp
  src/lib/option/single_option.cpp
)

# -------------------------------- Header files -------------------------------- #

# Set the directories that should be included in the build command for this target
# when running g++ these will be included as -I/directory/path/
target_include_directories(${PROJECT_NAME}
  PRIVATE
    ${PROJECT_SOURCE_DIR}/src/include
    ${PROJECT_SOURCE_DIR}/src/include/option
)

# ------------------------------ Compile options ------------------------------- #

# Set the C++ standard we will use to compile the library
set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED ON
)

# Add flags to the compiler
target_compile_options(${PROJECT_NAME} PRIVATE
  -Wall
  -Wextra
  -Wshadow
  -O3
)