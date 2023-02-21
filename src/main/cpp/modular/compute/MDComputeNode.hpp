#pragma once

#include <string>

namespace modular {
namespace compute {

enum NodeType { VERTEX, OPERATION, PROBLEM };
enum Operation { PRIME, SERIES, PARALLEL };
enum SplitDirection { NONE, LEFT, RIGHT, MIXED };

typedef int VertexID;

class MDComputeNode {
 public:
  NodeType node_type;
  Operation op_type;
  SplitDirection split_type;
  VertexID vertex;  // pivot for problem node
  int comp_number;
  int tree_number;
  int num_marks;
  int num_left_split_children;   // number of the children with split LEFT or MIXED; used only in refinement
  int num_right_split_children;  // number of the children with split RIGHT or MIXED; used only in refinement
  bool active;
  bool connected;

  MDComputeNode(NodeType node_type = NodeType::PROBLEM)
      : node_type(node_type),  //
        op_type(Operation::PRIME),
        split_type(SplitDirection::NONE),
        vertex(-1),
        comp_number(-1),
        tree_number(-1),
        num_marks(0),
        num_left_split_children(0),
        num_right_split_children(0),
        active(false),
        connected(false) {}

  MDComputeNode(MDComputeNode const &node)
      : node_type(node.node_type),
        op_type(node.op_type),
        split_type(node.split_type),
        vertex(node.vertex),
        comp_number(node.comp_number),
        tree_number(node.tree_number),
        num_marks(node.num_marks),
        num_left_split_children(0),   // initialize to zero, not copying from the original
        num_right_split_children(0),  // initialize to zero, not copying from the original
        active(node.active),
        connected(node.connected) {}

  static MDComputeNode new_vertex_node(VertexID vertex) {
    auto ret = MDComputeNode(NodeType::VERTEX);
    ret.vertex = vertex;
    return ret;
  }

  static MDComputeNode new_operation_node(Operation op_type) {
    auto ret = MDComputeNode(NodeType::OPERATION);
    ret.op_type = op_type;
    return ret;
  }

  static MDComputeNode new_problem_node(bool connected) {
    auto ret = MDComputeNode(NodeType::PROBLEM);
    ret.connected = connected;
    return ret;
  }

  bool is_vertex_node() const { return node_type == NodeType::VERTEX; }
  bool is_operation_node() const { return node_type == NodeType::OPERATION; }
  bool is_problem_node() const { return node_type == NodeType::PROBLEM; }

  bool is_marked() const { return num_marks > 0; }
  void add_mark() { ++num_marks; }
  int number_of_marks() const { return num_marks; }
  void clear_marks() { num_marks = 0; }

  bool is_split_marked(SplitDirection split_type) {
    return this->split_type == SplitDirection::MIXED || this->split_type == split_type;
  }

  void set_split_mark(SplitDirection split_type) {
    if (this->split_type == split_type) {
      // already set
    } else if (this->split_type == SplitDirection::NONE) {
      this->split_type = split_type;
    } else {
      this->split_type = SplitDirection::MIXED;
    }
  }

  void increment_num_split_children(SplitDirection split_type) {
    if (split_type == SplitDirection::LEFT) {
      ++num_left_split_children;
    } else {
      ++num_right_split_children;
    }
  }

  void decrement_num_split_children(SplitDirection split_type) {
    if (split_type == SplitDirection::LEFT) {
      --num_left_split_children;
    } else {
      --num_right_split_children;
    }
  }

  int get_num_split_children(SplitDirection split_type) const {
    return split_type == SplitDirection::LEFT ? num_left_split_children : num_right_split_children;
  }

  void clear_num_split_children() { num_left_split_children = num_right_split_children = 0; }

  void clear() {
    comp_number = -1;
    tree_number = -1;
    // num_marks = 0;
    split_type = SplitDirection::NONE;
    clear_num_split_children();
  }

  std::string to_string() const {
    switch (node_type) {
      case NodeType::VERTEX: return std::to_string(vertex); break;
      case NodeType::OPERATION:
        switch (op_type) {
          case Operation::PRIME: return "P"; break;
          case Operation::SERIES: return "J"; break;
          case Operation::PARALLEL: return "U"; break;
        }
      case NodeType::PROBLEM: return vertex < 0 ? "C-" : "C" + std::to_string(vertex);
    }
    return "";
  }
};

std::ostream &operator<<(std::ostream &os, MDComputeNode const &node);

}  // namespace compute
}  // namespace modular
