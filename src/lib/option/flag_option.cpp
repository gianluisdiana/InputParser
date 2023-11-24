#include "../../include/option/flag_option.hpp"

namespace input {

FlagOption& FlagOption::toInt(void) {
  return to<int>([](const bool& value) -> int {
    return value ? 1 : 0;
  });
}

FlagOption& FlagOption::toDouble(void) {
  return to<double>([](const bool& value) -> double {
    return value ? 1.0 : 0.0;
  });
}

FlagOption& FlagOption::toFloat(void) {
  return to<float>([](const bool& value) -> float {
    return value ? 1.0f : 0.0f;
  });
}

} // namespace input