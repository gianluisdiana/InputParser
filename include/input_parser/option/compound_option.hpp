/**
 * @file compound_option.hpp
 * @author Gian Luis Bolivar Diana (gianluisbolivar1@gmail.com)
 * @version 0.3
 * @date January 08, 2024
 * @copyright Copyright (c) 2024
 *
 * @brief File containing the description of a compound option used by the
 * parser, it can be describe as an option that requires at least one extra
 * parameter (it is represented by a vector of values of the template type).
 *
 */

#ifndef INPUT_COMPOUND_OPTION_HPP_
#define INPUT_COMPOUND_OPTION_HPP_

#include <input_parser/option/base_option.hpp>

namespace input_parser {

/**
 * @brief A compound option is an option that requires at least one more
 * argument.
 *
 * @example
 *   <-c> <chain1 chain2 chain3 ...>
 *    │            └─> The extra required arguments.
 *    └─> The name of the option.
 * @tparam ValueType The type of the values that the option will contain.
 */
template <typename ValueType>
class CompoundOption final :
  public BaseOption<ValueType, std::vector<std::string>> {
 public:
  /**
   * @brief Constructs an empty option with the provided names.
   *
   * @tparam T Type of the mandatory name (must be strings or const char*)
   * @tparam Ts Types of the names (same type as T)
   * @param name The name of the option
   * @param extra_names Extra names that the option can be recognized by
   */
  explicit CompoundOption(
    const string_kind auto name, const string_kind auto... extra_names
  ) : BaseOption<ValueType, std::vector<std::string>>(name, extra_names...) {
    argument_name_ = " value1 value2 ...";
  }

  /**
   * @brief Indicates if the option is a compound option.
   *
   * @return True.
   */
  [[nodiscard]] bool isCompound() const override {
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
  template <class T>
  CompoundOption &
  to(const std::function<T(const std::vector<std::string> &)> &transformation);

  /**
   * @brief Transform each option value using the provided function.
   * The function must take a const std::string& as argument and return the
   * type provided as template argument.
   *
   * @tparam T The type to transform the values to.
   * @param transformation The function that transforms the values of the option
   * @return The instance of the object that called this method.
   */
  CompoundOption &
  elementsTo(const std::function<ValueType(const std::string &)> &transformation
  );

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
  CompoundOption &toInt() override;

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
  CompoundOption &toDouble() override;

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
  CompoundOption &toFloat() override;

  // ---------------------- Dinamically casted methods ---------------------- //

  CompoundOption &addDefaultValue(const ValueType &value) {
    return dynamic_cast<CompoundOption &>(
      BaseOption<ValueType, std::vector<std::string>>::addDefaultValue(value)
    );
  }

  CompoundOption &addDescription(const std::string &description) {
    return dynamic_cast<CompoundOption &>(
      BaseOption<ValueType, std::vector<std::string>>::addDescription(
        description
      )
    );
  }

  template <typename T>
  requires same_as_any<T, ValueType, std::vector<std::string>>
  CompoundOption &addConstraint(
    const std::function<bool(const T &)> &constraint,
    const std::string &error_message = ""
  ) {
    return dynamic_cast<CompoundOption &>(
      BaseOption<ValueType, std::vector<std::string>>::addConstraint(
        constraint, error_message
      )
    );
  }

  CompoundOption &addTransformation(
    const std::function<ValueType(TransformationType)> &transformation
  ) {
    return dynamic_cast<CompoundOption &>(
      BaseOption<ValueType, std::vector<std::string>>::addTransformation(
        transformation
      )
    );
  }

  CompoundOption &beRequired(const bool &required = true) {
    return dynamic_cast<CompoundOption &>(
      BaseOption<ValueType, std::vector<std::string>>::beRequired(required)
    );
  }
};

template <typename ValueType>
CompoundOption<ValueType> &CompoundOption<ValueType>::elementsTo(
  const std::function<ValueType(const std::string &)> &transformation
) {
  return addTransformation([](const auto &values) {
    std::vector<ValueType> transformed_values(values.size());
    std::transform(
      values.begin(), values.end(), transformed_values.begin(), transformation
    );
    return transformed_values;
  });
}

template <typename ValueType>
CompoundOption<ValueType> &CompoundOption<ValueType>::toInt() {
  return elementsTo<int>([](const std::string &str) -> int {
    return std::stoi(str);
  });
}

template <typename ValueType>
CompoundOption<ValueType> &CompoundOption<ValueType>::toDouble() {
  return elementsTo<double>([](const std::string &str) -> double {
    return std::stod(str);
  });
}

template <typename ValueType>
CompoundOption<ValueType> &CompoundOption<ValueType>::toFloat() {
  return elementsTo<float>([](const std::string &str) -> float {
    return std::stof(str);
  });
}

}  // namespace input_parser

#endif  // INPUT_COMPOUND_OPTION_HPP_