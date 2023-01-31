import unittest

from modular.compute.assembly import remove_degenerate_duplicates, delineate, compute_mu, assemble_tree
from modular.compute.MDComputeNode import MDComputeNode, OperationType
from modular.tree.RootedTree import RootedTree


class TestAssembly(unittest.TestCase):
    """Tests assembly module."""

    def test_remove_degenerate_duplicates(self) -> None:
        tree: RootedTree[MDComputeNode] = RootedTree()
        vnodes = [tree.create_node(MDComputeNode.new_vertex_node(i)) for i in range(3)]
        n0 = tree.create_node(MDComputeNode.new_operation_node(OperationType.PARALLEL))
        n1 = tree.create_node(MDComputeNode.new_operation_node(OperationType.PARALLEL))
        tree.move_to(vnodes[2], n0)
        tree.move_to(vnodes[1], n1)
        tree.move_to(vnodes[0], n1)
        tree.move_to(n1, n0)

        self.assertEqual(str(n0), '(U(U(0)(1))(2))')
        remove_degenerate_duplicates(tree, n0)
        self.assertEqual(str(n0), '(U(0)(1)(2))')

    def test_delineate(self) -> None:
        # K_1
        tree: RootedTree[MDComputeNode] = RootedTree()

        v0 = tree.create_node(MDComputeNode.new_vertex_node(0))
        prob = tree.create_node(MDComputeNode.new_problem_node(False))
        tree.move_to(v0, prob)

        ps = [v0]
        pi = 0
        prob.data.vertex = pi

        mu = compute_mu(ps, pi, [[]])
        self.assertEqual(mu, [0])

        bounds = delineate(pi, [False], [False], [False], mu)
        self.assertEqual(bounds, [])

        root = assemble_tree(tree, ps, pi, bounds)
        self.assertEqual(str(root), '(0)')

        # 3K_1
        tree: RootedTree[MDComputeNode] = RootedTree()
        vs = [tree.create_node(MDComputeNode.new_vertex_node(i)) for i in range(3)]
        prob = tree.create_node(MDComputeNode.new_problem_node(False))
        tree.move_to(vs[2], prob)
        tree.move_to(vs[1], prob)
        tree.move_to(vs[0], prob)

        ps = vs
        pi = 0
        prob.data.vertex = pi

        mu = compute_mu(ps, pi, [[], [], []])
        self.assertEqual(mu, [0, 0, 0])

        bounds = delineate(pi, [False] * 3, [False] * 3, [False] * 3, mu)
        self.assertEqual(bounds, [])

        root = assemble_tree(tree, ps, pi, bounds)
        self.assertEqual(str(root), '(U(2)(1)(0))')

        # P_3
        tree: RootedTree[MDComputeNode] = RootedTree()
        vs = [tree.create_node(MDComputeNode.new_vertex_node(i)) for i in range(3)]
        prob = tree.create_node(MDComputeNode.new_problem_node(False))
        tree.move_to(vs[2], prob)
        tree.move_to(vs[0], prob)
        tree.move_to(vs[1], prob)

        ps = [vs[1], vs[0], vs[2]]
        pi = 1
        prob.data.vertex = pi

        mu = compute_mu(ps, pi, [[1, 2], [0], []])
        self.assertEqual(mu, [2, 1, 1])

        bounds = delineate(pi, [False] * 3, [False] * 3, [False] * 3, mu)
        self.assertEqual(bounds, [(1, 2)])

        root = assemble_tree(tree, ps, pi, bounds)
        self.assertEqual(str(root), '(J(1)(U(2)(0)))')
