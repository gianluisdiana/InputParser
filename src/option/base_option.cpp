/**
 * @file parser.hpp
 * @author Gian Luis Bolivar Diana (gianluisbolivar1@gmail.com)
 * @version 0.1
 * @date November 27, 2023
 * @copyright Copyright (c) 2023
 *
 * @brief File containing the implementation of a common option used by the
 * parser, that can be a flag, a single value or compound values.
 *
 */

#include <input_parser/option/base_option.hpp>
#include <input_parser/parsing_error.hpp>

namespace input_parser {

BaseOption &BaseOption::addDefaultValue(const std::any &default_value) {
  default_value_ = default_value;
  return beRequired(false);
}

BaseOption &BaseOption::addDescription(const std::string &description) {
  description_ = description;
  return *this;
}

void BaseOption::setValue(const std::any &value) {
  if (transform_before_check_) {
    value_ = transformation_(value);
    checkConstraints(value_);
  } else {
    checkConstraints(value);
    value_ = transformation_(value);
  }
}

BaseOption &BaseOption::transformBeforeCheck() {
  transform_before_check_ = true;
  return *this;
}

BaseOption &BaseOption::beRequired(const bool required) {
  required_ = required;
  return *this;
}

// ---------------------------- Private methods ---------------------------- //

void BaseOption::checkConstraints(const std::any &value) const {
  for (const auto &constraint : constraints_) {
    if (!constraint.call(value)) {
      const std::string error_message = constraint.getErrorMessage();
      throw ParsingError(
        error_message == "" ? "Constraint not satisfied." : error_message
      );
    }
  }
}

}  // namespace input_parser