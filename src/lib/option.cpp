#include "../include/option.hpp"
#include "../include/transformation.hpp"
#include "../include/parsing_error.hpp"

namespace input {

Option::Option(const OptionType& type) : type_{type}, value_{}, default_value_{},
  names_{}, description_{}, required_{true}, transformation_{nullptr}, constraints_{},
  transform_before_check_{false} {}

Option& Option::addDefaultValue(const std::any& default_value) {
  default_value_ = default_value;
  return *this;
}

Option& Option::addDescription(const std::string& description) {
  description_ = description;
  return *this;
}

void Option::setValue(const std::any& value) {
  if (transform_before_check_) {
    value_ = applyTransformation(value);
    checkConstraints(value_);
  } else {
    checkConstraints(value);
    value_ = applyTransformation(value);
  }
}


Option& Option::transformBeforeCheck(void) {
  transform_before_check_ = true;
  return *this;
}

Option& Option::beRequired(const bool required) {
  required_ = required;
  return *this;
}

// ---------------------------- Private methods ---------------------------- //

void Option::checkConstraints(const std::any& value) const {
  for (const auto& constraint : constraints_) {
    if (!constraint.call(value)) {
      const std::string error_message = constraint.getErrorMessage();
      throw ParsingError(error_message == "" ?
        "Constraint not satisfied." : error_message);
    }
  }
}

} // namespace input