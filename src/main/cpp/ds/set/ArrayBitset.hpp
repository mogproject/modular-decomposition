#pragma once
#undef _GLIBCXX_DEBUG                              // disable run-time bound checking, etc
#pragma GCC optimize("Ofast,inline,unroll-loops")  // Ofast = O3,fast-math,allow-store-data-races,no-protect-parens

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "ds/set/basic_set.hpp"
#include "util/util.hpp"

namespace ds {

template <typename DataType, int N, bool Validate>
class ArrayBitset : public basic_set<int> {
 private:
  std::size_t capacity_;
  DataType data_[N];

  typedef ArrayBitset<DataType, N, Validate> This;

  static constexpr std::size_t const B = sizeof(DataType) * 8;

  static constexpr inline int ctz(DataType x) {
    if (B == 128) {
      if (x & (-1ULL)) return __builtin_ctzll(x);
      return 64 + __builtin_ctzll(x >> (B / 2));
    }
    if (B == 64) return __builtin_ctzll(x);
    return __builtin_ctz(x);
  }
  static constexpr inline int popcount(DataType x) {
    if (B == 128) return __builtin_popcountll(x >> (B / 2)) + __builtin_popcountll(x);
    if (B == 64) return __builtin_popcountll(x);
    return __builtin_popcount(x);
  }

  static constexpr DataType const ONE = 1;

  inline void verify_argument_(int x, char const* label) const {
    if (Validate) {
      if (0 <= x && x < static_cast<int>(capacity_)) return;
      throw std::invalid_argument(util::format("ArrayBitset[%s]: x (%d) must be between 0 and %d", label, x, capacity_ - 1));
    }
  }

 public:
  ArrayBitset(std::size_t n = 0): capacity_(n) {
    if (n > B * N) throw std::invalid_argument("ArrayBitset: n too large");
    if (N > 0) std::memset(data_, 0, std::max(1UL, sizeof(data_)));
  }

  /**
   * Constructs a singleton.
   */
  ArrayBitset(std::size_t n, int x) : ArrayBitset(n) { *this |= x; }

  /**
   * Constructs from a list.
   */
  ArrayBitset(std::size_t n, std::vector<int> const& xs) : ArrayBitset(n) {
    for (int x : xs) *this |= x;
  }

  /**
   * Copy constructor.
   */
  ArrayBitset(This const& other) {
    capacity_ = other.capacity_;
    std::memcpy(data_, other.data_, sizeof(data_));
  }

  /**
   * Assignment operator.
   */
  void operator=(This const& other) {
    capacity_ = other.capacity_;
    std::memcpy(data_, other.data_, sizeof(data_));
  }

  void clear() {
    if (N > 0) std::memset(data_, 0, std::max(1UL, sizeof(data_)));
  }

  bool empty() const {
    for (std::size_t i = 0; i < N; ++i) {
      if (data_[i]) return false;
    }
    return true;
  }

  int front() const {
    int ret = -1;
    for (std::size_t i = 0; i < N; ++i) {
      if (data_[i]) {
        ret = i * B + ctz(data_[i]);
        break;
      }
    }
    return ret < static_cast<int>(capacity_) ? ret : -1;
  }

  int pop_front() {
    for (std::size_t i = 0; i < N; ++i) {
      if (data_[i]) {
        std::size_t offset = ctz(data_[i]);
        std::size_t ret = i * B + offset;
        if (ret < capacity_) {
          data_[i] ^= ONE << offset;
          return ret;
        }
      }
    }
    return -1;
  }

  int back() const {
    throw std::runtime_error("not implemented");
    return -1;
  }

  int pop_back() {
    throw std::runtime_error("not implemented");
    return -1;
  }

  //--------------------------------------------------------
  //    Operators
  //--------------------------------------------------------

  /**
   * negation (not)
   */
  This operator~() const {
    This ret(capacity_);
    for (std::size_t i = 0; i < N; ++i) ret.data_[i] = ~data_[i];
    // trim extra bits
    if (N > 0 && capacity_ % B) ret.data_[N - 1] &= (ONE << (capacity_ % B)) - 1;
    return ret;
  }

  /**
   * set/union (or)
   */
  This& operator|=(int x) {
    verify_argument_(x, "|=");

    data_[x / B] |= ONE << (x % B);
    return *this;
  }

  This& operator|=(This const& rhs) {
    if (capacity_ != rhs.capacity_) throw std::invalid_argument("inconsistent size");

    for (std::size_t i = 0; i < N; ++i) data_[i] |= rhs.data_[i];
    return *this;
  }

  /**
   * exclusive or (xor)
   */
  This& operator^=(int x) {
    verify_argument_(x, "^=");

    data_[x / B] ^= ONE << (x % B);
    return *this;
  }

  This& operator^=(This const& rhs) {
    if (capacity_ != rhs.capacity_) throw std::invalid_argument("inconsistent size");

    for (int i = 0; i < N; ++i) data_[i] ^= rhs.data_[i];
    return *this;
  }

