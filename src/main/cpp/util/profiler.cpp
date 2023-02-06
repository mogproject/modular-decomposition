#include "profiler.hpp"

namespace util {

// Utilities
void pcount(Profiler* prof, std::string const& label, int param) {
  if (prof) prof->count(label, param);
}

void pstart(Profiler* prof, std::string const& label, int param) {
  if (prof) prof->start_timer(label, param);
}

void pstop(Profiler* prof, std::string const& label, int param) {
  if (prof) prof->stop_timer(label, param);
}

void pprint(Profiler* prof) {
  if (prof) prof->print();
}

}  // namespace util
