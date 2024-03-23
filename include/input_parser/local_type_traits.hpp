#ifndef _INPUT_LOCAL_TYPE_TRAITS_HPP_
#define _INPUT_LOCAL_TYPE_TRAITS_HPP_

#include <string>

namespace input_parser {

/**
 * @brief A type trait that checks if a type is a string (const char* is also
 * considered a string).
 * @tparam T The type to check
 */
template <class T>
constexpr const bool is_string_type =
  std::is_same_v<T, std::string> || std::is_same_v<T, const char *>;

}  // namespace input_parser

#endif  // _INPUT_LOCAL_TYPE_TRAITS_HPP_