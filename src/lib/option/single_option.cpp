#include "../../include/option/single_option.hpp"

namespace input {

SingleOption& SingleOption::toInt(void) {
  return to<int>([](const std::string& value) -> int {
    return std::stoi(value);
  });
}

SingleOption& SingleOption::toDouble(void) {
  return to<double>([](const std::string& value) -> double {
    return std::stod(value);
  });
}

SingleOption& SingleOption::toFloat(void) {
  return to<float>([](const std::string& value) -> float {
    return std::stof(value);
  });
}

} // namespace input