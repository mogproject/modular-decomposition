#pragma once

#include "ds/set/ArrayBitset.hpp"
#include "ds/set/SortedVectorSet.hpp"

namespace ds {
namespace graph {
/**
 * @brief Graph with integer labels.
 *
 * @tparam Set set implementation class
 */
class Graph {
 private:
  /** Number of vertices. */
  std::size_t n_;

  /** Number of vertices. */
  std::size_t m_;

  /** True if the adjacency sets are in dense representation. */
  bool dense_;

  /** Adjacency sets. */
  std::vector<basic_set<int>*> adj_;

  /** Masks for removed vertices. */
  basic_set<int>* removed_;

  bool is_valid(int v) const { return 0 <= v && v < static_cast<int>(adj_.size()) && !removed_->get(v); }

  basic_set<int>* create_set(std::size_t n, bool dense) {
    if (dense) {
      if (n <= 1 << 6) {
        return new ArrayBitset6(n);
      } else if (n <= 1 << 7) {
        return new ArrayBitset7(n);
      } else if (n <= 1 << 8) {
        return new ArrayBitset8(n);
      } else if (n <= 1 << 9) {
        return new ArrayBitset9(n);
      } else if (n <= 1 << 10) {
        return new ArrayBitset10(n);
      } else if (n <= 1 << 11) {
        return new ArrayBitset11(n);
      } else if (n <= 1 << 12) {
        return new ArrayBitset12(n);
      } else if (n <= 1 << 13) {
        return new ArrayBitset13(n);
      } else {
        throw std::invalid_argument("Graph: n too large for dense representation");
      }
    } else {
      return new SortedVectorSet();
    }
  }

 public:
  Graph(std::size_t n = 0, std::vector<std::pair<int, int>> const& edges = {}, bool dense = false)
      : n_(n), m_(0), dense_(dense) {
    for (int i = 0; i < static_cast<int>(n); ++i) adj_.push_back(create_set(n, dense));
    removed_ = create_set(n, dense);
    for (auto& p : edges) add_edge(p.first, p.second);
  }

  std::size_t number_of_nodes() const { return n_; }
  std::size_t number_of_edges() const { return m_; }

  int add_vertex() {
    int x;
    if (removed_->empty()) {  // all vertices are in use
      if (dense_) {
        throw std::invalid_argument("add_vertex: cannot extend capacity");
      } else {
        adj_.push_back(create_set(n_, dense_));
        // `removed_` should be SortedVectorSet so no need to replace
        x = n_++;
      }
    } else {
      // reuse one of the removed vertices
      x = dense_ ? removed_->pop_front() : removed_->pop_back();
      ++n_;
    }
    return x;
  }

  void add_edge(int u, int v) {
    if (!is_valid(u)) throw std::invalid_argument("add_edge: invalid u");
    if (!is_valid(v)) throw std::invalid_argument("add_edge: invalid v");
    if (u == v) throw std::invalid_argument("add_edge: loop is not allowed");

    if (!has_edge(u, v)) {
      adj_[u]->set(v);
      adj_[v]->set(u);
      ++m_;
    }
  }

  void remove_vertex(int v) {
    if (!is_valid(v)) throw std::invalid_argument("remove_vertex: invalid v");

    --n_;
    m_ -= adj_[v]->size();

    for (auto u : adj_[v]->to_vector()) adj_[u]->reset(v);
    adj_[v]->clear();
    removed_->set(v);
  }

  void remove_edge(int u, int v) {
    if (!is_valid(u)) throw std::invalid_argument("remove_edge: invalid u");
    if (!is_valid(v)) throw std::invalid_argument("remove_edge: invalid v");
    if (!adj_[u]->get(v)) throw std::invalid_argument("remove_edge: edge does not exist");

    adj_[u]->reset(v);
    adj_[v]->reset(u);
    --m_;
  }

  std::vector<int> neighbors(int v) const { return adj_[v]->to_vector(); }

  int degree(int v) const { return adj_[v]->size(); }

  bool has_vertex(int v) const { return is_valid(v); }

  bool has_edge(int u, int v) const {
    if (!is_valid(u) || !is_valid(v) || u == v) return false;
    return degree(u) <= degree(v) ? adj_[u]->get(v) : adj_[v]->get(u);
  }
};
}  // namespace graph
}  // namespace ds
