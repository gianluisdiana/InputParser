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

#ifndef INPUT_BASE_OPTION_HPP_
#define INPUT_BASE_OPTION_HPP_

#include <stdexcept>

#include <input_parser/constraint.hpp>
#include <input_parser/local_concepts.hpp>
#include <input_parser/parsing_error.hpp>

namespace input_parser {

/**
 * @brief A class that represents a basic command line option.
 * @details A basic option is composed of:
 *
 * - (At least) one name that the option can be recognized by.
 * - A description of what the option does.
 * - A value that the option can have.
 * - A default value, used when the value is not provided.
 * - A list of constraints that the value must satisfy.
 * - A list of constraints that the value must satisfy before being transformed.
 * - A transformation function that transforms the value of the option.
 *
 * @tparam ValueType The type of the value in the option.
 * @tparam TransformationType The type of the value before being transformed.
 * */
template <typename ValueType, typename TransformationType>
class BaseOption {
 public:
  /**
   * @brief Constructs an empty option with the provided names.
   *
   * @param name The name of the option
   * @param extra_names Extra names that the option can be recognized by
   */
  explicit BaseOption(string_kind auto name, string_kind auto... extra_names);

  /** @brief Virtual destructor */
  virtual ~BaseOption() = default;

  // -------------------------------- Adders -------------------------------- //

  /**
   * @brief Assigns a default value to the option.
   *  It also makes the option not required.
   *
   * @param default_value The default value to assign to the option
   * @return The instance of the object that called this method
   */
  BaseOption &addDefaultValue(const ValueType &default_value);

  /**
   * @brief Assigns a description of the option.
   * The description is a short explanation of what the option does.
   *
   * @param description The new description
   * @return The instance of the object that called this method.
   */
  BaseOption &addDescription(const std::string &description);

  /**
   * @brief Adds a constraint to the option.
   *
   * If the type of the parameter in the callback function is the same as the
   * one provided as ValueType, the constraint will be applied after the
   * transformation function (if provided). Otherwise, the constraint will be
   * applied before the transformation function.
   *
   * In the case that the ValueType and the TransformationType are the same,
   * the constraint will be applied after the transformation function.
   * @see setValue(TransformationType)
   *
   * @tparam T The type of the value of the option
   * @param constraint A function that receives the value of the option and
   * returns a boolean indicating if the value is valid.
   * @param error_message The error message to be displayed if the constraint
   * fails.
   * @return The instance of the object that called this method.
   */
  template <typename T>
  requires same_as_any<T, ValueType, TransformationType>
  BaseOption &addConstraint(
    const std::function<bool(const T &)> &constraint,
    const std::string &error_message = ""
  );

  // TODO: CHANGE TYPENAME TO DEDUCE TYPE LIKE IN THESIS

  /**
   * @brief Adds a transformation function to the option.
   *   The transformation function will be applied to the value of the option
   * when a new value is set.
   * @see SetValue(TransformationType)
   *
   * @param transformation A function that transforms the value of the option.
   * @return The instance of the object that called this method.
   */
  BaseOption &addTransformation(
    const std::function<ValueType(TransformationType)> &transformation
  );

  // ------------------------------- Getters ------------------------------- //

  /**
   * @brief Gets the option's value.
   *   If the option has no value, the default value will be returned.
   *   If a transformation function was provided, the value (but not the
   * default one) will be transformed before being returned.
   *
   * @return If the option has a value, the value of the option. Otherwise, the
   * default value of the option.
   */
  [[nodiscard]] ValueType getValue() const;

  /**
   * @brief Gets the option's default value.
   *   If no default value was provided, an exception will be thrown.
   *
   * @return The default value of the option.
   */
  [[nodiscard]] ValueType getDefaultValue() const;

  /** @brief Gets the names of the option */
  [[nodiscard]] const std::vector<std::string> &getNames() const {
    return names_;
  }

  /** @brief Gets the description of the option */
  [[nodiscard]] const std::string &getDescription() const {
    return description_;
  }

  /** @brief Gets the argument placeholder of the option (if needed). */
  [[nodiscard]] const std::string &getArgumentName() const {
    return argument_name_;
  }

  // ------------------------------- Setters ------------------------------- //

  /**
   * @brief Sets the value of the option.
   *   If the value does not satisfy the constraints, an exception will be
   * thrown.
   *   The value will be transformed and then checked against the constraints.
   *   If the transformation function was not provided and the
   * TransformationType is not the same as the ValueType, an exception will be
   * thrown.
   *
   * @param value The value to set to the option
   */
  void setValue(const TransformationType &value);

  // -------------------------------- Checks -------------------------------- //

  /** @brief Checks if the option is a flag */
  [[nodiscard]] virtual bool isFlag() const {
    return false;
  }

  /** @brief Checks if the option will require an extra parameter */
  [[nodiscard]] virtual bool isSingle() const {
    return false;
  }

  /** @brief Checks if the option will require at least one extra parameter */
  [[nodiscard]] virtual bool isCompound() const {
    return false;
  }

