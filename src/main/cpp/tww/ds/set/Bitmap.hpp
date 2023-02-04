#pragma once
#undef _GLIBCXX_DEBUG                              // disable run-time bound checking, etc
#pragma GCC optimize("Ofast,inline,unroll-loops")  // Ofast = O3,fast-math,allow-store-data-races,no-protect-parens

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "tww/util/util.hpp"

namespace tww {
namespace ds {

/**
 * Bitmap of variable length.
 */
class Bitmap {
 private:
  // typedef unsigned int data_type;
  typedef unsigned long long data_type;  // best for general purposes
  // typedef __uint128_t data_type;
  static constexpr bool const validation_enabled = false;

  static constexpr int const B = sizeof(data_type) * 8;
  static constexpr inline int ctz(data_type x) {
    if (B == 128) {
      if (x & (-1ULL)) return __builtin_ctzll(x);
      return 64 + __builtin_ctzll(x >> (B / 2));
    }
    if (B == 64) return __builtin_ctzll(x);
    return __builtin_ctz(x);
  }
  static constexpr inline int popcount(data_type x) {
    if (B == 128) return __builtin_popcountll(x >> (B / 2)) + __builtin_popcountll(x);
    if (B == 64) return __builtin_popcountll(x);
    return __builtin_popcount(x);
  }

  static constexpr data_type const ONE = 1;

  std::size_t n_;
  std::vector<data_type> data_;

  inline void verify_argument_(int x, char const* label) const {
    if (validation_enabled) {
      if (0 <= x && x < static_cast<int>(capacity())) return;
      throw std::invalid_argument(util::format("Bitmap[%s]: x (%d) must be between 0 and %d", label, x, capacity() - 1));
    }
  }

 public:
  Bitmap(std::size_t n = 0) : n_(n), data_((n + (B - 1)) / B) {}

  /**
   * Constructs a singleton.
   */
  Bitmap(std::size_t n, int x) : Bitmap(n) { *this |= x; }
  /**
   * Constructs from a list.
   */
  Bitmap(std::size_t n, std::vector<int> const& xs) : Bitmap(n) {
    for (int x : xs) *this |= x;
  }

  /**
   * Copy constructor.
   */
  Bitmap(Bitmap const& other) {
    this->n_ = other.n_;
    this->data_ = other.data_;
  }

  void operator=(Bitmap const& other) {
    n_ = other.n_;
    data_ = other.data_;
  }

  inline std::size_t capacity() const { return n_; }

  inline void clear() {
    for (std::size_t i = 0; i < data_.size(); ++i) data_[i] = 0;
  }

  inline bool empty() const {
    for (auto d : data_) {
      if (d) return false;
    }
    return true;
  }

  inline int front() const {
    int ret = -1;
    for (std::size_t i = 0; i < data_.size(); ++i) {
      if (data_[i]) {
        ret = i * B + ctz(data_[i]);
        break;
      }
    }
    return ret < static_cast<int>(capacity()) ? ret : -1;
  }

  inline int pop_front() {
    for (std::size_t i = 0; i < data_.size(); ++i)
      if (data_[i]) {
        int offset = ctz(data_[i]);
        int ret = i * B + offset;
        if (ret < static_cast<int>(capacity())) {
          data_[i] ^= ONE << offset;
          return ret;
        }
      }
    return -1;
  }

  //--------------------------------------------------------
  //    Operators
  //--------------------------------------------------------

  /**
   * negation (not)
   */
  Bitmap operator~() const {
    Bitmap ret(capacity());
    for (std::size_t i = 0; i < data_.size(); ++i) ret.data_[i] = ~data_[i];
    // trim extra bits
    if (capacity() % B) ret.data_.back() &= (ONE << (capacity() % B)) - 1;
    return ret;
  }

  /**
   * set/union (or)
   */
  Bitmap& operator|=(int x) {
    verify_argument_(x, "|=");

    data_[x / B] |= ONE << (x % B);
    return *this;
  }

  Bitmap& operator|=(Bitmap const& rhs) {
    if (capacity() != rhs.capacity()) throw std::invalid_argument("inconsistent size");

    for (std::size_t i = 0; i < data_.size(); ++i) data_[i] |= rhs.data_[i];
    return *this;
  }

  /**
   * exclusive or (xor)
   */
  Bitmap& operator^=(int x) {
    verify_argument_(x, "^=");

    data_[x / B] ^= ONE << (x % B);
    return *this;
  }

