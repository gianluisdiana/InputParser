#ifndef _INPUT_LOCAL_CONCEPTS_HPP_
#define _INPUT_LOCAL_CONCEPTS_HPP_

#include <concepts>
#include <string>

namespace input_parser {

/**
 * @brief Checks if a type is a string (const char* is also considered a
 * string).
 */
template <typename T>
concept StringKind =
  std::same_as<T, std::string> || std::same_as<T, const char *>;

}  // namespace input_parser

#endif  // _INPUT_LOCAL_CONCEPTS_HPP_