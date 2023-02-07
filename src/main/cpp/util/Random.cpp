#include "Random.hpp"

#include <cassert>
#include <stdexcept>

namespace util {
namespace impl {

/**
 * @brief Samples numbers without replacement
 *
 * @tparam Int result type
 * @param rand Random instance
 * @param n population size
 * @param k sample size
 * @param ret reference to the resulting vector
 * @param offset size of already-processed elements
 */
template <typename Int>
static void sampleint_pool(util::Random &rand, Int n, Int k, std::vector<Int> &ret, Int offset = 0) {
  std::vector<Int> xs;
  for (Int i = offset; i < n; ++i) xs.push_back(i);
  for (Int i = 0; i < k; ++i) {
    Int last_pos = n - offset - 1 - i;
    Int j = rand.randint<Int>(0, last_pos);
    ret.push_back(xs[j]);
    xs[j] = xs[last_pos];  // move non-selected item into vacancy
  }
}

/**
 * @brief Samples elements without replacement using Vitter's Algorithm A.
 *
 * Running time: O(n)
 * Number of random variables: k
 *
 * @tparam Int result type
 * @param rand Random instance
 * @param n population size
 * @param k sample size
 * @param ret reference to the resulting vector
 * @param offset size of already-processed elements
 */
template <typename Int>
static void vitter_method_a(util::Random &rand, Int n, Int k, std::vector<Int> &ret, Int offset = 0) {
  Int cursor = offset;
  n -= offset;
  assert(k <= n);
  if (k == 0) return;

  for (; n > k && k >= 2; --n, --k) {
    for (double v = rand.random() * n / (n - k); v < 1; ++cursor, v = v * n / (n - k)) { --n; }
    ret.push_back(cursor++);
  }
  if (n == k) {  // Special case (n=k): choose all n elements
    for (; n > 0; --n) ret.push_back(cursor++);
  } else {  // Special case (k=1): uniformly pick one element from n elements
    ret.push_back(rand.randint<Int>(cursor, cursor + n - 1));
  }
}

template <typename Int>
static double vitter_get_x(util::Random &rand, Int n, Int k, double p) {
  while (true) {
    double x = n * (1 - p);
    if (x < n - k + 1) return x;
    p = std::pow(rand.random(), 1.0 / k);
  }
}

template <typename Int>
static double vitter_get_z(Int n, Int k, Int s) {
  double ret = 1.0;
  double bottom = k - 1 > s ? n - k : n - s - 1;
  double top = n - 1;
  Int stop = k - 1 > s ? n - s : n - k + 1;

  for (Int t = n - 1; t >= stop; --t, top -= 1.0, bottom -= 1.0) ret *= top / bottom;
  return ret;
}

template <typename Int>
static Int vitter_get_skip_distance(util::Random &rand, Int n, Int k, double &p) {
  while (true) {
    // Step D2: generate U and X
    double u = rand.random();
    double x = vitter_get_x(rand, n, k, p);  // real in [0,q)
    Int s = static_cast<Int>(std::floor(x));

    // Step D3: accept?
    Int q = n - k + 1;
    double y = std::pow(u * n / q, 1.0 / (k - 1));
    p = y * (1 - x / n) * q / (q - s);  // update p
    if (p <= 1.0) return s;             // accept and reuse p

    // Step D4: accept?
    double z = vitter_get_z(n, k, s);
    if (n / (n - x) >= y * std::pow(z, 1.0 / (k - 1))) {
      p = std::pow(rand.random(), 1.0 / (k - 1));  // update p
      return s;                                    // accept
    }

    p = std::pow(rand.random(), 1.0 / k);
  }
}

/*
 * Running time: O(k)
 */
template <typename Int>
static std::vector<Int> vitter_method_d(util::Random &rand, Int n, Int k) {
  assert(0 <= k && k <= n);

  if (k == 0) return {};

  std::vector<Int> ret;

  Int const alpha = 13;
  Int cursor = 0;                               // number of examined items
  double p = std::pow(rand.random(), 1.0 / k);  // precompute exponential distribution

  while (k > 1 && k * alpha < n - cursor) {
    Int s = vitter_get_skip_distance(rand, n - cursor, k, p);
    cursor += s;              // skip S items
    ret.push_back(cursor++);  // select this item
    --k;
  }

  assert(cursor < n);
  if (k == n - cursor) {
    // Special case (k=remain): choose all remaining elements
    for (; cursor < n; ++cursor) ret.push_back(cursor);
  } else if (k > 1) {
    // vitter_method_a(rand, n, k, ret, cursor);
    sampleint_pool<Int>(rand, n, k, ret, cursor);  // preferred method
  } else {
    // Special case (k=1): uniformly pick one element from the remain
    ret.push_back(rand.randint<Int>(cursor, n - 1));
  }
  return ret;
}
}  // namespace impl

template <typename Int>
Int Random::randint(Int a, Int b) {
  if (a > b) throw std::invalid_argument("Random::randint(): b must be at least a");
  std::uniform_int_distribution<Int> d(a, b);
  return d(gen_);
}

/*
 * Implementation using Vitter's algorithm.
 */
template <typename Int>
std::vector<Int> Random::sampleint(Int n, Int k) {
  if (k < 0) throw std::invalid_argument("Random::sampleint(): k must be non-negative");
  if (n < k) throw std::invalid_argument("Random::sampleint(): k must be at most n");

  return impl::vitter_method_d<Int>(*this, n, k);
}

//--------------------------------------------------------
//    Explicit instantiation
//--------------------------------------------------------
template uint32_t Random::randint(uint32_t a, uint32_t b);
template uint64_t Random::randint(uint64_t a, uint64_t b);
template int32_t Random::randint(int32_t a, int32_t b);
template int64_t Random::randint(int64_t a, int64_t b);

template std::vector<uint32_t> Random::sampleint(uint32_t n, uint32_t k);
template std::vector<uint64_t> Random::sampleint(uint64_t n, uint64_t k);
template std::vector<int32_t> Random::sampleint(int32_t n, int32_t k);
template std::vector<int64_t> Random::sampleint(int64_t n, int64_t k);

}  // namespace util
