#include <input/option/compound_option.hpp>

namespace input {

CompoundOption &CompoundOption::toInt() {
  return elementsTo<int>([](const std::string &str) -> int {
    return std::stoi(str);
  });
}

CompoundOption &CompoundOption::toDouble() {
  return elementsTo<double>([](const std::string &str) -> double {
    return std::stod(str);
  });
}

CompoundOption &CompoundOption::toFloat() {
  return elementsTo<float>([](const std::string &str) -> float {
    return std::stof(str);
  });
}

}  // namespace input