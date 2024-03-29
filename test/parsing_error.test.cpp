#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <input_parser/parsing_error.hpp>

namespace input_parser {

TEST(ParsingError_class, ShouldInheritFromInvalidArgument) {
  EXPECT_THAT(ParsingError(""), ::testing::An<std::invalid_argument>());
}

}  // namespace input_parser