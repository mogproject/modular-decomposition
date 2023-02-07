#include "util.hpp"
#include <cstdarg>

namespace util {
//==================================================================================================
// String Utilities
//==================================================================================================
/**
 * @brief Safer alternative to sprintf.
 *
 * @param fmt
 * @param ...
 * @return formatted string
 */
std::string format(char const* fmt, ...) {
  static std::string buffer;  // should have continuous memory space
  va_list marker;

  // first pass: find the length
  va_start(marker, fmt);
  auto length = std::snprintf(nullptr, 0, fmt, marker);
  va_end(marker);

  // second pass: create string
  char* str = new char[length + 1];
  va_start(marker, fmt);
  std::snprintf(str, length + 1, fmt, marker);
  va_end(marker);

  std::string ret(str);
  delete[] str;
  return ret;
}
}  // namespace util
