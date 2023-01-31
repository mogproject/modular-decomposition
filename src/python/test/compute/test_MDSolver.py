import unittest

import networkx as nx
from modular.compute.assembly import remove_degenerate_duplicates, delineate, compute_mu, assemble_tree
from modular.compute.MDComputeNode import MDComputeNode, OperationType
from modular.compute.MDSolver import MDSolver
from modular.tree.RootedTree import RootedTree


class TestMDSolver(unittest.TestCase):
    """Tests MDSolver module."""

    def test_compute(self):
        g1 = nx.empty_graph(2)

        r = MDSolver.compute(g1)
        self.assertEqual(str(r[1]), '(U(0)(1))')

        g1.add_edge(0, 1)

        r = MDSolver.compute(g1)
        self.assertEqual(str(r[1]), '(J(1)(0))')

        g5 = nx.empty_graph(5)
        r = MDSolver.compute(g5)
        self.assertEqual(str(r[1]), '(U(0)(1)(2)(3)(4))')

        g4 = nx.empty_graph(4)
        g4.add_edges_from([(0, 1), (1, 2), (2, 3)])
        r = MDSolver.compute(g4)
        self.assertEqual(str(r[1]), '(P(3)(2)(1)(0))')

        g4.add_edge(0, 2)
        g4.add_edge(1, 3)
        r = MDSolver.compute(g4)
        self.assertEqual(str(r[1]), '(J(1)(2)(U(3)(0)))')

        g4.add_edge(0, 3)
        r = MDSolver.compute(g4)
        self.assertEqual(str(r[1]), '(J(1)(2)(3)(0))')

    def test_compute_2(self):
        g = nx.empty_graph(5)
        g.add_edges_from([(0, 1), (0, 2), (0, 3), (1, 2), (1, 4), (2, 4), (3, 4)])
        r = MDSolver.compute(g)
        self.assertEqual(str(r[1]), '(J(U(3)(J(1)(2)))(U(4)(0)))')

        g = nx.empty_graph(5)
        g.add_edges_from([(0, 1), (1, 3), (2, 3), (4, 3)])
        r = MDSolver.compute(g)
        self.assertEqual(str(r[1]), '(P(U(4)(2))(3)(1)(0))')

        g = nx.empty_graph(8)
        g.add_edges_from([
            (0, 2), (0, 3), (0, 6), (0, 7),
            (1, 6),
            (2, 3), (2, 4), (2, 5), (2, 7),
            (3, 4), (3, 5),
            (4, 5), (4, 6), (4, 7),
            (5, 6), (5, 7),
        ])
        r = MDSolver.compute(g)
        self.assertEqual(str(r[1]), '(P(1)(6)(J(2)(U(3)(7)))(U(J(4)(5))(0)))')
