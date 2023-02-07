#include "FastSet.hpp"

#include <algorithm>

namespace ds {

/**
 * @brief Fixed-size, constant-time integer set representation.
 */
FastSet::FastSet(std::size_t size) : generation_(0) { data_.resize(size, -1); }

std::size_t FastSet::capacity() const { return data_.size(); }

std::size_t FastSet::size() const { return std::count(data_.begin(), data_.end(), generation_); }

/**
 * @brief Clears the set
 *
 * O(1)
 */
void FastSet::clear() {
  ++generation_;
  if (generation_ == 0) {
    auto sz = data_.size();
    data_.clear();
    data_.resize(sz, -1);
  }
}

/**
 * @brief Resizes the capacity of the set.
 *
 * O(size)
 */
void FastSet::resize(std::size_t size) {
  generation_ = 0;
  data_.clear();
  data_.resize(size, -1);
}

/**
 * @brief Inserts one element to the set.
 *
 * O(1)
 */
void FastSet::set(int x) {
  if (x < 0 || static_cast<int>(data_.size()) <= x) throw std::invalid_argument("fast_set::set(): out of range");
  data_[x] = generation_;
}

/**
 * @brief Removes one element from the set.
 *
 * O(1)
 */
void FastSet::reset(int x) {
  if (x < 0 || static_cast<int>(data_.size()) <= x) throw std::invalid_argument("fast_set::reset(): out of range");
  data_[x] = -1;
}

/**
 * @brief Checks if the given element is in the set.
 *
 * O(1)
 */
bool FastSet::get(int x) const { return 0 <= x && x < static_cast<int>(data_.size()) && data_[x] == generation_; }

}  // namespace ds
