#include "Bitmap.hpp"

namespace ds {

std::ostream& operator<<(std::ostream& os, Bitmap const& bm) {
  os << "Bitmap({";
  int i = 0;
  for (auto x : bm.to_vector()) {
    if (i++ > 0) os << ",";
    os << x;
  }
  return os << "})";
}

}  // namespace ds
