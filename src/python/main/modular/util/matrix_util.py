import numpy as np


def equivalent_classes(A: np.ndarray, axis: int = 0) -> list[list[int]]:
    """
    Computes maximal sets of row indices, any two of which have the same row values.

    i.e. i ~ j if and only if A[i] == A[j]

    Example:
      In [1]: A = np.array([[1, 2, 3], [2, 3, 1], [1, 2, 3], [2, 2, 2], [2, 2, 2]])

      In [2]: equivalent_classes(A)
      Out[2]: [[0, 2], [3, 4], [1]]
    """

    u, ind = np.unique(A, axis=axis, return_inverse=True)
    ret: list[list[int]] = [[] for _ in range(len(u))]
    for i, x in enumerate(ind):
        ret[x] += [i]
    return ret
