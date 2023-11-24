#ifndef _INPUT_OPTION_MULTIPLE_OPTION_HPP_
#define _INPUT_OPTION_MULTIPLE_OPTION_HPP_

#include "../option.hpp" // #include "./base_option.hpp"

namespace input {

/**
 * @brief A multiple option is an option that requires at least one more
 * argument.
 *
 * @example
 *   <-c> <chain1 chain2 chain3 ...>
 *    │            └─> The extra required arguments.
 *    └─> The name of the option.
 */
class MultipleOption : public Option {
 public:
  MultipleOption(void) : Option(OptionType::Multiple) {}

  /**
   * @brief Transform the vector that contains the option's values using the
   * provided function. The function must take a const std::vector<std::string>&
   * as argument and return the type provided as template argument.
   *
   * Only works for multiple options.
   *
   * @tparam T The type to transform the vector to.
   * @param transformation The function that transforms the vector of values
   * @return The instance of the object that called this method.
   */
  template <class T> MultipleOption&
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
  template <class T> MultipleOption&
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
  MultipleOption& toInt(void) override;

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
  MultipleOption& toDouble(void) override;

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
  MultipleOption& toFloat(void) override;

  // ------------------------ Static casted methods ------------------------ //

  inline MultipleOption& addDefaultValue(const std::any& value) {
    return static_cast<MultipleOption&>(Option::addDefaultValue(value));
  }

  inline MultipleOption& addDescription(const std::string& description) {
    return static_cast<MultipleOption&>(Option::addDescription(description));
  }

  template <class... Ts>
  std::enable_if_t<(is_string_type<Ts> && ...), MultipleOption&>
  inline addNames(const Ts... names) {
    return static_cast<MultipleOption&>(Option::addNames(names...));
  }

  template <class T> inline MultipleOption&
  addConstraint(const std::function<bool(const T&)>& constraint,
    const std::string& error_message) {
    return static_cast<MultipleOption&>(Option::addConstraint(constraint, error_message));
  }

  inline MultipleOption& transformBeforeCheck(void) {
    return static_cast<MultipleOption&>(Option::transformBeforeCheck());
  }

  inline MultipleOption& beRequired(const bool& required = true) {
    return static_cast<MultipleOption&>(Option::beRequired(required));
  }
};

template <class T> MultipleOption&
MultipleOption::to(
  const std::function<T(const std::vector<std::string>&)>& transformation) {
  transformation_ = [transformation](const std::any& value) -> auto {
    return transformation(std::any_cast<std::vector<std::string>>(value));
  };
  return *this;
}

template <class T> MultipleOption&
MultipleOption::elementsTo(const std::function<T(const std::string&)>& transformation) {
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

#endif // _INPUT_OPTION_MULTIPLE_OPTION_HPP_