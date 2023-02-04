#include <vector>

namespace tww {
namespace ds {

class SortedVectorSet {
 private:
  std::vector<int> data_;

 public:
  SortedVectorSet(int capacity = 0) {}

  std::size_t size() const { return data_.size(); }

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
}  // namespace tww
