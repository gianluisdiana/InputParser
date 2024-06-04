#ifndef INPUT_LOCAL_CONCEPTS_HPP_
#define INPUT_LOCAL_CONCEPTS_HPP_

#include <concepts>
#include <string>

namespace input_parser {

/**
 * @brief A concept that checks if the type is the same as any of the provided
 * types.
 * @tparam Type The type to check.
 * @tparam Types The types to compare with.
 */
template <typename Type, typename... Types>
concept same_as_any = (... || std::is_same_v<Type, Types>);

/**
 * @brief Checks if a type is a string (const char* is also considered a
 * string).
 * @tparam T The type to check.
 */
template <typename T>
concept string_kind = same_as_any<T, std::string, const char *>;

}  // namespace input_parser

#endif  // INPUT_LOCAL_CONCEPTS_HPP_