/**
 * @brief Functional programming utilitles. This file contains macros. Use with caution.
 */

#pragma once

#include <algorithm>
#include <vector>
#include <list>
#include <functional>

namespace util {

template <typename A, typename B>
std::vector<B> fmap_impl(std::function<B(A)> f, std::vector<A> const& xs) {
  std::vector<B> ret;
  std::transform(xs.begin(), xs.end(), std::back_inserter(ret), f);
  return ret;
}

template <typename A, typename B>
std::list<B> fmap_impl(std::function<B(A)> f, std::list<A> const& xs) {
  std::list<B> ret;
  std::transform(xs.begin(), xs.end(), std::back_inserter(ret), f);
  return ret;
}

/**
 * @brief Functional map of a vector.
 * 
 * `ff` is for expanding a lambda expression.
 */
#define fmap(f, xs) ({\
auto ff=f;\
tww::util::fmap_impl<std::remove_reference<decltype(*xs.begin())>::type,\
decltype(ff(*xs.begin()))>(ff, xs);\
})

}  // namespace util