  Bitmap& operator^=(Bitmap const& rhs) {
    if (capacity() != rhs.capacity()) throw std::invalid_argument("inconsistent size");

    for (std::size_t i = 0; i < data_.size(); ++i) data_[i] ^= rhs.data_[i];
    return *this;
  }

  /**
   * intersection (and)
   */
  Bitmap operator&=(int x) {
    verify_argument_(x, "&=");

    for (int i = 0; i < static_cast<int>(data_.size()); ++i) {
      if (i == x / B) {
        data_[x / B] &= ONE << (x % B);
      } else {
        data_[i] = 0;
      }
    }

    return *this;
  }

  Bitmap& operator&=(Bitmap const& rhs) {
    if (capacity() != rhs.capacity()) throw std::invalid_argument("inconsistent size");

    for (std::size_t i = 0; i < data_.size(); ++i) data_[i] &= rhs.data_[i];
    return *this;
  }

  /**
   * reset/set minus
   */
  Bitmap& operator-=(int x) {
    verify_argument_(x, "-=");

    data_[x / B] &= ~(ONE << (x % B));
    return *this;
  }

  Bitmap& operator-=(Bitmap const& rhs) {
    if (capacity() != rhs.capacity()) throw std::invalid_argument("inconsistent size");

    for (std::size_t i = 0; i < data_.size(); ++i) data_[i] &= ~rhs.data_[i];
    return *this;
  }

  // clang-format off
  friend Bitmap operator|(Bitmap const& lhs, int x) { Bitmap ret(lhs); ret |= x; return ret; }
  friend Bitmap operator|(Bitmap const& lhs, Bitmap const& rhs) { Bitmap ret(lhs); ret |= rhs; return ret; }
  friend Bitmap operator^(Bitmap const& lhs, int x) { Bitmap ret(lhs); ret ^= x; return ret; }
  friend Bitmap operator^(Bitmap const& lhs, Bitmap const& rhs) { Bitmap ret(lhs); ret ^= rhs; return ret; }
  friend Bitmap operator&(Bitmap const& lhs, int x) { Bitmap ret(lhs); ret &= x; return ret; }
  friend Bitmap operator&(Bitmap const& lhs, Bitmap const& rhs) { Bitmap ret(lhs); ret &= rhs; return ret; }
  friend Bitmap operator-(Bitmap const& lhs, int x) { Bitmap ret(lhs); ret -= x; return ret; }
  friend Bitmap operator-(Bitmap const& lhs, Bitmap const& rhs) { Bitmap ret(lhs); ret -= rhs; return ret; }
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
    for (std::size_t i = 0; i < data_.size(); ++i) {
      for (auto x = data_[i]; x; x &= x - 1) ret.push_back(i * B + ctz(x));
    }
    return ret;
  }

  /**
   * equality
   */
  friend inline bool operator==(Bitmap const& lhs, Bitmap const& rhs) {
    if (lhs.capacity() != rhs.capacity()) return false;
    for (std::size_t i = 0; i < lhs.data_.size(); ++i) {
      if (lhs.data_[i] != rhs.data_[i]) return false;
    }
    return true;
  }

  friend inline bool operator!=(Bitmap const& lhs, Bitmap const& rhs) { return !(lhs == rhs); }

  /**
   * @brief Returns the number of 1-bits.
   */
  std::size_t count() const {
    std::size_t ret = 0;
    for (std::size_t i = 0; i < data_.size(); ++i) ret += popcount(data_[i]);
    return ret;
  }

  inline bool subset(Bitmap const& rhs) const { return (*this & rhs) == *this; }

  inline bool superset(Bitmap const& rhs) const { return rhs.subset(*this); }

  std::string to_string() const {
    std::stringstream ss;
    for (int i = data_.size() - 1; i >= 0; --i) { ss << std::setfill('0') << std::setw(B / 4) << std::hex << data_[i]; }
    return ss.str();
  }

  void resize(std::size_t new_size) {
    if (n_ > new_size) throw std::invalid_argument("cannot shrink the data");
    if (n_ == new_size) return;  // do nothing
    n_ = new_size;
    data_.resize((n_ + (B - 1)) / B);
  }

  //--------------------------------------------------------
  //    Aliases
  //--------------------------------------------------------
  std::size_t size() const { return count(); }
  void set(int x) { *this |= x; }
  void reset(int x) { *this -= x; }
  bool get(int x) { return (*this)[x]; }
  static Bitmap intersect(Bitmap const& s, Bitmap const& t) { return s & t; }
  static Bitmap Union(Bitmap const& s, Bitmap const& t) { return s | t; }
};

std::ostream& operator<<(std::ostream& os, Bitmap const& bm);

}  // namespace ds
}  // namespace tww
