#include "../../include/option/multiple_option.hpp"

namespace input {

MultipleOption& MultipleOption::toInt(void) {
  return elementsTo<int>([](const std::string& str) -> int {
    return std::stoi(str);
  });
}

MultipleOption& MultipleOption::toDouble(void) {
  return elementsTo<double>([](const std::string& str) -> double {
    return std::stod(str);
  });
}

MultipleOption& MultipleOption::toFloat(void) {
  return elementsTo<float>([](const std::string& str) -> float {
    return std::stof(str);
  });
}

} // namespace input