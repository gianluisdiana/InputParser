/**
 * @file parser.hpp
 * @author Gian Luis Bolivar Diana (gianluisbolivar1@gmail.com)
 * @version 0.1
 * @date November 27, 2023
 * @copyright Copyright (c) 2023
 *
 * @brief File containing the description of a common option used by the
 * parser.
 *   An option can be:
 *  - A flag: An option that does not require any extra parameter. It is
 * represented by a boolean value.
 *  - A single option: An option that requires one extra parameter. It is
 * represented by a value of any type.
 *  - A multiple option: An option that requires at least one extra parameter.
 * It is represented by a vector of values of any type.
 *
 */

#ifndef _INPUT_OPTION_HPP_
#define _INPUT_OPTION_HPP_

#include "./transformation.hpp"
#include "./constraint.hpp"

namespace input {

/**
 * @brief A type trait that checks if a type is a string (const char* is also
 * considered a string).
 * @tparam T The type to check
 */
template <class T>
constexpr const bool
is_string_type = std::is_same_v<T, std::string> || std::is_same_v<T, const char*>;

/** @brief The type of an option */
enum class OptionType {
  /**
   *  @brief A flag option is an option that must be placed alone. Represents a
   * boolean value.
   *
   * @example
   *   <-d>
   *    └─> The name of the flag.
   */
  Flag,
  /**
   * @brief A single option is an option that requires one more argument.
   *
   * @example
   *  <-f> <file_name>
   *    │       └─> The extra required argument.
   *    └─> The name of the option.
   */
  Single,
  /**
   * @brief A multiple option is an option that requires at least one more
   * argument.
   *
   * @example
   *   <-c> <chain1 chain2 chain3 ...>
   *    │            └─> The extra required arguments.
   *    └─> The name of the option.
   */
  Multiple
};

/** @brief A class that represents a command line option */
class Option {
 public:
  /**
   * @brief Constructs an empty option
   *
   * @param type The type of the option
   */
  Option(const OptionType& type);

  // ------------------------------- Adders ------------------------------- //

  /**
   * @brief Assigns a default value to the option.
   *  It also makes the option not required.
   *
   * @param value The default value to assign to the option
   * @return The instance of the object that called this method
   */
  Option& addDefaultValue(const std::any& value);

  /**
   * @brief Assigns a list of names to the option
   *
   * @tparam Ts Types of the names (must be strings or const char*)
   * @param names The names to assign to the option
   * @return The instance of the object that called this method.
   */
  template <class... Ts>
  std::enable_if_t<(is_string_type<Ts> && ...), Option&>
  addNames(const Ts... names);

  /**
   * @brief Assigns a value to the description of the option
   *
   * @param description The new description
   * @return The instance of the object that called this method.
   */
  Option& addDescription(const std::string& description);

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
  Option& addConstraint(const std::function<bool(const T&)>& constraint,
    const std::string& error_message = "");

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
  const T getValue(void) const;

  /** @brief Gets the names of the option */
  inline const std::vector<std::string>& getNames(void) const {
    return names_;
  }

  /** @brief Gets the description of the option */
  inline const std::string& getDescription(void) const {
    return description_;
  }

  /**
   * @brief Sets the value of the option.
   * If the value does not satisfy the constraints, an exception will be thrown.
   * The value will be transformed if a transformation function was provided.
   *
   * @param value The value to set to the option
   */
  void setValue(const std::any& value);

  // ------------------------------- Checks ------------------------------- //

  /** @brief Checks if the option is a flag */
  inline bool isFlag() const {
    return type_ == OptionType::Flag;
  }

  /** @brief Checks if the option will require an extra parameter */
  inline bool isSingle() const {
    return type_ == OptionType::Single;
  }

