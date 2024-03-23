#include <input_parser/option/flag_option.hpp>

namespace input_parser {

FlagOption &FlagOption::toInt() {
  return to<int>([](const bool &value) -> int { return value ? 1 : 0; });
}

FlagOption &FlagOption::toDouble() {
  return to<double>([](const bool &value) -> double {
    return value ? 1.0 : 0.0;
  });
}

FlagOption &FlagOption::toFloat() {
  return to<float>([](const bool &value) -> float {
    return value ? 1.0f : 0.0f;
  });
}

}  // namespace input_parser