# modular-decomposition

[![Unit Test](https://github.com/mogproject/modular-decomposition/actions/workflows/ut.yml/badge.svg)](https://github.com/mogproject/modular-decomposition/actions/workflows/ut.yml) [![Coverage Status](https://coveralls.io/repos/github/mogproject/modular-decomposition/badge.svg?branch=main)](https://coveralls.io/github/mogproject/modular-decomposition?branch=main)

Graph modular decomposition implemented with C++ and Python 3. 

The code implements the algorithm described in *Simpler, Linear-Time Modular Decomposition via Recursive Factorizing Permutations*
by Marc Tedder, Derek Corneil, Michel Habib, and Christophe Paul (appeared at [ICALP 2008](https://link.springer.com/chapter/10.1007/978-3-540-70575-8_52)).

## Dependencies

- NetworkX (`pip install networkx`)
- NumPy (`pip install numpy`)

### Dependencies for Unit Testing

- pytest (`pip install pytest`)
- pytest-cov (`pip install pytest-cov`)
- MyPy (`pip install mypy`)

## For Developers

- [Developer Guide](https://github.com/mogproject/modular-decomposition/wiki/Developer-Guide)

| Task | Command |
| :--- | :--- |
| Run all unit tests | `make test` |
| Open Jupyter Lab   | `make lab`  |

## Special Thanks

- NetworkX stubs taken from [eggplants/networkx-stubs](https://github.com/eggplants/networkx-stubs)

