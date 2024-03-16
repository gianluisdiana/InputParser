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

#include <iostream>
#include <set>

#include <input/parser.hpp>

namespace input {

Parser& Parser::addOption(const std::function<Option()>& create_option) {
  auto option = std::make_shared<Option>(create_option());
  std::visit([this, option](auto&& opt) {
    for (const auto& name : opt.getNames()) {
      if (this->hasOption(name)) throw std::invalid_argument("Option already exists!");
      this->options[name] = option;
    }
  }, *option);
  return *this;
}

Parser& Parser::addHelpOption(void) {
  return addOption([] -> auto {
    return FlagOption("-h", "--help")
      .addDescription("Shows how to use the program.")
      .addDefaultValue(false);
  });
}

void Parser::parse(int argc, char* raw_argv[]) {
  std::vector<std::string> argv(raw_argv, raw_argv + argc);
  for (int index = 1; index < argc; ++index) {
    if (hasHelpOption() && isHelpOption(argv[index])) {
      displayUsage();
      exit(0);
    }
    if (hasFlag(argv[index])) parseFlag(argv[index]);
    else if (hasSingle(argv[index])) index += parseSingle(argv, index);
    else if (hasMultiple(argv[index])) index += parseMultiple(argv, index);
    else throw ParsingError("Invalid arguments provided!");
  }
  checkMissingOptions();
}

bool Parser::hasFlag(const std::string& name) const {
  return hasOption(name) && std::visit([](auto&& opt) {
    return opt.isFlag();
  }, *options.at(name));
}

bool Parser::hasSingle(const std::string& name) const {
  return hasOption(name) && std::visit([](auto&& opt) {
    return opt.isSingle();
  }, *options.at(name));
}

bool Parser::hasMultiple(const std::string& name) const {
  return hasOption(name) && std::visit([](auto&& opt) {
    return opt.isCompound();
  }, *options.at(name));
}

void Parser::setOptionValue(Option& option, const std::any& value) {
  std::visit([&value](auto&& opt) {
    opt.setValue(value);
  }, option);
}

void Parser::parseFlag(const std::string& flag_name) {
  std::visit([this](auto&& opt) {
    opt.hasDefaultValue() ? opt.setValue(!opt.template getDefaultValue<bool>()) :
      opt.setValue(true);
  }, *options[flag_name]);
}

int Parser::parseSingle(const std::vector<std::string>& arguments,
  const unsigned int index) {
  if (index + 1 >= arguments.size() || hasOption(arguments[index + 1])) {
    throw ParsingError(
      "After the " + arguments[index] + " option should be an extra argument!");
  }
  setOptionValue(*options[arguments[index]], arguments[index + 1]);
  return 1;
}

int Parser::parseMultiple(const std::vector<std::string>& arguments,
  const unsigned int index) {
  std::vector<std::string> values{};
  auto local_index = index + 1;
  while (local_index < arguments.size() && !hasOption(arguments[local_index])) {
    values.push_back(arguments[local_index]);
    ++local_index;
  }
  if (local_index == index + 1) {
    throw ParsingError("After the " +
      arguments[index] + " option should be at least an extra argument!");
  }
  setOptionValue(*options[arguments[index]], values);
  return local_index - index - 1;
}

void Parser::checkMissingOptions(void) const {
  for (const auto& [option_name, option] : options) {
    std::visit([](auto&& opt) {
      if (opt.isRequired() && !opt.hasValue() && !opt.hasDefaultValue())
        throw ParsingError("Missing option " + opt.getNames()[0]);
    }, *option);
  }
}

/**
 * Format:
 * NAME:
 *  ./exec_name
 *
 * SYNOPSIS:
 *  ./exec_name [-f, --flag] [-s, --single <extra_argument>] [-c, --compound <extra_argument1> <extra_argument2> ...]
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
// void Parser::displayManual(void) const {
//   std::string synopsis = "SYNOPSIS:\n\t" + getExecutableName() + " ";
//   std::string description = "DESCRIPTION:\n\t" + getDescription() + "\n";

//   std::set<Option*> displayed_options;
//   for (const auto& [_, option] : options) {
//     if (displayed_options.contains(option)) continue;
//     const std::vector<std::string>& names = option->getNames();
//     for (size_t name_index = 0; name_index < names.size() - 1; ++name_index) {
//       description += names[name_index] + ", ";
//     }
//     description += names.back() + ":\n\t" + option->getDescription() + "\n\n";
//     displayed_options.insert(option);
//   }
//   const std::string name = "NAME:\n\t" + getExecutableName() + "\n";
//   const std::string author = "AUTHOR:\n\tWritten by " + getAuthor() + "\n";
//   std::cout << name << "\n" << synopsis << "\n" << description << "\n" << author << "\n";
// }

void Parser::displayUsage(void) const {
  std::string usage = "Usage: ./exec_name";
  std::string description = "";
  std::set<std::shared_ptr<Option>> options_displayed;
  for (const auto& [option_name, option] : options) {
    if (options_displayed.contains(option)) continue;
    std::visit([&usage, option_name, &description](auto&& opt) {
      const std::pair<std::string, std::string> brackets_or_not = opt.isRequired() ?
        std::make_pair("<", ">") : std::make_pair("[", "]");
      usage += " " + brackets_or_not.first + option_name;
      // + opt.getExtraArgumentName() + brackets_or_not.second;
      using T = std::decay_t<decltype(opt)>;
      if constexpr (std::is_same_v<T, SingleOption>) {
        usage += " extra_argument";
      } else if constexpr (std::is_same_v<T, CompoundOption>) {
        usage += " extra_argument1 extra_argument2 ...";
      }
      usage += brackets_or_not.second;
      if (opt.getDescription() != "") {
        description += option_name + " -> " + opt.getDescription() + "\n";
      }
    }, *option);
    options_displayed.insert(option);
  }
  std::cout << usage << "\n\n" << description << "\n";
}

} // namespace input