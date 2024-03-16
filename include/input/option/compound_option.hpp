/**
 * @file compound_option.hpp
 * @author Gian Luis Bolivar Diana (gianluisbolivar1@gmail.com)
 * @version 0.3
 * @date January 08, 2024
 * @copyright Copyright (c) 2024
 *
 * @brief File containing the description of a compound option used by the
 * parser, it can be describe as an option that requires at least one extra
 * parameter (it is represented by a vector of values of any type).
 *
 */

#ifndef _INPUT_COMPOUND_OPTION_HPP_
#define _INPUT_COMPOUND_OPTION_HPP_

#include <input/option/base_option.hpp>

namespace input {

/**
 * @brief A compound option is an option that requires at least one more
 * argument.
 *
 * @example
 *   <-c> <chain1 chain2 chain3 ...>
 *    │            └─> The extra required arguments.
 *    └─> The name of the option.
 */
class CompoundOption : public BaseOption {
 public:
  CompoundOption(void) : BaseOption() {}

  /**
   * @brief Indicates if the option is a compound option.
   *
   * @return True.
   */
  inline bool isCompound(void) const override {
    return true;
  }

  /**
   * @brief Transform the vector that contains the option's values using the
   * provided function. The function must take a const std::vector<std::string>&
   * as argument and return the type provided as template argument.
   *
   * Only works for compound options.
   *
   * @tparam T The type to transform the vector to.
   * @param transformation The function that transforms the vector of values
   * @return The instance of the object that called this method.
   */
  template <class T> CompoundOption&
  to(const std::function<T(const std::vector<std::string>&)>& transformation);

  /**
   * @brief Transform each option value using the provided function.
   * The function must take a const std::string& as argument and return the
   * type provided as template argument.
   *
   * @tparam T The type to transform the values to.
   * @param transformation The function that transforms the values of the option
   * @return The instance of the object that called this method.
   */
  template <class T> CompoundOption&
  elementsTo(const std::function<T(const std::string&)>& transformation);

  /**
   * @brief Converts all the elements of the option to integers.
   *
   * Shortcut for:
   * ```cpp
   * option.elementsTo<int>([](const std::string& value) -> int {
   *   return std::stoi(value);
   * });
   * ```
   */
  CompoundOption& toInt(void) override;

  /**
   * @brief Converts all the elements of the option to doubles.
   *
   * Shortcut for:
   * ```cpp
   * option.elementsTo<double>([](const std::string& value) -> double {
   *   return std::stod(value);
   * });
   * ```
   */
  CompoundOption& toDouble(void) override;

  /**
   * @brief Converts all the elements of the option to floats.
   *
   * Shortcut for:
   * ```cpp
   * option.elementsTo<float>([](const std::string& value) -> float {
   *   return std::stof(value);
   * });
   * ```
   */
  CompoundOption& toFloat(void) override;

  // ------------------------ Static casted methods ------------------------ //

  inline CompoundOption& addDefaultValue(const std::any& value) {
    return static_cast<CompoundOption&>(BaseOption::addDefaultValue(value));
  }

  inline CompoundOption& addDescription(const std::string& description) {
    return static_cast<CompoundOption&>(BaseOption::addDescription(description));
  }

  template <class... Ts>
  std::enable_if_t<(is_string_type<Ts> && ...), CompoundOption&>
  inline addNames(const Ts... names) {
    return static_cast<CompoundOption&>(BaseOption::addNames(names...));
  }

  template <class T> inline CompoundOption&
  addConstraint(const std::function<bool(const T&)>& constraint,
    const std::string& error_message) {
    return static_cast<CompoundOption&>(BaseOption::addConstraint(constraint, error_message));
  }

  inline CompoundOption& transformBeforeCheck(void) {
    return static_cast<CompoundOption&>(BaseOption::transformBeforeCheck());
  }

  inline CompoundOption& beRequired(const bool& required = true) {
    return static_cast<CompoundOption&>(BaseOption::beRequired(required));
  }
};

template <class T> CompoundOption&
CompoundOption::to(
  const std::function<T(const std::vector<std::string>&)>& transformation) {
  transformation_ = [transformation](const std::any& value) -> auto {
    return transformation(std::any_cast<std::vector<std::string>>(value));
  };
  return *this;
}

template <class T> CompoundOption&
CompoundOption::elementsTo(const std::function<T(const std::string&)>& transformation) {
  transformation_ = [transformation](const std::any& values) -> auto {
    const auto string_values = std::any_cast<std::vector<std::string>>(values);
    std::vector<T> transformed_values;
    for (const auto& value : string_values) {
      transformed_values.push_back(transformation(value));
    }
    return transformed_values;
  };
  return *this;
}

} // namespace input

#endif // _INPUT_COMPOUND_OPTION_HPP_