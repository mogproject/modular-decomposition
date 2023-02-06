#pragma once

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <cassert>

#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_CYAN "\x1b[36m"
#define ANSI_RESET "\x1b[0m"

//==================================================================================================
// Traces
//==================================================================================================
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#if TRACE_ON
// #define TRACE(format, ...) printf("%s(%d) " format "\n", __FUNCTION__, __LINE__, __VA_ARGS__)
#define TRACE(format, ...) \
  printf("%s%s(%d):%s " format "\n", ANSI_BLUE, __FILENAME__, __LINE__, ANSI_RESET, __VA_ARGS__);
#define TRACE0(s) printf("%s%s(%d):%s %s\n", ANSI_BLUE, __FILENAME__, __LINE__, ANSI_RESET, s);
#else
#define TRACE(format, ...)
#define TRACE0(s)
#endif

//==================================================================================================
// Type traits
//==================================================================================================
/**
 * @brief Check if the given type parameter is std::map or std::unordered_map.
 *
 * @tparam T type parameter to check
 */
template <typename T>
struct is_map : std::false_type {};

template <typename K, typename V>
struct is_map<std::map<K, V>> : std::true_type {};

template <typename K, typename V>
struct is_map<std::unordered_map<K, V>> : std::true_type {};

/**
 * @brief Check if the given type parameter is std::set or std::unordered_set.
 *
 * @tparam T type parameter to check
 */
template <typename T>
struct is_set : std::false_type {};

template <typename U>
struct is_set<std::set<U>> : std::true_type {};

template <typename U>
struct is_set<std::unordered_set<U>> : std::true_type {};

namespace util {
/**
 * @brief Safer alternative to sprintf.
 *
 * @param fmt
 * @param ...
 * @return formatted string
 */
std::string format(char const* fmt, ...);

//==================================================================================================
// STL Extensions (assuming SFINAE)
//==================================================================================================
/** Implementation specialized for map and set. */
template <typename T, typename U>
typename std::enable_if<is_map<T>::value || is_set<T>::value, bool>::type contains(T const& col, U const& x) {
  return col.find(x) != col.end();
}

/** Otherwise, use std::find to search for the element */
template <typename T, typename U>
typename std::enable_if<!(is_map<T>::value || is_set<T>::value), bool>::type contains(T const& col, U const& x) {
  return std::find(col.begin(), col.end(), x) != col.end();
}
}  // namespace util
