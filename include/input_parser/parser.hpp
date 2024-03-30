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

#include <unordered_map>
#include <variant>

#include <input_parser/option/compound_option.hpp>
#include <input_parser/option/flag_option.hpp>
#include <input_parser/option/single_option.hpp>
#include <input_parser/parsing_error.hpp>

namespace input_parser {

/** @brief The type of an option */
using Option = std::variant<FlagOption, CompoundOption, SingleOption>;

/**
 * @brief Represents a parser of the arguments provided when the program is
 * executed.
 */
class Parser {
 public:
  /** @brief Create an empty parser with no options */
  Parser();

  // -------------------------------- Adders ------------------------------- //

  /**
   * @brief Adds an option to be parsed.
   *
   * @tparam CreateFunction The type of the function that creates the option.
   * @param create_option A function that returns the option.
   * @return The instance of the object that called this method.
   */
  template <typename CreateFunction>
  Parser &addOption(const CreateFunction &create_option
  ) requires(std::is_invocable_r_v<Option, CreateFunction>);

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
  Parser &addHelpOption();

  // ------------------------------- Getters ------------------------------- //

  /**
   * @brief Gets the value from an option.
   *
   * @param name The name of the option.
   * @tparam T The type of the value to be returned.
   * @return The value of the option casted to the type provided.
   */
  template <class T = std::string>
  const T getValue(const std::string &name) const;

  // -------------------------------- Utility ------------------------------ //

  /**
   * @brief Parses command line input to provide values ​​for previously
   * added options. If an option is omitted (it was not specified), a
   * ParsingError exception will be thrown.
   *
   * @param argc The amount of arguments provided when executing the program.
   * @param raw_argv A vector of strings with the arguments.
   */
  void parse(unsigned int argc, char *raw_argv[]);

  /**
   * @brief Shows to the user how to execute the program correctly.
   */
  std::string usage() const;

 private:
  // All the options registered.
  std::unordered_map<std::string, Option> options_;
  // Helper map to get the option by name.
  std::unordered_map<std::string, std::string> names_;

  // ---------------------------- Static Methods --------------------------- //

  /**
   * @brief Sets the value of an option.
   *
   * @param option The option to be changed.
   * @param value The value to be assigned to the option.
   */
  static void setOptionValue(Option &option, const std::any &value);

  // ------------------------------- Getters ------------------------------- //

  /** @brief Gives readonly access to the option with the provided name */
  inline const Option &getOption(const std::string &name) const {
    return options_.at(names_.at(name));
  }

  /** @brief Gives read-write access to the option with the provided name */
  inline Option &getOption(const std::string &name) {
    return options_.at(names_.at(name));
  }

  // ------------------------------- Checks ------------------------------- //

  /**
   * @brief Tells if the parser has an option with the name provided.
   *
   * @param name The name of the possible option.
   * @return Whether the parser registered the option or not.
   */
  inline bool hasOption(const std::string &name) const {
    return names_.find(name) != names_.end();
  }

  /**
   * @brief Tells if the parser has a flag option with the name provided.
   *
   * @param name The name of the possible option.
   * @return Whether the parser registered the option or not.
   */
  bool hasFlag(const std::string &name) const;

  /**
   * @brief Tells if the parser has a single option with the name provided.
   *
   * @param name The name of the possible option.
   * @return Whether the parser registered the option or not.
   */
  bool hasSingle(const std::string &name) const;

  /**
   * @brief Tells if the parser has a compound option with the name provided.
   *
   * @param name The name of the possible option.
   * @return Whether the parser registered the option or not.
   */
  bool hasCompound(const std::string &name) const;

  /**
   * @brief Check if there are options that have not been specified.
   *  If so, a std::invalid_argument error will be thrown.
   */
  void checkMissingOptions() const;

  /**
   * @brief Check if the help option was specified.
   * If so, display the usage and exit the program.
   */
  void checkHelpOption() const;

  // ------------------------- Individual parsers -------------------------- //

  /**
   * @brief Changes the flag option provided to true.
   * If the option has a default value, the value will be changed to the
   * opposite of the default value.
   *
   * @param flag_name The name of the option to give the value of true.
   */
  void parseFlag(const std::string &flag_name);

  /**
   * @brief Saves the extra argument after the single option.
   *   Checks if that argument was supplied and is not another option specified.
   *
   * @param arguments All the arguments provided by command line.
   * @param index The index of the single option to parse.
   * @return How many arguments have been read.
   */
  unsigned int parseSingle(
    const std::vector<std::string> &arguments, const unsigned int index
  );

  /**
   * @brief Reads all the extra arguments provided after the compound option.
   *   Checks if the arguments were supplied and are not another option
   * specified.
   *
   * @param arguments All the arguments provided by command line.
   * @param index The index of the compound option to parse.
   * @return How many arguments have been read.
   */
  unsigned int parseCompound(
    const std::vector<std::string> &arguments, const unsigned int index
  );
};

template <typename CreateFunction>
Parser &Parser::addOption(const CreateFunction &create_option
) requires(std::is_invocable_r_v<Option, CreateFunction>) {
  const auto option = create_option();
  const auto &reference_name = option.getNames().front();
  for (const auto &name : option.getNames()) {
    if (hasOption(name)) throw std::invalid_argument("Option already exists!");
    names_[name] = reference_name;
  }
  options_.emplace(reference_name, option);
  return *this;
}

template <class T>
const T Parser::getValue(const std::string &name) const {
  if (!hasOption(name)) {
    throw ParsingError(
      "The option " + name + " was not assigned at the parser"
    );
  }
  return std::visit(
    [](auto &&opt) { return opt.template getValue<T>(); }, getOption(name)
  );
}

}  // namespace input_parser

#endif  // _INPUT_PARSER_HPP_