/**
 * @file parser.hpp
 * @author Gian Luis Bolivar Diana (gianluisbolivar1@gmail.com)
 * @version 0.5
 * @date July 13, 2024
 * @copyright Copyright (c) 2024
 *
 * @brief File containing the description of a c++23 command line parser.
 *
 */

#ifndef INPUT_PARSER_PARSER_HPP_
#define INPUT_PARSER_PARSER_HPP_

#include <unordered_map>

#include <input_parser/option/compound_option.hpp>
#include <input_parser/option/flag_option.hpp>
#include <input_parser/option/single_option.hpp>
#include <input_parser/parsing_error.hpp>

namespace input_parser {

/**
 * @brief Concept to check if the type is an option kind.
 *
 * @tparam OptionType The type to check. It must be a Flag, Single or Compound
 * option.
 * @tparam Types The types that the value of the option can be.
 */
template <typename OptionType, typename... Types>
concept option_kind = same_as_any<
  OptionType, FlagOption<Types>..., SingleOption<Types>...,
  CompoundOption<Types>...>;

/**
 * @brief Represents a parser of the arguments provided when the program is
 * executed.
 *
 * @tparam Types All the possible types that the option's values can have.
 */
template <typename... Types>
class Parser {
 public:
  // ------------------------------- Typedefs ------------------------------ //

  /**
   * @brief Represents a map with a string as key and a certain option as value.
   *
   * @tparam OptionType The type of the option to be stored.
   */
  template <typename OptionType>
  using OptionMap =
    std::unordered_map<std::string, std::reference_wrapper<OptionType>>;

  /** @brief The type of the container that holds all the options */
  using OptionContainer = std::tuple<
    OptionMap<FlagOption<Types>>..., OptionMap<SingleOption<Types>>...,
    OptionMap<CompoundOption<Types>>...>;

  // ------------------------------ Constructors --------------------------- //

  /** @brief Create an empty parser with no options */
  Parser() = default;

  // -------------------------------- Adders ------------------------------- //

  /**
   * @brief Adds an option to be parsed.
   *
   * @tparam OptionType The type of the option to be added.
   * @param option A reference to the option that will be added.
   * @return The instance of the object that called this method.
   */
  template <typename OptionType>
  requires option_kind<OptionType, Types...>
  Parser &addOption(OptionType &option);

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
  OptionContainer options_ {};
  // Helper map to get the option by name.
  std::unordered_map<std::string, std::string> names_ {};

  /**
   * @brief Gives read-write access to the option with the provided name.
   * The option must be of the type provided in the template.
   *
   * @tparam OptionType The type of the option to be returned.
   * @param name The name of the option to be returned.
   * @return A reference to the option with the provided name.
   */
  template <typename OptionType>
  requires option_kind<OptionType, Types...>
  OptionType &getOption(const std::string &name) {
    return std::get<OptionMap<OptionType>>(options_).at(names_[name]).get();
  }

  // ------------------------------- Checks ------------------------------- //

  /**
   * @brief Tells if the parser has an option with the name provided.
   *
   * @param name The name of the possible option.
   * @return Whether the parser registered the option or not.
   */
  bool hasOptionName(const std::string &name) const {
    return names_.find(name) != names_.end();
  }

  /**
   * @brief Tells if the parser has an option with the reference name provided.
   *
   * @tparam OptionType The type of the option to be checked. Must be a Flag,
   * Single or Compound option.
   * @param reference_name The reference name of the option.
   * @return Whether the parser registered the option or not.
   */
  template <typename OptionType>
  requires option_kind<OptionType, Types...>
  bool hasOption(const std::string &reference_name) const {
    return std::get<OptionMap<OptionType>>(options_).contains(reference_name);
  }

  /**
   * @brief Tells if the parser has a flag option with the name provided.
   *
   * @param reference_name The reference name of the option.
   * @return Whether the parser has a flag option with the name provided or not.
   */
  bool hasFlag(const std::string &reference_name) const {
    return (... || hasOption<FlagOption<Types>>(reference_name));
  }

  /**
   * @brief Tells if the parser has a single option with the name provided.
   *
   * @param reference_name The reference name of the option.
   * @return Whether the parser has a single option with the name provided or
   * not.
   */
  bool hasSingle(const std::string &reference_name) const {
    return (... || hasOption<SingleOption<Types>>(reference_name));
  }

  /**
   * @brief Tells if the parser has a compound option with the name provided.
   *
   * @param reference_name The reference name of the option.
   * @return Whether the parser has a compound option with the name provided or
   * not.
   */
  bool hasCompound(const std::string &reference_name) const {
    return (... || hasOption<CompoundOption<Types>>(reference_name));
  }

  /**
   * @brief Check if there are options that have not been specified.
   *  If so, a ParsingError error will be thrown.
   */
  void checkMissingOptions() const;

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
  unsigned int
  parseSingle(const std::vector<std::string> &arguments, unsigned int index);

  /**
   * @brief Reads all the extra arguments provided after the compound option.
   *   Checks if the arguments were supplied and are not another option
   * specified.
   *
   * @param arguments All the arguments provided by command line.
   * @param index The index of the compound option to parse.
   * @return How many arguments have been read.
   */
  unsigned int
  parseCompound(const std::vector<std::string> &arguments, unsigned int index);
};

}  // namespace input_parser

#endif  // INPUT_PARSER_PARSER_HPP_

#include <input_parser/parser.tpp>