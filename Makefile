PYTHON=python3

SRC_PY=src/main/python
TEST_PY=src/test/python
STUB_PY=src/main/stubs

PYTEST_OPTS=""  # --full-trace

export PYTHONPATH=$(SRC_PY)
export MYPYPATH=$(STUB_PY)

test:
	mypy $(SRC_PY)
	$(PYTHON) -m pytest -x --cov=$(SRC_PY) --cov-report=lcov:./coverage/lcov.info $(PYTEST_OPTS) $(TEST_PY)

lab:
	jupyter-lab

.PHONY: test lab
