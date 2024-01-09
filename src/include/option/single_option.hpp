/**
 * @file single_option.hpp
 * @author Gian Luis Bolivar Diana (gianluisbolivar1@gmail.com)
 * @version 0.3
 * @date January 08, 2024
 * @copyright Copyright (c) 2024
 *
 * @brief File containing the description of a flag option used by the
 * parser, it can be described as an option that requires one extra parameter
 * (it is represented by a value of any type).
 *
 */

#ifndef _INPUT_SINGLE_OPTION_HPP_
#define _INPUT_SINGLE_OPTION_HPP_

#include <option/base_option.hpp>

namespace input {

/**
 * @brief A single option is an option that requires one more argument.
 *
 * @example
 *  <-f> <file_name>
 *    │       └─> The extra required argument.
 *    └─> The name of the option.
 */
class SingleOption : public BaseOption {
 public:
  SingleOption(void) : BaseOption() {}

  /**
   * @brief Indicates if the option is a single option.
   *
   * @return True.
   */
  inline bool isSingle(void) const override {
    return true;
  }

  /**
   * @brief Transforms the value of the option using the provided function.
   * The function must take a const std::string& as argument and return the
   * type provided as template argument.
   *
   * @tparam T The type to transform the value to.
   * @param transformation The function that transforms the value of the option
   * @return The instance of the object that called this method.
   */
  template <class T>
  SingleOption& to(const std::function<T(const std::string&)>& transformation);

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
  SingleOption& toInt(void) override;

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
  SingleOption& toDouble(void) override;

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
  SingleOption& toFloat(void) override;

  // ------------------------ Static casted methods ------------------------ //

  inline SingleOption& addDefaultValue(const std::any& value) {
    return static_cast<SingleOption&>(BaseOption::addDefaultValue(value));
  }

  inline SingleOption& addDescription(const std::string& description) {
    return static_cast<SingleOption&>(BaseOption::addDescription(description));
  }

  template <class... Ts>
  std::enable_if_t<(is_string_type<Ts> && ...), SingleOption&>
  inline addNames(const Ts... names) {
    return static_cast<SingleOption&>(BaseOption::addNames(names...));
  }

  template <class T> inline SingleOption&
  addConstraint(const std::function<bool(const T&)>& constraint,
    const std::string& error_message) {
    return static_cast<SingleOption&>(BaseOption::addConstraint(constraint, error_message));
  }

  inline SingleOption& transformBeforeCheck(void) {
    return static_cast<SingleOption&>(BaseOption::transformBeforeCheck());
  }

  inline SingleOption& beRequired(const bool& required = true) {
    return static_cast<SingleOption&>(BaseOption::beRequired(required));
  }
};

template <class T> SingleOption&
SingleOption::to(const std::function<T(const std::string&)>& transformation) {
  transformation_ = [transformation](const std::any& value) -> auto {
    return transformation(std::any_cast<std::string>(value));
  };
  return *this;
}

} // namespace input

#endif // _INPUT_SINGLE_OPTION_HPP_