#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <input_parser/option/base_option.hpp>
#include <input_parser/parsing_error.hpp>

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

struct MyStruct {
  int value;

  bool operator==(const MyStruct &other) const {
    return value == other.value;
  }
};

class MyClass {
 public:
  MyClass(int value) : value_(value) {}

  bool operator==(const MyClass &other) const {
    return value_ == other.value_;
  }

 private:
  int value_;
};

// ----------------------------- Constructor ----------------------------- //

TEST(BaseOption_constructor, ShouldReceiveStringOrConstCharPointer) {
  std::string name = "string";
  const char *name2 = "const char pointer";
  EXPECT_NO_THROW(MockOption(name));
  EXPECT_NO_THROW(MockOption(name2));
}

TEST(BaseOption_constructor, ShouldReceiveOneName) {
  EXPECT_NO_THROW(MockOption("name"));
}

TEST(BaseOption_constructor, ShouldReceiveMultipleNames) {
  EXPECT_NO_THROW(MockOption("name", "n", "nm"));
}

// ------------------------------- Getters ------------------------------- //

TEST(BaseOption_getters, ShouldReturnArrayWithNames) {
  const auto option = MockOption("-v", "--version", "-V");
  const auto expected = std::vector<std::string>({"-v", "--version", "-V"});
  EXPECT_THAT(option.getNames(), ::testing::ContainerEq(expected));
}

TEST(BaseOption_getters, ShouldStartWithoutDescription) {
  const auto option = MockOption("name");
  EXPECT_EQ(option.getDescription(), "");
}

TEST(BaseOption_getters, ShouldReturnDescription) {
  auto option = MockOption("name");
  const auto description = "My cool looking description";
  option.addDescription(description);
  EXPECT_EQ(option.getDescription(), description);
}

TEST(BaseOption_getters, ShouldThrowInvalidArgumentIfNoDefaultValue) {
  const auto option = MockOption("name");
  EXPECT_THROW(option.getDefaultValue<int>(), std::invalid_argument);
  EXPECT_THROW(option.getDefaultValue<double>(), std::invalid_argument);
  EXPECT_THROW(option.getDefaultValue<MyClass>(), std::invalid_argument);
}

TEST(BaseOption_getters, ShouldReturnDefaultValueIfNoValue) {
  auto option = MockOption("name");
  const int expected = 20;
  option.addDefaultValue(expected);
  EXPECT_EQ(option.getValue<decltype(expected)>(), expected);
}

TEST(BaseOption_getters, ShouldReturnDefaultValueTransformedIfNoValue) {
  class MyOption : public MockOption {
   public:
    MyOption() : MockOption("name") {}

    BaseOption &negates() {
      transformation_ = [](const std::any &value) {
        return !std::any_cast<bool>(value);
      };
      return *this;
    }
  };

  auto option = MyOption();
  option.negates().addDefaultValue(true);
  EXPECT_FALSE(option.getValue<bool>());
  EXPECT_FALSE(option.getDefaultValue<bool>());
}

TEST(BaseOption_getters, StartsWithNoArgumentName) {
  const auto option = MockOption("name");
  EXPECT_EQ(option.getArgumentName(), "");
}

// ------------------------------- Setters ------------------------------- //

TEST(BaseOption_setters, ShouldAssignAPrimitiveToTheValue) {
  auto option = MockOption("name");
  const int expected = 22;
  option.setValue(expected);
  EXPECT_TRUE(option.hasValue());
  EXPECT_EQ(option.getValue<decltype(expected)>(), expected);
}

TEST(BaseOption_setters, ShouldAssignAnStructToTheValue) {
  auto option = MockOption("name");
  const auto expected = MyStruct(22);
  option.setValue(expected);
  EXPECT_TRUE(option.hasValue());
  EXPECT_EQ(option.getValue<decltype(expected)>(), expected);
}

TEST(BaseOption_setters, ShouldAssignAClassObjectToTheValue) {
  auto option = MockOption("name");
  const auto expected = MyClass(-4);
  option.setValue(expected);
  EXPECT_TRUE(option.hasValue());
  EXPECT_EQ(option.getValue<decltype(expected)>(), expected);
}

// -------------------------------- Checks -------------------------------- //

TEST(BaseOption_checks, ShouldNotBeAFlagOption) {
  const auto option = MockOption("notAFlagOption");
  EXPECT_FALSE(option.isFlag());
}

TEST(BaseOption_checks, ShouldNotBeASingleOption) {
  const auto option = MockOption("notASingleOption");
  EXPECT_FALSE(option.isSingle());
}

TEST(BaseOption_checks, ShouldNotBeACompoundOption) {
  const auto option = MockOption("notACompoundOption");
  EXPECT_FALSE(option.isCompound());
}

TEST(BaseOption_checks, ShouldBeRequiredByDefault) {
  const auto option = MockOption("option");
  EXPECT_TRUE(option.isRequired());
}

TEST(BaseOption_checks, ShouldStartWithoutValue) {
  const auto option = MockOption("name");
  EXPECT_FALSE(option.hasValue());
}

TEST(BaseOption_checks, ShouldStartWithoutDefaultValue) {
  const auto option = MockOption("name");
  EXPECT_FALSE(option.hasDefaultValue());
}

