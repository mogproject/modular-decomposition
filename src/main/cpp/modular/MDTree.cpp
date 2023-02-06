#include "MDTree.hpp"

namespace modular {

std::ostream &operator<<(std::ostream &os, MDNode const &node) { return os << node.to_string(); }

}  // namespace modular
