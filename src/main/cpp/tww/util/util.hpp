#pragma once

#include <string>

namespace tww {
namespace util {
/**
 * @brief Safer alternative to sprintf.
 *
 * @param fmt
 * @param ...
 * @return formatted string
 */
std::string format(char const* fmt, ...);
}  // namespace util
}  // namespace tww
