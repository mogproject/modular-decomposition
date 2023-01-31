import unittest

import numpy as np
from modular.util.matrix_util import equivalent_classes


class TestMatrixUtil(unittest.TestCase):
    """Tests matrix_util module."""

    def test_equivalent_classes(self):
        """Tests equivalent_classes()."""

        self.assertListEqual(equivalent_classes(np.array([])), [])

        A = np.array([[1, 2, 3], [1, 2, 3], [1, 2, 3], [1, 2, 3]])
        self.assertListEqual(equivalent_classes(A), [[0, 1, 2, 3]])

        A = np.array([[1, 2, 3], [2, 3, 1], [1, 2, 3], [2, 2, 2], [2, 2, 2]])
        self.assertListEqual(equivalent_classes(A), [[0, 2], [3, 4], [1]])

        A = np.array([[1, 2, 3], [2, 3, 1], [1, 2, 3], [1, 1, 1], [1, 1, 1]])
        self.assertListEqual(equivalent_classes(A), [[3, 4], [0, 2], [1]])

        A = np.array([[1, 2, 3], [2, 3, 1], [2, 2, 3], [1, 1, 1], [2, 1, 1]])
        self.assertListEqual(equivalent_classes(A), [[3], [0], [4], [2], [1]])
