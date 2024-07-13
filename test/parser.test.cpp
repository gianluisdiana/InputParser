#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <input_parser/parser.hpp>

namespace input_parser {

// ------------------------------- AddOption ------------------------------- //

TEST(Parser_addOption, ThrowsErrorWithOptionsWithSameName) {
  auto names_option = CompoundOption<std::vector<std::string>>("-n", "--names");
  auto parser = Parser<std::string, std::vector<std::string>>();
  EXPECT_NO_THROW(parser.addOption(names_option));
  auto name_option = SingleOption<std::string>("-n", "--name");
  EXPECT_THROW(
    {
      try {
        parser.addOption(name_option);
      } catch (const std::invalid_argument &e) {
        EXPECT_STREQ(e.what(), "Option already exists!");
        throw;
      }
    },
    std::invalid_argument
  );
}

TEST(Parser_addOption, AddsFlagOption) {
  auto flag_option = FlagOption<bool>("-f", "--flag");
  auto parser = Parser<bool>().addOption(flag_option);
  const char *argv[] = {"test", "--flag"};
  parser.parse(2, (char **)argv);
  EXPECT_TRUE(flag_option.getValue());
}

TEST(Parser_addOption, AddsSingleOption) {
  auto single = SingleOption<std::string>("-s", "--single");
  auto parser = Parser<std::string>().addOption(single);
  auto expected = "value";
  const char *argv[] = {"test", "--single", expected};
  parser.parse(3, (char **)argv);
  EXPECT_EQ(single.getValue(), expected);
  EXPECT_EQ(single.getValue(), expected);
}

TEST(Parser_addOption, AddsCompoundOption) {
  auto compound = CompoundOption<std::vector<std::string>>("-c", "--compound");
  auto parser = Parser<std::vector<std::string>>().addOption(compound);
  auto expected = std::vector<std::string> {"value1", "value2"};
  const char *argv[] = {"test", "--compound", "value1", "value2"};
  parser.parse(4, (char **)argv);
  EXPECT_EQ(compound.getValue(), expected);
}

TEST(Parser_addOption, AddsMixedOptions) {
  auto flag_option = FlagOption<bool>("-f", "--flag");
  auto single_option = SingleOption<std::string>("-s", "--single");
  auto compound_option =
    CompoundOption<std::vector<std::string>>("-c", "--compound");
  auto parser = Parser<bool, std::string, std::vector<std::string>>();
  parser.addOption(flag_option)
    .addOption(single_option)
    .addOption(compound_option);
  const auto single_expected = "value";
  const auto compound_expected = std::vector<std::string> {"value1", "value2"};
  const char *argv[] = {
    "test",
    "--flag",
    "--single",
    single_expected,
    "--compound",
    compound_expected[0].c_str(),
    compound_expected[1].c_str()
  };
  parser.parse(7, (char **)argv);
  EXPECT_TRUE(flag_option.getValue());
  EXPECT_EQ(single_option.getValue(), single_expected);
  EXPECT_EQ(compound_option.getValue(), compound_expected);
}

// // ----------------------------- AddHelpOption -----------------------------
// //

// TEST(Parser_addHelpOption, AddsOptionalHelpOption) {
//   auto parser = Parser().addHelpOption();
//   const char *argv[] = {"test"};
//   EXPECT_NO_THROW(parser.parse(1, (char **)argv));
// }

// TEST(Parser_addHelpOption, AddsHelpOptionWithOtherOptions) {
//   auto parser = Parser().addHelpOption().addOption([] {
//     return FlagOption("-v", "--verbose");
//   });
//   const char *argv[] = {"test", "-v"};
//   EXPECT_NO_THROW(parser.parse(2, (char **)argv));
// }

// // --------------------------------- Parse ---------------------------------
// //

TEST(Parser_parse, DoesNotThrowErrorParsingWithoutOptions) {
  auto parser = Parser();
  const char *argv[] = {"test"};
  EXPECT_NO_THROW(parser.parse(1, (char **)argv));
}

TEST(Parser_parse, ThrowsErrorParsingParametersWithoutOptions) {
  auto parser = Parser();
  const char *argv[] = {"test", "param"};
  EXPECT_THROW(
    {
      try {
        parser.parse(2, (char **)argv);
      } catch (const ParsingError &e) {
        EXPECT_STREQ(e.what(), "Invalid arguments provided!");
        throw;
      }
    },
    ParsingError
  );
}

TEST(Parser_parse, ParsesFlagOption) {
  auto verbose = FlagOption<bool>("-v", "--verbose");
  auto parser = Parser<bool>().addOption(verbose);
  const char *argv[] = {"test", "--verbose"};
  EXPECT_NO_THROW(parser.parse(2, (char **)argv));
}

TEST(Parser_parse, ParsesFlagOptionAsTrue) {
  auto verbose = FlagOption<bool>("-v", "--verbose");
  auto parser = Parser<bool>().addOption(verbose);

  const char *argv[] = {"test", "--verbose"};
  parser.parse(2, (char **)argv);
  EXPECT_TRUE(verbose.getValue());

  const char *other_argv[] = {"test", "-v"};
  parser.parse(2, (char **)other_argv);
  EXPECT_TRUE(verbose.getValue());
}

// TEST(Parser_parse, GetsDefaultOptionNegated) {
// auto verbose = FlagOption<bool>("-v", "--verbose").addDefaultValue(true);
// auto parser = Parser<bool>().addOption(verbose);
//
// const char *argv[] = {"test", "-v"};
// EXPECT_NO_THROW(parser.parse(2, (char **)argv));
// EXPECT_FALSE(verbose.getValue());
// EXPECT_FALSE(parser.getValue<bool>("--verbose"));
// }

TEST(Parser_parse, ParsesSingleOption) {
  auto single = SingleOption<std::string>("-s", "--single");
  auto parser = Parser<std::string>().addOption(single);
  const auto expected = "value";
  const char *argv[] = {"test", "--single", expected};
  EXPECT_NO_THROW(parser.parse(3, (char **)argv));
}

TEST(Parser_parse, ParsesSingleOptionAsString) {
  auto single = SingleOption<std::string>("-s", "--single");
  auto parser = Parser<std::string>().addOption(single);
  const auto expected = "value";
  const char *argv[] = {"test", "--single", expected};
  parser.parse(3, (char **)argv);
  EXPECT_EQ(single.getValue(), expected);
}

TEST(Parser_parse, ParsesSingleOptionAsOtherType) {
  auto single = SingleOption<double>("-s", "--single").toDouble();
  auto parser = Parser<double>().addOption(single);
  const auto number_expected = 12.1;
  const char *argv[] = {
    "test", "--single", std::to_string(number_expected).c_str()
  };
  parser.parse(3, (char **)argv);
  EXPECT_EQ(single.getValue(), number_expected);
}

TEST(Parser_parse, ThrowsErrorExpectingSingleOptionArgument) {
  auto single = SingleOption<int>("-s", "--single").toInt();
  auto parser = Parser<int>().addOption(single);
  const char *argv[] = {"test", "--single"};
  EXPECT_THROW(
    {
      try {
        parser.parse(2, (char **)argv);
      } catch (const ParsingError &e) {
        EXPECT_STREQ(
          e.what(), "After the --single option should be an extra argument!"
        );
        throw;
      }
    },
    ParsingError
  );
}

TEST(Parser_parse, ThrowsErrorParsingSingleIfNoTransformationProvided) {
  auto single = SingleOption<long>("-n", "--number");
  auto parser = Parser<long>().addOption(single);
  const char *argv[] = {"test", "-n", "52"};
  EXPECT_THROW(
    {
      try {
        parser.parse(3, (char **)argv);
      } catch (const std::invalid_argument &error) {
        EXPECT_STREQ(error.what(), "No transformation function provided.");
        throw error;
      }
    },
    std::invalid_argument
  );
}

TEST(Parser_parse, ParsesCompoundOption) {
  auto compound = CompoundOption<std::vector<std::string>>("-c", "--compound");
  auto parser = Parser<std::vector<std::string>>().addOption(compound);
  const auto expected = std::vector<std::string> {"value1", "value2"};
  const char *argv[] = {"test", "--compound", "value1", "value2"};
  EXPECT_NO_THROW(parser.parse(4, (char **)argv));
}

TEST(Parser_parse, ParsesCompoundOptionAsStringVector) {
  auto values_option = CompoundOption<std::vector<std::string>>("-v");
  auto parser = Parser<std::vector<std::string>>().addOption(values_option);
  const auto expected = std::vector<std::string> {"value1", "value2"};
  const char *argv[] = {"test", "-v", "value1", "value2"};
  parser.parse(4, (char **)argv);
  EXPECT_EQ(values_option.getValue(), expected);
}

TEST(Parser_parse, ThrowsErrorExpectingCompoundOptionArgument) {
  auto chars_option = CompoundOption<std::string>("--chars");
  auto parser = Parser<std::string>().addOption(chars_option);
  const char *argv[] = {"test", "--chars"};
  EXPECT_THROW(
    {
      try {
        parser.parse(2, (char **)argv);
      } catch (const ParsingError &e) {
        EXPECT_STREQ(
          e.what(), "After the --chars option should be at least an extra "
                    "argument!"
        );
        throw;
      }
    },
    ParsingError
  );
}

TEST(Parser_parse, ThrowsErrorExpectingNotProvidedOption) {
  auto display_text_option = FlagOption<bool>("-d", "--display-text");
  auto text_option = SingleOption<std::string>("-t", "--text");
  auto parser = Parser<bool, std::string>()
                  .addOption(display_text_option)
                  .addOption(text_option);
  const char *argv[] = {"test", "-t", "value"};
  EXPECT_THROW(
    {
      try {
        parser.parse(3, (char **)argv);
      } catch (const ParsingError &e) {
        EXPECT_STREQ(e.what(), "Missing option: -d\n");
        throw;
      }
    },
    ParsingError
  );
}

// TEST(Parser_parse, ThrowsExceptionParsingAndProvidingHelpOption) {
//   auto parser = Parser().addHelpOption();
//   const char *argv[] = {"test", "-h"};
//   EXPECT_THROW(
//     {
//       try {
//         parser.parse(2, (char **)argv);
//       } catch (const ParsingError &e) {
//         EXPECT_STREQ(e.what(), parser.usage().c_str());
//         throw;
//       }
//     },
//     ParsingError
//   );
// }

// TEST(Parser_parse, ThrowsHelpOptionWithOtherOptions) {
//   auto parser = Parser().addHelpOption().addOption([] {
//     return FlagOption("-v", "--verbose");
//   });
//   const char *argv[] = {"test", "-h", "-v"};
//   EXPECT_THROW(
//     {
//       try {
//         parser.parse(3, (char **)argv);
//       } catch (const ParsingError &e) {
//         EXPECT_STREQ(e.what(), parser.usage().c_str());
//         throw;
//       }
//     },
//     ParsingError
//   );
// }

}  // namespace input_parser