#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <input_parser/option/base_option.hpp>
#include <input_parser/parsing_error.hpp>

namespace input_parser {

/** @brief Mock class since BaseOption has purely virtual methods */
template <typename ValueType, typename TransformationType = ValueType>
class MockOption : public BaseOption<ValueType, TransformationType> {
 public:
  /**
   * @brief Default constructor to avoid compilation errors using Google
   * functions
   * @details For testing purposes
   */
  MockOption() : BaseOption<ValueType, TransformationType>("") {};

  MockOption(
    string_kind auto const name, string_kind auto const... extra_names
  ) : BaseOption<ValueType, TransformationType>(name, extra_names...) {}

  BaseOption<ValueType, TransformationType> &toInt() override {
    throw std::runtime_error("Not implemented");
  }

  BaseOption<ValueType, TransformationType> &toDouble() override {
    throw std::runtime_error("Not implemented");
  }

  BaseOption<ValueType, TransformationType> &toFloat() override {
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
  MyClass(int value = 0) : value_(value) {}

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
  EXPECT_NO_THROW(MockOption<int>(name));
  EXPECT_NO_THROW(MockOption<char>(name2));
}

TEST(BaseOption_constructor, ShouldReceiveOneName) {
  EXPECT_NO_THROW(MockOption<bool>("name"));
}

TEST(BaseOption_constructor, ShouldReceiveMultipleNames) {
  EXPECT_NO_THROW(MockOption<long>("name", "n", "nm"));
}

// // ------------------------------- Getters ------------------------------- //

TEST(BaseOption_getters, ShouldReturnArrayWithNames) {
  const auto option = MockOption<double>("-v", "--version", "-V");
  const auto expected = std::vector<std::string>({"-v", "--version", "-V"});

  EXPECT_THAT(option.getNames(), ::testing::ContainerEq(expected));
}

TEST(BaseOption_getters, ShouldStartWithoutDescription) {
  const auto option = MockOption<std::string>("name");

  EXPECT_EQ(option.getDescription(), "");
}

TEST(BaseOption_getters, ShouldReturnDescription) {
  const auto description = "My cool looking description";
  auto option = MockOption<long long>("name");
  option.addDescription(description);

  EXPECT_EQ(option.getDescription(), description);
}

TEST(BaseOption_getters, ShouldReturnValue) {
  const bool expected = true;
  auto option = MockOption<bool>("name");
  option.setValue(expected);

  EXPECT_EQ(option.getValue(), expected);
}

TEST(BaseOption_getters, ShouldThrowInvalidArgumentIfNoValue) {
  const auto option = MockOption<int>("name");
  EXPECT_THROW(auto _ = option.getValue(), std::invalid_argument);
}

TEST(BaseOption_getters, ShouldReturnDefaultValue) {
  const int expected = 20;
  auto option = MockOption<int>("name");
  option.addDefaultValue(expected);

  EXPECT_EQ(option.getDefaultValue(), expected);
}

TEST(BaseOption_getters, ShouldThrowInvalidArgumentIfNoDefaultValue) {
  const auto option = MockOption<MyClass>("name");
  EXPECT_THROW(auto _ = option.getDefaultValue(), std::invalid_argument);
}

TEST(BaseOption_getters, ShouldReturnDefaultValueIfNoValue) {
  const unsigned int expected = 20;
  auto option = MockOption<unsigned int>("name");
  option.addDefaultValue(expected);

  EXPECT_EQ(option.getValue(), expected);
  EXPECT_EQ(option.getDefaultValue(), option.getValue());
}

TEST(BaseOption_getters, ShouldReturnDefaultValueNotTransformedIfNoValue) {
  class MyOption : public MockOption<bool> {
   public:
    MyOption() : MockOption<bool>("name") {}

    BaseOption &negates() {
      return addTransformation([](const auto &value) { return !value; });
    }
  };

  auto option = MyOption();
  option.negates().addDefaultValue(true);
  EXPECT_TRUE(option.getValue());
  EXPECT_TRUE(option.getDefaultValue());
}

TEST(BaseOption_getters, StartsWithNoArgumentName) {
  const auto option = MockOption<int>("name");
  EXPECT_EQ(option.getArgumentName(), "");
}

// // ------------------------------- Setters ------------------------------- //

TEST(BaseOption_setters, ShouldAssignAPrimitiveToValue) {
  auto option = MockOption<long>("name");
  const long expected = 22L;
  option.setValue(expected);
  EXPECT_TRUE(option.hasValue());
  EXPECT_EQ(option.getValue(), expected);
}

TEST(BaseOption_setters, ShouldAssignAnStructToValue) {
  auto option = MockOption<MyStruct>("name");
  const MyStruct expected = {.value = 22};
  option.setValue(expected);
  EXPECT_TRUE(option.hasValue());
  EXPECT_EQ(option.getValue(), expected);
}

TEST(BaseOption_setters, ShouldAssignAClassObjectToValue) {
  auto option = MockOption<MyClass>("name");
  const auto expected = MyClass(-4);
  option.setValue(expected);
  EXPECT_TRUE(option.hasValue());
  EXPECT_EQ(option.getValue(), expected);
}

TEST(BaseOption_setters, ShouldNotTransformValueIfTypesMatch) {
  class MyOption : public MockOption<std::string> {
   public:
    MyOption() : MockOption<std::string>("name") {}

    BaseOption &uppercase() {
      return addTransformation([](const auto &value) {
        std::string upper = value;
        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
        return upper;
      });
    }
  };

  const auto input = "lowercase";
  auto option = MyOption();
  option.uppercase().setValue(input);
  EXPECT_EQ(option.getValue(), input);
}

TEST(BaseOption_setters, ShouldTransformValueIfTypesDontMatch) {
  class MyOption : public MockOption<int, std::string> {
   public:
    MyOption() : MockOption<int, std::string>("name") {}

    BaseOption &doubles() {
      return addTransformation([](const auto &value) {
        return std::stoi(value) * 2;
      });
    }
  };

  auto option = MyOption();
  const int expected = 8;
  option.doubles().setValue("4");
  EXPECT_EQ(option.getValue(), expected);
}

// -------------------------------- Checks -------------------------------- //

TEST(BaseOption_checks, ShouldNotBeAFlagOption) {
  const auto option = MockOption<bool>("notAFlagOption");
  EXPECT_FALSE(option.isFlag());
}

TEST(BaseOption_checks, ShouldNotBeASingleOption) {
  const auto option = MockOption<std::string>("notASingleOption");
  EXPECT_FALSE(option.isSingle());
}

TEST(BaseOption_checks, ShouldNotBeACompoundOption) {
  const auto option =
    MockOption<std::vector<std::string>>("notACompoundOption");
  EXPECT_FALSE(option.isCompound());
}

TEST(BaseOption_checks, ShouldBeRequiredByDefault) {
  const auto option = MockOption<char>("option");
  EXPECT_TRUE(option.isRequired());
}

TEST(BaseOption_checks, ShouldStartWithoutValue) {
  const auto option = MockOption<long double>("name");
  EXPECT_FALSE(option.hasValue());
}

TEST(BaseOption_checks, ShouldStartWithoutDefaultValue) {
  const auto option = MockOption<short>("name");
  EXPECT_FALSE(option.hasDefaultValue());
}

// -------------------------------- Adders -------------------------------- //

TEST(BaseOption_adders, ShouldAddDescription) {
  const auto description = "description";
  auto option = MockOption<bool>("name");
  option.addDescription(description);

  EXPECT_EQ(option.getDescription(), description);
}

TEST(BaseOption_adders, ShouldAddDefaultValue) {
  const int expected = 20;
  auto option = MockOption<int>("name");
  option.addDefaultValue(expected);

  EXPECT_FALSE(option.hasValue());
  EXPECT_TRUE(option.hasDefaultValue());
  EXPECT_EQ(option.getValue(), expected);
  EXPECT_EQ(option.getDefaultValue(), expected);
}

TEST(BaseOption_adders, ShouldAddStructToTheDefaultValue) {
  const auto expected = MyStruct(22);
  auto option = MockOption<MyStruct>("name");
  option.addDefaultValue(expected);

  EXPECT_FALSE(option.hasValue());
  EXPECT_TRUE(option.hasDefaultValue());
  EXPECT_EQ(option.getDefaultValue(), expected);
}

TEST(BaseOption_adders, ShouldAddClassObjectToTheDefaultValue) {
  const auto expected = MyClass(-4);
  auto option = MockOption<MyClass>("name");
  option.addDefaultValue(expected);

  EXPECT_FALSE(option.hasValue());
  EXPECT_TRUE(option.hasDefaultValue());
  EXPECT_EQ(option.getDefaultValue(), expected);
}

TEST(BaseOption_adders, ShouldAddConstraint) {
  const auto isZero = [](const auto &value) { return value == 0; };
  auto option = MockOption<int>("name");
  option.addConstraint<int>(isZero, "Value must be 0");

  EXPECT_THROW(option.setValue(1), ParsingError);
}

TEST(BaseOption_adders, ShouldThrowParsingErrorFailingConstraint) {
  const auto isEven = [](const auto &value) { return value % 2 == 0; };
  auto option = MockOption<int>("name");
  option.addConstraint<int>(isEven, "Value must be even");
  EXPECT_THROW(option.setValue(1), ParsingError);
}

TEST(BaseOption_adders, ShouldStoreErrorMessageAtParsingError) {
  const auto isOdd = [](const int &value) { return value % 2 == 1; };
  const auto error_message = "Value must be odd";
  auto option = MockOption<int>("name");
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
  const auto isZero = [](const MyStruct &value) { return value.value == 0; };
  auto option = MockOption<MyStruct>("name");
  option.addConstraint<MyStruct>(isZero, "Value must be 0");

  EXPECT_NO_THROW(option.setValue(MyStruct(0)));
  EXPECT_THROW(option.setValue(MyStruct(10)), ParsingError);
}

TEST(BaseOption_adders, ShouldAddConstraintWithClass) {
  const auto isZero = [](const MyClass &value) { return value == MyClass(0); };
  auto option = MockOption<MyClass>("name");
  option.addConstraint<MyClass>(isZero, "Value must be 0");

  EXPECT_NO_THROW(option.setValue(MyClass(0)));
  EXPECT_THROW(option.setValue(MyClass(10)), ParsingError);
}

TEST(BaseOption_adders, ConstraintShouldNotAffectDefaultValue) {
  const auto isGreaterThanAMillion = [](const int &value) {
    return value > 1'000'000;
  };
  auto option = MockOption<long>("name");
  option.addConstraint<long>(isGreaterThanAMillion, "Value must be 0");
  const int expected = 20;

  option.addDefaultValue(expected);
  EXPECT_TRUE(option.hasDefaultValue());
  EXPECT_EQ(option.getDefaultValue(), expected);
}

// ---------------------------- Transformations ---------------------------- //

TEST(BaseOption_transformation, ShouldNotImplementToInt) {
  auto option = MockOption<int>("name");
  EXPECT_THROW(option.toInt(), std::runtime_error);
}

TEST(BaseOption_transformation, ShouldNotImplementToDouble) {
  auto option = MockOption<int>("name");
  EXPECT_THROW(option.toDouble(), std::runtime_error);
}

TEST(BaseOption_transformation, ShouldNotImplementToFloat) {
  auto option = MockOption<int>("name");
  EXPECT_THROW(option.toFloat(), std::runtime_error);
}

TEST(BaseOption_transformation, ShouldTransformValue) {
  class MyOption : public MockOption<int, std::string> {
   public:
    MyOption() : MockOption<int, std::string>("name") {}

    BaseOption &timesTen() {
      return addTransformation([](const auto &value) {
        return std::stoi(value) * 10;
      });
    }
  };

  auto option = MyOption();
  const int expected = 40;
  option.timesTen().setValue("4");
  EXPECT_EQ(option.getValue(), expected);
}

TEST(BaseOption_transformation, ShouldApplyTransformationBeforeCheck) {
  class MyOption : public MockOption<int, std::string> {
   public:
    MyOption() : MockOption<int, std::string>("name") {}

    BaseOption &doubles() {
      return addTransformation([](const auto &value) {
        return std::stoi(value) * 2;
      });
    }
  };

  auto option = MyOption();
  option.doubles().addConstraint<int>(
    [](const int &value) { return value < 10; }, "Value must be lower than 10"
  );
  const int expected = 4;
  EXPECT_NO_THROW(option.setValue("4"));
  EXPECT_EQ(option.getValue(), expected * 2);
  EXPECT_THROW(option.setValue("16"), ParsingError);
}

}  // namespace input_parser