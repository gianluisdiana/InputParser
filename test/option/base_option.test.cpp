#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <input_parser/option/base_option.hpp>

namespace input_parser {

/** @brief Mock class since BaseOption has purely virtual methods */
class MockOption : public BaseOption {
 public:
  /**
   * @brief Default constructor to avoid compilation errors using Google
   * functions
   * @details For testing purposes
   */
  MockOption() : BaseOption("") {};

  template <
    typename T, typename... Ts,
    typename = typename std::enable_if_t<
      is_string_type<T> && (is_string_type<Ts> && ...)>>
  MockOption(const T name, const Ts... extra_names) :
    BaseOption(name, extra_names...) {}

  inline BaseOption &toInt() override {
    throw std::runtime_error("Not implemented");
  }

  inline BaseOption &toDouble() override {
    throw std::runtime_error("Not implemented");
  }

  inline BaseOption &toFloat() override {
    throw std::runtime_error("Not implemented");
  }
};

// ------------------------------- Constructors ------------------------------- //

TEST(BaseOptionConstructorTests, ShouldReceiveStringOrConstCharPointer) {
  std::string name = "string";
  const char *name2 = "const char pointer";
  EXPECT_NO_THROW(MockOption(name));
  EXPECT_NO_THROW(MockOption(name2));
}

TEST(BaseOptionConstructorTests, ShouldReceiveOneName) {
  EXPECT_NO_THROW(MockOption("name"));
}

TEST(BaseOptionConstructorTests, ShouldReceiveMultipleNames) {
  EXPECT_NO_THROW(MockOption("name", "n", "nm"));
}

// ------------------------------- Getters ------------------------------- //

TEST(BaseOptionGettersTests, ShouldReturnArrayWithNames) {
  const auto option = MockOption("-v", "--version", "-V");
  const auto expected = std::vector<std::string>({"-v", "--version", "-V"});
  EXPECT_THAT(option.getNames(), ::testing::ContainerEq(expected));
}

TEST(BaseOptionGettersTests, ShouldStartWithoutDescription) {
  const auto option = MockOption("name");
  EXPECT_EQ(option.getDescription(), "");
}

TEST(BaseOptionGettersTests, ShouldAssignAValue) {
  auto option = MockOption("name");
  const int expected = 22;
  option.setValue(expected);
  EXPECT_TRUE(option.hasValue());
  EXPECT_EQ(option.getValue<decltype(expected)>(), expected);
}

TEST(BaseOptionGettersTests, ShouldAssignADefaultValue) {
  auto option = MockOption("name");
  const int expected = 20;
  option.addDefaultValue(expected);
  EXPECT_TRUE(option.hasDefaultValue());
  EXPECT_EQ(option.getDefaultValue<decltype(expected)>(), expected);
}

// -------------------------------- Checks -------------------------------- //

TEST(BaseOptionChecksTests, ShouldNotBeAFlagOption) {
  const auto option = MockOption("notAFlagOption");
  EXPECT_FALSE(option.isFlag());
}

TEST(BaseOptionChecksTests, ShouldNotBeASingleOption) {
  const auto option = MockOption("notASingleOption");
  EXPECT_FALSE(option.isSingle());
}

TEST(BaseOptionChecksTests, ShouldNotBeACompoundOption) {
  const auto option = MockOption("notACompoundOption");
  EXPECT_FALSE(option.isCompound());
}

TEST(BaseOptionChecksTests, ShouldBeRequiredByDefault) {
  const auto option = MockOption("option");
  EXPECT_TRUE(option.isRequired());
}

TEST(BaseOptionChecksTests, ShouldStartWithoutValue) {
  const auto option = MockOption("name");
  EXPECT_FALSE(option.hasValue());
}

TEST(BaseOptionChecksTests, ShouldStartWithoutDefaultValue) {
  const auto option = MockOption("name");
  EXPECT_FALSE(option.hasDefaultValue());
}

// -------------------------------- Adders -------------------------------- //

TEST(BaseOptionAddersTest, ShouldAddDescription) {
  auto option = MockOption("name");
  const auto description = "description";
  option.addDescription(description);
  EXPECT_EQ(option.getDescription(), description);
}

TEST(BaseOptionAddersTest, ShouldAddDefaultValue) {
  auto option = MockOption("");
  const auto expected = "my_value";
  option.addDefaultValue(expected);
  EXPECT_TRUE(option.hasDefaultValue());
  EXPECT_EQ(option.getDefaultValue<decltype(expected)>(), expected);
}
// TEST(BaseOptionAddersTest, ) {}

}  // namespace input_parser