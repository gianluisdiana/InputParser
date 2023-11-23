#include <iostream>
#include <set>

#include "../include/parser.hpp"

namespace input {

Parser& Parser::addOption(const std::function<Option()>& create_option) {
  Option* option = new Option(create_option());
  for (const auto& name : option->getNames()) {
    if (hasOption(name)) throw std::invalid_argument("Option already exists!");
    options[name] = option;
  }
  return *this;
}

Parser& Parser::addHelpOption(void) {
  return addOption([] -> auto {
    return Option(OptionType::Flag)
      .addNames("-h", "--help")
      .addDescription("Shows how to use the program.")
      .beRequired(false);
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
  if (options.contains(name)) return options.at(name)->isFlag();
  return false;
}

bool Parser::hasSingle(const std::string& name) const {
  if (options.contains(name)) return options.at(name)->isSingle();
  return false;
}

bool Parser::hasMultiple(const std::string& name) const {
  if (options.contains(name)) return options.at(name)->isMultiple();
  return false;
}

int Parser::parseSingle(const std::vector<std::string>& arguments,
  const unsigned int index) {
  if (index + 1 >= arguments.size() || hasOption(arguments[index + 1])) {
    throw ParsingError(
      "After the " + arguments[index] + " option should be an extra argument!");
  }
  options[arguments[index]]->setValue(arguments[index + 1]);
  return 1;
}

int Parser::parseMultiple(const std::vector<std::string>& arguments,
  const unsigned int index) {
  unsigned int local_index = index + 1;
  std::vector<std::string> values{};
  while (local_index < arguments.size() && !hasOption(arguments[local_index])) {
    values.push_back(arguments[local_index]);
    local_index++;
  }
  if (local_index == index + 1) {
    throw ParsingError("After the " +
      arguments[index] + " option should be at least an extra argument!");
  }
  options[arguments[index]]->setValue(values);
  return local_index - index - 1;
}

void Parser::checkMissingOptions(void) const {
  for (const auto& [option_name, option] : options) {
    if (option->isRequired() && !option->hasValue() && !option->hasDefaultValue())
      throw ParsingError("Missing option " + option_name);
  }
}

void Parser::displayUsage(void) const {
  std::string usage = "Usage: ./exec_name";
  std::string description = "";
  std::set<Option*> option_names;
  for (const auto& [option_name, option] : options) {
    if (option_names.contains(option)) continue;
    const std::pair<std::string, std::string> brackets_or_not =
      option->isRequired() ? std::make_pair("<", ">") : std::make_pair("[", "]");
    usage += " " + brackets_or_not.first + option_name;
    if (option->isSingle()) {
      usage += " extra_argument";
    } else if (option->isMultiple()) {
      usage += " extra_argument1 extra_argument2 ...";
    }
    usage += brackets_or_not.second;
    if (option->getDescription() != "") {
      description += option_name + " -> " + option->getDescription() + "\n";
    }

    option_names.insert(option);
  }
  std::cout << usage << "\n\n" << description << "\n";
}

} // namespace input