  /** @brief Checks if the option is required */
  [[nodiscard]] bool isRequired() const {
    return required_;
  }

  /** @brief Checks if the option has a value defined */
  [[nodiscard]] bool hasValue() const {
    return has_value_;
  }

  /** @brief Checks if the option has a default value defined */
  [[nodiscard]] bool hasDefaultValue() const {
    return has_default_value_;
  }

  // ------------------------ Common transformations ------------------------ //

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
   * @brief Makes the option required or not.
   *
   * @param required Whether the option should be required or not. True by
   * default.
   * @return The instance of the object that called this method.
   */
  BaseOption &beRequired(bool required = true);

 private:
  // ------------------------------ Attributes ------------------------------ //

  // The value of the option.
  ValueType value_;
  // The default value of the option.
  ValueType default_value_;
  // All the names the option can be recognized by.
  std::vector<std::string> names_;
  // Short explanation of what the option does.
  std::string description_;
  // Indicates if the option is required.
  bool required_ {true};
  // A function that transforms the value of the option.
  std::function<ValueType(const TransformationType &)> transformation_;
  // A list of constraints that the value of the option must satisfy.
  std::vector<Constraint<ValueType>> constraints_;
  // A list of constraints that the value of the option must satisfy before
  // being transformed.
  std::vector<Constraint<TransformationType>>
    constraints_before_transformation_;
  // The placeholder for the argument of the option.
  std::string argument_name_;
  // Indicates if the option has a value defined.
  bool has_value_ {false};
  // Indicates if the option has a default value defined.
  bool has_default_value_ {false};

  // --------------------------- Private methods --------------------------- //

  /**
   * @brief Checks if the provided value satisfies all the constraints.
   *
   * @tparam T The type of the value to check (and the type the constrainst
   * takes as argument).
   * @param value The value to check.
   * @param constraints The constraints to check.
   */
  template <typename T>
  requires same_as_any<T, ValueType, TransformationType>
  void checkConstraints(
    const T &value, const std::vector<Constraint<T>> &constraints
  ) const;
};

template <typename VT, typename TT>
BaseOption<VT, TT>::BaseOption(
  string_kind auto name, string_kind auto... extra_names
) : names_ {name, extra_names...} {}

// --------------------------------- Adders --------------------------------- //

template <typename VT, typename TT>
BaseOption<VT, TT> &
BaseOption<VT, TT>::addDefaultValue(const VT &default_value) {
  default_value_ = default_value;
  has_default_value_ = true;
  return beRequired(false);
}

template <typename VT, typename TT>
BaseOption<VT, TT> &
BaseOption<VT, TT>::addDescription(const std::string &description) {
  description_ = description;
  return *this;
}

template <typename VT, typename TT>
template <typename T>
requires same_as_any<T, VT, TT>
BaseOption<VT, TT> &BaseOption<VT, TT>::addConstraint(
  const std::function<bool(const T &)> &constraint,
  const std::string &error_message
) {
  if constexpr (std::is_same_v<T, VT>) {
    constraints_.emplace_back(constraint, error_message);
  } else {
    constraints_before_transformation_.emplace_back(constraint, error_message);
  }
  return *this;
}

template <typename VT, typename TT>
BaseOption<VT, TT> &BaseOption<VT, TT>::addTransformation(
  const std::function<VT(TT)> &transformation
) {
  transformation_ = transformation;
  return *this;
}

// --------------------------------- Getters -------------------------------- //

template <typename VT, typename TT>
VT BaseOption<VT, TT>::getValue() const {
  if (!hasValue()) { return getDefaultValue(); }
  return value_;
}

template <typename VT, typename TT>
VT BaseOption<VT, TT>::getDefaultValue() const {
  if (!hasDefaultValue()) { throw std::invalid_argument("No default value"); }
  return default_value_;
}

// --------------------------------- Setters -------------------------------- //

template <typename VT, typename TT>
void BaseOption<VT, TT>::setValue(const TT &value) {
  if constexpr (std::is_same_v<VT, TT>) {
    value_ = value;
  } else if (transformation_ != nullptr) {
    checkConstraints(value, constraints_before_transformation_);
    value_ = transformation_(value);
  } else {
    throw std::invalid_argument("No transformation function provided.");
  }
  checkConstraints(value_, constraints_);
  has_value_ = true;
}

// ----------------------------- Other methods ----------------------------- //

template <typename VT, typename TT>
BaseOption<VT, TT> &BaseOption<VT, TT>::beRequired(const bool required) {
  required_ = required;
  return *this;
}

template <typename VT, typename TT>
template <typename T>
requires same_as_any<T, VT, TT>
void BaseOption<VT, TT>::checkConstraints(
  const T &value, const std::vector<Constraint<T>> &constraints
) const {
  for (const Constraint<T> &constraint : constraints) {
    if (!constraint.call(value)) {
      const std::string &error_message = constraint.getErrorMessage();
      throw ParsingError(
        error_message.empty() ? "Constraint not satisfied." : error_message
      );
    }
  }
}

}  // namespace input_parser

#endif  // INPUT_BASE_OPTION_HPP_