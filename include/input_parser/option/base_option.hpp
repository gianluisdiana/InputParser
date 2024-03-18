/**
 * @file base_option.hpp
 * @author Gian Luis Bolivar Diana (gianluisbolivar1@gmail.com)
 * @version 0.3
 * @date January 08, 2024
 * @copyright Copyright (c) 2024
 *
 * @brief File containing the description of a common option used by the
 * parser.
 *   An option can be:
 *  - A compound option: An option that requires at least one extra parameter.
 * It is represented by a vector of values of any type.
 *  - A flag: An option that does not require any extra parameter. It is
 * represented by a boolean value.
 *  - A single option: An option that requires one extra parameter. It is
 * represented by a value of any type.
 *
 */

#ifndef _INPUT_BASE_OPTION_HPP_
#define _INPUT_BASE_OPTION_HPP_

#include <stdexcept>

#include <input_parser/constraint.hpp>
#include <input_parser/local_type_traits.hpp>

namespace input_parser {

/** @brief A class that represents a command line option */
class BaseOption {
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
  BaseOption(const T name, const Ts... extra_names);

  // ------------------------------- Adders ------------------------------- //

  /**
   * @brief Assigns a default value to the option.
   *  It also makes the option not required.
   *
   * @param value The default value to assign to the option
   * @return The instance of the object that called this method
   */
  BaseOption &addDefaultValue(const std::any &value);

  /**
   * @brief Assigns a value to the description of the option
   *
   * @param description The new description
   * @return The instance of the object that called this method.
   */
  BaseOption &addDescription(const std::string &description);

  /**
   * @brief Adds a constraint to the option.
   *   This method must be used only if te value type is string or if the
   * transformationBeforeCheck method was called before.
   *
   * @tparam T The type of the value of the option
   * @param constraint A function that receives the value of the option and
   * returns a boolean indicating if the value is valid.
   * @param error_message The error message to be displayed if the constraint
   * fails.
   * @return The instance of the object that called this method.
   */
  template <class T>
  BaseOption &addConstraint(
    const std::function<bool(const T &)> &constraint,
    const std::string &error_message = ""
  );

  // ------------------------------- Getters ------------------------------- //

  /**
   * @brief Gets the value of the option.
   *   If the option has no value, the default value will be returned.
   *   If a transformation function was provided, the value will be transformed
   * before being returned.
   *
   * @tparam T The type of the value to be returned.
   * @return The value of the option casted to the specified type.
   */
  template <class T>
  const T getValue() const;

  /**
   * @brief Gets the default value of the option.
   *   If no default value was provided, an exception will be thrown.
   *
   * @tparam T The type of the value to be returned.
   * @return The default value of the option casted to the specified type.
   */
  template <class T>
  const T getDefaultValue() const;

  /** @brief Gets the names of the option */
  inline const std::vector<std::string> &getNames() const {
    return names_;
  }

  /** @brief Gets the description of the option */
  inline const std::string &getDescription() const {
    return description_;
  }

  // ------------------------------- Setters ------------------------------- //

  /**
   * @brief Sets the value of the option.
   * If the value does not satisfy the constraints, an exception will be thrown.
   * The value will be transformed if a transformation function was provided.
   *
   * @param value The value to set to the option
   */
  void setValue(const std::any &value);

  // ------------------------------- Checks ------------------------------- //

  /** @brief Checks if the option is a flag */
  virtual inline bool isFlag() const {
    return false;
  }

  /** @brief Checks if the option will require an extra parameter */
  virtual inline bool isSingle() const {
    return false;
  }

  /** @brief Checks if the option will require at least one extra parameter */
  virtual inline bool isCompound() const {
    return false;
  }

  /** @brief Checks if the option is required */
  inline bool isRequired() const {
    return required_;
  }

  /** @brief Checks if the option has a value defined */
  inline bool hasValue() const {
    return value_.has_value();
  }

  /** @brief Checks if the option has a default value defined */
  inline bool hasDefaultValue() const {
    return default_value_.has_value();
  }

  // ----------------------- Common transformations ----------------------- //

  /**
   * @brief Defines a transformation function that transforms the value of the
   * option to an integer. Must be implemented by the derived classes.
   *
   * @return The instance of the object that called this method.
   */
  virtual BaseOption &toInt() = 0;

  /**
   * @brief Defines a transformation function that transforms the value of the
   * option to a double. Must be implemented by the derived classes.
   *
   * @return The instance of the object that called this method.
   */
  virtual BaseOption &toDouble() = 0;

  /**
   * @brief Defines a transformation function that transforms the value of the
   * option to a float. Must be implemented by the derived classes.
   *
   * @return The instance of the object that called this method.
   */
  virtual BaseOption &toFloat() = 0;

  // ---------------------------- Other methods ---------------------------- //

  /**
   * @brief If a transformation function was provided, apply the function
   * before checking the constraints (at the moment of setting the value).
   *
   * @return The instance of the object that called this method.
   */
  BaseOption &transformBeforeCheck();

  /**
   * @brief Makes the option required or not.
   *
   * @param required Whether the option should be required or not. True by
   * default.
   * @return The instance of the object that called this method.
   */
  BaseOption &beRequired(const bool required = true);

 protected:
  // The value of the option
  std::any value_;
  // The default value of the option
  std::any default_value_;
  // All the names the option can be recognized by
  std::vector<std::string> names_;
  // Short explanation of what the option does
  std::string description_;
  // Indicates if the option is required
  bool required_;
  // Indicates if the transformation function should be applied before or after
  // the constraints
  bool transform_before_check_;
  // A function that transforms the value of the option
  std::function<std::any(const std::any &)> transformation_;
  // A list of constraints that the value of the option must satisfy
  std::vector<Constraint> constraints_;

  /**
   * @brief Applies the transformation function to the provided value
   * (if a transformation function was provided).
   *
   * @param value The value to transform
   * @return The transformed value (or the original value if no transformation
   * function was provided).
   */
  inline const std::any applyTransformation(const std::any &value) const {
    return transformation_ == nullptr ? value : transformation_(value);
  }

  /**
   * @brief Checks if the provided value satisfies all the constraints.
   *
   * @param value The value to check
   */
  void checkConstraints(const std::any &value) const;
};

template <
  typename T, typename... Ts,
  typename =
    typename std::enable_if_t<is_string_type<T> && (is_string_type<Ts> && ...)>>
BaseOption::BaseOption(const T name, const Ts... extra_names) :
  value_ {}, default_value_ {}, names_ {std::vector<std::string> {
                                  name, extra_names...}},
  description_ {}, required_ {true}, transform_before_check_ {false},
  transformation_ {nullptr}, constraints_ {} {}

template <class T>
BaseOption &BaseOption::addConstraint(
  const std::function<bool(const T &)> &constraint,
  const std::string &error_message
) {
  constraints_.emplace_back(
    [constraint](const std::any &value) -> bool {
      return constraint(std::any_cast<T>(value));
    },
    error_message
  );
  return *this;
}

template <class T>
const T BaseOption::getValue() const {
  if (!hasValue()) return getDefaultValue<T>();
  return std::any_cast<T>(value_);
}

template <class T>
const T BaseOption::getDefaultValue() const {
  if (!hasDefaultValue()) throw std::invalid_argument("No default value");
  return std::any_cast<T>(applyTransformation(default_value_));
}

}  // namespace input_parser

#endif  // _INPUT_BASE_OPTION_HPP_