#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <input_parser/parser.hpp>

// ------------------------------- AddOption ------------------------------- //

TEST(Parser_addOption, ThrowsErrorWithOptionsWithSameName) {
  auto parser = input_parser::Parser();
  EXPECT_NO_THROW(parser.addOption<input_parser::CompoundOption>([] {
    return input_parser::CompoundOption("-n", "--names");
  }));
  EXPECT_THROW(
    {
      try {
        parser.addOption<input_parser::SingleOption>([] {
          return input_parser::SingleOption("-n", "--name");
        });
      } catch (const std::invalid_argument &e) {
        EXPECT_STREQ(e.what(), "Option already exists!");
        throw;
      }
    },
    std::invalid_argument
  );
}

TEST(Parser_addOption, AddsFlagOption) {
  auto parser = input_parser::Parser().addOption<input_parser::FlagOption>([] {
    return input_parser::FlagOption("-f", "--flag");
  });
  const char *argv[] = {"test", "--flag"};
  parser.parse(2, (char **)argv);
  EXPECT_TRUE(parser.getValue<bool>("-f"));
  EXPECT_TRUE(parser.getValue<bool>("--flag"));
}

TEST(Parser_addOption, AddsSingleOption) {
  auto parser =
    input_parser::Parser().addOption<input_parser::SingleOption>([] {
      return input_parser::SingleOption("-s", "--single");
    });
  auto expected = "value";
  const char *argv[] = {"test", "--single", expected};
  parser.parse(3, (char **)argv);
  EXPECT_EQ(parser.getValue<std::string>("-s"), expected);
  EXPECT_EQ(parser.getValue<std::string>("--single"), expected);
}

TEST(Parser_addOption, AddsCompoundOption) {
  auto parser =
    input_parser::Parser().addOption<input_parser::CompoundOption>([] {
      return input_parser::CompoundOption("-c", "--compound");
    });
  auto expected = std::vector<std::string> {"value1", "value2"};
  const char *argv[] = {"test", "--compound", "value1", "value2"};
  parser.parse(4, (char **)argv);
  EXPECT_EQ(parser.getValue<decltype(expected)>("-c"), expected);
  EXPECT_EQ(parser.getValue<decltype(expected)>("--compound"), expected);
}

TEST(Parser_addOption, AddsMixedOptions) {
  auto parser = input_parser::Parser();
  parser
    .addOption<input_parser::FlagOption>([] {
      return input_parser::FlagOption("-f", "--flag");
    })
    .addOption<input_parser::SingleOption>([] {
      return input_parser::SingleOption("-s", "--single");
    })
    .addOption<input_parser::CompoundOption>([] {
      return input_parser::CompoundOption("-c", "--compound");
    });
  const auto single_expected = "value";
  const auto compound_expected = std::vector<std::string> {"value1", "value2"};
  const char *argv[] = {
    "test",
    "--flag",
    "--single",
    single_expected,
    "--compound",
    compound_expected[0].c_str(),
    compound_expected[1].c_str()};
  parser.parse(7, (char **)argv);
  EXPECT_TRUE(parser.getValue<bool>("-f"));
  EXPECT_EQ(parser.getValue<std::string>("-s"), single_expected);
  EXPECT_EQ(parser.getValue<std::vector<std::string>>("-c"), compound_expected);
}

// --------------------------------- Parse --------------------------------- //

TEST(Parser_parse, DoesNotThrowErrorParsingWithoutOptions) {
  auto parser = input_parser::Parser();
  const char *argv[] = {"test"};
  EXPECT_NO_THROW(parser.parse(1, (char **)argv));
}

TEST(Parser_parse, ThrowsErrorParsingParametersWithoutOptions) {
  auto parser = input_parser::Parser();
  auto argv = new const char *[2] { "test", "param" };
  EXPECT_THROW(
    {
      try {
        parser.parse(2, (char **)argv);
      } catch (const input_parser::ParsingError &e) {
        EXPECT_STREQ(e.what(), "Invalid arguments provided!");
        throw;
      }
    },
    input_parser::ParsingError
  );
}

TEST(Parser_parse, ParsesFlagOption) {
  auto parser = input_parser::Parser().addOption<input_parser::FlagOption>([] {
    return input_parser::FlagOption("-v", "--verbose");
  });
  const char *argv[] = {"test", "--verbose"};
  EXPECT_NO_THROW(parser.parse(2, (char **)argv));
}

