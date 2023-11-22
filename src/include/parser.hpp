#ifndef _INPUT_PARSER_HPP_
#define _INPUT_PARSER_HPP_

#include <unordered_map>

#include "./option.hpp"

namespace input {

/**
 * @brief Represents a parser of the arguments provided when the program is
 * executed.
 */
class Parser {
  public:
  /**
   * @brief Adds an option to be parsed. The option type will be determined by
   * the enum OptionType.
   *
   * @param create_option A function that returns the option.
   * @return The instance of the object that called this method.
   */
  Parser& addOption(const std::function<Option()>& create_option);

    /**
   * @brief Gets the value from an option.
   *
   * @param name The name of the option.
   * @tparam T The type of the value to be returned.
   */
  template <class T = std::string>
  inline const T getValue(const std::string& name) const {
    return options.at(name)->getValue<T>();
  }

  /**
   * @brief Parses command line input to provide values ​​for previously added
   * options.
   *   If an option is omitted (it was not specified), a std::invalid_argument
   * error will be thrown.
   *
   * @param argc The amount of arguments provided when executing the program.
   * @param raw_argv A vector of strings with the arguments.
   */
  void parse(int argc, char* raw_argv[]);

  /**
   * @brief Shows to the user how to execute the program correctly.
   */
  void displayUsage() const;

 private:
  // All the options registered.
  std::unordered_map<std::string, Option*> options;

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
  bool isFlag(const std::string& name) const;

  /**
   * @brief Tells if the parser has a single option with the name provided.
   *
   * @param name The name of the possible option.
   * @return Whether the parser registered the option or not.
   */
  bool isSingle(const std::string& name) const;

  /**
   * @brief Tells if the parser has a multiple option with the name provided.
   *
   * @param name The name of the possible option.
   * @return Whether the parser registered the option or not.
   */
  bool isMultiple(const std::string& name) const;

  /**
   * @brief Changes the flag option provided to true.
   *
   * @param flag_name The name of the option to give the value of true.
   */
  inline void parseFlag(const std::string& flag_name) {
    options[flag_name]->setValue(true);
  }

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
   * @brief Reads all the extra arguments provided after the multiple option.
   *   Checks if the arguments were supplied and are not another option specified.
   *
   * @param arguments All the arguments provided by command line.
   * @param index The index of the multiple option to parse.
   * @return How many arguments have been read.
   */
  int parseMultiple(const std::vector<std::string>& arguments,
    const unsigned int index);

  /**
   * @brief Check if there are options that have not been specified.
   *  If so, a std::invalid_argument error will be thrown.
   */
  void checkMissingOptions() const;
};

} // namespace input

#endif // _INPUT_PARSER_HPP_