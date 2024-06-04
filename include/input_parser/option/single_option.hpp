/**
 * @file single_option.hpp
 * @author Gian Luis Bolivar Diana (gianluisbolivar1@gmail.com)
 * @version 0.3
 * @date January 08, 2024
 * @copyright Copyright (c) 2024
 *
 * @brief File containing the description of a flag option used by the
 * parser, it can be described as an option that requires one extra parameter
 * (it is represented by a value of the template type).
 *
 */

#ifndef INPUT_SINGLE_OPTION_HPP_
#define INPUT_SINGLE_OPTION_HPP_

#include <input_parser/option/base_option.hpp>

namespace input_parser {

/**
 * @brief A single option is an option that requires one more argument.
 *
 * @example
 *  <-f> <file_name>
 *    │       └─> The extra required argument.
 *    └─> The name of the option.
 * @tparam ValueType The type of the value inside the option.
 */
template <typename ValueType>
class SingleOption final : public BaseOption<ValueType, std::string> {
 public:
  /**
   * @brief Constructs an empty option with the provided names.
   *
   * @tparam T Type of the mandatory name (must be strings or const char*)
   * @tparam Ts Types of the names (same type as T)
   * @param name The name of the option
   * @param extra_names Extra names that the option can be recognized by
   */
  explicit SingleOption(
    const string_kind auto name, const string_kind auto... extra_names
  ) : BaseOption<ValueType, std::string>(name, extra_names...) {
    argument_name_ = " value";
  }

  /**
   * @brief Indicates if the option is a single option.
   *
   * @return True.
   */
  [[nodiscard]] bool isSingle() const override {
    return true;
  }

  /**
   * @brief Transform the string value to an integer.
   *
   * Shortcut for:
   * ```cpp
   * option.to<int>([](const std::string& value) -> int {
   *   return std::stoi(value);
   * });
   * ```
   *
   * @return The option itself.
   */
  SingleOption &toInt() override;

  /**
   * @brief Transform the string value to a double.
   *
   * Shortcut for:
   * ```cpp
   * option.to<double>([](const std::string& value) -> double {
   *   return std::stod(value);
   * });
   * ```
   *
   * @return The option itself.
   */
  SingleOption &toDouble() override;

  /**
   * @brief Transform the string value to a float.
   *
   * Shortcut for:
   * ```cpp
   * option.to<float>([](const std::string& value) -> float {
   *   return std::stof(value);
   * });
   * ```
   *
   * @return The option itself.
   */
  SingleOption &toFloat() override;

  // ---------------------- Dinamically casted methods ---------------------- //

  SingleOption &addDefaultValue(const ValueType &value) {
    return dynamic_cast<SingleOption &>(
      BaseOption<ValueType, std::string>::addDefaultValue(value)
    );
  }

  SingleOption &addDescription(const std::string &description) {
    return dynamic_cast<SingleOption &>(
      BaseOption<ValueType, std::string>::addDescription(description)
    );
  }

  template <typename T>
  requires same_as_any<T, ValueType, std::string>
  SingleOption &addConstraint(
    const std::function<bool(const T &)> &constraint,
    const std::string &error_message = ""
  ) {
    return dynamic_cast<SingleOption &>(
      BaseOption<ValueType, std::string>::addConstraint(
        constraint, error_message
      )
    );
  }

  SingleOption &
  addTransformation(const std::function<ValueType(std::string)> &transformation
  ) {
    return dynamic_cast<SingleOption &>(
      BaseOption<ValueType, std::string>::addTransformation(transformation)
    );
  }

  SingleOption &beRequired(const bool &required = true) {
    return dynamic_cast<SingleOption &>(
      BaseOption<ValueType, std::string>::beRequired(required)
    );
  }
};

template <typename ValueType>
SingleOption<ValueType> &SingleOption<ValueType>::toInt() {
  return addTransformation([](const auto &value) -> int {
    return std::stoi(value);
  });
}

template <typename ValueType>
SingleOption<ValueType> &SingleOption<ValueType>::toDouble() {
  return addTransformation([](const auto &value) -> double {
    return std::stod(value);
  });
}

template <typename ValueType>
SingleOption<ValueType> &SingleOption<ValueType>::toFloat() {
  return addTransformation([](const auto &value) -> float {
    return std::stof(value);
  });
}

}  // namespace input_parser

#endif  // INPUT_SINGLE_OPTION_HPP_