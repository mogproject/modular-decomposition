#pragma once

#include <vector>
#include <algorithm>

#include "ds/set/basic_set.hpp"

namespace ds {

class SortedVectorSet : public basic_set<int> {
 private:
  std::vector<int> data_;

 public:
  SortedVectorSet() {}

  std::size_t size() const { return data_.size(); }

  int capacity() const { return -1; }

  void set(int x) {
    auto it = std::lower_bound(data_.begin(), data_.end(), x);
    if (it == data_.end() || *it != x) data_.insert(it, x);
  }

  void reset(int x) {
    auto it = std::lower_bound(data_.begin(), data_.end(), x);
    if (it != data_.end() && *it == x) data_.erase(it);
  }

  bool get(int x) const {
    auto it = std::lower_bound(data_.begin(), data_.end(), x);
    return (it != data_.end() && *it == x);
  }

  bool empty() const { return data_.empty(); }

  void clear() { data_.clear(); }

  std::vector<int> to_vector() const { return data_; }

  int front() const { return empty() ? -1 : data_.front(); }

  int pop_front() {
    if (empty()) return -1;
    int ret = data_.front();
    data_.erase(data_.begin());
    return ret;
  }

  int back() const { return empty() ? -1 : data_.back(); }

  int pop_back() {
    if (empty()) return -1;
    int ret = data_.back();
    data_.pop_back();
    return ret;
  }

  static SortedVectorSet intersect(SortedVectorSet const& s, SortedVectorSet const& t) {
    SortedVectorSet result;
    std::set_intersection(s.data_.begin(), s.data_.end(), t.data_.begin(), t.data_.end(), std::back_inserter(result.data_));
    return result;
  }

  static SortedVectorSet Union(SortedVectorSet const& s, SortedVectorSet const& t) {
    SortedVectorSet result;
    std::set_union(s.data_.begin(), s.data_.end(), t.data_.begin(), t.data_.end(), std::back_inserter(result.data_));
    return result;
  }
};

}  // namespace ds
