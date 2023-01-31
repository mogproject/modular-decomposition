import unittest

import networkx as nx
import numpy as np
from modular.compute_naive.MDNaiveSolver import MDNaiveSolver


class TestMDNaiveSOlver(unittest.TestCase):
    """Tests MDNaiveSolver class."""

    def test_compute_implication_matrix(self):
        G = nx.empty_graph(10)
        G.add_edges_from([
            (0, 4), (4, 2), (2, 6), (9, 7), (7, 1), (1, 5), (3, 8),
            (3, 9), (3, 7), (3, 1), (3, 5), (8, 0), (8, 4), (8, 2), (8, 6)
        ])

        self.assertListEqual(
            MDNaiveSolver.compute_implication_matrix(G).tolist(), [
                # 0  1  2  3  4  5  6  7  8  9
                [0, 0, 0, 0, 1, 0, 0, 0, 2, 0],
                [0, 0, 0, 2, 0, 3, 0, 3, 0, 0],
                [0, 0, 0, 0, 1, 0, 1, 0, 2, 0],
                [0, 2, 0, 0, 0, 2, 0, 2, 2, 2],
                [1, 0, 1, 0, 0, 0, 0, 0, 2, 0],
                [0, 3, 0, 2, 0, 0, 0, 0, 0, 0],
                [0, 0, 1, 0, 0, 0, 0, 0, 2, 0],
                [0, 3, 0, 2, 0, 0, 0, 0, 0, 3],
                [2, 0, 2, 2, 2, 0, 2, 0, 0, 0],
                [0, 0, 0, 2, 0, 0, 0, 3, 0, 0]
            ]
        )

        G = nx.complete_graph(5)
        self.assertListEqual(
            MDNaiveSolver.compute_implication_matrix(G).tolist(), [
                [0, 1, 2, 3, 4],
                [1, 0, 2, 3, 4],
                [2, 2, 0, 3, 4],
                [3, 3, 3, 0, 4],
                [4, 4, 4, 4, 0],
            ]
        )

        G = nx.path_graph(5)
        self.assertListEqual(
            MDNaiveSolver.compute_implication_matrix(G).tolist(), [
                [0, 1, 0, 0, 0],
                [1, 0, 1, 0, 0],
                [0, 1, 0, 1, 0],
                [0, 0, 1, 0, 1],
                [0, 0, 0, 1, 0],
            ]
        )
