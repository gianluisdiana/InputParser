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

#ifndef INPUT_PARSER_PARSER_HPP_
#define INPUT_PARSER_PARSER_HPP_

#include <unordered_map>
#include <variant>

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
  using OptionContainer = std::tuple<OptionMap<FlagOption<Types>>...>;

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
  Parser &addOption(OptionType &option) {
    const auto &reference_name = option.getNames().front();
    for (const auto &name : option.getNames()) {
      if (hasOption(name)) {
        throw std::invalid_argument("Option already exists!");
      }
      names_.insert({name, reference_name});
    }
    std::get<OptionMap<OptionType>>(options_).insert({reference_name, option});
    return *this;
  }

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
  OptionContainer options_;
  // Helper map to get the option by name.
  std::unordered_map<std::string, std::string> names_;

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
  bool hasOption(const std::string &name) const {
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
   * @brief Check if there are options that have not been specified.
   *  If so, a std::invalid_argument error will be thrown.
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
};

// --------------------------------- Parse --------------------------------- //

template <typename... Types>
void Parser<Types...>::parse(unsigned int argc, char *raw_argv[]) {
  const std::vector<std::string> argv(raw_argv, raw_argv + argc);
  for (unsigned int index = 1; index < argc; ++index) {
    if (!hasOption(argv[index])) {
      throw ParsingError("Invalid arguments provided!");
    }
    if (hasFlag(argv[index])) { parseFlag(argv[index]); }
  }
  checkMissingOptions();
}

// --------------------------------- Checks -------------------------------- //

template <typename... Types>
bool Parser<Types...>::hasFlag(const std::string &name) const {
  const std::string &reference_name = names_.at(name);
  return (
    ... ||
    std::get<OptionMap<FlagOption<Types>>>(options_).contains(reference_name)
  );
}

template <typename... Types>
void Parser<Types...>::checkMissingOptions() const {
  std::string missing_options;
  std::apply(
    [this, &missing_options](const auto &...option_maps) {
      auto checkOption = [&](const auto option_map) {
        for (const auto &[name, option_ref] : option_map) {
          auto &option = option_ref.get();
          if (!option.isValid()) {
            missing_options += "Missing option: " + name + "\n";
          }
        }
      };
      (checkOption(option_maps), ...);
    },
    options_
  );
  if (!missing_options.empty()) { throw ParsingError(missing_options); }
}

// ---------------------------- Individual parsers -------------------------- //

template <typename... Types>
void Parser<Types...>::parseFlag(const std::string &flag_name) {
  const std::string &reference_name = names_.at(flag_name);
  std::apply(
    [&, this](const auto &...option_maps) {
      const auto setFlagValueIfHasOption = [&](const auto &option_map) {
        if (option_map.contains(reference_name)) {
          option_map.at(reference_name).get().setValue(true);
        }
      };
      (setFlagValueIfHasOption(option_maps), ...);
    },
    options_
  );
}

template <typename... Types>
std::string Parser<Types...>::usage() const {
  std::string usage = "Usage:\n";
  return usage;
}

}  // namespace input_parser

#endif  // INPUT_PARSER_PARSER_HPP_