#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>

namespace util {
class Benchmark {
 public:
  template <typename Func>
  /**
   * @brief Benchmarks one function.
   *
   * @param f function to run; may be executed multiple times
   * @param min_elapsed  required elapsed time in microseconds (default: 100ms)
   * @return elapsed time per one function execution in seconds
   */
  static double bench_function(Func f, long long min_elapsed = 100000LL) {
    int num_iterations = 1;

    while (true) {
      auto start_time = std::chrono::system_clock::now();
      for (int i = 0; i < num_iterations; ++i) f();
      auto end_time = std::chrono::system_clock::now();

      long long elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
      if (elapsed_us < min_elapsed) {
        // too fast; increase the number of iterations
        auto x = static_cast<double>(min_elapsed) * num_iterations;
        num_iterations = static_cast<int>(std::ceil(x / std::max(1LL, elapsed_us)));
      } else {
        return 1e-9 * elapsed_us / num_iterations;
      }
    }
  }
};
}  // namespace util
