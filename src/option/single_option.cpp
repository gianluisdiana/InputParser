#include <input_parser/option/single_option.hpp>

namespace input_parser {

SingleOption &SingleOption::toInt() {
  return to<int>([](const std::string &value) -> int {
    return std::stoi(value);
  });
}

SingleOption &SingleOption::toDouble() {
  return to<double>([](const std::string &value) -> double {
    return std::stod(value);
  });
}

SingleOption &SingleOption::toFloat() {
  return to<float>([](const std::string &value) -> float {
    return std::stof(value);
  });
}

}  // namespace input_parser