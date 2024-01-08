/**
 * @file parser.hpp
 * @author Gian Luis Bolivar Diana (gianluisbolivar1@gmail.com)
 * @version 0.1
 * @date November 27, 2023
 * @copyright Copyright (c) 2023
 *
 * @brief File containing the description of a c++ command line parser.
 *
 */

#ifndef _INPUT_PARSER_HPP_
#define _INPUT_PARSER_HPP_

#include <memory>
#include <unordered_map>
#include <variant>

#include "./parsing_error.hpp"
#include "../include/option/flag_option.hpp"
#include "../include/option/compound_option.hpp"
#include "../include/option/single_option.hpp"

namespace input {

/** @brief The type of an option */
using Option = std::variant<FlagOption, CompoundOption, SingleOption>;

/**
 * @brief Represents a parser of the arguments provided when the program is
 * executed.
 */
class Parser {
  public:
  /**
   * @brief Adds an option to be parsed. The option must be a flag, single or
   * compound option.
   *
   * @param create_option A function that returns the option.
   * @return The instance of the object that called this method.
   */
  Parser& addOption(const std::function<Option()>& create_option);

  /**
   * @brief Adds a basic help option to the parser.
   *  The option will be a flag and will have the names "-h" and "--help".
   *
   *  Shortcut for:
   * ```cpp
   *    addOption([] -> auto { return FlagOption()
   *      .addNames("-h", "--help")
   *      .addDescription("Shows how to use the program.")
   *      .addDefaultValue(false);
   *    });
   * ```
   *
   * @return The instance of the object that called this method.
   */
  Parser& addHelpOption(void);

  /**
   * @brief Gets the value from an option.
   *
   * @param name The name of the option.
   * @tparam T The type of the value to be returned.
   * @return The value of the option casted to the type provided.
   */
  template <class T = std::string>
  const T getValue(const std::string& name) const;

  /**
   * @brief Parses command line input to provide values ​​for previously added
   * options.
   *   If an option is omitted (it was not specified), a ParsingError exception
   * will be thrown.
   *
   * @param argc The amount of arguments provided when executing the program.
   * @param raw_argv A vector of strings with the arguments.
   */
  void parse(int argc, char* raw_argv[]);

  /**
   * @brief Shows to the user how to execute the program correctly.
   */
  void displayUsage(void) const;

 private:
  // All the options registered.
  std::unordered_map<std::string, std::shared_ptr<Option>> options;

  /**
   * @brief Tells if the parser has an option with the name provided.
   *
   * @param name The name of the possible option.
   * @return Whether the parser registered the option or not.
   */
  inline bool hasOption(const std::string& name) const {
    return options.contains(name);
  }

  /**
   * @brief Tells if the parser has a flag option with the name provided.
   *
   * @param name The name of the possible option.
   * @return Whether the parser registered the option or not.
   */
  bool hasFlag(const std::string& name) const;

  /**
   * @brief Tells if the parser has a single option with the name provided.
   *
   * @param name The name of the possible option.
   * @return Whether the parser registered the option or not.
   */
  bool hasSingle(const std::string& name) const;

  /**
   * @brief Tells if the parser has a compound option with the name provided.
   *
   * @param name The name of the possible option.
   * @return Whether the parser registered the option or not.
   */
  bool hasMultiple(const std::string& name) const;

  /**
   * @brief Tells if the parser has a help option.
   *
   * @return Whether the parser registered the option or not.
   */
  inline bool hasHelpOption(void) const {
    return hasFlag("-h") || hasFlag("--help");
  }

  /**
   * @brief Tells if the option provided is the help option.
   *
   * @param name The name of the possible option.
   * @return Whether the option is the help option or not.
   */
  inline bool isHelpOption(const std::string& name) const {
    return name == "-h" || name == "--help";
  }

  /**
   * @brief Sets the value of an option.
   *
   * @param option The option to be changed.
   * @param value The value to be assigned to the option.
   */
  void setOptionValue(Option& option, const std::any& value);

  /**
   * @brief Changes the flag option provided to true.
   * If the option has a default value, the value will be changed to the opposite
   * of the default value.
   *
   * @param flag_name The name of the option to give the value of true.
   */
  void parseFlag(const std::string& flag_name);

  /**
   * @brief Saves the extra argument after the single option.
   *   Checks if that argument was supplied and is not another option specified.
   *
   * @param arguments All the arguments provided by command line.
   * @param index The index of the single option to parse.
   * @return How many arguments have been read.
   */
  int parseSingle(const std::vector<std::string>& arguments,
    const unsigned int index);

  /**
   * @brief Reads all the extra arguments provided after the compound option.
   *   Checks if the arguments were supplied and are not another option specified.
   *
   * @param arguments All the arguments provided by command line.
   * @param index The index of the compound option to parse.
   * @return How many arguments have been read.
   */
  int parseMultiple(const std::vector<std::string>& arguments,
    const unsigned int index);

  /**
   * @brief Check if there are options that have not been specified.
   *  If so, a std::invalid_argument error will be thrown.
   */
  void checkMissingOptions(void) const;
};

template <class T>
const T Parser::getValue(const std::string& name) const {
  if (!hasOption(name)) {
    throw ParsingError("The option " + name + " was not assigned at the parser");
  }
  return std::visit([](auto&& opt) {
    return opt.template getValue<T>();
  }, *options.at(name));
}

} // namespace input

#endif // _INPUT_PARSER_HPP_