#ifndef _INPUT_OPTION_SINGLE_OPTION_HPP_
#define _INPUT_OPTION_SINGLE_OPTION_HPP_

#include "../option.hpp" // #include "./base_option.hpp"

namespace input {

/**
 * @brief A single option is an option that requires one more argument.
 *
 * @example
 *  <-f> <file_name>
 *    │       └─> The extra required argument.
 *    └─> The name of the option.
 */
class SingleOption : public Option {
 public:
  SingleOption(void) : Option(OptionType::Single) {}

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
    return static_cast<SingleOption&>(Option::addDefaultValue(value));
  }

  inline SingleOption& addDescription(const std::string& description) {
    return static_cast<SingleOption&>(Option::addDescription(description));
  }

  template <class... Ts>
  std::enable_if_t<(is_string_type<Ts> && ...), SingleOption&>
  inline addNames(const Ts... names) {
    return static_cast<SingleOption&>(Option::addNames(names...));
  }

  template <class T> inline SingleOption&
  addConstraint(const std::function<bool(const T&)>& constraint,
    const std::string& error_message) {
    return static_cast<SingleOption&>(Option::addConstraint(constraint, error_message));
  }

  inline SingleOption& transformBeforeCheck(void) {
    return static_cast<SingleOption&>(Option::transformBeforeCheck());
  }

  inline SingleOption& beRequired(const bool& required = true) {
    return static_cast<SingleOption&>(Option::beRequired(required));
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

#endif // _INPUT_OPTION_SINGLE_OPTION_HPP_