// -------------------------------- Adders -------------------------------- //

TEST(BaseOption_adders, ShouldAddDescription) {
  auto option = MockOption("name");
  const auto description = "description";
  option.addDescription(description);
  EXPECT_EQ(option.getDescription(), description);
}

TEST(BaseOption_adders, ShouldAddDefaultValue) {
  auto option = MockOption("name");
  const int expected = 20;
  option.addDefaultValue(expected);
  EXPECT_TRUE(option.hasDefaultValue());
  EXPECT_EQ(option.getDefaultValue<decltype(expected)>(), expected);
}

TEST(BaseOption_adders, ShouldAddStructToTheDefaultValue) {
  auto option = MockOption("name");
  const auto expected = MyStruct(22);
  option.addDefaultValue(expected);
  EXPECT_TRUE(option.hasDefaultValue());
  EXPECT_FALSE(option.hasValue());
  EXPECT_EQ(option.getDefaultValue<decltype(expected)>(), expected);
}

TEST(BaseOption_adders, ShouldAddClassObjectToTheDefaultValue) {
  auto option = MockOption("name");
  const auto expected = MyClass(-4);
  option.addDefaultValue(expected);
  EXPECT_FALSE(option.hasValue());
  EXPECT_TRUE(option.hasDefaultValue());
  EXPECT_EQ(option.getDefaultValue<decltype(expected)>(), expected);
}

TEST(BaseOption_adders, ShouldAddConstraint) {
  auto option = MockOption("name");
  const auto isZero = [](const auto &value) { return value == 0; };
  option.addConstraint<int>(isZero, "Value must be 0");
  EXPECT_THROW(option.setValue(1), ParsingError);
}

TEST(BaseOption_adders, ShouldThrowParsingErrorFailingConstraint) {
  auto option = MockOption("name");
  const auto isEven = [](const auto &value) { return value % 2 == 0; };
  option.addConstraint<int>(isEven, "Value must be even");
  EXPECT_THROW(option.setValue(1), ParsingError);
}

TEST(BaseOption_adders, ShouldStoreErrorMessageAtParsingError) {
  auto option = MockOption("name");
  const auto isOdd = [](const int &value) { return value % 2 == 1; };
  const auto error_message = "Value must be odd";
  option.addConstraint<int>(isOdd, error_message);
  EXPECT_THROW(
    {
      try {
        option.setValue(2);
      } catch (const ParsingError &error) {
        EXPECT_STREQ(error.what(), error_message);
        throw;
      }
    },
    ParsingError
  );
}

TEST(BaseOption_adders, ShouldAddConstraintWithStruct) {
  auto option = MockOption("name");
  const auto isZero = [](const MyStruct &value) { return value.value == 0; };
  option.addConstraint<MyStruct>(isZero, "Value must be 0");
  EXPECT_NO_THROW(option.setValue(MyStruct(0)));
  EXPECT_THROW(option.setValue(MyStruct(10)), ParsingError);
}

TEST(BaseOption_adders, ShouldAddConstraintWithClass) {
  auto option = MockOption("name");
  const auto isZero = [](const MyClass &value) { return value == MyClass(0); };
  option.addConstraint<MyClass>(isZero, "Value must be 0");
  EXPECT_NO_THROW(option.setValue(MyClass(0)));
  EXPECT_THROW(option.setValue(MyClass(10)), ParsingError);
}

TEST(BaseOption_adders, ConstraintShouldNotAffectDefaultValue) {
  auto option = MockOption("name");
  const auto isGreaterThanAMillion = [](const int &value) {
    return value > 1'000'000;
  };
  option.addConstraint<int>(isGreaterThanAMillion, "Value must be 0");
  const int expected = 20;
  EXPECT_NO_THROW(option.addDefaultValue(expected));
  EXPECT_TRUE(option.hasDefaultValue());
  EXPECT_EQ(option.getDefaultValue<decltype(expected)>(), expected);
}

// ---------------------------- Transformations ---------------------------- //

TEST(BaseOption_transformation, ShouldNotImplementToInt) {
  auto option = MockOption("name");
  EXPECT_THROW(option.toInt(), std::runtime_error);
}

TEST(BaseOption_transformation, ShouldNotImplementToDouble) {
  auto option = MockOption("name");
  EXPECT_THROW(option.toDouble(), std::runtime_error);
}

TEST(BaseOption_transformation, ShouldNotImplementToFloat) {
  auto option = MockOption("name");
  EXPECT_THROW(option.toFloat(), std::runtime_error);
}

TEST(BaseOption_transformation, ShouldApplyTransformationBeforeCheck) {
  class MyOption : public MockOption {
   public:
    MyOption() : MockOption("name") {}

    BaseOption &doubles() {
      transformation_ = [](const std::any &value) {
        return std::any_cast<int>(value) * 2;
      };
      return *this;
    }
  };

  auto option = MyOption();
  option.doubles()
    .addConstraint<int>(
      [](const int &value) { return value < 10; }, "Value must be lower than 10"
    )
    .transformBeforeCheck();
  const int expected = 4;
  EXPECT_NO_THROW(option.setValue(expected));
  EXPECT_EQ(option.getValue<int>(), expected * 2);
  EXPECT_THROW(option.setValue(expected * 2), ParsingError);
}

}  // namespace input_parser