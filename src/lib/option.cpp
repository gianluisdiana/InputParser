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
  if (transform_before_check_) {
    value_ = applyTransformation(value);
    checkConstraints(value_);
  } else {
    checkConstraints(value);
    value_ = applyTransformation(value);
  }
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
      if (transform_before_check_) throw ParsingError("Constraint not satisfied");
      throw ParsingError("Constraint not satisfied, value: " + valueToString(value));
    }
  }
}

const std::string Option::valueToString(const std::any& value) const {
  if (isFlag()) return std::any_cast<bool>(value) ? "true" : "false";
  if (isSingle()) return std::any_cast<std::string>(value);
  std::string string_value = "";
  for (const auto& val : std::any_cast<std::vector<std::string>>(value)) {
    string_value += val + " ";
  }
  return string_value;
}

} // namespace input