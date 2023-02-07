import unittest

import networkx as nx
from modular import modular_decomposition
from random import Random


class TestMDTree(unittest.TestCase):
    """Tests MDTree class."""

    def check_property(self, G: nx.Graph):
        n = len(G)

        t_naive = modular_decomposition(G, sorted=True, solver='naive')
        t_linear = modular_decomposition(G, sorted=True, solver='linear')

        self.assertEqual(str(t_naive), str(t_linear), msg=f'n={len(G)}, edges={G.edges()}')
        self.assertEqual(t_naive.modular_width(), t_linear.modular_width(), msg=f'n={len(G)}, edges={G.edges()}')

        # complement graph
        c_naive = modular_decomposition(nx.complement(G), sorted=True, solver='naive')
        c_linear = modular_decomposition(nx.complement(G), sorted=True, solver='linear')

        self.assertEqual(str(c_naive), str(c_linear), msg=f'n={len(G)}, edges={G.edges()}')
        self.assertEqual(str(c_naive), str(t_naive).replace('J', 'X').replace('U', 'J').replace('X', 'U'), msg=f'n={len(G)}, edges={G.edges()}')

        self.assertEqual(t_naive.modular_width(), c_naive.modular_width(), msg=f'n={len(G)}, edges={G.edges()}')
        self.assertEqual(t_linear.modular_width(), c_linear.modular_width(), msg=f'n={len(G)}, edges={G.edges()}')

        # vertex permutation
        mapping = {i: n - 1 - i for i in range(n)}
        H = nx.relabel_nodes(G, mapping, copy=True)
        p_naive = modular_decomposition(H, sorted=True, solver='naive')
        p_linear = modular_decomposition(H, sorted=True, solver='linear')

        self.assertEqual(t_naive.modular_width(), p_naive.modular_width(), msg=f'n={len(G)}, edges={G.edges()}')
        self.assertEqual(t_naive.modular_width(), p_linear.modular_width(), msg=f'n={len(G)}, edges={G.edges()}')

    def generate_mw_bounded_graph(self, n: int, max_mw: int, p: float, rand: Random) -> nx.Graph:
        if n == 0:
            return nx.Graph()
        max_mw = max(max_mw, 3)

        G = nx.empty_graph(1)
        while len(G) < n:
            # pick a vertex to substitute
            prev_n = len(G)
            x = rand.randint(0, prev_n - 1)

            # create a replacement
            nn = min(n - prev_n + 1, max_mw)
            H = nx.erdos_renyi_graph(nn, p, seed=rand)
            labels = {i: prev_n + i if i < nn - 1 else x for i in range(nn)}

            # substitue
            nbrs = list(G.neighbors(x))
            G.remove_node(x)
            G.add_node(x)
            G.add_nodes_from(range(prev_n, prev_n + nn - 1))

            G.add_edges_from((labels[u], labels[v]) for u, v, in H.edges())
            G.add_edges_from((labels[i], u) for i in range(nn) for u in nbrs)
        return G

    def test_modular_decomposition(self):
        t = modular_decomposition(nx.empty_graph(0), sorted=True)
        self.assertIsNone(t)

        t = modular_decomposition(nx.empty_graph(1), sorted=True)
        self.assertEqual(t.modular_width(), 0)
        self.assertEqual(str(t), '(0)')

        t = modular_decomposition(nx.empty_graph(10), sorted=True)
        self.assertEqual(t.modular_width(), 0)
        self.assertEqual(str(t), '(U(0)(1)(2)(3)(4)(5)(6)(7)(8)(9))')

        G = nx.empty_graph(6)
        G.add_edges_from([(1, 2), (1, 0), (4, 3), (3, 4)])
        t = modular_decomposition(G, sorted=True)
        self.assertEqual(t.modular_width(), 0)
        self.assertEqual(str(t), '(U(J(U(0)(2))(1))(J(3)(4))(5))')

        G = nx.empty_graph(14)
        G.add_edges_from([
            (1, 8), (1, 11), (1, 2), (1, 10), (1, 12), (1, 9), (1, 13), (8, 11), (2, 10),
            (0, 4), (0, 7), (0, 3), (4, 5), (4, 7), (3, 7), (3, 5), (3, 6), (5, 6), (7, 6),
        ])
        t = modular_decomposition(G, sorted=True)
        self.assertEqual(t.modular_width(), 6)
        self.assertEqual(str(t), '(U(P(0)(3)(4)(5)(6)(7))(J(1)(U(J(2)(10))(J(8)(11))(9)(12)(13))))')

        t = modular_decomposition(nx.complement(G), sorted=True)
        self.assertEqual(t.modular_width(), 6)
        self.assertEqual(str(t), '(J(P(0)(3)(4)(5)(6)(7))(U(1)(J(U(2)(10))(U(8)(11))(9)(12)(13))))')

        G = nx.empty_graph(10)
        G.add_edges_from([
            (0, 1), (1, 2), (2, 3),
            (8, 9),
            (8, 0), (8, 1), (8, 2), (8, 3),
            (9, 4),
        ])
        t = modular_decomposition(G, sorted=True)
        self.assertEqual(t.modular_width(), 4)
        self.assertEqual(str(t), '(U(P(P(0)(1)(2)(3))(4)(8)(9))(5)(6)(7))')

        t = modular_decomposition(nx.complement(G), sorted=True)
        self.assertEqual(t.modular_width(), 4)
        self.assertEqual(str(t), '(J(P(P(0)(1)(2)(3))(4)(8)(9))(5)(6)(7))')

        G = nx.empty_graph(10)
        G.add_edges_from([
            (0, 1), (1, 2), (2, 3), (4, 5), (5, 6), (6, 7),
            (8, 9),
            (8, 0), (8, 1), (8, 2), (8, 3),
            (9, 4), (9, 5), (9, 6), (9, 7),
        ])
        t = modular_decomposition(G, sorted=True)
        self.assertEqual(t.modular_width(), 4)
        self.assertEqual(str(t), '(P(P(0)(1)(2)(3))(P(4)(5)(6)(7))(8)(9))')

        t = modular_decomposition(nx.complement(G), sorted=True)
        self.assertEqual(t.modular_width(), 4)
        self.assertEqual(str(t), '(P(P(0)(1)(2)(3))(P(4)(5)(6)(7))(8)(9))')

        G = nx.empty_graph(7)
        G.add_edges_from([(0, 1), (0, 2), (0, 3), (0, 4), (0, 5), (0, 6), (1, 2), (2, 3), (3, 4), (5, 6)])
        t = modular_decomposition(G, sorted=True)
        self.assertEqual(t.modular_width(), 4)
        self.assertEqual(str(t), '(J(0)(U(P(1)(2)(3)(4))(J(5)(6))))')

        t = modular_decomposition(nx.complement(G), sorted=True)
        self.assertEqual(t.modular_width(), 4)
        self.assertEqual(str(t), '(U(0)(J(P(1)(2)(3)(4))(U(5)(6))))')

        G = nx.empty_graph(10)
        G.add_edges_from([
            (0, 4), (4, 2), (2, 6), (9, 7), (7, 1), (1, 5), (3, 8),
            (3, 9), (3, 7), (3, 1), (3, 5), (8, 0), (8, 4), (8, 2), (8, 6)
        ])
        t = modular_decomposition(G, sorted=True)
        self.assertEqual(t.modular_width(), 4)
        self.assertEqual(str(t), '(P(P(0)(2)(4)(6))(P(1)(5)(7)(9))(3)(8))')

        t = modular_decomposition(nx.complement(G), sorted=True)
        self.assertEqual(t.modular_width(), 4)
        self.assertEqual(str(t), '(P(P(0)(2)(4)(6))(P(1)(5)(7)(9))(3)(8))')

        G = nx.empty_graph(10)
        G.add_edges_from([
            (1, 2), (2, 3), (3, 4),
            (5, 6), (6, 7), (7, 8),
            (0, 1), (0, 2), (0, 3), (0, 4),
            (9, 5), (9, 6), (9, 7), (9, 8),
            (1, 5), (1, 6), (1, 7), (1, 8),
            (2, 5), (2, 6), (2, 7), (2, 8),
            (3, 5), (3, 6), (3, 7), (3, 8),
            (4, 5), (4, 6), (4, 7), (4, 8),
        ])
        t = modular_decomposition(G, sorted=True)
        self.assertEqual(t.modular_width(), 4)
        self.assertEqual(str(t), '(P(0)(P(1)(2)(3)(4))(P(5)(6)(7)(8))(9))')

        t = modular_decomposition(nx.complement(G), sorted=True)
        self.assertEqual(t.modular_width(), 4)
        self.assertEqual(str(t), '(P(0)(P(1)(2)(3)(4))(P(5)(6)(7)(8))(9))')

        G = nx.Graph()
        G.add_nodes_from(['a', 'b', 'c', 'd'])
        G.add_edges_from([('a', 'b'), ('b', 'c'), ('c', 'a'), ('b', 'd')])
        t = modular_decomposition(G, sorted=True, solver='naive')
        self.assertEqual(t.modular_width(), 0)
        self.assertEqual(str(t), "(J(U(J('a')('c'))('d'))('b'))")

        t = modular_decomposition(G, sorted=True, solver='linear')
        self.assertEqual(t.modular_width(), 0)
        self.assertEqual(str(t), "(J(U(J('a')('c'))('d'))('b'))")

        G = nx.hypercube_graph(3)
        t = modular_decomposition(G, sorted=True, solver='naive')
        self.assertEqual(t.modular_width(), 8)
        self.assertEqual(str(t), '(P((0, 0, 0))((0, 0, 1))((0, 1, 0))((0, 1, 1))((1, 0, 0))((1, 0, 1))((1, 1, 0))((1, 1, 1)))')

        t = modular_decomposition(G, sorted=True, solver='linear')
        self.assertEqual(t.modular_width(), 8)
        self.assertEqual(str(t), '(P((0, 0, 0))((0, 0, 1))((0, 1, 0))((0, 1, 1))((1, 0, 0))((1, 0, 1))((1, 1, 0))((1, 1, 1)))')

    def test_modular_decomposition_2(self):
        G = nx.empty_graph(6)
        G.add_edges_from([(0, 2), (2, 4), (4, 3)])

        t = modular_decomposition(G, sorted=True, solver='naive')
        self.assertEqual(t.modular_width(), 4)
        self.assertEqual(str(t), '(U(P(0)(2)(3)(4))(1)(5))')

        t = modular_decomposition(nx.complement(G), sorted=True, solver='naive')
        self.assertEqual(t.modular_width(), 4)
        self.assertEqual(str(t), '(J(P(0)(2)(3)(4))(1)(5))')

        t = modular_decomposition(G, sorted=True, solver='linear')
        self.assertEqual(t.modular_width(), 4)
        self.assertEqual(str(t), '(U(P(0)(2)(3)(4))(1)(5))')

        t = modular_decomposition(nx.complement(G), sorted=True, solver='linear')
        self.assertEqual(t.modular_width(), 4)
        self.assertEqual(str(t), '(J(P(0)(2)(3)(4))(1)(5))')

    def test_modular_decomposition_3(self):
        G = nx.empty_graph(7)
        G.add_edges_from([(5, 2), (5, 0), (5, 6), (1, 3)])

        t = modular_decomposition(G, sorted=True, solver='naive')
        self.assertEqual(t.modular_width(), 0)
        self.assertEqual(str(t), '(U(J(U(0)(2)(6))(5))(J(1)(3))(4))')

        t = modular_decomposition(nx.complement(G), sorted=True, solver='naive')
        self.assertEqual(t.modular_width(), 0)
        self.assertEqual(str(t), '(J(U(J(0)(2)(6))(5))(U(1)(3))(4))')

        t = modular_decomposition(G, sorted=True, solver='linear')
        self.assertEqual(t.modular_width(), 0)
        self.assertEqual(str(t), '(U(J(U(0)(2)(6))(5))(J(1)(3))(4))')

        t = modular_decomposition(nx.complement(G), sorted=True, solver='linear')
        self.assertEqual(t.modular_width(), 0)
        self.assertEqual(str(t), '(J(U(J(0)(2)(6))(5))(U(1)(3))(4))')

    def test_modular_decomposition_4(self):
        G = nx.empty_graph(13)
        G.add_edges_from([(1, 9), (2, 8), (2, 11), (3, 5), (3, 7), (4, 11), (5, 9), (6, 12), (7, 10)])

        t = modular_decomposition(G, sorted=True, solver='naive')
        self.assertEqual(t.modular_width(), 6)
        self.assertEqual(str(t), '(U(0)(P(1)(3)(5)(7)(9)(10))(P(2)(4)(8)(11))(J(6)(12)))')

        t = modular_decomposition(nx.complement(G), sorted=True, solver='naive')
        self.assertEqual(t.modular_width(), 6)
        self.assertEqual(str(t), '(J(0)(P(1)(3)(5)(7)(9)(10))(P(2)(4)(8)(11))(U(6)(12)))')

        t = modular_decomposition(G, sorted=True, solver='linear')
        self.assertEqual(t.modular_width(), 6)
        self.assertEqual(str(t), '(U(0)(P(1)(3)(5)(7)(9)(10))(P(2)(4)(8)(11))(J(6)(12)))')

        t = modular_decomposition(nx.complement(G), sorted=True, solver='linear')
        self.assertEqual(t.modular_width(), 6)
        self.assertEqual(str(t), '(J(0)(P(1)(3)(5)(7)(9)(10))(P(2)(4)(8)(11))(U(6)(12)))')

    def test_modular_decomposition_5(self):
        G = nx.empty_graph(8)
        G.add_edges_from([(0, 1), (1, 2), (2, 3), (4, 5), (5, 6), (6, 7)])

        t1 = modular_decomposition(nx.complement(G), sorted=True, solver='naive')
        t2 = modular_decomposition(nx.complement(G), sorted=True, solver='linear')
        self.assertEqual(str(t1), str(t2))

        t = modular_decomposition(G, sorted=True, solver='naive')
        self.assertEqual(t.modular_width(), 4)
        self.assertEqual(str(t), '(U(P(0)(1)(2)(3))(P(4)(5)(6)(7)))')

        t = modular_decomposition(nx.complement(G), sorted=True, solver='naive')
        self.assertEqual(t.modular_width(), 4)
        self.assertEqual(str(t), '(J(P(0)(1)(2)(3))(P(4)(5)(6)(7)))')

        t = modular_decomposition(G, sorted=True, solver='linear')
        self.assertEqual(t.modular_width(), 4)
        self.assertEqual(str(t), '(U(P(0)(1)(2)(3))(P(4)(5)(6)(7)))')

        t = modular_decomposition(nx.complement(G), sorted=True, solver='linear')
        self.assertEqual(t.modular_width(), 4)
        self.assertEqual(str(t), '(J(P(0)(1)(2)(3))(P(4)(5)(6)(7)))')

    def test_modular_decomposition_6(self):
        G = nx.empty_graph(6)
        G.add_edges_from([(0, 1), (1, 2), (2, 3), (2, 4), (4, 5)])

        t = modular_decomposition(G, sorted=True, solver='naive')
        self.assertEqual(t.modular_width(), 6)
        self.assertEqual(str(t), '(P(0)(1)(2)(3)(4)(5))')

        t = modular_decomposition(nx.complement(G), sorted=True, solver='naive')
        self.assertEqual(t.modular_width(), 6)
        self.assertEqual(str(t), '(P(0)(1)(2)(3)(4)(5))')

        t = modular_decomposition(G, sorted=True, solver='linear')
        self.assertEqual(t.modular_width(), 6)
        self.assertEqual(str(t), '(P(0)(1)(2)(3)(4)(5))')

        t = modular_decomposition(nx.complement(G), sorted=True, solver='linear')
        self.assertEqual(t.modular_width(), 6)
        self.assertEqual(str(t), '(P(0)(1)(2)(3)(4)(5))')

    def test_modular_decomposition_7(self):
        G = nx.empty_graph(6)
        G.add_edges_from([(1, 5), (2, 4), (3, 5), (3, 4), (3, 0), (5, 4)])

        # self.check_property(G)

        t = modular_decomposition(G, sorted=True, solver='naive')
        self.assertEqual(t.modular_width(), 6)
        self.assertEqual(str(t), '(P(0)(1)(2)(3)(4)(5))')

        t = modular_decomposition(nx.complement(G), sorted=True, solver='naive')
        self.assertEqual(t.modular_width(), 6)
        self.assertEqual(str(t), '(P(0)(1)(2)(3)(4)(5))')

        t = modular_decomposition(G, sorted=True, solver='linear')
        self.assertEqual(str(t), '(P(0)(1)(2)(3)(4)(5))')
        self.assertEqual(t.modular_width(), 6)

        t = modular_decomposition(nx.complement(G), sorted=True, solver='linear')
        self.assertEqual(str(t), '(P(0)(1)(2)(3)(4)(5))')
        self.assertEqual(t.modular_width(), 6)

    def test_modular_decomposition_8(self):
        G = nx.empty_graph(14)
        G.add_edges_from([
            (0, 2), (0, 5), (1, 2), (1, 3), (2, 4), (3, 5), (3, 12), (5, 13),
            (6, 10), (6, 13), (7, 8), (7, 11), (9, 13), (11, 13)
        ])
        self.check_property(G)

    def test_modular_decomposition_9(self):
        G = nx.empty_graph(9)
        G.add_edges_from([(0, 7), (1, 4), (2, 4), (2, 7), (2, 8), (4, 5)])
        self.check_property(G)

    def test_modular_decomposition_10(self):
        G = nx.empty_graph(12)
        G.add_edges_from([(0, 8), (1, 6), (1, 7), (4, 8), (5, 7), (6, 8), (6, 9), (8, 9), (9, 11)])
        self.check_property(G)

    def test_modular_decomposition_11(self):
        G = nx.empty_graph(11)
        G.add_edges_from([(0, 5), (1, 3), (1, 8), (3, 8), (4, 9), (7, 8), (8, 9)])
        self.check_property(G)

    def test_modular_decomposition_12(self):
        G = nx.empty_graph(14)
        G.add_edges_from([
            (0, 10), (0, 13), (1, 3), (1, 10), (2, 13), (3, 9), (3, 10), (3, 13),
            (4, 7), (5, 9), (5, 10), (9, 10), (11, 13)
        ])
        self.check_property(G)

    def test_modular_decomposition_13(self):
        G = nx.empty_graph(8)
        G.add_edges_from([(0, 3), (0, 7), (1, 3), (1, 6), (2, 3), (2, 4), (2, 5), (3, 4), (3, 6), (3, 7), (4, 5), (4, 6)])
        self.check_property(G)

    def test_modular_decomposition_14(self):
        G = nx.empty_graph(13)
        G.add_edges_from([
            (0, 12), (1, 2), (1, 3), (1, 9), (3, 4), (3, 5), (3, 6),
            (3, 8), (3, 10), (3, 11), (3, 12), (4, 11), (5, 10), (6, 11), (6, 12),
            (7, 12), (8, 9), (8, 12), (9, 10), (9, 12), (10, 11)
        ])
        self.check_property(G)

    def test_modular_decomposition_15(self):
        G = nx.empty_graph(14)
        G.add_edges_from([
            (0, 1), (0, 4), (0, 7), (0, 8), (0, 12), (1, 7), (1, 9), (1, 10),
            (1, 11), (2, 4), (3, 5), (3, 6), (3, 9), (3, 11), (3, 13), (4, 12),
            (5, 12), (6, 13), (7, 8), (8, 12), (9, 11), (9, 13)
        ])
        # there used to be a non-deterministic bug
        for _ in range(10):
            self.check_property(G)

    def test_modular_decomposition_16(self):
        G = nx.empty_graph(25)
        G.add_edges_from([
            (0, 1),(0, 2),(0, 3),(1, 4),(1, 5),(1, 6),(2, 7),(2, 8),(2, 9),
            (3, 10),(3, 11),(3, 12),(4, 13),(4, 14),(4, 15),(5, 16),(5, 17),(5, 18),
            (6, 19),(6, 20),(6, 21),(7, 22),(7, 23),(7, 24),
        ])

        t = modular_decomposition(G, sorted=True, solver='linear')
        self.assertEqual(str(t), '(P(0)(1)(2)(3)(4)(5)(6)(7)(U(8)(9))(U(10)(11)(12))(U(13)(14)(15))(U(16)(17)(18))(U(19)(20)(21))(U(22)(23)(24)))')
        self.assertEqual(t.modular_width(), 14)
        self.check_property(G)

    def test_modular_decomposition_random(self):
        rand = Random(12345)
        min_n = 5
        max_n = 20
        num_iterations = 5
        # num_iterations = 1000
        def ps(n): return [1 / n, 0.1, 0.2, 0.3]

        for n in range(min_n, max_n + 1):
            for _ in range(num_iterations):
                for p in ps(n):
                    G = nx.erdos_renyi_graph(n, p, seed=rand)
                    self.check_property(G)

    def test_modular_decomposition_random_mw_bounded(self):
        rand = Random(12345)
        min_n = 30
        max_n = 50
        step_n = 5
        num_iterations = 10
        mw = 5
        p = 0.5

        for n in range(min_n, max_n + 1, step_n):
            for _ in range(num_iterations):
                G = self.generate_mw_bounded_graph(n, mw, p, rand)
                t = modular_decomposition(G, solver='linear')
                self.assertLessEqual(t.modular_width(), mw)
                self.check_property(G)

    def test_modular_decomposition_random_mw_bounded_large(self):
        import sys
        rec_lim = sys.getrecursionlimit()
        rand = Random(12345)

        for _ in range(10):
            G = self.generate_mw_bounded_graph(500, 4, 0.5, rand)

            sys.setrecursionlimit(70)
            t = modular_decomposition(G, solver='linear')
            sys.setrecursionlimit(rec_lim)

            self.assertLessEqual(t.modular_width(), 4)

    def test_modular_decomposition_internal(self):
        G = nx.empty_graph(7)
        G.add_edges_from([(0, 1), (0, 2), (0, 3), (0, 4), (0, 5), (0, 6), (1, 2), (2, 3), (3, 4), (5, 6)])
        t = modular_decomposition(G, sorted=True)
        self.assertEqual(str(t), '(J(0)(U(P(1)(2)(3)(4))(J(5)(6))))')

        self.assertEqual(
            [(nd.data.vertices_begin, nd.data.vertices_end) for nd in t.root.dfs_preorder_nodes()],
            [(0, 7), (0, 1), (1, 7), (1, 5), (1, 2), (2, 3), (3, 4), (4, 5), (5, 7), (5, 6), (6, 7)]
        )
