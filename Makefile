PYTHON=python3

SRC_PY=src/main/python
TEST_PY=src/test/python
STUB_PY=src/main/stubs

export PYTHONPATH=$(SRC_PY)
export MYPYPATH=$(STUB_PY)

test:
	mypy $(SRC_PY)
	$(PYTHON) -m pytest -x --cov=./src/main/python --cov-report=lcov:./coverage/lcov.info $(TEST_PY)

lab:
	jupyter-lab

.PHONY: test lab
