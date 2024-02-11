#pragma once

#include <vector>

namespace ds {
template <typename T>
class basic_set {
 public:
  virtual std::size_t size() const = 0;
  virtual int capacity() const = 0;  // -1: unbounded
  virtual void set(T x) = 0;
  virtual void reset(T x) = 0;
  virtual bool get(T x) const = 0;
  virtual bool empty() const = 0;
  virtual void clear() = 0;
  virtual std::vector<T> to_vector() const = 0;
  virtual int front() const = 0;  // -1: not found
  virtual int pop_front() = 0;  // -1: not found
  virtual int back() const = 0;  // -1: not found
  virtual int pop_back() = 0;  // -1: not found
  virtual ~basic_set() = default;
};
}  // namespace ds