  /**
   * intersection (and)
   */
  This operator&=(int x) {
    verify_argument_(x, "&=");

    for (int i = 0; i < N; ++i) {
      if (i == x / B) {
        data_[x / B] &= ONE << (x % B);
      } else {
        data_[i] = 0;
      }
    }

    return *this;
  }

  This& operator&=(This const& rhs) {
    if (capacity_ != rhs.capacity_) throw std::invalid_argument("inconsistent size");

    for (int i = 0; i < N; ++i) data_[i] &= rhs.data_[i];
    return *this;
  }

  /**
   * reset/set minus
   */
  This& operator-=(int x) {
    verify_argument_(x, "-=");

    data_[x / B] &= ~(ONE << (x % B));
    return *this;
  }

  This& operator-=(This const& rhs) {
    if (capacity_ != rhs.capacity_) throw std::invalid_argument("inconsistent size");

    for (int i = 0; i < N; ++i) data_[i] &= ~rhs.data_[i];
    return *this;
  }

  // clang-format off
  friend This operator|(This const& lhs, int x) { ArrayBitset ret(lhs); ret |= x; return ret; }
  friend This operator|(This const& lhs, This const& rhs) { ArrayBitset ret(lhs); ret |= rhs; return ret; }
  friend This operator^(This const& lhs, int x) { ArrayBitset ret(lhs); ret ^= x; return ret; }
  friend This operator^(This const& lhs, This const& rhs) { ArrayBitset ret(lhs); ret ^= rhs; return ret; }
  friend This operator&(This const& lhs, int x) { ArrayBitset ret(lhs); ret &= x; return ret; }
  friend This operator&(This const& lhs, This const& rhs) { ArrayBitset ret(lhs); ret &= rhs; return ret; }
  friend This operator-(This const& lhs, int x) { ArrayBitset ret(lhs); ret -= x; return ret; }
  friend This operator-(This const& lhs, This const& rhs) { ArrayBitset ret(lhs); ret -= rhs; return ret; }
  // clang-format on

  /**
   * get
   */
  bool operator[](int x) const {
    verify_argument_(x, "[]");
    return (data_[x / B] >> (x % B)) & ONE;
  }

  std::vector<int> to_vector() const {
    std::vector<int> ret;
    for (std::size_t i = 0; i < N; ++i) {
      for (auto x = data_[i]; x; x &= x - 1) ret.push_back(i * B + ctz(x));
    }
    return ret;
  }

  /**
   * equality
   */
  friend inline bool operator==(This const& lhs, This const& rhs) {
    if (lhs.capacity() != rhs.capacity()) return false;
    for (std::size_t i = 0; i < N; ++i) {
      if (lhs.data_[i] != rhs.data_[i]) return false;
    }
    return true;
  }

  friend inline bool operator!=(This const& lhs, This const& rhs) { return !(lhs == rhs); }

  /**
   * @brief Returns the number of 1-bits.
   */
  std::size_t size() const {
    std::size_t ret = 0;
    for (std::size_t i = 0; i < N; ++i) ret += popcount(data_[i]);
    return ret;
  }

  int capacity() const { return capacity_; }

  inline bool subset(This const& rhs) const { return (*this & rhs) == *this; }

  inline bool superset(This const& rhs) const { return rhs.subset(*this); }

  //--------------------------------------------------------
  //    Aliases
  //--------------------------------------------------------
  void set(int x) { *this |= x; }
  void reset(int x) { *this -= x; }
  bool get(int x) const { return (*this)[x]; }
  static This intersect(This const& s, This const& t) { return s & t; }
  static This Union(This const& s, This const& t) { return s | t; }
};

typedef ArrayBitset<uint64_t, 1, false> ArrayBitset6;         // supports up to 0 <= x < 64
typedef ArrayBitset<uint64_t, 1 << 1, false> ArrayBitset7;    // supports up to 0 <= x < 128
typedef ArrayBitset<uint64_t, 1 << 2, false> ArrayBitset8;    // supports up to 0 <= x < 256
typedef ArrayBitset<uint64_t, 1 << 3, false> ArrayBitset9;    // supports up to 0 <= x < 512
typedef ArrayBitset<uint64_t, 1 << 4, false> ArrayBitset10;   // supports up to 0 <= x < 1024
typedef ArrayBitset<uint64_t, 1 << 5, false> ArrayBitset11;   // supports up to 0 <= x < 2048
typedef ArrayBitset<uint64_t, 1 << 6, false> ArrayBitset12;   // supports up to 0 <= x < 4096
typedef ArrayBitset<uint64_t, 1 << 7, false> ArrayBitset13;   // supports up to 0 <= x < 8192
typedef ArrayBitset<uint64_t, 1 << 8, false> ArrayBitset14;   // supports up to 0 <= x < 16384
typedef ArrayBitset<uint64_t, 1 << 9, false> ArrayBitset15;   // supports up to 0 <= x < 32768
typedef ArrayBitset<uint64_t, 1 << 10, false> ArrayBitset16;  // supports up to 0 <= x < 65536
typedef ArrayBitset<uint64_t, 1 << 11, false> ArrayBitset17;  // supports up to 0 <= x < 131072

}  // namespace ds
