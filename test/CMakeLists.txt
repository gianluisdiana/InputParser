cmake_minimum_required(VERSION 3.22)
project(input_parser_tests)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------------- Set GTest -------------------------------- #

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# ------------------------------ Source files ------------------------------- #

set(SOURCE
  "option/base_option.test.cpp"
  constraint.test.cpp
  parser.test.cpp
  parsing_error.test.cpp
)

# ------------------------------- Executable -------------------------------- #

add_executable(${PROJECT_NAME}
  ${SOURCE}
)

# ------------------------------ Dependencies ------------------------------- #

target_link_libraries(${PROJECT_NAME}
  GTest::gtest_main
  GTest::gmock_main
  input_parser
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})