TEST(Parser_parse, ParsesFlagOptionAsTrue) {
  auto parser = input_parser::Parser().addOption<input_parser::FlagOption>([] {
    return input_parser::FlagOption("-v", "--verbose");
  });
  const char *argv[] = {"test", "--verbose"};
  parser.parse(2, (char **)argv);
  EXPECT_TRUE(parser.getValue<bool>("-v"));
  EXPECT_TRUE(parser.getValue<bool>("--verbose"));
}

TEST(Parser_parse, GetsDefaultOptionNegated) {
  auto parser = input_parser::Parser().addOption<input_parser::FlagOption>([] {
    return input_parser::FlagOption("-v", "--verbose").addDefaultValue(true);
  });
  const char *argv[] = {"test", "-v"};
  EXPECT_NO_THROW(parser.parse(2, (char **)argv));
  EXPECT_FALSE(parser.getValue<bool>("-v"));
  EXPECT_FALSE(parser.getValue<bool>("--verbose"));
}

TEST(Parser_parse, ParsesSingleOption) {
  auto parser =
    input_parser::Parser().addOption<input_parser::SingleOption>([] {
      return input_parser::SingleOption("-s", "--single");
    });
  const auto expected = "value";
  const char *argv[] = {"test", "--single", expected};
  EXPECT_NO_THROW(parser.parse(3, (char **)argv));
}

TEST(Parser_parse, ParsesSingleOptionAsString) {
  auto parser =
    input_parser::Parser().addOption<input_parser::SingleOption>([] {
      return input_parser::SingleOption("-s", "--single");
    });
  const auto expected = "value";
  const char *argv[] = {"test", "--single", expected};
  parser.parse(3, (char **)argv);
  EXPECT_EQ(parser.getValue<std::string>("-s"), expected);
  EXPECT_EQ(parser.getValue<std::string>("--single"), expected);
}

TEST(Parser_parse, ThrowsErrorExpectingSingleOptionArgument) {
  auto parser =
    input_parser::Parser().addOption<input_parser::SingleOption>([] {
      return input_parser::SingleOption("-s", "--single");
    });
  const char *argv[] = {"test", "--single"};
  EXPECT_THROW(
    {
      try {
        parser.parse(2, (char **)argv);
      } catch (const input_parser::ParsingError &e) {
        EXPECT_STREQ(
          e.what(), "After the --single option should be an extra argument!"
        );
        throw;
      }
    },
    input_parser::ParsingError
  );
}

TEST(Parser_parse, ParsesCompoundOption) {
  auto parser =
    input_parser::Parser().addOption<input_parser::CompoundOption>([] {
      return input_parser::CompoundOption("-c", "--compound");
    });
  const auto expected = std::vector<std::string> {"value1", "value2"};
  const char *argv[] = {"test", "--compound", "value1", "value2"};
  EXPECT_NO_THROW(parser.parse(4, (char **)argv));
}

TEST(Parser_parse, ParsesCompoundOptionAsStringVector) {
  auto parser =
    input_parser::Parser().addOption<input_parser::CompoundOption>([] {
      return input_parser::CompoundOption("-c", "--compound");
    });
  const auto expected = std::vector<std::string> {"value1", "value2"};
  const char *argv[] = {"test", "--compound", "value1", "value2"};
  parser.parse(4, (char **)argv);
  EXPECT_EQ(parser.getValue<decltype(expected)>("-c"), expected);
  EXPECT_EQ(parser.getValue<decltype(expected)>("--compound"), expected);
}

TEST(Parser_parse, ThrowsErrorExpectingCompoundOptionArgument) {
  auto parser =
    input_parser::Parser().addOption<input_parser::CompoundOption>([] {
      return input_parser::CompoundOption("-c", "--compound");
    });
  const char *argv[] = {"test", "-c"};
  EXPECT_THROW(
    {
      try {
        parser.parse(2, (char **)argv);
      } catch (const input_parser::ParsingError &e) {
        EXPECT_STREQ(
          e.what(), "After the -c option should be at least an extra argument!"
        );
        throw;
      }
    },
    input_parser::ParsingError
  );
}

TEST(Parser_parse, ThrowsErrorExpectingNotProvidedOption) {
  auto parser = input_parser::Parser();
  parser
    .addOption<input_parser::FlagOption>([] {
      return input_parser::FlagOption("-v", "--verbose");
    })
    .addOption<input_parser::SingleOption>([] {
      return input_parser::SingleOption("-s", "--single");
    });
  const char *argv[] = {"test", "--single", "value"};
  EXPECT_THROW(
    {
      try {
        parser.parse(3, (char **)argv);
      } catch (const input_parser::ParsingError &e) {
        EXPECT_STREQ(e.what(), "Missing option -v");
        throw;
      }
    },
    input_parser::ParsingError
  );
}
