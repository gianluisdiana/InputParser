/**
 * @file parser.hpp
 * @author Gian Luis Bolivar Diana (gianluisbolivar1@gmail.com)
 * @version 0.1
 * @date November 27, 2023
 * @copyright Copyright (c) 2023
 *
 * @brief File containing the implementation of a c++ command line parser
 * that uses the BaseOption class to represent the options that can be parsed
 * and the ParsingError class to represent the errors that can be generated
 * when parsing the arguments provided.
 *
 */

#include <any>
#include <utility>
#include <variant>
#include <vector>

#include <input_parser/parser.hpp>
#include <input_parser/parsing_error.hpp>

namespace input_parser {

// ---------------------------- Static methods ---------------------------- //

void Parser::setOptionValue(Option &option, const std::any &value) {
  std::visit([&value](auto &&opt) { opt.setValue(value); }, option);
}

// -------------------------------- Adders -------------------------------- //

Parser &Parser::addHelpOption() {
  return addOption([] {
    return FlagOption("-h", "--help")
      .addDescription("Shows how to use the program.")
      .addDefaultValue(false);
  });
}

void Parser::parse(unsigned int argc, char *raw_argv[]) {
  const std::vector<std::string> argv(raw_argv, raw_argv + argc);
  for (unsigned int index = 1; index < argc; ++index) {
    if (!hasOption(argv[index])) {
      throw ParsingError("Invalid arguments provided!");
    }
    if (hasFlag(argv[index])) {
      parseFlag(argv[index]);
    } else if (hasSingle(argv[index])) {
      index += parseSingle(argv, index);
    } else if (hasCompound(argv[index])) {
      index += parseCompound(argv, index);
    }
  }
  checkHelpOption();
  checkMissingOptions();
}

// -------------------------------- Checks -------------------------------- //

bool Parser::hasFlag(const std::string &name) const {
  return hasOption(name) &&
         std::visit([](auto &&opt) { return opt.isFlag(); }, getOption(name));
}

bool Parser::hasSingle(const std::string &name) const {
  return hasOption(name) &&
         std::visit([](auto &&opt) { return opt.isSingle(); }, getOption(name));
}

bool Parser::hasCompound(const std::string &name) const {
  return hasOption(name) &&
         std::visit(
           [](auto &&opt) { return opt.isCompound(); }, getOption(name)
         );
}

void Parser::checkMissingOptions() const {
  for (const auto &[_, option] : options_) {
    std::visit(
      [](auto &&opt) {
        if (opt.isRequired() && !opt.hasValue() && !opt.hasDefaultValue()) {
          throw ParsingError("Missing option " + opt.getNames()[0]);
        }
      },
      option
    );
  }
}

void Parser::checkHelpOption() const {
  if (hasOption("-h") && getValue<bool>("-h")) { throw ParsingError(usage()); }
}

// -------------------------- Individual parsers -------------------------- //

void Parser::parseFlag(const std::string &flag_name) {
  std::visit(
    [](auto &&opt) {
      opt.setValue(
        opt.hasDefaultValue() ? !opt.template getDefaultValue<bool>() : true
      );
    },
    getOption(flag_name)
  );
}

unsigned int Parser::parseSingle(
  const std::vector<std::string> &arguments, const unsigned int index
) {
  if (index + 1 >= arguments.size() || hasOption(arguments[index + 1])) {
    throw ParsingError(
      "After the " + arguments[index] + " option should be an extra argument!"
    );
  }
  Parser::setOptionValue(getOption(arguments[index]), arguments[index + 1]);
  return 1;
}

unsigned int Parser::parseCompound(
  const std::vector<std::string> &arguments, const unsigned int index
) {
  std::vector<std::string> values {};
  auto local_index = index + 1;
  while (local_index < arguments.size() && !hasOption(arguments[local_index])) {
    values.push_back(arguments[local_index]);
    ++local_index;
  }
  if (local_index == index + 1) {
    throw ParsingError(
      "After the " + arguments[index] +
      " option should be at least an extra argument!"
    );
  }
  Parser::setOptionValue(getOption(arguments[index]), values);
  return local_index - index - 1;
}

/**
 * Format:
 * NAME:
 *  ./exec_name
 *
 * SYNOPSIS:
 *  ./exec_name [-f, --flag] [-s, --single <extra_argument>] [-c, --compound
 * <extra_argument1> <extra_argument2> ...]
 *
 * DESCRIPTION:
 * Program description.
 *
 * -h, --help
 *   Shows how to use the program.
 *
 * -f, --flag
 *   Flag description.
 *
 * -s, --single <extra_argument>
 *   Single description.
 *
 * -c, --compound <extra_argument1> <extra_argument2> ...
 *   Compound description.
 *
 * AUTHOR:
 *   Written by ...
 */
// void Parser::displayManual() const {
//   std::string synopsis = "SYNOPSIS:\n\t" + getExecutableName() + " ";
//   std::string description = "DESCRIPTION:\n\t" + getDescription() + "\n";
// std::set<Option*> displayed_options;
// for (const auto& [_, option] : options) {
//   if (displayed_options.contains(option)) continue;
//   const std::vector<std::string>& names = option->getNames();
//   for (size_t name_index = 0; name_index < names.size() - 1; ++name_index) {
//     description += names[name_index] + ", ";
//   }
//   description += names.back() + ":\n\t" + option->getDescription() + "\n\n";
//   displayed_options.insert(option);
// }
// const std::string name = "NAME:\n\t" + getExecutableName() + "\n";
// const std::string author = "AUTHOR:\n\tWritten by " + getAuthor() + "\n";
// std::cout << name << "\n" << synopsis << "\n" << description << "\n" <<
// author << "\n";
//

std::string Parser::usage() const {
  std::string usage = "Usage: ./exec_name";
  std::string description;
  for (const auto &[option_name, option] : options_) {
    std::visit(
      [&](auto &&opt) {
        const std::pair<std::string, std::string> brackets_or_not =
          opt.isRequired() ? std::make_pair("<", ">")
                           : std::make_pair("[", "]");
        usage += " " + brackets_or_not.first + option_name +
                 opt.getArgumentName() + brackets_or_not.second;
        if (opt.getDescription() != "") {
          description += option_name + " -> " + opt.getDescription() + "\n";
        }
      },
      option
    );
  }
  return usage + "\n\n" + description + "\n";
}

}  // namespace input_parser