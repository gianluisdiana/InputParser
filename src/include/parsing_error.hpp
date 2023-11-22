#ifndef _PARSING_ERROR_HPP_
#define _PARSING_ERROR_HPP_

#include <stdexcept>

namespace input {

/** @brief Represents an error ocurred parsing a program arguments */
class ParsingError : public std::invalid_argument {
  public:
  /**
   * @brief Construct a ParsingError calling the std::invalid_arguments
   * constructor
   *
   * @param message The message to be shown.
   */
  explicit ParsingError(const std::string& message) :
    std::invalid_argument(message) {}
};

} // namespace input

#endif // _PARSING_ERROR_HPP_