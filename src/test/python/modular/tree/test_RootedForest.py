import unittest

from modular.tree.RootedForest import RootedForest


def parse_bool(s: str): return [c != '0' for c in s]


def create_forest():
    tree = RootedForest(verify=True)  # type: RootedForest[int]
    nodes = [tree.create_node(i) for i in range(20)]

    #  0    3       10   13
    #       |            |
    #       4-5---1      14-15---11
    #       | |          |  |
    #       7 2-9        17 12-19
    #       |            |
    #       8-6          18-16
    relations = [[3, 1], [3, 5], [3, 4], [5, 9], [5, 2], [4, 7], [7, 6], [7, 8],
                 [13, 11], [13, 15], [13, 14], [15, 19], [15, 12], [14, 17], [17, 16], [17, 18]]
    for c, p in relations:
        tree.move_to(nodes[p], nodes[c])
    return tree, nodes


class TestRootedForest(unittest.TestCase):
    """Tests RootedForest class."""

    def check_consistency(self, nodes):
        for x in nodes:
            # left & right connections
            if x.left:
                self.assertEqual(x.left.right, x)
            if x.right:
                self.assertEqual(x.right.left, x)

            # children
            self.assertEqual(len(x.get_children()), x.number_of_children())

            # parent
            if x.parent:
                self.assertIn(x, x.parent.get_children())

    def test_basic_functions(self):
        tree: RootedForest[int] = RootedForest()
        self.assertFalse(tree)
        tree, nodes = create_forest()

        #
        #    Node properties
        #

        # Node#is_root()
        self.assertListEqual([x.is_root() for x in nodes[:10]], parse_bool('1001000000'))

        # Node#has_parent()
        self.assertListEqual([x.has_parent() for x in nodes[:10]], parse_bool('0110111111'))

        # Node#is_first_child()
        self.assertListEqual([x.is_first_child() for x in nodes[:10]], parse_bool('0010100110'))

        # Node#is_last_child()
        self.assertListEqual([x.is_last_child() for x in nodes[:10]], parse_bool('0100001101'))

        # Node#is_leaf()
        self.assertListEqual([x.is_leaf() for x in nodes[:10]], parse_bool('1110001011'))

        # Node#has_child()
        self.assertListEqual([x.has_child() for x in nodes[:10]], parse_bool('0001110100'))

        # Node#has_only_one_child()
        self.assertListEqual([x.has_only_one_child() for x in nodes[:10]], parse_bool('0000100000'))

        # Node#number_of_children()
        self.assertListEqual([x.number_of_children() for x in nodes[:10]], [0, 0, 0, 3, 1, 2, 0, 2, 0, 0])

        # Node#__str__()
        self.assertListEqual(
            [str(x) for x in nodes[:10]],
            ['(0)', '(1)', '(2)', '(3(4(7(8)(6)))(5(2)(9))(1))', '(4(7(8)(6)))', '(5(2)(9))', '(6)', '(7(8)(6))', '(8)', '(9)']
        )

        # Node#get_children()
        self.assertListEqual(
            [[c.data for c in x.get_children()] for x in nodes[:10]],
            [[], [], [], [4, 5, 1], [7], [2, 9], [], [8, 6], [], []]
        )

        # Node#get_leaves()
        self.assertListEqual(
            [[c.data for c in x.get_leaves()] for x in nodes[:10]],
            [[0], [1], [2], [1, 9, 2, 6, 8], [6, 8], [9, 2], [6], [6, 8], [8], [9]]
        )

        # Node#get_ancestors()
        self.assertListEqual(
            [[c.data for c in x.get_ancestors()] for x in nodes[:10]],
            [[], [3], [5, 3], [], [3], [3], [7, 4, 3], [4, 3], [7, 4, 3], [5, 3]]
        )

        # Node#get_root()
        self.assertListEqual([x.get_root().data for x in nodes[:10]], [0, 3, 3, 3, 3, 3, 3, 3, 3, 3])

        #
        #    RootedTree properties
        #

        # __len__
        self.assertEqual(len(tree), 20)

        # __bool__
        self.assertTrue(tree)

        self.assertEqual(len(tree.roots), 4)

        # consistency check
        self.check_consistency(nodes)

    def test_node_dfs_reverse_preorder_nodes(self):
        nodes = create_forest()[1]
        self.assertEqual([x.data for x in nodes[0].dfs_preorder_nodes()], [0])
        self.assertEqual([x.data for x in nodes[3].dfs_preorder_nodes()], [3, 4, 7, 8, 6, 5, 2, 9, 1])

    def test_node_dfs_reverse_preorder_nodes(self):
        nodes = create_forest()[1]
        self.assertEqual([x.data for x in nodes[0].dfs_reverse_preorder_nodes()], [0])
        self.assertEqual([x.data for x in nodes[3].dfs_reverse_preorder_nodes()], [3, 1, 5, 9, 2, 4, 7, 6, 8])

    def test_node_bfs_nodes(self):
        nodes = create_forest()[1]
        self.assertEqual([x.data for x in nodes[0].bfs_nodes()], [0])
        self.assertEqual([x.data for x in nodes[3].bfs_nodes()], [3, 4, 5, 1, 7, 2, 9, 8, 6])

    def test_detach(self):
        tree, nodes = create_forest()
        self.assertEqual(str(nodes[3]), '(3(4(7(8)(6)))(5(2)(9))(1))')
        self.assertEqual(nodes[3].number_of_children(), 3)
        tree.detach(nodes[5])
        self.assertEqual(str(nodes[3]), '(3(4(7(8)(6)))(1))')
        self.assertEqual(str(nodes[5]), '(5(2)(9))')
        self.assertEqual(nodes[3].number_of_children(), 2)
        tree.detach(nodes[4])
        self.assertEqual(str(nodes[3]), '(3(1))')
        self.assertEqual(str(nodes[4]), '(4(7(8)(6)))')
        self.assertEqual(nodes[3].number_of_children(), 1)
        tree.detach(nodes[1])
        self.assertEqual(str(nodes[3]), '(3)')
        self.assertEqual(str(nodes[1]), '(1)')
        self.assertEqual(nodes[3].number_of_children(), 0)
        tree.detach(nodes[0])
        self.assertEqual(str(nodes[0]), '(0)')

        self.assertEqual(len(tree), 20)
        self.check_consistency(nodes)

    def test_remove(self):
        tree, nodes = create_forest()

        tree.remove(nodes[2])
        tree.remove(nodes[9])
        tree.remove(nodes[5])
        self.assertEqual(str(nodes[3]), '(3(4(7(8)(6)))(1))')
        self.assertEqual(len(tree), 17)

        i99 = tree.create_node(99)
        tree.move_to(i99, nodes[3])
        self.assertEqual(str(nodes[3]), '(3(99)(4(7(8)(6)))(1))')
        self.assertEqual(len(tree), 18)

        i98 = tree.create_node(98)
        tree.move_to(i98, i99)
        self.assertEqual(str(nodes[3]), '(3(99(98))(4(7(8)(6)))(1))')
        self.assertEqual(len(tree), 19)

        i97 = tree.create_node(97)
        tree.move_to(i97, i98)
        self.assertEqual(str(nodes[3]), '(3(99(98(97)))(4(7(8)(6)))(1))')
        self.assertEqual(len(tree), 20)

        i96 = tree.create_node(96)
        tree.move_to(i96, i98)
        self.assertEqual(str(nodes[3]), '(3(99(98(96)(97)))(4(7(8)(6)))(1))')
        self.assertEqual(len(tree), 21)

        tree.remove(i97)
        self.assertEqual(str(nodes[3]), '(3(99(98(96)))(4(7(8)(6)))(1))')
        self.assertEqual(len(tree), 20)

        self.check_consistency(nodes)

    def test_swap(self):
        tree, nodes = create_forest()

        tree.swap(nodes[5], nodes[15])
        self.assertEqual(str(nodes[3]), '(3(4(7(8)(6)))(15(12)(19))(1))')
        self.assertEqual(str(nodes[13]), '(13(14(17(18)(16)))(5(2)(9))(11))')

        tree.swap(nodes[4], nodes[11])
        self.assertEqual(str(nodes[3]), '(3(11)(15(12)(19))(1))')
        self.assertEqual(str(nodes[13]), '(13(14(17(18)(16)))(5(2)(9))(4(7(8)(6))))')

        tree.detach(nodes[7])
        tree.swap(nodes[1], nodes[7])
        self.assertEqual(str(nodes[3]), '(3(11)(15(12)(19))(7(8)(6)))')

        tree.swap(nodes[7], nodes[1])
        tree.swap(nodes[1], nodes[7])
        self.assertEqual(str(nodes[3]), '(3(11)(15(12)(19))(7(8)(6)))')

        self.check_consistency(nodes)

        # swap with root
        tree, nodes = create_forest()
        tree.swap(nodes[0], nodes[10])
        self.assertSetEqual({node.data for node in tree.roots}, {0, 3, 10, 13})
        tree.swap(nodes[0], nodes[4])
        self.assertSetEqual({node.data for node in tree.roots}, {3, 4, 10, 13})

    def test_replace(self):
        tree, nodes = create_forest()
        tree.replace(nodes[3], nodes[5])
        self.assertEqual(str(nodes[3]), '(3(4(7(8)(6)))(1))')
        self.assertEqual(str(nodes[5]), '(5(2)(9))')
        self.check_consistency(nodes)

    def test_move_to_before(self):
        tree, nodes = create_forest()

        tree.move_to_before(nodes[15], nodes[5])
        self.assertEqual(str(nodes[3]), '(3(4(7(8)(6)))(15(12)(19))(5(2)(9))(1))')

        tree.move_to_before(nodes[11], nodes[1])
        self.assertEqual(str(nodes[3]), '(3(4(7(8)(6)))(15(12)(19))(5(2)(9))(11)(1))')

        tree.move_to_before(nodes[14], nodes[4])
        self.assertEqual(str(nodes[3]), '(3(14(17(18)(16)))(4(7(8)(6)))(15(12)(19))(5(2)(9))(11)(1))')

        tree.move_to_before(nodes[11], nodes[4])
        self.assertEqual(str(nodes[3]), '(3(14(17(18)(16)))(11)(4(7(8)(6)))(15(12)(19))(5(2)(9))(1))')

        self.check_consistency(nodes)

        tree, nodes = create_forest()
        tree.move_to_before(nodes[0], nodes[1])
        self.assertSetEqual({x.data for x in tree.roots}, {3, 10, 13})
        self.assertEqual(str(nodes[3]), '(3(4(7(8)(6)))(5(2)(9))(0)(1))')

        tree, nodes = create_forest()
        tree.move_to_before(nodes[0], nodes[4])
        self.assertSetEqual({x.data for x in tree.roots}, {3, 10, 13})
        self.assertEqual(str(nodes[3]), '(3(0)(4(7(8)(6)))(5(2)(9))(1))')

        self.check_consistency(nodes)

    def test_move_to_after(self):
        tree, nodes = create_forest()

        tree.move_to_after(nodes[15], nodes[5])
        self.assertEqual(str(nodes[3]), '(3(4(7(8)(6)))(5(2)(9))(15(12)(19))(1))')

        tree.move_to_after(nodes[11], nodes[1])
        self.assertEqual(str(nodes[3]), '(3(4(7(8)(6)))(5(2)(9))(15(12)(19))(1)(11))')

        tree.move_to_after(nodes[14], nodes[4])
        self.assertEqual(str(nodes[3]), '(3(4(7(8)(6)))(14(17(18)(16)))(5(2)(9))(15(12)(19))(1)(11))')

        tree.move_to_after(nodes[11], nodes[4])
        self.assertEqual(str(nodes[3]), '(3(4(7(8)(6)))(11)(14(17(18)(16)))(5(2)(9))(15(12)(19))(1))')
        self.check_consistency(nodes)

        tree, nodes = create_forest()
        tree.move_to_after(nodes[0], nodes[1])
        self.assertSetEqual({x.data for x in tree.roots}, {3, 10, 13})
        self.assertEqual(str(nodes[3]), '(3(4(7(8)(6)))(5(2)(9))(1)(0))')

        tree, nodes = create_forest()
        tree.move_to_after(nodes[0], nodes[4])
        self.assertSetEqual({x.data for x in tree.roots}, {3, 10, 13})
        self.assertEqual(str(nodes[3]), '(3(4(7(8)(6)))(0)(5(2)(9))(1))')

        self.check_consistency(nodes)

    def test_make_first_child(self):
        tree, nodes = create_forest()

        tree.make_first_child(nodes[3])
        self.assertEqual(str(nodes[3]), '(3(4(7(8)(6)))(5(2)(9))(1))')
        tree.make_first_child(nodes[4])
        self.assertEqual(str(nodes[3]), '(3(4(7(8)(6)))(5(2)(9))(1))')
        tree.make_first_child(nodes[5])
        self.assertEqual(str(nodes[3]), '(3(5(2)(9))(4(7(8)(6)))(1))')
        tree.make_first_child(nodes[1])
        self.assertEqual(str(nodes[3]), '(3(1)(5(2)(9))(4(7(8)(6))))')
        tree.make_first_child(nodes[2])
        self.assertEqual(str(nodes[3]), '(3(1)(5(2)(9))(4(7(8)(6))))')
        tree.make_first_child(nodes[9])
        self.assertEqual(str(nodes[3]), '(3(1)(5(9)(2))(4(7(8)(6))))')
        tree.make_first_child(nodes[7])
        self.assertEqual(str(nodes[3]), '(3(1)(5(9)(2))(4(7(8)(6))))')

        self.check_consistency(nodes)

    def test_add_children_from(self):
        tree, nodes = create_forest()

        tree.add_children_from(nodes[3], nodes[13])
        self.assertEqual(str(nodes[3]), '(3(14(17(18)(16)))(15(12)(19))(11)(4(7(8)(6)))(5(2)(9))(1))')
        self.assertEqual(str(nodes[13]), '(13)')
        self.assertEqual(nodes[3].number_of_children(), 6)
        self.assertEqual(nodes[13].number_of_children(), 0)

        tree.add_children_from(nodes[3], nodes[1])
        self.assertEqual(str(nodes[3]), '(3(14(17(18)(16)))(15(12)(19))(11)(4(7(8)(6)))(5(2)(9))(1))')
        self.assertEqual(nodes[3].number_of_children(), 6)
        self.assertEqual(nodes[1].number_of_children(), 0)

        tree.add_children_from(nodes[3], nodes[4])
        self.assertEqual(str(nodes[3]), '(3(7(8)(6))(14(17(18)(16)))(15(12)(19))(11)(4)(5(2)(9))(1))')
        self.assertEqual(nodes[3].number_of_children(), 7)
        self.assertEqual(nodes[4].number_of_children(), 0)

        tree.add_children_from(nodes[4], nodes[5])
        self.assertEqual(str(nodes[3]), '(3(7(8)(6))(14(17(18)(16)))(15(12)(19))(11)(4(2)(9))(5)(1))')

        self.check_consistency(nodes)

    def test_replace_by_children(self):
        tree, nodes = create_forest()

        tree.replace_by_children(nodes[5])
        self.assertEqual(str(nodes[3]), '(3(4(7(8)(6)))(2)(9)(1))')
        self.assertEqual(nodes[3].number_of_children(), 4)

        tree.replace_by_children(nodes[4])
        self.assertEqual(str(nodes[3]), '(3(7(8)(6))(2)(9)(1))')
        self.assertEqual(nodes[3].number_of_children(), 4)

        tree.replace_by_children(nodes[7])
        self.assertEqual(str(nodes[3]), '(3(8)(6)(2)(9)(1))')
        self.assertEqual(nodes[3].number_of_children(), 5)

        tree.replace_by_children(nodes[1])
        self.assertEqual(str(nodes[3]), '(3(8)(6)(2)(9))')
        self.assertEqual(nodes[3].number_of_children(), 4)

        self.check_consistency(nodes)

    def test_replace_children(self):
        tree, nodes = create_forest()

        tree.replace_children(nodes[5], nodes[15])
        self.assertEqual(str(nodes[3]), '(3(4(7(8)(6)))(5(15(12)(19)))(1))')

        tree.replace_children(nodes[1], nodes[0])
        self.assertEqual(str(nodes[3]), '(3(4(7(8)(6)))(5(15(12)(19)))(1(0)))')

        tree.replace_children(nodes[3], nodes[1])
        self.assertEqual(str(nodes[3]), '(3(1(0)))')

        self.check_consistency(nodes)
