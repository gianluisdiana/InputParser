#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <input_parser/constraint.hpp>

namespace input_parser {

TEST(Constraint_constructor, ShouldReceiveCallbackAndMessage) {
  const auto callback = [](const long &) { return true; };
  const auto message = "Error message";
  EXPECT_NO_THROW(Constraint<long>(callback, message));
}

TEST(Constraint_getErrorMessage, ShouldDisplayErrorMessage) {
  const auto callback = [](const char &) { return false; };
  const auto message = "Error message";
  const auto constraint = Constraint<char>(callback, message);
  EXPECT_EQ(constraint.getErrorMessage(), message);
}

TEST(Constraint_call, ShouldReturnABoolean) {
  const auto callback = [](const double &) { return true; };
  const auto message = "Error message";
  const auto constraint = Constraint<double>(callback, message);
  EXPECT_TRUE(constraint.call(0));
}

TEST(Constraint_call, ShouldReceiveAValueAndUseIt) {
  const auto isEven = [](const int &value) { return value % 2 == 0; };
  const auto message = "The value must be even";
  const auto constraint = Constraint<int>(isEven, message);
  EXPECT_TRUE(constraint.call(2));
  EXPECT_TRUE(constraint.call(543'678));
  EXPECT_TRUE(constraint.call(1'000'000));
  EXPECT_FALSE(constraint.call(3));
  EXPECT_FALSE(constraint.call(398'651));
  EXPECT_FALSE(constraint.call(999'999));
}

TEST(Constraint_call, ShouldBeAbleToThrowExceptions) {
  const auto callback = [](const double &) -> bool {
    throw std::runtime_error("Error");
  };
  const auto message = "Error message";
  const auto constraint = Constraint<double>(callback, message);
  EXPECT_THROW(auto _ = constraint.call(0), std::runtime_error);
}

}  // namespace input_parser