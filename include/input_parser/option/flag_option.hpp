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

#ifndef INPUT_FLAG_OPTION_HPP_
#define INPUT_FLAG_OPTION_HPP_

#include <input_parser/option/base_option.hpp>

namespace input_parser {

/**
 *  @brief A flag option is an option that must be placed alone. Represents a
 * boolean value.
 *
 * @example
 *   <-d>
 *    └─> The name of the flag.
 * @tparam ValueType The type of the value inside the option.
 */
template <typename ValueType>
class FlagOption final : public BaseOption<ValueType, bool> {
 public:
  /**
   * @brief Constructs an empty option with the provided names.
   *
   * @tparam T Type of the mandatory name (must be strings or const char*)
   * @tparam Ts Types of the names (same type as T)
   * @param name The name of the option
   * @param extra_names Extra names that the option can be recognized by
   */
  explicit FlagOption(
    string_kind auto const name, string_kind auto const... extra_names
  ) : BaseOption<ValueType, bool>(name, extra_names...) {}

  /**
   * @brief Indicates if the option is a flag.
   *
   * @return True.
   */
  [[nodiscard]] bool isFlag() const override {
    return true;
  }

  // ------------------------ Static casted methods ------------------------ //

  FlagOption &addDefaultValue(const ValueType &value) {
    return dynamic_cast<FlagOption &>(
      BaseOption<ValueType, bool>::addDefaultValue(value)
    );
  }

  FlagOption &addDescription(const std::string &description) {
    return dynamic_cast<FlagOption &>(
      BaseOption<ValueType, bool>::addDescription(description)
    );
  }

  template <typename T>
  requires same_as_any<T, ValueType, bool>
  FlagOption &addConstraint(
    const std::function<bool(const T &)> &constraint,
    const std::string &error_message = ""
  ) {
    return dynamic_cast<FlagOption &>(
      BaseOption<ValueType, bool>::addConstraint(constraint, error_message)
    );
  }

  FlagOption &
  addTransformation(const std::function<ValueType(bool)> &transformation) {
    return dynamic_cast<FlagOption &>(
      BaseOption<ValueType, bool>::addTransformation(transformation)
    );
  }

  FlagOption &beRequired(const bool &required = true) {
    return dynamic_cast<FlagOption &>(
      BaseOption<ValueType, bool>::beRequired(required)
    );
  }
};

}  // namespace input_parser

#endif  // INPUT_FLAG_OPTION_HPP_