PYTHON=python3

SRC_PY=src/main/python
TEST_PY=src/test/python
STUB_PY=src/main/stubs

export PYTHONPATH=$(SRC_PY)
export MYPYPATH=$(STUB_PY)

test:
	mypy $(SRC_PY)
	$(PYTHON) -m pytest -x --cov=. --cov-report=xml:cov.xml $(TEST_PY)

.PHONY: test
