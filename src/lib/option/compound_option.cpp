#include <option/compound_option.hpp>

namespace input {

CompoundOption& CompoundOption::toInt(void) {
  return elementsTo<int>([](const std::string& str) -> int {
    return std::stoi(str);
  });
}

CompoundOption& CompoundOption::toDouble(void) {
  return elementsTo<double>([](const std::string& str) -> double {
    return std::stod(str);
  });
}

CompoundOption& CompoundOption::toFloat(void) {
  return elementsTo<float>([](const std::string& str) -> float {
    return std::stof(str);
  });
}

} // namespace input