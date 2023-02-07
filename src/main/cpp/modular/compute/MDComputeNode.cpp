#include "MDComputeNode.hpp"

namespace modular {
namespace compute {

std::ostream &operator<<(std::ostream &os, MDComputeNode const &node) { return os << node.to_string(); }

}  // namespace compute
}  // namespace modular