  /** @brief Checks if the option will require at least one extra parameter */
  inline bool isMultiple() const {
    return type_ == OptionType::Multiple;
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
   * @brief If the option is a single option, transforms the value of the
   * option to an integer. If the option is a multiple option, transforms all
   * the values of the option to integers.
   *
   * @return The instance of the object that called this method.
   */
  Option& toInt(void);

  /**
   * @brief If the option is a single option, transforms the value of the
   * option to an double. If the option is a multiple option, transforms all
   * the values of the option to doubles.
   *
   * @return The instance of the object that called this method.
   */
  Option& toDouble(void);

  /**
   * @brief If the option is a single option, transforms the value of the
   * option to an float. If the option is a multiple option, transforms all
   * the values of the option to floats.
   *
   * @return The instance of the object that called this method.
   */
  Option& toFloat(void);

  /**
   * @brief Transforms the value of the option using the provided function.
   * The function must take a const std::string& as argument and return the
   * type provided as template argument.
   *
   * Only works for single options.
   *
   * @tparam T The type to transform the value to.
   * @param transformation The function that transforms the value of the option
   * @return The instance of the object that called this method.
   */
  template <class T>
  Option& to(const std::function<T(const std::string&)>& transformation);

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
  template <class T>
  Option& to(const std::function<T(const std::vector<std::string>&)>& transformation);

  /**
   * @brief Transform each option value using the provided function.
   * The function must take a const std::string& as argument and return the
   * type provided as template argument.
   *
   * @tparam T The type to transform the values to.
   * @param transformation The function that transforms the values of the option
   * @return The instance of the object that called this method.
   */
  template <class T>
  Option& elementsTo(const std::function<T(const std::string&)>& transformation);

  // ---------------------------- Other methods ---------------------------- //

  /**
   * @brief If a transformation function was provided, apply the function
   * before checking the constraints (at the moment of setting the value).
   *
   * @return The instance of the object that called this method.
   */
  Option& transformBeforeCheck(void);

  /**
   * @brief Makes the option required or not.
   *
   * @param required Whether the option should be required or not. True by
   * default.
   * @return The instance of the object that called this method.
   */
  Option& beRequired(const bool required = true);

 private:
  // The type of the option
  const OptionType type_;
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
  std::function<std::any(const std::any&)> transformation_;
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
  inline const std::any applyTransformation(const std::any& value) {
    return transformation_ == nullptr ? value : transformation_(value);
  }

  /**
   * @brief Checks if the provided value satisfies all the constraints.
   *
   * @param value The value to check
   */
  void checkConstraints(const std::any& value) const;

  /**
   * @brief Depending on the type of the option, converts the value to a
   * string.
   *  If the option is multiple, the values will be separated by a space.
   *
   * @param value The value to convert
   * @return The string representation of the value
   */
  const std::string valueToString(const std::any& value) const;
};

template <class... Ts>
std::enable_if_t<(is_string_type<Ts> && ...), Option&>
Option::addNames(const Ts... names) {
  names_ = std::vector<std::string>{names...};
  return *this;
}

template <class T>
Option& Option::addConstraint(const std::function<bool(const T&)>& constraint,
  const std::string& error_message) {
  constraints_.emplace_back([constraint](const std::any& value) -> bool {
    return constraint(std::any_cast<T>(value));
  }, error_message);
  return *this;
}

template <class T>
const T Option::getValue(void) const {
  if (!hasValue()) return std::any_cast<T>(default_value_);
  return std::any_cast<T>(value_);
}

template <class T>
Option& Option::to(const std::function<T(const std::string&)>& transformation) {
  transformation_ = [transformation](const std::any& value) -> auto {
    return Transformation::to<T>(value, transformation);
  };
  return *this;
}

template <class T>
Option& Option::to(
  const std::function<T(const std::vector<std::string>&)>& transformation) {
  transformation_ = [transformation](const std::any& value) -> auto {
    return Transformation::to<T>(value, transformation);
  };
  return *this;
}

template <class T>
Option& Option::elementsTo(const std::function<T(const std::string&)>& transformation) {
  if (isMultiple()) {
    transformation_ = [transformation](const std::any& value) -> auto {
      return Transformation::elementsTo<T>(value, transformation);
    };
  } else {
    transformation_ = [transformation](const std::any& value) -> auto {
      return Transformation::to<T>(value, transformation);
    };
  }
  return *this;
}

} // namespace input

#endif // _INPUT_OPTION_HPP_