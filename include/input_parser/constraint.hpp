/**
 * @file constraint.hpp
 * @author Gian Luis Bolivar Diana (gianluisbolivar1@gmail.com)
 * @version 0.1
 * @date November 27, 2023
 * @copyright Copyright (c) 2023
 *
 * @brief File containing the description of a constraint that can be used
 * to validate the arguments provided to the parser.
 *   The constraint is a function that must return a boolean value, if the
 * function returns false, an error message is displayed.
 *
 */

#ifndef _INPUT_CONSTRAINT_HPP_
#define _INPUT_CONSTRAINT_HPP_

#include <any>
#include <functional>
#include <string>

namespace input_parser {

/**
 * @brief Represents a function that must be satisfy when passing a certain
 * argument. If the function returns false, an error message is displayed.
 */
class Constraint {
 public:
  /**
   * @brief Construct a constraint using a function and an error message.
   *
   * @param call The function that must be satisfied.
   * @param error_message The error message to be displayed if the function
   * returns false.
   */
  Constraint(
    const std::function<bool(const std::any &)> &call,
    const std::string &error_message
  ) : call_ {call}, error_message_ {error_message} {}

  /** @brief Gives read-only access to the error message */
  inline const std::string &getErrorMessage() const {
    return error_message_;
  }

  /**
   * @brief Calls the constraint function with the given value.
   *
   * @param value The value to be passed to the constraint function.
   * @return Whether the constraint function returns true or false.
   */
  inline bool call(const std::any &value) const {
    return call_(value);
  }

 private:
  // The function that must be satisfied.
  const std::function<bool(const std::any &)> call_;
  // The error message to be displayed if the function returns false.
  const std::string error_message_;
};

}  // namespace input_parser

#endif  // _INPUT_CONSTRAINT_HPP_