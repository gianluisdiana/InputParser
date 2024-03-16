/**
 * @file flag_option.hpp
 * @author Gian Luis Bolivar Diana (gianluisbolivar1@gmail.com)
 * @version 0.3
 * @date January 08, 2024
 * @copyright Copyright (c) 2024
 *
 * @brief File containing the description of a flag option used by the
 * parser, it can be describe as an option that does not require any extra
 * parameter (it is represented by a boolean value).
 *
 */

#ifndef _INPUT_FLAG_OPTION_HPP_
#define _INPUT_FLAG_OPTION_HPP_

#include <input/option/base_option.hpp>

namespace input {

/**
 *  @brief A flag option is an option that must be placed alone. Represents a
 * boolean value.
 *
 * @example
 *   <-d>
 *    └─> The name of the flag.
 */
class FlagOption : public BaseOption {
 public:
  /**
   * @brief Constructs an empty option with the provided names.
   *
   * @tparam T Type of the mandatory name (must be strings or const char*)
   * @tparam Ts Types of the names (same type as T)
   * @param name The name of the option
   * @param extra_names Extra names that the option can be recognized by
   */
  template <
    typename T, typename... Ts,
    typename = typename std::enable_if_t<
      is_string_type<T> && (is_string_type<Ts> && ...)>>
  FlagOption(const T name, const Ts... extra_names) :
    BaseOption(name, extra_names...) {}

  /**
   * @brief Indicates if the option is a flag.
   *
   * @return True.
   */
  inline bool isFlag() const override {
    return true;
  }

  /**
   * @brief Transforms the value of the option using the provided function.
   * The function must take a const bool& as argument and return the type
   * provided as template argument.
   *
   * @tparam T The type to transform the value to.
   * @param transformation The function that transforms the value of the option
   * @return The instance of the object that called this method.
   */
  template <class T>
  FlagOption &to(const std::function<T(const bool &)> &transformation);

  /**
   * @brief Converts the value of the option to an integer.
   *
   * Shortcut for:
   * ```cpp
   * option.to<int>([](const bool& value) -> int {
   *  return value ? 1 : 0;
   * });
   */
  FlagOption &toInt(void) override;

  /**
   * @brief Converts the value of the option to a double.
   *
   * Shortcut for:
   * ```cpp
   * option.to<double>([](const bool& value) -> double {
   *  return value ? 1.0 : 0.0;
   * });
   */
  FlagOption &toDouble(void) override;

  /**
   * @brief Converts the value of the option to a float.
   *
   * Shortcut for:
   * ```cpp
   * option.to<float>([](const bool& value) -> float {
   *  return value ? 1.0f : 0.0f;
   * });
   */
  FlagOption &toFloat(void) override;

  // ------------------------ Static casted methods ------------------------ //

  inline FlagOption &addDefaultValue(const std::any &value) {
    return static_cast<FlagOption &>(BaseOption::addDefaultValue(value));
  }

  inline FlagOption &addDescription(const std::string &description) {
    return static_cast<FlagOption &>(BaseOption::addDescription(description));
  }

  template <class T>
  inline FlagOption &addConstraint(
    const std::function<bool(const T &)> &constraint,
    const std::string &error_message
  ) {
    return static_cast<FlagOption &>(
      BaseOption::addConstraint(constraint, error_message)
    );
  }

  inline FlagOption &transformBeforeCheck(void) {
    return static_cast<FlagOption &>(BaseOption::transformBeforeCheck());
  }

  inline FlagOption &beRequired(const bool &required = true) {
    return static_cast<FlagOption &>(BaseOption::beRequired(required));
  }
};

template <class T>
FlagOption &
FlagOption::to(const std::function<T(const bool &)> &transformation) {
  transformation_ = [transformation](const std::any &value) -> std::any {
    return transformation(std::any_cast<bool>(value));
  };
  return *this;
}

}  // namespace input

#endif  // _INPUT_FLAG_OPTION_HPP_