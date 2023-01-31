import unittest

from collections import defaultdict
import networkx as nx
from modular.compute.pivot import *
from modular.compute.MDComputeNode import MDComputeNode
from modular.tree.RootedTree import RootedTree, Node

class TestPivot(unittest.TestCase):
    """Tests pivot module."""

    def test_do_pivot(self):
        G: nx.Graph = nx.empty_graph(4)
        G.add_edges_from([(0, 3), (1, 0), (1, 3), (3, 2)])

        tree: RootedTree[MDComputeNode] = RootedTree()
        vertex_nodes = {i: tree.create_node(MDComputeNode.new_vertex_node(i)) for i in G.nodes()}
        alpha_list: dict[int, set[MDComputeNode]] = defaultdict(set)
        visited: set[int] = set()

        # create the main problem
        main_prob = tree.create_node(MDComputeNode.new_problem_node(connected=False))

        # add vertex nodes (cannot be removed)
        for i in reversed(list(G.nodes())):
            tree.move_to(vertex_nodes[i], main_prob)

        current = main_prob
        self.assertEqual(str(current), '(C-(0)(1)(2)(3))')

        fc = current.first_child
        visited.add(fc)
        pivoted = do_pivot(G, tree, vertex_nodes, alpha_list, visited, current, fc.data.vertex)
        self.assertEqual(str(pivoted), '(C0(C-(1)(3))(C-(0))(C-(2)))')

        current = pivoted.first_child
        fc = current.first_child
        visited.add(fc)
        p2 = do_pivot(G, tree, vertex_nodes, alpha_list, visited, current, fc.data.vertex)
        self.assertEqual(str(p2.parent), '(C0(C1(C-(3))(C-(1)))(C-(0))(C-(2)))')

        current = p2.first_child
        fc = current.first_child
        process_neighbors(G, tree, vertex_nodes, alpha_list, visited, fc.data.vertex, current, None)
        self.assertEqual(str(current.get_root()), '(C0(C1(C-(3))(C-(1)))(C-(0))(C-(2)))')
