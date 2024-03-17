#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <input_parser/constraint.hpp>

namespace input_parser {

TEST(ConstraintTests, ShouldReceiveCallbackAndMessage) {
  const auto callback = [](const std::any &) { return true; };
  const auto message = "Error message";
  EXPECT_NO_THROW(Constraint(callback, message));
}

TEST(ConstraintTests, ShouldDisplayErrorMessage) {
  const auto callback = [](const std::any &) { return false; };
  const auto message = "Error message";
  const auto constraint = Constraint(callback, message);
  EXPECT_EQ(constraint.getErrorMessage(), message);
}

TEST(ConstraintTests, ShouldReturnABoolean) {
  const auto callback = [](const std::any &) { return true; };
  const auto message = "Error message";
  const auto constraint = Constraint(callback, message);
  EXPECT_TRUE(constraint.call(0));
}

TEST(ConstraintTests, ShouldReceiveAValueAndUseIt) {
  const auto isEven = [](const std::any &value) {
    return std::any_cast<int>(value) % 2 == 0;
  };
  const auto message = "The value must be even";
  const auto constraint = Constraint(isEven, message);
  EXPECT_TRUE(constraint.call(2));
  EXPECT_TRUE(constraint.call(543'678));
  EXPECT_TRUE(constraint.call(1'000'000));
  EXPECT_FALSE(constraint.call(3));
  EXPECT_FALSE(constraint.call(398'651));
  EXPECT_FALSE(constraint.call(999'999));
}

TEST(ConstraintTests, ShouldBeAbleToThrowExceptions) {
  const auto callback = [](const std::any &) -> bool {
    throw std::runtime_error("Error");
  };
  const auto message = "Error message";
  const auto constraint = Constraint(callback, message);
  EXPECT_THROW(constraint.call(0), std::runtime_error);
}

}  // namespace input_parser