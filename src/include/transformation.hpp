#ifndef _INPUT_TRANSFORMATION_HPP_
#define _INPUT_TRANSFORMATION_HPP_

#include <any>
#include <functional>
#include <string>
#include <vector>

namespace input {

/** @brief A collection of functions that transform values.*/
class Transformation {
 private:
  // Allow BaseOption to access private members
  friend class BaseOption;
  // -------------------- Single value transformations -------------------- //

  /**
   * @brief Transform a string to the type T using the given transformation.
   *
   * @tparam T The type to be transformed to.

   * @param value The value to be transformed. It must be a string.
   * @param transformation The transformation to be used.
   * @return The string transformed to the type T.
   */
  template <class T>
  static inline const std::any
  to(const std::any& value, const std::function<T(const std::string&)>& transformation) {
    return transformation(std::any_cast<std::string>(value));
  }

  /**
   * @brief Transform a vector of strings to the type T using the given
   * transformation.
   *
   * @tparam T The type to be transformed to.
   * @param value The value to be transformed. It must be a vector of strings.
   * @param transformation The transformation to be used.
   * @return The vector of strings transformed to the type T.
   */
  template <class T>
  static inline const std::any
  to(const std::any& value,
    const std::function<T(const std::vector<std::string>&)>& transformation) {
    return transformation(std::any_cast<std::vector<std::string>>(value));
  }

  /**
   * @brief Transforms a string to a integer.
   *
   * @param value The value to be transformed. It must be a string.
   * @return A integer.
   */
  static inline const std::any toInt(const std::any& value) {
    return to<int>(value, [](const std::string& str) -> int {
      return std::stoi(str);
    });
  }

  /**
   * @brief Transforms a string to a float.
   *
   * @param value The value to be transformed. It must be a string.
   * @return A float.
   */
  static inline const std::any toFloat(const std::any& value) {
    return to<float>(value, [](const std::string& str) -> float {
      return std::stof(str);
    });
  }

  /**
   * @brief Transforms a string to a double.
   *
   * @param value The value to be transformed. It must be a string.
   * @return A double.
   */
  static inline const std::any toDouble(const std::any& value) {
    return to<double>(value, [](const std::string& str) -> double {
      return std::stod(str);
    });
  }

  // ------------------- Multiple values transformations ------------------- //

  /**
   * @brief Transforma all the elements of a string vector to the type T using
   * the given transformation.
   *
   * @tparam T The type to be transformed to.
   * @param value The value to be transformed. It must be a vector of strings.
   * @param transformation The transformation to be used.
   * @return A vector of the type T.
   */
  template <class T>
  static const std::any elementsTo(
    const std::any& value, const std::function<T(const std::string&)>& transformation);

  /**
   * @brief Transforms all the elements of a string vector to integers.
   *
   * @param value The value to be transformed. It must be a vector of strings.
   * @return A vector of integers.
   */
  static const inline std::any allElementsToInt(const std::any& value) {
    return elementsTo<int>(value, [](const std::string& str) -> int {
      return std::stoi(str);
    });
  }

  /**
   * @brief Transforms all the elements of a string vector to floats.
   *
   * @param value The value to be transformed. It must be a vector of strings.
   * @return A vector of floats.
   */
  static const inline std::any allElementsToFloat(const std::any& value) {
    return elementsTo<float>(value, [](const std::string& str) -> float {
      return std::stof(str);
    });
  }

  /**
   * @brief Transforms all the elements of a string vector to doubles.
   *
   * @param value The value to be transformed. It must be a vector of strings.
   * @return A vector of doubles.
   */
  static const inline std::any allElementsToDouble(const std::any& value) {
    return elementsTo<double>(value, [](const std::string& str) -> double {
      return std::stod(str);
    });
  }
};

template <class T>
const std::any Transformation::elementsTo(
  const std::any& values, const std::function<T(const std::string&)>& transformation) {
  const auto string_values = std::any_cast<std::vector<std::string>>(values);
  std::vector<T> transformed_values;
  for (const auto& value : string_values) {
    transformed_values.push_back(transformation(value));
  }
  return transformed_values;
}

} // namespace input

#endif // _INPUT_TRANSFORMATION_HPP_