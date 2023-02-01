import unittest

from modular.compute.MDComputeNode import MDComputeNode, OperationType, SplitDirection
from modular.tree.RootedForest import RootedForest
from modular.compute.refinement import get_max_subtrees, refine_one_node, group_sibling_nodes


class TestRefinement(unittest.TestCase):
    """Tests refinement module."""

    def test_get_max_subtrees(self):
        """Tests get_max_subtrees()."""
        tree: RootedForest[MDComputeNode] = RootedForest()
        prob = tree.create_node(MDComputeNode.new_problem_node(False))
        op1 = tree.create_node(MDComputeNode.new_operation_node(OperationType.PARALLEL))
        op2 = tree.create_node(MDComputeNode.new_operation_node(OperationType.SERIES))
        op3 = tree.create_node(MDComputeNode.new_operation_node(OperationType.PARALLEL))
        op4 = tree.create_node(MDComputeNode.new_operation_node(OperationType.PARALLEL))
        op5 = tree.create_node(MDComputeNode.new_operation_node(OperationType.SERIES))
        vs = [tree.create_node(MDComputeNode.new_vertex_node(i)) for i in range(8)]

        tree.move_to(vs[5], op5)
        tree.move_to(vs[4], op5)
        tree.move_to(op5, op4)
        tree.move_to(op4, prob)
        tree.move_to(vs[1], op4)
        tree.move_to(vs[0], prob)
        tree.move_to(vs[7], op3)
        tree.move_to(vs[3], op3)
        tree.move_to(op3, op2)
        tree.move_to(vs[2], op2)
        tree.move_to(vs[6], op1)
        tree.move_to(op2, op1)
        tree.move_to(op1, prob)

        self.assertEqual(len(tree.roots), 1)
        self.assertEqual(str(list(tree.roots)[0]), '(C-(U(J(2)(U(3)(7)))(6))(0)(U(1)(J(4)(5))))')

        self.assertSetEqual(set(get_max_subtrees([vs[i] for i in [0, 3, 4, 5, 7]])), {vs[0], op3, op5})
        self.assertSetEqual(set(get_max_subtrees([vs[i] for i in [0, 1, 4, 5]])), {vs[0], op4})
        self.assertSetEqual(set(get_max_subtrees([vs[i] for i in [2, 3, 7, 6]])), {op1})
        self.assertSetEqual(set(get_max_subtrees([vs[i] for i in [2, 3, 7, 4, 5]])), {op2, op5})

    def test_refine_one_node(self):
        """Tests refine_one_node()."""

        def setup():
            tree: RootedForest[MDComputeNode] = RootedForest()
            prob = tree.create_node(MDComputeNode.new_problem_node(False))
            op0 = tree.create_node(MDComputeNode.new_operation_node(OperationType.PRIME))
            op1 = tree.create_node(MDComputeNode.new_operation_node(OperationType.PRIME))
            op2 = tree.create_node(MDComputeNode.new_operation_node(OperationType.SERIES))
            op3 = tree.create_node(MDComputeNode.new_operation_node(OperationType.PRIME))
            op4 = tree.create_node(MDComputeNode.new_operation_node(OperationType.PRIME))
            vs = [tree.create_node(MDComputeNode.new_vertex_node(i)) for i in range(9)]

            tree.move_to(op0, prob)
            tree.move_to(vs[8], op0)
            tree.move_to(vs[7], op0)
            tree.move_to(vs[6], op0)
            tree.move_to(vs[5], op1)
            tree.move_to(vs[4], op2)
            tree.move_to(vs[3], op3)
            tree.move_to(vs[2], op4)
            tree.move_to(vs[1], op4)
            tree.move_to(vs[0], op4)
            tree.move_to(op1, op0)
            tree.move_to(op2, op1)
            tree.move_to(op3, op2)
            tree.move_to(op4, op3)
            return tree, prob, vs, op0, op1, op2, op3, op4

        tree, prob, vs, op0, op1, op2, op3, op4 = setup()
        self.assertEqual(repr(prob), '(C-(P-(P-(J-(P-(P-(0-)(1-)(2-))(3-))(4-))(5-))(6-)(7-)(8-)))')
        sg = group_sibling_nodes(tree, [vs[0]])[0]
        refine_one_node(tree, sg[0], SplitDirection.LEFT, False)
        self.assertEqual(repr(prob), '(C-(P<(P<(J<(P<(P<(0<)(1<)(2<))(3<))(4-))(5<))(6<)(7<)(8<)))')

        tree, prob, vs, op0, op1, op2, op3, op4 = setup()
        sg = group_sibling_nodes(tree, [vs[1]])[0]
        refine_one_node(tree, sg[0], SplitDirection.LEFT, False)
        self.assertEqual(repr(prob), '(C-(P<(P<(J<(P<(P<(1<)(0<)(2<))(3<))(4-))(5<))(6<)(7<)(8<)))')

        tree, prob, vs, op0, op1, op2, op3, op4 = setup()
        sg = group_sibling_nodes(tree, [vs[2]])[0]
        refine_one_node(tree, sg[0], SplitDirection.LEFT, False)
        self.assertEqual(repr(prob), '(C-(P<(P<(J<(P<(P<(2<)(0<)(1<))(3<))(4-))(5<))(6<)(7<)(8<)))')

        tree, prob, vs, op0, op1, op2, op3, op4 = setup()
        sg = group_sibling_nodes(tree, [vs[3]])[0]
        refine_one_node(tree, sg[0], SplitDirection.LEFT, False)
        self.assertEqual(repr(prob), '(C-(P<(P<(J<(P<(3<)(P<(0-)(1-)(2-)))(4-))(5<))(6<)(7<)(8<)))')

        tree, prob, vs, op0, op1, op2, op3, op4 = setup()
        sg = group_sibling_nodes(tree, [vs[4]])[0]
        refine_one_node(tree, sg[0], SplitDirection.LEFT, False)
        self.assertEqual(repr(prob), '(C-(P<(P<(J<(J<(P-(P-(0-)(1-)(2-))(3-)))(4<))(5<))(6<)(7<)(8<)))')

        tree, prob, vs, op0, op1, op2, op3, op4 = setup()
        sg = group_sibling_nodes(tree, [vs[5]])[0]
        refine_one_node(tree, sg[0], SplitDirection.LEFT, False)
        self.assertEqual(repr(prob), '(C-(P<(P<(5<)(J<(P-(P-(0-)(1-)(2-))(3-))(4-)))(6<)(7<)(8<)))')

        tree, prob, vs, op0, op1, op2, op3, op4 = setup()
        sg = group_sibling_nodes(tree, [vs[6]])[0]
        refine_one_node(tree, sg[0], SplitDirection.LEFT, False)
        self.assertEqual(repr(prob), '(C-(6<)(P<(P<(J-(P-(P-(0-)(1-)(2-))(3-))(4-))(5-))(7<)(8<)))')

        tree, prob, vs, op0, op1, op2, op3, op4 = setup()
        sg = group_sibling_nodes(tree, [vs[7]])[0]
        refine_one_node(tree, sg[0], SplitDirection.LEFT, False)
        self.assertEqual(repr(prob), '(C-(7<)(P<(P<(J-(P-(P-(0-)(1-)(2-))(3-))(4-))(5-))(6<)(8<)))')

        tree, prob, vs, op0, op1, op2, op3, op4 = setup()
        sg = group_sibling_nodes(tree, [vs[8]])[0]
        refine_one_node(tree, sg[0], SplitDirection.LEFT, False)
        self.assertEqual(repr(prob), '(C-(8<)(P<(P<(J-(P-(P-(0-)(1-)(2-))(3-))(4-))(5-))(6<)(7<)))')

        tree, prob, vs, op0, op1, op2, op3, op4 = setup()
        sg = group_sibling_nodes(tree, [op4])[0]
        refine_one_node(tree, sg[0], SplitDirection.LEFT, False)
        self.assertEqual(repr(prob), '(C-(P<(P<(J<(P<(P<(0-)(1-)(2-))(3<))(4-))(5<))(6<)(7<)(8<)))')

        tree, prob, vs, op0, op1, op2, op3, op4 = setup()
        sg = group_sibling_nodes(tree, [op4])[0]
        refine_one_node(tree, sg[0], SplitDirection.LEFT, True)
        self.assertEqual(repr(prob), '(C-(P<(P<(J<(P<(P<(0<)(1<)(2<))(3<))(4-))(5<))(6<)(7<)(8<)))')

        tree, prob, vs, op0, op1, op2, op3, op4 = setup()
        sg = group_sibling_nodes(tree, [op3])[0]
        refine_one_node(tree, sg[0], SplitDirection.LEFT, False)
        self.assertEqual(repr(prob), '(C-(P<(P<(J<(J<(4-))(P<(P-(0-)(1-)(2-))(3-)))(5<))(6<)(7<)(8<)))')

        tree, prob, vs, op0, op1, op2, op3, op4 = setup()
        sg = group_sibling_nodes(tree, [op2])[0]
        refine_one_node(tree, sg[0], SplitDirection.LEFT, False)
        self.assertEqual(repr(prob), '(C-(P<(P<(J<(P-(P-(0-)(1-)(2-))(3-))(4-))(5<))(6<)(7<)(8<)))')

        tree, prob, vs, op0, op1, op2, op3, op4 = setup()
        sg = group_sibling_nodes(tree, [op1])[0]
        refine_one_node(tree, sg[0], SplitDirection.LEFT, False)
        self.assertEqual(repr(prob), '(C-(P<(J-(P-(P-(0-)(1-)(2-))(3-))(4-))(5-))(P<(6<)(7<)(8<)))')

        tree, prob, vs, op0, op1, op2, op3, op4 = setup()
        sg = group_sibling_nodes(tree, [op0])[0]
        refine_one_node(tree, sg[0], SplitDirection.LEFT, False)
        self.assertEqual(repr(prob), '(C-(P-(P-(J-(P-(P-(0-)(1-)(2-))(3-))(4-))(5-))(6-)(7-)(8-)))')
