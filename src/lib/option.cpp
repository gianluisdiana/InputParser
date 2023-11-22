#include "../include/option.hpp"
#include "../include/transformation.hpp"
#include "../include/parsing_error.hpp"

namespace input {

Option::Option(const OptionType& type) : type_{type}, value_{}, default_value_{},
  names_{}, description_{}, transformation_{nullptr}, constraints_{},
  transform_before_check_{false} {}

Option& Option::addDefaultValue(const std::any& default_value) {
  default_value_ = default_value;
  return *this;
}

Option& Option::addDescription(const std::string& description) {
  description_ = description;
  return *this;
}

Option& Option::addConstraint(const std::function<bool(const std::any&)>& constraint) {
  constraints_.push_back(constraint);
  return *this;
}

void Option::setValue(const std::any& value) {
  std::any current_value;
  if (transform_before_check_) {
    current_value = applyTransformation(value);
    checkConstraints(current_value);
  } else {
    checkConstraints(value);
    current_value = applyTransformation(value);
  }
  value_ = current_value;
}

// ------------------------ Common transformations ------------------------ //

Option& Option::toInt(void) {
  if (isSingle()) transformation_ = Transformation::toInt;
  else if (isMultiple()) transformation_ = Transformation::allElementsToInt;
  return *this;
}

Option& Option::toDouble(void) {
  if (isSingle()) transformation_ = Transformation::toDouble;
  else if (isMultiple()) transformation_ = Transformation::allElementsToDouble;
  return *this;
}

Option& Option::toFloat(void) {
  if (isSingle()) transformation_ = Transformation::toFloat;
  else if (isMultiple()) transformation_ = Transformation::allElementsToFloat;
  return *this;
}

Option& Option::transformBeforeCheck(void) {
  transform_before_check_ = true;
  return *this;
}

// ---------------------------- Private methods ---------------------------- //

void Option::checkConstraints(const std::any& value) const {
  for (const auto& constraint : constraints_) {
    if (!constraint(value)) {
      if (transform_before_check_) {
        throw input::ParsingError("Constraint not satisfied");
      }
      throw input::ParsingError("Constraint not satisfied, value: " +
        std::any_cast<std::string>(value));
    }
  }
}

} // namespace input